//
// Created by artin on 23/6/2017.
//

#include <opencv2/core/mat.hpp>
#include <queue>
#include <cstdio>
#include <opencv/cv.hpp>
#include <set>
#include <iostream>
#include <map>
#include "../ImageBasicOperations.h"
#include "MotionProcessing.h"
#include "ContourDatabase.h"
#include "MoveValidity.h"

using namespace cv;
using namespace std;

int frameQueueSize = 100;

bool motionStarted = false;

// value representing how much of % from the frames from the queue to save for BG SUB
float queuePercentageSave = 50.0;

std::queue<cv::Mat> queueOfFrames;

// here will be the frames that will be feed to the background subtraction
vector<Mat> startingList;

// here will be the frames that will be feed to the bg sub after the motion ended
vector<Mat> endingList;

// Variable indicating how many frames we should wait before taking the ending frame to do the BG SUB
int waitForFrames = -1;

int framesSavedAfterMotionEnded = 40;

// Here we will save the 'after motion' frame to feed to BG SUB
Mat endMotionFrame;

// Flag indicating that we have the frames for the BG SUB process
bool readyToExtractObject = false;

// The number of white pixels needed to consider the motion STARTED
int startMotionPixelsThreshold = 1000;

// The number of white pixel needed to cosinder the motion ENDED
int endMotionPixelsThreshold = 1000;

int frameNumbers = 0;

Mat savedMogMask;

/**
 * Function used to keep a queue of {@link frameQueueSize} frames
 * @param frame The frame that will be placed in the queue
 */
void addToQueueFrame(cv::Mat &frame) {
    if (queueOfFrames.size() == frameQueueSize) {
        queueOfFrames.pop();
    }
    queueOfFrames.push(frame);
}

/**
 * Iterates through the image Mat and counts the white and the black pixels and put them in a vector
 *
 * @param src The Mat of the frame we are counting the pixels
 * @param frameName The name ( count ) of the frame
 * @param wbPixels The returne vector that has on the FIRST position the white pixels and on the SECOND the black ones
 */
void countPixels(cv::Mat &src, const char *frameName, vector<int> &wbPixels) {
    int count_black = 0;
    int count_white = 0;
    for (int y = 0; y < src.rows; y++) {
        for (int x = 0; x < src.cols; x++) {
            if (src.at<uchar>(y, x) != 0) {
                if (src.at<cv::Vec3b>(y, x) == cv::Vec3b(255, 255, 255)) {
                    count_white++;
                } else if (src.at<cv::Vec3b>(y, x) == cv::Vec3b(0, 0, 0)) {
                    count_black++;
                }
            }
        }
    }
    wbPixels.push_back(count_white);
    wbPixels.push_back(count_black);
//    printf("\n Frame (%s) has White:%d black %d", frameName, count_white, count_black); // print for debug purpose
}

/**
 * If the motion ended, we need to wait some frames to get the 'before motion' frame
 */
void handleMotionEnded(cv::Mat &frame, cv::Mat &frameMogMask, vector<int> &wbPixels) {
    bool endListReady = false;
    // If the frames we needed to wait in order to not have any hands in the frame passed
    if (waitForFrames == 0) {
        // Because the mog for motion detection is doing well at detection them, we save the 40 mog mask frame as the
        // binary image from where we will take the contour
        if (framesSavedAfterMotionEnded == 40) {
            savedMogMask = frameMogMask.clone();
        }
        if (framesSavedAfterMotionEnded != 0) {
            Mat frameGray;
//            cv::cvtColor(frame, frameGray, CV_BGR2GRAY);
//            equalizeHist( frameGray, frameGray );
//            GaussianBlur(frameGray, frameGray, Size(3, 3), 0, 0);
            endingList.push_back(frame);
            framesSavedAfterMotionEnded--;
        }
        if (framesSavedAfterMotionEnded == 0 || motionStarted) {
            unsigned int endListSize = endingList.size() - 10; // just to be sure we didnt caught th han
            endingList.resize(endListSize);
            framesSavedAfterMotionEnded--;
            endListReady = true;
        }

        if (endListReady) {
            // set the flag in order to extract the object
            readyToExtractObject = true;
            // Decrement one more time to be negative
            waitForFrames--;
        }
        // For debug
//        namedWindow("Motion ended frame", CV_WINDOW_AUTOSIZE);
//        imshow("Motion ended frame", endMotionFrame);
    } else {
        if (!(wbPixels[0] >= startMotionPixelsThreshold)) {
            waitForFrames--;
        }
    }
}

/**
 * If the motion started, save the 'before motion' frames
 */
