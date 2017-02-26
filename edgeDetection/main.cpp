#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <io.h>

#include "IPM.h"
#include "imagePreparation.h"


using namespace cv;
using namespace std;

void warpImage();

void getCornerList();

void getEdgesTry();

int test();

void houghLines(cv::Mat &src, cv::Mat &gr);


int main(int argc, char **argv) {

//    Mat im,imGrey;
//
//    im = imagePreparation::readImage("../chess.jpg"); //read the image
//    namedWindow("original", CV_WINDOW_AUTOSIZE);
//    imshow("original", im);

//    imGrey = imagePreparation::convertImageGreyscale(im); //convert it to greyscale
//    im = imagePreparation::blurImage(imGrey); //blur the image
//    im = imagePreparation::CannyThreshold(im); //apply CANNY , the threshold is good at 80
//    im = imagePreparation::dilationImage(im,2,2); //apply dilation , 2,2 are good parameters
//    warpImage();
//    getCornerList();
//    featureDetection::firstTry();
//    km::kmeanstry();
//    getEdgesTry();
    test();
    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}

/**
 * This function will warp the image
 */
void warpImage() {
    Mat im, imGrey, originalImage;
    originalImage = imagePreparation::readImage("../chess.jpg"); //read the image
    imGrey = imagePreparation::convertImageGreyscale(originalImage); //convert it to greyscale
    im = imagePreparation::blurImage(imGrey); //blur the image
    vector<Point2f> corners = imagePreparation::getCornerPoints(im);
    im = imagePreparation::warpImage(originalImage, corners);

    namedWindow("test", CV_WINDOW_AUTOSIZE);
    imshow("test", im);
}

void getCornerList() {
    Mat im, imGrey, originalImage;
    originalImage = imagePreparation::readImage("../chess.jpg"); //read the image
    imGrey = imagePreparation::convertImageGreyscale(originalImage);
    imGrey = imagePreparation::blurImage(imGrey);
    vector<Point> corners;
    goodFeaturesToTrack(imGrey, corners, 200, 0.05, 15);
    for (int i = 0; i < corners.size(); i++) {
        circle(originalImage, Point(corners[i].x, corners[i].y), 3, 255, 1, 8, 0);
    }
    namedWindow("test", CV_WINDOW_AUTOSIZE);
    imshow("test", originalImage);
}

//void getEdgesTry(){
//    Mat im,imGrey;
//    im= imagePreparation::readImage("../chess.jpg"); //read the image
//    Size size(1000,1000);
//    resize(im,im,size);
//
//
//    imGrey = imagePreparation::convertImageGreyscale(im); //convert it to greyscale
//    im = imagePreparation::blurImage(imGrey); //blur the image
//    im=imagePreparation::CannyThreshold(im,40,3);
//
//    namedWindow("original", CV_WINDOW_AUTOSIZE);
//    imshow("original", im);
//
//}

Mat src, src_gray;
Mat dst, detected_edges, cdst;

int lowThreshold;
int const max_lowThreshold = 255;
int ratio = 3;
int kernel_size = 3;
char *window_name = "Edge Map";

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void *) {
    /// Reduce noise with a kernel 3x3
//    blur( src_gray, detected_edges, Size(3,3) );
    /// Canny detector
    Canny(src, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);
    cvtColor(detected_edges, src_gray, CV_GRAY2BGR);

    houghLines(detected_edges, src_gray);

    /// Using Canny's output as a mask, we display our result
//    dst = Scalar::all(0);
//    src.copyTo( dst, detected_edges);
//    imshow( "Ceva", detected_edges );
//    (dst, cdst, CV_GRAY2BGR);

}

/**
 * This function will be used to write the input mat along with one of its line and the point coordinates into a jpg file so it will be easier to view
 * @param src
 * @param pointNr
 * @param pointX
 * @param pointY
 * @param iteration
 */
void putTextIntoImage(cv::Mat &src, int pointNr, int pointX, int pointY, int iteration) {
    int check = mkdir("../images/result");
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 2;
    int thickness = 3;
    char s[30];
    sprintf(s, "(%d) %d %d", pointNr, pointX, pointY);
    putText(src, s, Point(pointX, pointY), fontFace, fontScale, Scalar(0, 0, 255), thickness, 8, false);
    char path[50];
    sprintf(path, "../images/result/grey_iteration_%d.jpg", iteration);
    imwrite(path, src);
}

/**
 *
 * this function uses probabilistic hough lines to identify all the lines in the image
 *
 * @param src The mat from where we will get the lines
 * @param gr The mat where we will write the lines for viewing
 */
