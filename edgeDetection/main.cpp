#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "ImageBasicOperations.h"
#include "EdgeDetecting.h"
#include "backgroundSubtract.h"
#include "src/VideoProcessing.h"

using namespace cv;
using namespace std;

void warpImage();

void getCornerList();

int detectEdges();

int main(int argc, char **argv) {

//    warpImage();
//    getCornerList();
//    featureDetection::firstTry();
//    km::kmeanstry();
//    getEdgesTry();
//    detectEdges();
//    backgroundSubtract::startBackgroundSubtract();
    VideoProcessing::watchTheVideo((char *) "D:\\Facultate\\c++Project\\opencv_c-c-\\edgeDetection\\videos\\vid_new_1.mp4");
//    detectEdges();
    waitKey(0);
    return (0);
}

void processTheFrames(){

}

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

int detectEdges() {
    /// Load an image
    Mat src = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
    // Size size(1000,700);
    // resize(src,src,size);


    if (!src.data) { return -1; }
    else {
        ChessSquareMatrix squareMatrix(8);
//        EdgeDetecting::startProcess(src,squareMatrix);
    }
}