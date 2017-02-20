//
// Created by rartin on 20/2/2017.
//

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <ctime>

#include "IPM.h"
#include "imagePreparation.h"

using namespace cv;
using namespace std;


/**
 * Global variables
 */
//The image SRC and the SRC_GREY
Mat imgReadSrc, imgGreySrc, imgBlurredSrc, imgBlurredCannySrc;


/**
 * Function used to read the static image and save the MAT in imageReadSrc
 * @return The error code.
 */
cv::Mat imagePreparation::readImage(String image_name) {
    //Declare the path of the image we will be working on
    // Read the file
    imgReadSrc = imread(image_name);
    // Check for invalid input
    if (!imgReadSrc.data) {
        std::cout << "Could not open or find the frame" << std::endl;
        return Mat(1, 1, CV_64F, 0.0);
    } else {
        return imgReadSrc;
    }
}

/**
 * Function used to convert the image in a GREY SCALE image and save it in imageGreySrc
 *
 * @param src The Mat we want to convert to grey scale
 * @return The grey scale Mat
 */
cv::Mat imagePreparation::convertImageGreyscale(cv::Mat &src) {
    Mat resultDst;
    //Convert the image to greyscale
    cvtColor(src, resultDst, CV_BGR2GRAY);
    return resultDst;
}

/**
 * Function used to blur de image
 *
 * The src image should be GREYSCALE
 *
 * @param src The Mat we want to blur
 * @return The blurred Mat
 */
cv::Mat imagePreparation::blurImage(cv::Mat &src) {
    Mat resultDst;
    blur(src, resultDst, Size(3, 3));
    return resultDst;
}

/**
* Function used to apply a dilation to the image
 *
 * Useful if the lines are not fully connected and we need them to be (e.g. after a CANNY filter)
 *
 *  2 , 2 are good parameters
 *
 *
 * @param src The Mat we want to apply the dilation
 * @return The Mat that the dilation was applied
 */
cv::Mat imagePreparation::dilationImage(cv::Mat &src,int dilation_elem,int dilation_size) {
    Mat resultDst;

    int dilation_type;
    if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
    else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
    else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

    Mat element = getStructuringElement(dilation_type,
                                        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                        Point(dilation_size, dilation_size));
    /// Apply the dilation operation
    dilate(src, resultDst, element);
    return resultDst;
}

/**
 * This function will apply a CANNY filter on the blurred image, will find all the corners using the "findContours()" and the extract only the 4 corners (minX,maxX,minY,maxY)
 *
 * @param src The image we want to find the corners. Should be blurred
 * @return The vectors with the corner points
 */
vector<Point2f> imagePreparation::getCornerPoints(cv::Mat &src) {
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //using this thresh we will get get only the table outline
    int thresh = 255;
    // Detect edges using canny
    Mat imgBlurredCannyDst;
    Canny(src, imgBlurredCannyDst, thresh, thresh * 2, 3);
    //Find all the corners
    findContours(imgBlurredCannyDst, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0));

    // findContours() function returns arrays of points and must extract the extremity corners
    vector<Point> co = contours[0];

    // The vector of extremity corners to call the warp function
    vector<Point2f> corners;

    // The first value is the first corner so we save it (min Y & minX or maxX)
    corners.push_back(co[0]);

    // We go through all the points and search for the rest of the corners
    // TODO: needs testing
    for (int i = 1; i < co.size(); i++) {
        if (co[i].x > co[i - 1].x && co[i].y > co[i - 1].y && corners.size() == 1) { //x'>x y'>y
            corners.push_back(co[i]); //second corner
        } else if (co[i].x > co[i - 1].x && co[i].y < co[i - 1].y && corners.size() == 2) { //x'>x y'<y
            corners.push_back(co[i]); //second third
        } else if (co[i].x < co[i - 1].x && co[i].y < co[i - 1].y && corners.size() == 3) { //x'<x y'<y
            corners.push_back(co[i]); //second third
        }
    }

    // Draw contours that the findContours() found
    Mat drawing = Mat::zeros(imgBlurredCannySrc.size(), CV_8UC3);
    RNG rng(12345);
    for (size_t i = 1; i < 2; i++) {
        Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        drawContours(drawing, contours, (int) i, color, 1, 8, hierarchy, 0, Point());
    }

    return corners;
}

/**
 * This function will warp the image, we will need to provide the corners of the region we want to warp
 *
 * @param image The image we apply the warp
 * @param points The corners of the region we will extract and warp
 * @return The warped image
 */
cv::Mat imagePreparation::warpImage(cv::Mat &image, vector<Point2f> &points) {
//    //get the width and height of the src image
    int width = 0, height = 0;
    width = image.cols;
    height = image.rows;

    //Set the points

    // The 4-points at the input image
    vector<Point2f> origPoints;
//    origPoints.push_back( Point2f(0, height) );
//    origPoints.push_back( Point2f(width, height) );
//    origPoints.push_back( Point2f(width/2+30, 140) );
//    origPoints.push_back( Point2f(width/2-50, 140) );

    // The 4-points correspondences in the destination image
    vector<Point2f> dstPoints;
    dstPoints.push_back(Point2f(0, height));
    dstPoints.push_back(Point2f(width, height));
    dstPoints.push_back(Point2f(width, 0));
    dstPoints.push_back(Point2f(0, 0));
    IPM ipm(Size(width, height), Size(width, height), points, dstPoints);

//    if (imgReadSrc.channels() == 3)
//        cvtColor(src, inputImgGray, CV_BGR2GRAY);
//    else
//        src.copyTo(inputImgGray);

    // Process
    Mat imgHomographyDst;
    clock_t begin = clock();
    ipm.applyHomography(image, imgHomographyDst);
    clock_t end = clock();
    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
    printf("%.2f (ms)\r", 1000 * elapsed_secs);

    return imgHomographyDst;

//    ipm.drawPoints(origPoints, sr);
//
//    // View
//    imshow("Input", inputImg);
//    imshow("Output", outputImg);
}

/**
 * This function applies a CANNY filter and the result image will have only lines that form the image
 * @param src The image we want to apply CANNY
 * @return Canny image
 */
cv::Mat imagePreparation::CannyThreshold(cv::Mat &src) {
    Mat resultDst;
    int lowThreshold = 80; //this is hardcoded because give the best result , TODO: maybe change it as a parameter
    int ratio = 3;
    int kernel_size = 3;
    /// Reduce noise with a kernel 3x3
    /// Canny detector
    Canny(src, resultDst, lowThreshold, lowThreshold * ratio, kernel_size);
    return resultDst;
}