void houghLines(cv::Mat &src, cv::Mat &gr) {

    // Apply a dilation to identify more lines
    src = imagePreparation::dilationImage(src, 2, 3);
    imshow(window_name, src);

    // The vector where we will keep the lines , they come in a pair of 4 coordinates 0-1 for the first point and 2-3 for the other , together they make the line
    vector<Vec4i> lines;

    // This will detect the lines: rho : The resolution of the parameter r in pixels. We use 1 pixel.
    //                             theta :The resolution of the parameter \theta in radians. We use 1 degree (CV_PI/180) , formule: 1) radiani=grade*pi/180  2) grade=radiani*180/pi
    //                             threshold : Only those liens are returned that get enough votes , current is 30
    //                             minLineLength : Minimum line length. Line segments shorter than that are rejected , current is 60
    //                             maxLineGap : Maximum allowed gap between points on the same line to link them , current is 3
    HoughLinesP(src, lines, 1, CV_PI / 180, 30, 100, 3);

    // These vectors will be used to separate the lines into 2 groups depending on their line angle using the ATAN2 function
    // The results should be a VALUE1 and -VALUE1 and maybe some VALUE2,3,4 that are not important

    // From test we saw that the prominent values are  2.XX and -2.XX and some 1.XX , -1.XX
    // TODO we could hard code this or make an algorithm to search for the most prominent V/-V value
    vector<Vec4i> linesAngle1, linesAngle2;

    // This lowThreshold if is used only for testing values and not do useless work
    if (lowThreshold >= 240) {

        for (size_t i = 0; i < lines.size(); i++) {
            Vec4i l = lines[i];

            //This will get the angle that the line is making with the X axis
            // Using this angle we can separate the lines into + and -
            Point p1, p2;
            p1 = Point(l[0], l[1]);
            p2 = Point(l[2], l[3]);
            double angle = atan2((double) (p1.y - p2.y), (double) (p1.x - p2.x));

            // HARD CODED
            int dec = (int) angle;
            if (dec == 2) {
                linesAngle1.push_back(l);
            } else if (dec == -2) {
                linesAngle2.push_back(l);
            }
        }
        printf("Lines found with angle 2:%d and with -2 : %d \n",linesAngle1.size(),linesAngle2.size());
    }

    Mat oneTypeOfLines1,oneTypeOfLines2;
    oneTypeOfLines1=gr.clone();
    oneTypeOfLines2=gr.clone();

    for (size_t i=0;i<linesAngle1.size();i++){
        Vec4i l=linesAngle1[i];
        for(size_t j=1;linesAngle1.size();j++){
            Vec4i currentLine=linesAngle1[j];

        }
        line(oneTypeOfLines1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        Mat externalMat;
        cvtColor(detected_edges, externalMat, CV_GRAY2BGR);
        line(externalMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        //Create jpg files with the current line and the coordinates of its extremities
        putTextIntoImage(externalMat, 1, l[0], l[1], i);
        putTextIntoImage(externalMat, 2, l[2], l[3], i);
        printf("%d) Calculation>>>>> slope %lf length %lf \n",i,(l[3]-l[1]/(double)(l[2]-l[0])),norm(Point(l[2], l[3])-Point(l[0], l[1])));
    }
    imshow("oneTypeOfLines1", oneTypeOfLines1);

    for (size_t i=0;i<linesAngle2.size();i++){
        Vec4i l=linesAngle2[i];
        line(oneTypeOfLines2, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
    }
    imshow("oneTypeOfLines2", oneTypeOfLines2);

//    for (size_t i = 0; i < lines.size(); i++) {
//        Vec4i l = lines[i];
//        //We draw the line on the mat that we will gonna show with imShow(mat)
//        line(gr, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//
//        //We use this if because the best threshold is >240 and we don't put it on 255 directly because of testing
//        if (lowThreshold >= 240) {
//
//            //This will get the angle that the line is making with the X axis
//            // Using this angle we can separate the lines into + and -
//            Point p1, p2;
//            p1 = Point(l[0], l[1]);
//            p2 = Point(l[2], l[3]);
//            double angle = atan2((double) (p1.y - p2.y), (double) (p1.x - p2.x));
//
//            //This mat will be used in writing a jpg file
//            //We will create a different image for every line and show the coordinates of the points that are forming the line
//            Mat externalMat;
//            cvtColor(detected_edges, externalMat, CV_GRAY2BGR);
//            line(externalMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//            //Create jpg files with the current line and the coordinates of its extremities
//            putTextIntoImage(externalMat, 1, l[0], l[1], i);
//            putTextIntoImage(externalMat, 2, l[2], l[3], i);
//
//            printf("Current iteration %d angles %lf  \n", i, angle);
//        }
//    }
    imshow("detected lines", src_gray);
}


/** @function main */
int test() {
    /// Load an image
    src = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
//    Size size(1000,700);
//    resize(src,src,size);

    if (!src.data) { return -1; }

    /// Create a matrix of the same type and size as src (for dst)
    dst.create(src.size(), src.type());

    /// Convert the image to grayscale
    cvtColor(src, src_gray, CV_BGR2GRAY);

    /// Create a window
    namedWindow(window_name, CV_WINDOW_AUTOSIZE);
    namedWindow( "oneTypeOfLines1", CV_WINDOW_AUTOSIZE );
    namedWindow( "oneTypeOfLines2", CV_WINDOW_AUTOSIZE );
    namedWindow("detected lines", CV_WINDOW_AUTOSIZE);


    /// Create a Trackbar for user to enter threshold
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

    /// Show the image
    CannyThreshold(0, 0);

    /// Wait until user exit program by pressing a key
    waitKey(0);

    return 0;
}