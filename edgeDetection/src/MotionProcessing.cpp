//
// Created by artin on 23/6/2017.
//

#include <opencv2/core/mat.hpp>
#include <queue>
#include <cstdio>
#include <opencv/cv.hpp>
#include "VideoProcessing.h"
#include "../ImageBasicOperations.h"
#include "MotionProcessing.h"
#include "MogLearningSpeed.h"

using namespace cv;
using namespace std;

int frameQueueSize = 100;

bool motionStarted = false;

// value representing how much of % from the frames from the queue to save for BG SUB
int queuePercentageSave = 50;

std::queue<cv::Mat> queueOfFrames;

// here will be the frames that will be feed to the background subtraction
vector<Mat> startingList;

// Variable indicating how many frames we should wait before taking the ending frame to do the BG SUB
int waitForFrames = -1;

// Here we will save the 'after motion' frame to feed to BG SUB
Mat endMotionFrame;

// Flag indicating that we have the frames for the BG SUB process
bool readyToExtractObject = false;

// The number of white pixels needed to consider the motion STARTED
int startMotionPixelsThreshold = 1000;

// The number of white pixel needed to cosinder the motion ENDED
int endMotionPixelsThreshold = 200;

int frameNumbers = 0;


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
    printf("\n Frame (%s) has White:%d black %d", frameName, count_white, count_black); // print for debug purpose
}

/**
 * If the motion ended, we need to wait some frames to get the 'before motion' frame
 */
void handleMotionEnded() {
    // If the frames we needed to wait in order to not have any hands in the frame passed
    if (waitForFrames == 0) {
        endMotionFrame = queueOfFrames.front();
//            namedWindow("Motion ended frame", CV_WINDOW_AUTOSIZE);
//            imshow("Motion ended frame", endMotionFrame);
        // set the flag in order to extract the object
        readyToExtractObject = true;
        // Decrement one more time to be negative
        waitForFrames--;
    } else {
        waitForFrames--;
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
        startingList.push_back(frameFromQueue);
        queueOfFrames.pop();
    }
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
void checkMotion(cv::Mat &fmogFrame, const char *frameNr) {
    vector<int> wbPixels; // 0 position - white pixels , 1 position - black pixels

    countPixels(fmogFrame, frameNr, wbPixels); //count the number of white and black pixels

    // If there wasn't motion yet and the number of white pixels are growing, it means the motion is STARTING
    if (!motionStarted && wbPixels[0] >= startMotionPixelsThreshold && frameNumbers > 10) {
        handleMotionStarted();
        motionStarted = true; // set the motion started FLAG to TRUE
        setLearningAfterStart();
        printf("Motion started!");
    }

    // If there is already motion and the number of white pixels is reaching 0 it means the motion is ENDING
    if (motionStarted && wbPixels[0] <= endMotionPixelsThreshold) {
        //save the Mat at the end of the motion, also used to feed MOG2 to extract the piece
        waitForFrames = 40;
        motionStarted = false; // mark that the motion ended
        setLearningBeforeStart();
        printf("Motion ended!");
    }
}

/**
 * Uses MOG2 to extract the object that was moved
 * @param extractedObjMat
 */
void feedBackgroundAndGetObject(cv::Mat &extractedObjMat) {
    Ptr<BackgroundSubtractorMOG2> mog2ObjectIdentifier = createBackgroundSubtractorMOG2();
    mog2ObjectIdentifier->setShadowThreshold(200);
//    mog2ObjectIdentifier->setShadowValue(0);


    // Feed the background to the MOG2 object for 20 times
    for (int i = 0; i < startingList.size(); i++) {
        mog2ObjectIdentifier->apply(startingList[i], extractedObjMat, 0.5);
    }


    // TODO we could also store a list of frames after the motion ended, just and ideea
    // Apply the changed background and get the object as a filled contour image
    mog2ObjectIdentifier->apply(endMotionFrame, extractedObjMat, 0.0);
    Mat toShow = extractedObjMat.clone();
    namedWindow("Extract on this image", CV_WINDOW_AUTOSIZE);
    imshow("Extract on this image", extractedObjMat);
    extractedObjMat = imagePreparation::erosionImage(extractedObjMat, 2, 3); // Apply an erosion to remove the noise
}

bool MotionProcessing::watchMotion(Mat &frame, Mat &frameMogMask, const char *frameNr, Mat &motionResult) {

    // Add the frame to the queue
    addToQueueFrame(frame);

    // Check to see if there is motion
    checkMotion(frameMogMask, frameNr);

    // If the motion ended (bcs waitForFrames is >=0)
    if (waitForFrames >= 0) {
        handleMotionEnded();
    }

    frameNumbers++;

    // If the BG SUB components are ready
    if (readyToExtractObject) {
        // Get the object was moved
        feedBackgroundAndGetObject(motionResult);
        // Set the flag to false
        readyToExtractObject = false;
        return true;
    }
    return false;
}