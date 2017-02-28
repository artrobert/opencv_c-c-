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
void putTextIntoImage(const char *pathdir, cv::Mat &src, int pointNr, int pointX, int pointY, int iteration) {
    int check = mkdir(pathdir);
    int fontFace = FONT_HERSHEY_SIMPLEX;
    double fontScale = 2;
    int thickness = 3;
    char s[30];
    sprintf(s, "(%d) %d %d", pointNr, pointX, pointY);
    putText(src, s, Point(pointX, pointY), fontFace, fontScale, Scalar(0, 0, 255), thickness, 8, false);
    char path[50];
    sprintf(path, "%s/grey_iteration_%d.jpg", pathdir, iteration);
    imwrite(path, src);
}

/**
 * This function is used in the sort() call so we can sort ASCENDING the vector containing the lines after the X of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointXAsc(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0]);
}

/**
 * This function is used in the sort() call so we can sort DESCENDING the vector containing the lines after the X of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointXDesc(Vec4i v1, Vec4i v2) {
    return (v1.val[0] >= v2.val[0]);
}

/**
 * This function is used in the sort() call so we can sort ASCENDING the vector containing the lines after the Y of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointYAsc(Vec4i v1, Vec4i v2) {
    return (v1.val[1] <= v2.val[1]);
}

/**
 * This function is used in the sort() call so we can sort DESCENDING the vector containing the lines after the Y of the first point
 * @param v1 The first line
 * @param v2 The second line
 * @return returns if the X of the first point is smaller than the second lines first point X
 */
bool comparisonFirstPointYDesc(Vec4i v1, Vec4i v2) {
    return (v1.val[1] >= v2.val[1]);
}

/**
 * This function returns true if P1's X is smaller & Y is greater than P2's X and Y
 * @param v1 Point line 1
 * @param v2 Point line 2
 * @return true if P1's X is smaller & Y is greater than P2's X and Y false otherwise
 */
bool comparisonFirstPointXLowYHigh(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0] && v1.val[1] >= v2.val[1]);
}

/**
 * This function returns true if P1's X & Y is greater than P2's X and Y
 * @param v1 Point line 1
 * @param v2 Point line 2
 * @return true if P1's X is smaller & Y is greater than P2's X and Y false otherwise
 */
bool comparisonFirstPointXHighYHigh(Vec4i v1, Vec4i v2) {
    return (v1.val[0] <= v2.val[0] && v1.val[1] <= v2.val[1]);
}

/**
 * Function used to compare the X and Y coordinates of 2 points to see if they are equal
 * @param v1 A vector containing 4 points of a line, we take only the first point of the line
 * @param v2 A vector containing 4 points of a line, we take only the first point of the line
 * @return true if the X and Y of the points are equal
 */
bool uniquePointComparison(Vec4i v1, Vec4i v2) {
    return v1.val[0] == v2.val[0] && v1.val[1] == v2.val[1];
}

/**
 * This will take each line and compare it with the others and if the coordinates of its P1.X/P1.Y and P2.X/P2.Y are like +/- 15&10 pixels around, they will
 * select the line with the biggest length and override the smaller one so we can easily remove after
 *
 * Bug , some of the values are not erased !!!Strange!!
 *
 * @param src The vectorContaining the lines
 * @param comparisonType Type of comparison TODO:
 */
void orderAndRemoveLines(vector<Vec4i> &lines, int comparisonType) {
    for (size_t i = 0; i < lines.size(); i++) {
        Vec4i l = lines[i];
        double line1Size = norm(Point(l[2], l[3]) - Point(l[0], l[1]));
        for (size_t j = 0; j < lines.size(); j++) {
            Vec4i l2 = lines[j];
            if ((abs(l[0] - l2[0]) <= 15 && abs(l[1] - l2[1]) <= 10) ||
                (abs(l[2] - l2[2]) <= 15 && abs(l[3] - l2[3]) <= 10)) {
                double line2Size = norm(Point(l2[2], l2[3]) - Point(l2[0], l2[1]));
                if (line1Size < line2Size) {
                    lines[i] = lines[j];
                } else {
                    lines[j] = lines[i];
                }
            }
        }
    }
    // This will be used so we can remove the duplicates
    sort(lines.begin(), lines.end(), comparisonFirstPointXAsc);
    lines.erase(unique(lines.begin(), lines.end(), uniquePointComparison), lines.end());
}

