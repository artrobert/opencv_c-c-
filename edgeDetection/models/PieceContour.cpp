//
// Created by artin on 14/5/2017.
//

#include "PieceContour.h"

PieceContour::PieceContour() {
    // Initialize the lowest point as (0,0)
    lowestPoint = cv::Point(0, 0);
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

