#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <opencv2/imgproc.hpp>
#include <ctime>

#include "IPM.h"

using namespace cv;
using namespace std;

//Global variables

void CannyThreshold(int, void *);

//The image SRC and the SRC_GREY
Mat src, src_gray;

Mat src_blurred;


Mat dst, detected_edges;

int lowThreshold = 80;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
const char *window_name = "Edge Map";

//There are for dilatation
void Dilation(int, void *);

Mat dilation_dst, dilation_dst_show;

int dilation_elem, dilation_size = 0;
int const max_elem = 2;
int const max_kernel_size = 21;
const char *window_name_dilation = "Dilation result";

//Warp the image
void Warp();

Mat inputImgGray;

//Corner dection to help with the warp
void cornerHarris_demo(int, void *);

int Treshhold_corner_detection = 200;
int Max_Treshhold_corner_detection = 255;

const char *source_corner_detection_window = "Source corner detection window";
const char *corners_window = "Corners detected";

void dostuff();

int readImage(){
    //Declare the path of the image we will be working on
    String imageName("../chess.jpg");
    // Read the file
    src = imread(imageName);
    // Check for invalid input
    if (!src.data) {
        std::cout << "Could not open or find the frame" << std::endl;
        return -1;
    }
}

void convertImage(){
    //Create a matrix of the same type and size as src (for dst)
    dst.create(src.size(), src.type());
    //Convert the image to greyscale
    cvtColor(src, src_gray, CV_BGR2GRAY);
}

void blurTheGrayscale(){
    blur(src_gray, src_blurred, Size(3, 3));
}

void applyCannyFilter(){



}

void getCornerPoints(Mat canny_output){
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    //using this thresh we will get get only the table outline
    int thresh = 255;
    /// Detect edges using canny
    Canny( src_gray,canny_output , thresh, thresh*2, 3 );

    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Point> co=contours[0];

    vector<Point> corners; // the vector of corners to help to tell the warp what region to warp
    corners.push_back(co[0]);//first corner

    int x=co[0].x;
    int y=co[0].y;
    for(int i=1;i<co.size();i++){
        if(co[i].x>co[i-1].x && co[i].y > co[i-1].y && corners.size()==1){ //x'>x y'>y
            corners.push_back(co[i]); //second corner
        }else if(co[i].x>co[i-1].x && co[i].y < co[i-1].y && corners.size()==2){ //x'>x y'<y
            corners.push_back(co[i]); //second third
        }else if(co[i].x<co[i-1].x && co[i].y < co[i-1].y && corners.size()==3){ //x'<x y'<y
            corners.push_back(co[i]); //second third
        }
    }

    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 1; i< 2; i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, (int)i, color, 1, 8, hierarchy, 0, Point() );
    }
}

int main(int argc, char **argv) {

    readImage();
    convertImage();
    blurTheGrayscale();
    applyCannyFilter();



    cv::waitKey(0); // Wait for a keystroke in the window
    return 0;
}

void oldMain(){
    //Create a window for CANNY
//    namedWindow(window_name, CV_WINDOW_AUTOSIZE);

//    namedWindow(window_name_dilation, CV_WINDOW_AUTOSIZE);
//    namedWindow(source_corner_detection_window,CV_WINDOW_AUTOSIZE);
    //Create a Trackbar for user to enter CANNY threshold
//    createTrackbar("Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold);
    /// Create Dilation Trackbar
//    createTrackbar("Element:\n 0: Rect \n 1: Cross \n 2: Ellipse", window_name_dilation, &dilation_elem, max_elem, Dilation);
//    createTrackbar("Kernel size:\n 2n +1", window_name_dilation, &dilation_size, max_kernel_size, Dilation);
    //createTrackbar( "Threshold: ", source_corner_detection_window, &Treshhold_corner_detection, Max_Treshhold_corner_detection, cornerHarris_demo );
    //imshow( source_corner_detection_window, src );

    //Show the image
//    CannyThreshold(0, 0);
    //Dilation(0, 0);
    //cornerHarris_demo( 0, 0 );
    //findCorners();
    dostuff();
    //Wait until user exit program by pressing a key
}


/**
 * @function CannyThreshold
 * @brief Trackbar callback - Canny thresholds input with a ratio 1:3
 */
void CannyThreshold(int, void *) {
    /// Reduce noise with a kernel 3x3

    /// Canny detector
    Canny(detected_edges, detected_edges, lowThreshold, lowThreshold * ratio, kernel_size);

    /// Using Canny's output as a mask, we display our result
    dst = Scalar::all(0);

    src.copyTo(dst, detected_edges);
    dst.copyTo(dilation_dst);

    imshow(window_name, dst);
}