/**
 * This method should detect the intersection of 2 lines
 * @param o1
 * @param p1
 * @param o2
 * @param p2
 * @param r
 * @return
 */
// Finds the intersection of two lines, or returns false.
// The lines are defined by (o1, p1) and (o2, p2).
bool intersection(Point2f o1, Point2f p1, Point2f o2, Point2f p2,
                  Point2f &r) {
    Point2f x = o2 - o1;
    Point2f d1 = p1 - o1;
    Point2f d2 = p2 - o2;

    float cross = d1.x * d2.y - d1.y * d2.x;
    if (abs(cross) < /*EPS*/1e-8)
        return false;

    double t1 = (x.x * d2.y - x.y * d2.x) / cross;
    r = o1 + d1 * t1;
    return true;
}

// Returns 1 if the lines intersect, otherwise 0. In addition, if the lines
// intersect the intersection point may be stored in the floats i_x and i_y.
char get_line_intersection(float p0_x, float p0_y, float p1_x, float p1_y,
                           float p2_x, float p2_y, float p3_x, float p3_y, float *i_x, float *i_y) {
    float s1_x, s1_y, s2_x, s2_y;
    s1_x = p1_x - p0_x;
    s1_y = p1_y - p0_y;
    s2_x = p3_x - p2_x;
    s2_y = p3_y - p2_y;

    float s, t;
    s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
    t = (s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1) {
        // Collision detected
        if (i_x != NULL)
            *i_x = p0_x + (t * s1_x);
        if (i_y != NULL)
            *i_y = p0_y + (t * s1_y);
        return 1;
    }

    return 0; // No collision
}

void findSquareCoordinates(vector<Vec4i> &v1, vector<Vec4i> &v2) {
    vector<Vec4i> positiveAngleLines(v1);
    vector<Vec4i> negativeAngleLines(v2);

    //We sorted after the Y value ASCENDING for the / lines
    sort(positiveAngleLines.begin(), positiveAngleLines.end(), comparisonFirstPointYAsc);

    //We sorted after the X value DESCENDING for the \ lines in order to take the first value out (There are others ways but lets stick to this one)
    sort(negativeAngleLines.begin(), negativeAngleLines.end(), comparisonFirstPointXDesc);

    Vec4i fistLinePositive, secondLinePositive, firstLineNegative, secondLineNegative;

    //Get the bottom margin of the table and remove it from the vector
    fistLinePositive = positiveAngleLines[positiveAngleLines.size() - 1];
    positiveAngleLines.pop_back();

    //Get the left margin of the table and remove it from the vector
    firstLineNegative = negativeAngleLines[negativeAngleLines.size() - 1];
    negativeAngleLines.pop_back();

    //We sorted after the Y and X with the highest value ASCENDING for the / lines to get the one before the last line
    sort(positiveAngleLines.begin(), positiveAngleLines.end(), comparisonFirstPointXHighYHigh);

    //We sorted after the Y highest and X lowest value ASCENDING for the \ lines to get the one before the last line
    sort(negativeAngleLines.begin(), negativeAngleLines.end(), comparisonFirstPointXLowYHigh);

    secondLinePositive = positiveAngleLines[positiveAngleLines.size() - 1];
    positiveAngleLines.pop_back();

    secondLineNegative = negativeAngleLines[negativeAngleLines.size() - 1];
    negativeAngleLines.pop_back();

}

