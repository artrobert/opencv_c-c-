#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ImageBasicOperations.h"
#include "EdgeDetecting.h"
#include "backgroundSubtract.h"


using namespace cv;
using namespace std;

void warpImage();

void getCornerList();

void getEdgesTry();

int detectEdges();

Mat src;
Mat src_grayyy;
int thresh = 1;
int max_thresh = 255;
RNG rng(12345);

int main(int argc, char **argv) {

//    Mat im,imGrey;
//
//    im = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
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
//    detectEdges();
    backgroundSubtract::startBackgroundSubtract();
//    cv::waitKey(0); // Wait for a keystroke in the window

    waitKey(0);
    return (0);

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
    originalImage = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
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


/** @function main */
int detectEdges() {
    /// Load an image
    Mat src = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
    // Size size(1000,700);
    // resize(src,src,size);


    if (!src.data) { return -1; }
    else {
        EdgeDetecting::startProcess(src);
    }
}