void handleMotionStarted() {
    // If the motion started, we save the last n frames from framesQueue ( should be careful if 30 frames in queue,
    // don't take them all because there is a chance there is a hand)
    int maxFramesToSave = queueOfFrames.size() * (queuePercentageSave / 100);
    for (int i = 0; i < maxFramesToSave; i++) {
        Mat frameFromQueue;
        frameFromQueue = queueOfFrames.front();
//        cv::cvtColor(frameFromQueue, frameFromQueue, CV_BGR2GRAY);
//        equalizeHist(frameFromQueue, frameFromQueue);
//        GaussianBlur(frameFromQueue, frameFromQueue, Size(3, 3), 0, 0);
        startingList.push_back(frameFromQueue);
        queueOfFrames.pop();

    }

    // For debug
//    for(int i=0;i<startingList.size();i++){
//        printf("\nSaved frames:%d",i);
//        imshow("Saved frame", startingList.at(i));
//        waitKey(1000);
//    }
}

/**
 * Checks if a movement has started or is ending
 *
 * If there is movement, the number of white pixels in the binary frame will increase
 * If the number of white pixels start to decrease , the moving is ending
 *
 * @param fmogFrame The binary frame that we will be checked for movement
 * @param frameNr The name ( count ) of the frame (for debug)
 */
void checkMotion(cv::Mat frame, cv::Mat &fmogFrame, const char *frameNr) {
    vector<int> wbPixels; // 0 position - white pixels , 1 position - black pixels

    countPixels(fmogFrame, frameNr, wbPixels); //count the number of white and black pixels

    // If there wasn't motion yet and the number of white pixels are growing, it means the motion is STARTING
    if (!motionStarted && wbPixels[0] >= startMotionPixelsThreshold && frameNumbers > 10 && waitForFrames < 0) {
        handleMotionStarted();
        motionStarted = true; // set the motion started FLAG to TRUE
        setLearningAfterStart();
        printf("Motion started!");
    }

    // If there is already motion and the number of white pixels is reaching 0 it means the motion is ENDING
    if (motionStarted && wbPixels[0] <= endMotionPixelsThreshold) {
        //save the Mat at the end of the motion, also used to feed MOG2 to extract the piece
        waitForFrames = 40;
        framesSavedAfterMotionEnded = 40;
        motionStarted = false; // mark that the motion ended
//        setLearningBeforeStart();
        printf("Motion ended!");
    }

    // If the motion ended (bcs waitForFrames is >=0)
    if (waitForFrames >= 0) {
        handleMotionEnded(frame, fmogFrame, wbPixels);
    }
}

/**
 * Uses MOG2 to extract the object that was moved
 *
 * 11/7/17 Currently this not used anymore
 *
 * @param extractedObjMat
 */
Mat feedBackgroundAndGetObject(cv::Mat withPiece, cv::Mat withoutPiece) {
    Ptr<BackgroundSubtractorMOG2> mog2ObjectIdentifier = createBackgroundSubtractorMOG2();
    mog2ObjectIdentifier->setDetectShadows(true);
    mog2ObjectIdentifier->setShadowValue(0);
    mog2ObjectIdentifier->setHistory(50);

    cv::cvtColor(withPiece, withPiece, CV_BGR2GRAY);
    cv::cvtColor(withoutPiece, withoutPiece, CV_BGR2GRAY);
//    GaussianBlur(extractedWithoutPiece, extractedWithoutPiece, Size(3, 3), 0, 0);
//    imshow("without", src);

    Mat mogMask;

    mog2ObjectIdentifier->apply(withoutPiece, mogMask, 0.05);

    mog2ObjectIdentifier->apply(withPiece, mogMask, 0.0001);
    imshow("mog", mogMask);
    return mogMask;
}

Mat extractROI(Mat extractFrom, PieceContour pieceContour) {
    cv::Mat maskRoi = cv::Mat::zeros(extractFrom.size(), extractFrom.type());
    // Mask used to extract a single square, make it red so we can later binarize the image

    Point rook_points[4];

    rook_points[0] = Point(pieceContour.leftestPoint.x, pieceContour.highestPoint.y);
    rook_points[1] = Point(pieceContour.leftestPoint.x, pieceContour.lowestPoint.y);
    rook_points[2] = Point(pieceContour.rightestPoint.x, pieceContour.lowestPoint.y);
    rook_points[3] = Point(pieceContour.rightestPoint.x, pieceContour.highestPoint.y);

    // http://study.marearts.com/2016/07/opencv-drawing-example-line-circle.html
    // Fill the mask Mat with white pixels in order to extract the chess board square
    cv::fillConvexPoly(maskRoi, rook_points, 4, cv::Scalar(255, 255, 255));

//    imshow("before", maskRoi);
    cv::Mat extracted;
    extractFrom.copyTo(extracted, maskRoi);
//    cv::Rect myROI(pieceContour.leftestPoint.x,
//                   pieceContour.highestPoint.y,
//                   pieceContour.rightestPoint.x - pieceContour.leftestPoint.x+10,
//                   pieceContour.lowestPoint.y - pieceContour.highestPoint.y+10);

//    imshow("without", extracted);
    return extracted;
}

