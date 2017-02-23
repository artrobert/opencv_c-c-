#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "IPM.h"
#include "imagePreparation.h"
#include "featureDetection.h"
#include "km.h"

using namespace cv;
using namespace std;

void warpImage();
void getCornerList();
void getEdgesTry();
int test();

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
void warpImage(){
    Mat im,imGrey,originalImage;
    originalImage = imagePreparation::readImage("../chess.jpg"); //read the image
    imGrey = imagePreparation::convertImageGreyscale(originalImage); //convert it to greyscale
    im = imagePreparation::blurImage(imGrey); //blur the image
    vector<Point2f> corners=imagePreparation::getCornerPoints(im);
    im=imagePreparation::warpImage(originalImage,corners);

    namedWindow("test", CV_WINDOW_AUTOSIZE);
    imshow("test", im);
}

void getCornerList(){
    Mat im,imGrey,originalImage;
    originalImage = imagePreparation::readImage("../chess.jpg"); //read the image
    imGrey=imagePreparation::convertImageGreyscale(originalImage);
    imGrey=imagePreparation::blurImage(imGrey);
    vector<Point> corners;
    goodFeaturesToTrack(imGrey,corners,200,0.05,15);
    for(int i=0;i<corners.size();i++){
        circle(originalImage,Point(corners[i].x,corners[i].y),3,255,1,8,0);
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
Mat dst, detected_edges,cdst;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 255;
int ratio = 3;
int kernel_size = 3;
char* window_name = "Edge Map";

/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void*)
{
    /// Reduce noise with a kernel 3x3
//    blur( src_gray, detected_edges, Size(3,3) );
    /// Canny detector
    Canny( src, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );
    cvtColor(detected_edges,src_gray,CV_GRAY2BGR);

    /// Using Canny's output as a mask, we display our result
//    dst = Scalar::all(0);
//
//    src.copyTo( dst, detected_edges);
//    detected_edges=imagePreparation::dilationImage(detected_edges,2,2);
//    imshow( window_name, dst );
//    imshow( "Ceva", detected_edges );
//    (dst, cdst, CV_GRAY2BGR);

    vector<Vec4i> lines;
    HoughLinesP(detected_edges, lines, 1, CV_PI/180, 50, 50, 10 );
    for( size_t i = 0; i < lines.size(); i++ )
    {
        Vec4i l = lines[i];
        line( src_gray, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(0,0,255), 3, CV_AA);
    }
    imshow("detected lines", src_gray);
}


/** @function main */
int test()
{
    /// Load an image
    src= imagePreparation::readImage("../images/best_perspective_with_few_pieces_edited.jpg"); //read the image
    Size size(1000,700);
    resize(src,src,size);

    if( !src.data )
    { return -1; }

    /// Create a matrix of the same type and size as src (for dst)
    dst.create( src.size(), src.type() );

    /// Convert the image to grayscale
    cvtColor( src, src_gray, CV_BGR2GRAY );

    /// Create a window
    namedWindow( window_name, CV_WINDOW_AUTOSIZE );
    namedWindow( "Ceva", CV_WINDOW_AUTOSIZE );
    namedWindow( "detected lines", CV_WINDOW_AUTOSIZE );


    /// Create a Trackbar for user to enter threshold
    createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

    /// Show the image
    CannyThreshold(0, 0);

    /// Wait until user exit program by pressing a key
    waitKey(0);

    return 0;
}