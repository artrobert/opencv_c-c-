//
// Created by artin on 14/5/2017.
//

#include "PieceContour.h"

PieceContour::PieceContour() {
    lowestPoint = cv::Point(0, 0);
    highestPoint = cv::Point(INT_MAX, INT_MAX);
    rightestPoint = cv::Point(0, 0);
    leftestPoint = cv::Point(INT_MAX, INT_MAX);
}

void PieceContour::computeTheLowestPoint() {
    int sizeList = contour.size();
    if (sizeList != 0) {
        for (int i = 0; i < sizeList; i++) {
            if (lowestPoint.y < contour[i].y) {
                lowestPoint = contour[i];
            }
        }
    }
}

void PieceContour::computeTheHightestPoint() {
    int sizeList = contour.size();
    if (sizeList != 0) {
        for (int i = 0; i < sizeList; i++) {
            if (highestPoint.y > contour[i].y) {
                highestPoint = contour[i];
            }
        }
    }
}

void PieceContour::computeTheLeftestPoint() {
    int sizeList = contour.size();
    if (sizeList != 0) {
        for (int i = 0; i < sizeList; i++) {
            if (leftestPoint.x > contour[i].x) {
                leftestPoint = contour[i];
            }
        }
    }
}

void PieceContour::computeTheRightestPoint() {
    int sizeList = contour.size();
    if (sizeList != 0) {
        for (int i = 0; i < sizeList; i++) {
            if (rightestPoint.x < contour[i].x) {
                rightestPoint = contour[i];
            }
        }
    }
}