struct cmp {
    bool operator()(Point p, Point p2) const {
        return p.x < p2.x || p.y < p2.y;
    }

};

int decideMovedToSquare(Mat frame, ChessSquare op, ChessSquare mtp, PieceContour movedPieceContour,
                        PieceContour pastPieceContour, ChessSquareMatrix squareMatrix) {

    vector<ChessSquare> movedToSquares;

    Point middleOfThePiece = Point((movedPieceContour.lowestPoint.x + movedPieceContour.highestPoint.x) / 2,
                                   (movedPieceContour.lowestPoint.y + movedPieceContour.highestPoint.y) / 2);

    Point secondMiddle = Point((movedPieceContour.lowestPoint.x + middleOfThePiece.x) / 2,
                               (movedPieceContour.lowestPoint.y + middleOfThePiece.y) / 2);

    movedToSquares.push_back(squareMatrix.searchSquareAfterValues(movedPieceContour.lowestPoint));
    movedToSquares.push_back(squareMatrix.searchSquareAfterValues(middleOfThePiece));
    movedToSquares.push_back(squareMatrix.searchSquareAfterValues(secondMiddle));
    movedToSquares.push_back(squareMatrix.searchSquareAfterValues(movedPieceContour.highestPoint));

    for (size_t i = 0; i < movedToSquares.size(); i++) {
        ChessSquare currentValue = *std::next(movedToSquares.begin(), i);
        cout << "\nCan it be in this square :" << currentValue.index
             << " from here " << op.index << " "
             << MoveValidity::checkMove(op, currentValue);
    }

    cv::circle(frame, movedPieceContour.lowestPoint, 5, cv::Scalar(0, 0, 255), 2);
    cv::circle(frame, middleOfThePiece, 5, cv::Scalar(0, 0, 255), 2);
    cv::circle(frame, movedPieceContour.highestPoint, 5, cv::Scalar(0, 0, 255), 2);
    imshow("test", frame);

    Mat extractedRoiWithoutPiece = extractROI(startingList[0], movedPieceContour);
    Mat extractedRoiWithPiece = extractROI(endingList[endingList.size() - 1], movedPieceContour);

    Mat bgSubtractedPiece = feedBackgroundAndGetObject(extractedRoiWithPiece, extractedRoiWithoutPiece);

    PieceContour pieceFromMog;
    MotionProcessing::getContourFromMat(bgSubtractedPiece, pieceFromMog, pieceFromMog, false, false);
    ChessSquare backgroundSquare = squareMatrix.searchSquareAfterValues(pieceFromMog.lowestPoint);
    cout << "\nLowest point from mog " << backgroundSquare.index;

    Mat cannyWithPiece, cannyWithoutPiece;
    int lowThreshold = 100;

    Canny(extractedRoiWithPiece, cannyWithPiece, lowThreshold, lowThreshold * 3, 3);
//    cannyWithPiece = imagePreparation::dilationImage(cannyWithPiece, 2, 2);
    imshow("CannyWith", cannyWithPiece);

    Canny(extractedRoiWithoutPiece, cannyWithoutPiece, lowThreshold, lowThreshold * 3, 3);
//    cannyWithoutPiece = imagePreparation::dilationImage(cannyWithoutPiece, 2, 2);
    imshow("CannyWithout", cannyWithoutPiece);

    Mat diff;

    bitwise_xor(cannyWithPiece, cannyWithoutPiece, diff);
    diff = imagePreparation::dilationImage(diff, 2, 1);

//    diff = imagePreparation::erosionImage(diff, 2, 2);


//    diff = imagePreparation::erosionImage(diff, 2, 1);
    imshow("Dif", diff);

    PieceContour pieceFromCanny;
    MotionProcessing::getContourFromMat(diff, pieceFromCanny, pieceFromCanny, false, true);
    ChessSquare cannySquare = squareMatrix.searchSquareAfterValues(pieceFromCanny.lowestPoint);
    cout << "\nLowest point from canny " << cannySquare.index;

}