/**
 *
 * This function uses probabilistic Hough lines to identify all the lines in the image
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
//        std::sort(linesAngle1.begin(), linesAngle1.end(), comparisonFirstPointXAsc);
//        std::sort(linesAngle2.begin(), linesAngle2.end(), comparisonFirstPointYAsc);
        printf("Lines found with angle 2:%d and with -2 : %d \n", linesAngle1.size(), linesAngle2.size());
    }

    orderAndRemoveLines(linesAngle1, 1);
    orderAndRemoveLines(linesAngle2, 2);

    Mat oneTypeOfLines1, oneTypeOfLines2;
    oneTypeOfLines1 = gr.clone();
    oneTypeOfLines2 = gr.clone();

    for (size_t i = 0; i < linesAngle1.size(); i++) {
        Vec4i l = linesAngle1[i];
        line(oneTypeOfLines1, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        Mat externalMat;
        cvtColor(detected_edges, externalMat, CV_GRAY2BGR);
        line(externalMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
        //Create jpg files with the current line and the coordinates of its extremities
        putTextIntoImage("../images/result/angle2", externalMat, 1, l[0], l[1], i);
        putTextIntoImage("../images/result/angle2", externalMat, 2, l[2], l[3], i);
        printf("%d) Line P(%d,%d) P(%d,%d) slope %lf length %lf \n", i, l[0], l[1], l[2], l[3],
               (l[3] - l[1] / (double) (l[2] - l[0])), norm(Point(l[2], l[3]) - Point(l[0], l[1])));
    }
    if (linesAngle2.size() > 10) {
        Vec4i l6 = linesAngle1[8];
        Vec4i l8 = linesAngle1[6]; // this is reference slope
        float *rx, *ry;
        printf(" Line P(%d,%d) P(%d,%d) slope %lf \n", l6[0], l6[1], l8[3], l8[2],
               (l8[3] - l6[1] / (double) (l8[2] - l6[0])));
//        printf("These lines intersect %b",
//               get_line_intersection(l6[0], l6[1], l6[2], l6[3], l8[0], l8[1], l8[2], l8[3], rx, ry));
    }
    imshow("oneTypeOfLines1", oneTypeOfLines1);

//    for (size_t i = 0; i < linesAngle2.size(); i++) {
//        Vec4i l = linesAngle2[i];
//        line(oneTypeOfLines2, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//        Mat externalMat;
//        cvtColor(detected_edges, externalMat, CV_GRAY2BGR);
//        line(externalMat, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0, 0, 255), 3, CV_AA);
//        //Create jpg files with the current line and the coordinates of its extremities
//        putTextIntoImage("../images/result/anglen2", externalMat, 1, l[0], l[1], i);
//        putTextIntoImage("../images/result/anglen2", externalMat, 2, l[2], l[3], i);
//        printf("%d) Line P(%d,%d) P(%d,%d) slope %lf length %lf \n", i, l[0], l[1], l[2], l[3],
//               (l[3] - l[1] / (double) (l[2] - l[0])), norm(Point(l[2], l[3]) - Point(l[0], l[1])));
//    }
//    imshow("oneTypeOfLines2", oneTypeOfLines2);

//    if (linesAngle1.size() > 0 && linesAngle2.size() > 0) {
//        findSquareCoordinates(linesAngle1, linesAngle2);
//    }

    // THIS WILL PRINT ALL THE LINES ON THE IMAGE
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
    namedWindow("oneTypeOfLines1", CV_WINDOW_AUTOSIZE);
    namedWindow("oneTypeOfLines2", CV_WINDOW_AUTOSIZE);
    namedWindow("detected lines", CV_WINDOW_AUTOSIZE);


    /// Create a Trackbar for user to enter threshold
    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);

    /// Show the image
    CannyThreshold(0, 0);

    /// Wait until user exit program by pressing a key
    waitKey(0);

    return 0;
}