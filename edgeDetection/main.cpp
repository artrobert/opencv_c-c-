#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "IPM.h"
#include "ImageBasicOperations.h"
#include "EdgeDetecting.h"


using namespace cv;
using namespace std;

void warpImage();

void getCornerList();

void getEdgesTry();

int detectEdges();

//int main(int argc, char **argv) {
//
////    Mat im,imGrey;
////
////    im = imagePreparation::readImage("../images/manual_board_pieces_ed.jpg"); //read the image
////    namedWindow("original", CV_WINDOW_AUTOSIZE);
////    imshow("original", im);
//
////    imGrey = imagePreparation::convertImageGreyscale(im); //convert it to greyscale
////    im = imagePreparation::blurImage(imGrey); //blur the image
////    im = imagePreparation::CannyThreshold(im); //apply CANNY , the threshold is good at 80
////    im = imagePreparation::dilationImage(im,2,2); //apply dilation , 2,2 are good parameters
////    warpImage();
////    getCornerList();
////    featureDetection::firstTry();
////    km::kmeanstry();
////    getEdgesTry();
//    detectEdges();
//    cv::waitKey(0); // Wait for a keystroke in the window
//    return 0;
//}

int useEqualize = 0;
int th1, blursSize;
int saturation = 0;
int scale = 1;
const std::string winName = "Tuna fish";
cv::Mat src, dst;
cv::Mat brightness;

void onTunaFishTrackbar(int, void *) {
    cv::Mat hist, histImg, tmp;
    brightness.copyTo(tmp);

    if (saturation >= 2) {
        tmp.convertTo(tmp, CV_8UC1, scale, saturation);
    }
    cv::imshow("Result Fish Saturation", tmp);

    if (blursSize >= 3) {
        blursSize += (1 - blursSize % 2);
        cv::GaussianBlur(tmp, tmp, cv::Size(blursSize, blursSize), 0);
    }
    if (useEqualize)
        cv::equalizeHist(tmp, tmp);

    cv::imshow("Brightness Preprocess", tmp);
    imwrite("../img/BrightnessPreprocess.png", tmp);

    // threshold to select dark tuna
    cv::threshold(tmp, tmp, th1, 255, cv::THRESH_BINARY_INV);
    cv::imshow(winName, tmp);

    // find external contours ignores holes in the fish
    vector<vector<cv::Point> > contours;
    vector<cv::Vec4i> hierarchy;
    cv::findContours(tmp, contours, hierarchy, RETR_TREE, CV_CHAIN_APPROX_SIMPLE);

    // draw all contours and select the largest
    src.copyTo(dst);
    double maxDim = 0;
    int largest = -1;
    for (int i = 0; i < contours.size(); i++) {
        // draw all contours in red
        cv::drawContours(dst, contours, largest, cv::Scalar(0, 0, 255), 1);
        int dim = contours[i].size(); //area is more accurate but more expensive
        //double dim = contourArea(contours[i]);
        //double dim = cvRound(arcLength(contours[i], true));
        if (dim > maxDim) {
            maxDim = dim;
            largest = i;
        }
    }

    //The tuna as binary mask
    cv::Mat fishMask = cv::Mat::zeros(src.size(), CV_8UC1);
    //The tuna as contour
    vector<cv::Point> theFish;
    if (largest >= 0) {
        theFish = contours[largest];
        // draw selected contour in bold green
        cv::polylines(dst, theFish, true, cv::Scalar(0, 255, 0), 2);
        // draw the fish into its mask
        cv::drawContours(fishMask, contours, largest, 255, -1);
    }
    cv::imshow("Result Fish Mask", fishMask);
    cv::imshow("Result Contour", dst);
    cv::imwrite("../img/FishMask.png", fishMask);
    cv::imwrite("../img/Result.png", dst);
}

int main(int argc, char *argv[]) {
    src = cv::imread("../images/shape_black_on_black.jpg");
    Size size(500, 500);
    resize(src, src, size);

//    Mat saturated;

//    double saturation = 100;
///    double scale = 1;

// what it does here is dst = (uchar) ((double)src*scale+saturation);
//    src.convertTo(saturated, CV_8UC1, scale, saturation);

//    cv::imshow("Result", saturated);

    if (src.empty()) {
        cout << endl
             << "ERROR! Unable to read the image" << endl
             << "Press a key to terminate";
        cin.get();
        return 0;

    }
    imshow(winName, src);
    imshow("Src", src);

    cvtColor(src, dst, CV_BGR2HSV);
    vector<cv::Mat> hsv_planes;
    split(dst, hsv_planes);
    //hue = hsv_planes[0];
    //saturation = hsv_planes[1];
    brightness = hsv_planes[2];

    // default settings for params
    useEqualize = 1;
    blursSize = 21;
    th1 = 33.0 * 255 / 100; //tuna is dark than select dark zone below 33% of full range
    cv::createTrackbar("Equalize", winName, &useEqualize, 1, onTunaFishTrackbar, 0);
    cv::createTrackbar("Saturation", winName, &saturation, 255, onTunaFishTrackbar, 0);
    cv::createTrackbar("Saturation Scale", winName, &scale, 255, onTunaFishTrackbar, 0);
    cv::createTrackbar("Blur Sigma", winName, &blursSize, 100, onTunaFishTrackbar, 0);
    cv::createTrackbar("Threshold", winName, &th1, 255, onTunaFishTrackbar, 0);

    onTunaFishTrackbar(0, 0);

    cv::waitKey(0);
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