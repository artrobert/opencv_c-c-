
#include <iostream>
#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/features2d.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/calib3d.hpp"
#include "opencv2/xfeatures2d/nonfree.hpp"

#include "ImageBasicOperations.h"
#include "EdgeDetecting.h"
#include "backgroundSubtract.h"
#include "src/VideoProcessing.h"


using namespace cv;
using namespace std;

void warpImage();

void getCornerList();

int detectEdges();

void doStuff()
{
    Mat img_object = imread("../Result_worked.png", CV_LOAD_IMAGE_GRAYSCALE );
    Mat img_scene = imread( "../Result.png", CV_LOAD_IMAGE_GRAYSCALE );
    imshow("ceva",img_scene);

    //-- Step 1: Detect the keypoints and extract descriptors using SURF
    cv::Ptr<cv::xfeatures2d::SURF> siftDetector = cv::xfeatures2d::SurfFeatureDetector::create(400);
    cv::Ptr<cv::BRISK> briskDetector = cv::BRISK::create();


    std::vector<KeyPoint> keypoints_object, keypoints_scene;
    Mat descriptors_object, descriptors_scene;
    siftDetector->detectAndCompute( img_object, Mat(), keypoints_object, descriptors_object );
    siftDetector->detectAndCompute( img_scene, Mat(), keypoints_scene, descriptors_scene );
    //-- Step 2: Matching descriptor vectors using FLANN matcher
    FlannBasedMatcher matcher;
    std::vector< DMatch > matches;
    matcher.match(descriptors_object,descriptors_scene,matches,noArray());
    double max_dist = 0; double min_dist = 100;
    //-- Quick calculation of max and min distances between keypoints
    for( int i = 0; i < descriptors_object.rows; i++ )
    { double dist = matches[i].distance;
        if( dist < min_dist ) min_dist = dist;
        if( dist > max_dist ) max_dist = dist;
    }
    printf("-- Max dist : %f \n", max_dist );
    printf("-- Min dist : %f \n", min_dist );
    //-- Draw only "good" matches (i.e. whose distance is less than 3*min_dist )
    std::vector< DMatch > good_matches;
    for( int i = 0; i < descriptors_object.rows; i++ )
    { if( matches[i].distance <= 3*min_dist )
        { good_matches.push_back( matches[i]); }
    }
    Mat img_matches;
    drawMatches( img_object, keypoints_object, img_scene, keypoints_scene,
                 good_matches, img_matches, Scalar::all(-1), Scalar::all(-1),
                 std::vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS );
    //-- Localize the object
    std::vector<Point2f> obj;
    std::vector<Point2f> scene;
    for( size_t i = 0; i < good_matches.size(); i++ )
    {
        //-- Get the keypoints from the good matches
        obj.push_back( keypoints_object[ good_matches[i].queryIdx ].pt );
        scene.push_back( keypoints_scene[ good_matches[i].trainIdx ].pt );
    }
    Mat H = findHomography( obj, scene, RANSAC );
    //-- Get the corners from the image_1 ( the object to be "detected" )
    std::vector<Point2f> obj_corners(4);
    obj_corners[0] = cvPoint(0,0); obj_corners[1] = cvPoint( img_object.cols, 0 );
    obj_corners[2] = cvPoint( img_object.cols, img_object.rows ); obj_corners[3] = cvPoint( 0, img_object.rows );
    std::vector<Point2f> scene_corners(4);
    perspectiveTransform( obj_corners, scene_corners, H);
    //-- Draw lines between the corners (the mapped object in the scene - image_2 )
    line( img_matches, scene_corners[0] + Point2f( img_object.cols, 0), scene_corners[1] + Point2f( img_object.cols, 0), Scalar(0, 255, 0), 4 );
    line( img_matches, scene_corners[1] + Point2f( img_object.cols, 0), scene_corners[2] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[2] + Point2f( img_object.cols, 0), scene_corners[3] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    line( img_matches, scene_corners[3] + Point2f( img_object.cols, 0), scene_corners[0] + Point2f( img_object.cols, 0), Scalar( 0, 255, 0), 4 );
    //-- Show detected matches
    imshow( "Good Matches & Object detection", img_matches );




//    std::vector<cv::KeyPoint> siftKeypoints;
//    std::vector<cv::KeyPoint> briskKeypoints;
//
//    siftDetector->detect(img_scene, siftKeypoints);
//    briskDetector->detect(img_scene, briskKeypoints);
//
//    std::cout << "Detected " << siftKeypoints.size() << " SIFT keypoints." << std::endl;
//    std::cout << "Detected " << briskKeypoints.size() << " BRISK keypoints." << std::endl;

}




// unghi -1.3 grade
//inaltime 18~19 + 30 + 12

// distanta 16~16 cm


int main(int argc, char **argv) {

//    warpImage();
//    getCornerList();
//    featureDetection::firstTry();
//    km::kmeanstry();
//    getEdgesTry();
//    detectEdges();
//    backgroundSubtract::startBackgroundSubtract();
    VideoProcessing::watchTheVideo((char *) "D:\\Facultate\\c++Project\\opencv_c-c-\\edgeDetection\\videos\\vid_new_2.mp4");
//    doStuff();
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