/** @function Dilation */
void Dilation(int, void *) {
    int dilation_type;
    if (dilation_elem == 0) { dilation_type = MORPH_RECT; }
    else if (dilation_elem == 1) { dilation_type = MORPH_CROSS; }
    else if (dilation_elem == 2) { dilation_type = MORPH_ELLIPSE; }

    Mat element = getStructuringElement(dilation_type,
                                        Size(2 * dilation_size + 1, 2 * dilation_size + 1),
                                        Point(dilation_size, dilation_size));
    /// Apply the dilation operation
    dilate(dilation_dst, dilation_dst_show, element);
    imshow(window_name_dilation, dilation_dst_show);
}

//void Warp() {
//    //get the width and height of the src image
//    int width = 0, height = 0;
//    width = src.cols;
//    height = src.rows;
//    IPM ipm(Size(src., height), Size(width, height), origPoints, dstPoints);
//
//    if (src.channels() == 3)
//        cvtColor(src, inputImgGray, CV_BGR2GRAY);
//    else
//        src.copyTo(inputImgGray);
//
//    // Process
//    clock_t begin = clock();
//    ipm.applyHomography(inputImg, outputImg);
//    clock_t end = clock();
//    double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC;
//    printf("%.2f (ms)\r", 1000 * elapsed_secs);
//    ipm.drawPoints(origPoints, inputImg);
//
//    // View
//    imshow("Input", inputImg);
//    imshow("Output", outputImg);
//}

/** @function cornerHarris_demo */
void cornerHarris_demo(int, void *) {

    Mat dst, dst_norm, dst_norm_scaled, diluated_grey;
    //dst = Mat::zeros( src.size(), CV_32FC1 );

    /// Detector parameters
    int blockSize = 2;
    int apertureSize = 3;
    double k = 0.04;

    cv::cvtColor(dilation_dst, diluated_grey, CV_BGR2GRAY);

    /// Detecting corners
    cornerHarris(diluated_grey, dst, blockSize, apertureSize, k, BORDER_DEFAULT);

    /// Normalizing
    normalize(dst, dst_norm, 0, 255, NORM_MINMAX, CV_32FC1, Mat());
    convertScaleAbs(dst_norm, dst_norm_scaled);

    /// Drawing a circle around corners
    for (int j = 0; j < dst_norm.rows; j++) {
        for (int i = 0; i < dst_norm.cols; i++) {
            if ((int) dst_norm.at<float>(j, i) > Treshhold_corner_detection) {
                circle(dst_norm_scaled, Point(i, j), 5, Scalar(0), 2, 8, 0);
            }
        }
    }
    /// Showing the result
    imshow(source_corner_detection_window, dst_norm_scaled);
}

int thresh = 255;
int max_thresh = 255;
RNG rng(12345);

/// Function header
void thresh_callback(int, void* );

void dostuff(){

    /// Convert image to gray and blur it
    cvtColor( src, src_gray, COLOR_BGR2GRAY );
    blur( src_gray, src_gray, Size(3,3) );

    /// Create Window
    const char* source_window = "Source";
    namedWindow( source_window, WINDOW_AUTOSIZE );
    imshow( source_window, src );

    createTrackbar( " Canny thresh:", "Source", &thresh, max_thresh, thresh_callback );
    thresh_callback( 0, 0 );

}

/**
 * @function thresh_callback
 */
void thresh_callback(int, void* )
{
    Mat canny_output;
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;

    /// Detect edges using canny
    Canny( src_gray,canny_output , thresh, thresh*2, 3 );

    namedWindow( "canny", WINDOW_AUTOSIZE );
    imshow( "canny", canny_output );
    /// Find contours
    findContours( canny_output, contours, hierarchy, CV_RETR_TREE, CHAIN_APPROX_SIMPLE, Point(0, 0) );

    vector<Point> co=contours[0];
    for(int i=0;i<co.size();i++){
        printf("x:%d y:%d \n",co[i].x,co[i].y);
    }

    /// Draw contours
    Mat drawing = Mat::zeros( canny_output.size(), CV_8UC3 );
    for( size_t i = 1; i< 2; i++ )
    {
        Scalar color = Scalar( rng.uniform(0, 255), rng.uniform(0,255), rng.uniform(0,255) );
        drawContours( drawing, contours, (int)i, color, 1, 8, hierarchy, 0, Point() );
    }

    /// Show in a window
    namedWindow( "Contours", WINDOW_AUTOSIZE );
    imshow( "Contours", drawing );
}
