//
// Created by artin on 23/6/2017.
//

#include <opencv2/core/mat.hpp>
#include <queue>
#include <cstdio>
#include <opencv/cv.hpp>
#include "MotionProcessing.h"

using namespace cv;
using namespace std;

bool motionStarted = false;

// Variable indicating how many frames we should wait before taking the ending frame to do the BG SUB
int waitForFrames = -1;

// Flag indicating that we have the frames for the BG SUB process
bool readyToExtractObject = false;

// The number of white pixels needed to consider the motion STARTED
int startMotionPixelsThreshold = 1000;

// The number of white pixel needed to cosinder the motion ENDED
int endMotionPixelsThreshold = 400;

int frameNumbers = 0;

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
void handleMotionEnded(cv::Mat &mogMask, cv::Mat &motionResult) {
    // If the frames we needed to wait in order to not have any hands in the frame passed
    if (waitForFrames == 0) {
        motionResult = mogMask.clone();
        // set the flag in order to extract the object
        readyToExtractObject = true;
        // Decrement one more time to be negative
        waitForFrames--;

        // For debug
//        namedWindow("Motion ended frame", CV_WINDOW_AUTOSIZE);
//        imshow("Motion ended frame", endMotionFrame);
    } else {
        waitForFrames--;
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
        motionStarted = true; // set the motion started FLAG to TRUE
        setLearningAfterStart();
        printf("Motion started!");
    }

    // If there is already motion and the number of white pixels is reaching 0 it means the motion is ENDING
    if (motionStarted && wbPixels[0] <= endMotionPixelsThreshold) {
        //save the Mat at the end of the motion, also used to feed MOG2 to extract the piece
        waitForFrames = 10;
        motionStarted = false; // mark that the motion ended
        printf("Motion ended!");
    }
}

bool MotionProcessing::watchMotion(cv::Mat &frame, cv::Mat &frameMogMask, const char *frameNr, cv::Mat &motionResult) {

    // Check to see if there is motion
    checkMotion(frameMogMask, frameNr);

    // If the motion ended (bcs waitForFrames is >=0)
    if (waitForFrames >= 0) {
        handleMotionEnded(frameMogMask, motionResult);
    }

    frameNumbers++;

    // If the BG SUB components are ready
    if (readyToExtractObject) {
        // Set the flag to false
        readyToExtractObject = false;
        return true;
    }
    return false;
}