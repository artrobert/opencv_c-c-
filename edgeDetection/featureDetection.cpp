#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>

#include "imagePreparation.h"

using namespace cv;
using namespace std;

#include "featureDetection.h"

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

   BFMatcher matcher(NORM_L2,false);
    vector<DMatch> matches;
    matcher.match(desc2,desc1,matches,noArray());
    Mat m;
    drawMatches(img2,kpts2,img1,kpts1,matches,m,Scalar::all(200),Scalar::all(100));
    namedWindow("res", CV_WINDOW_AUTOSIZE);
    imshow("res", m);

}