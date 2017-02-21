#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/features2d.hpp>
#include <vector>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include<math.h>

#include "IPM.h"
#include "imagePreparation.h"

using namespace cv;
using namespace std;

#include "featureDetection.h"


const float inlier_threshold = 2.5f; // Distance threshold to identify inliers
const float nn_match_ratio = 0.8f;   // Nearest neighbor matching ratio

void featureDetection::detectFeatureORB(){
    Mat im1=imagePreparation::readImage("../chess_pie.jpg"); //read the image
    im1=imagePreparation::convertImageGreyscale(im1);
    Mat im2=imagePreparation::readImage("../chess_king.jpg"); //read the image
    im2=imagePreparation::convertImageGreyscale(im2);

    vector<KeyPoint> kpts1,kpts2;
    Mat desc1,desc2;
    Ptr<AKAZE> akaze=AKAZE::create();
    akaze->detectAndCompute(im1,noArray(),kpts1,desc1);
    akaze->detectAndCompute(im2,noArray(),kpts2,desc2);

    BFMatcher matcher(NORM_HAMMING);
    vector<vector<DMatch>> nn_matches;

    Mat homography;
    FileStorage fs("../H1to3p.xml", FileStorage::READ);
    fs.getFirstTopLevelNode() >> homography;

    matcher.knnMatch(desc1,desc2,nn_matches,2);

    vector<KeyPoint> matched1,matched2,inliers1,inliers2;
    vector<DMatch> good_matches;

    for(size_t i = 0; i < nn_matches.size(); i++) {
        DMatch first = nn_matches[i][0];
        float dist1 = nn_matches[i][0].distance;
        float dist2 = nn_matches[i][1].distance;

        if(dist1 < nn_match_ratio * dist2) {
            matched1.push_back(kpts1[first.queryIdx]);
            matched2.push_back(kpts2[first.trainIdx]);
        }
    }

    for(unsigned i = 0; i < matched1.size(); i++) {
        Mat col = Mat::ones(3, 1, CV_64F);
        col.at<double>(0) = matched1[i].pt.x;
        col.at<double>(1) = matched1[i].pt.y;

        col = homography * col;
        col /= col.at<double>(2);
        double dist = sqrt( pow((float)(col.at<double>(0) - matched2[i].pt.x), (float)2) +
                            pow((float)(col.at<double>(1) - matched2[i].pt.y), (float)2));

        if(dist < inlier_threshold) {
            int new_i = static_cast<int>(inliers1.size());
            inliers1.push_back(matched1[i]);
            inliers2.push_back(matched2[i]);
            good_matches.push_back(DMatch(new_i, new_i, 0));
        }
    }

    Mat res;
    drawMatches(im1, inliers1, im2, inliers2, good_matches, res);
    namedWindow("chess_king", CV_WINDOW_AUTOSIZE);
    imshow("chess_king", res);
//    imwrite("res.png", res);


}

void featureDetection::firstTry(){
    Mat img1=imagePreparation::readImage("../chess_pie.jpg"); //read the image
    std::vector<KeyPoint> kpts1;
    Ptr<FeatureDetector> detector = ORB::create();
    Mat desc1,result1;

    detector->detectAndCompute(img1,noArray(), kpts1,desc1);

    drawKeypoints(img1,kpts1,result1,Scalar::all(200));
    namedWindow("chess_full", CV_WINDOW_AUTOSIZE);
    imshow("chess_full", result1);

    Mat img2=imagePreparation::readImage("../chess_king.jpg"); //read the image
    std::vector<KeyPoint> kpts2;
    Ptr<FeatureDetector> detector_king = ORB::create();

    Mat desc2,result2;
    detector_king->detectAndCompute(img2,noArray(), kpts2,desc2);

    drawKeypoints(img2,kpts2,result2,Scalar::all(200));
    namedWindow("chess_king", CV_WINDOW_AUTOSIZE);
    imshow("chess_king", result2);

    
    BFMatcher matcher(NORM_L2, true);
//    matcher.match();
}