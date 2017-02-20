#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "IPM.h"
#include "imagePreparation.h"

using namespace cv;
using namespace std;

void warpImage();
void getCornerList();

int main(int argc, char **argv) {

    Mat im,imGrey;

    im = imagePreparation::readImage("../chess.jpg"); //read the image
    namedWindow("original", CV_WINDOW_AUTOSIZE);
    imshow("original", im);

//    imGrey = imagePreparation::convertImageGreyscale(im); //convert it to greyscale
//    im = imagePreparation::blurImage(imGrey); //blur the image
//    im = imagePreparation::CannyThreshold(im); //apply CANNY , the threshold is good at 80
//    im = imagePreparation::dilationImage(im,2,2); //apply dilation , 2,2 are good parameters
//    warpImage();
    getCornerList();
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