bool MotionProcessing::watchMotion(cv::Mat frame, cv::Mat frameMogMask, const char *frameNr,
                                   ChessSquareMatrix &squareMatrix) {

    // Add the frame to the queue
    addToQueueFrame(frame);

    // Check to see if there is motion
    checkMotion(frame, frameMogMask, frameNr);


    frameNumbers++;


    // If the BG SUB components are ready
    if (readyToExtractObject) {
        // Get the object was moved
//        feedBackgroundAndGetObject(motionResult);
        PieceContour movedPieceContour;
        PieceContour pastPieceContour;


        // Get the biggest 2 contours
        getContourFromMat(savedMogMask, movedPieceContour, pastPieceContour, true, false);

        cv::Mat cloneMog = frame.clone();

        cv::Scalar colorCircle1(0, 0, 255);
        cv::circle(cloneMog, movedPieceContour.lowestPoint, 5, colorCircle1, 2);
        cv::circle(cloneMog, pastPieceContour.lowestPoint, 5, colorCircle1, 2);
        imshow("clone Mog", cloneMog);


        // Get the squares of those two contours
        ChessSquare previousPlace = squareMatrix.searchSquareAfterValues(pastPieceContour.lowestPoint);
        ChessSquare currentPlace = squareMatrix.searchSquareAfterValues(movedPieceContour.lowestPoint);

        // For debug
        printf("\nSquare first %d\n", previousPlace.index);
        printf("\nSquare first %d\n", currentPlace.index);

        // Change positions to keep consistency of the past and present positions
        // If the previousSquare object didn't had a piece then it represents the current position of the piece
        if (!previousPlace.hasPiece) {
            ChessSquare auxSquare = currentPlace;
            currentPlace = previousPlace;
            previousPlace = auxSquare;

            PieceContour auxContour = movedPieceContour;
            movedPieceContour = pastPieceContour;
            pastPieceContour = movedPieceContour;

        }

        decideMovedToSquare(frame, previousPlace, currentPlace, movedPieceContour, pastPieceContour, squareMatrix);

        // Set the flag to false
        readyToExtractObject = false;
        return true;
    }
    return false;
}

void MotionProcessing::getContourFromMat(const Mat &mat, PieceContour &contour1, PieceContour &contour2, bool both,
                                         bool isCannyDid) {
    cdb::ContourDatabase contourDatabase; //used to calculate the layer of origin for the piece
    vector<vector<Point>> fullContour;

    Mat canny_output, matInput = mat.clone();

    if (!isCannyDid) {

        // We do a canny in order to get the contours of all the scene objects


        // We do the following operations in order to eliminate noise, but we will also remove the details
        matInput = imagePreparation::erosionImage(matInput, 2, 1);
        matInput = imagePreparation::dilationImage(matInput, 2, 2);

        imshow("inp", matInput);


        Canny(matInput.clone(), canny_output, 255, 255, 3);

    } else {
        canny_output = mat.clone();
    }

    findContours(canny_output, fullContour, CV_RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

//    TODO THIS IS USED TO PRINT THE Contour
    vector<Vec4i> hierarchy;
    namedWindow("Contours", CV_WINDOW_AUTOSIZE);
    Mat drawing = Mat::zeros(matInput.size(), CV_8UC3);
//    for (int i = 0; i < fullContour.size(); i++) {
//        Scalar color = Scalar(RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255), RNG(12345).uniform(0, 255));
//        drawContours(drawing, fullContour, i, color, 2, 8, hierarchy, 0, Point());
////        printf("\n%d / %d", i, fullContour.size());
//        imshow("Contours", drawing);
//        waitKey(500);
//    }

    int bigContourSize = fullContour.size();

    std::map<double, vector<Point>, std::greater<double>> m1; // map in decreasing order

    for (size_t i = 0; i < bigContourSize; i++) {
        double currentContourArea = arcLength(fullContour[i], true);
        m1[currentContourArea] = fullContour[i];
    }


    contour1.contour = m1.begin()->second;
//    cout << "\nSelected contour " << contour1.contour->;
    contour1.computeTheLowestPoint();
    contour1.computeTheHightestPoint();
    contour1.computeTheLeftestPoint();
    contour1.computeTheRightestPoint();
//    contour1.layer = contourDatabase.findOriginLayer(contour1.lowestPoint);

    if (both) {
        m1.erase(m1.begin());
        contour2.contour = m1.begin()->second;
        contour2.computeTheLowestPoint();
        contour2.computeTheHightestPoint();
        contour2.computeTheLeftestPoint();
        contour2.computeTheRightestPoint();
//        contour2.layer = contourDatabase.findOriginLayer(contour2.lowestPoint);
    }

//    int radiusCircle = 5;
//    cv::Scalar colorCircle1(0, 0, 255);
//    int thicknessCircle1 = 2;

//    cv::circle(drawing, contour1.lowestPoint, radiusCircle, colorCircle1, thicknessCircle1);
//    cv::circle(drawing, contour2.lowestPoint, radiusCircle, colorCircle1, thicknessCircle1);
//    imshow("dra", drawing);

}