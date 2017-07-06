//
// Created by artin on 14/5/2017.
//

#include <opencv2/core/types.hpp>

#ifndef EDGEDETECTION_PIECEIMAGEMODEL_H
#define EDGEDETECTION_PIECEIMAGEMODEL_H

using namespace cv;

/**
 * This class will contain info about:
 *  1) The contour or a piece
 *  2) The layer the contour is from
 *  3) The lowest point of the contour
 */
class PieceContour {

public:
    int layer; // from what layer this piece contour is from
    std::vector<Point> contour; // the contour
    cv::Point lowestPoint; // the lowest point in the contour ( to know where is the base of the piece)

    PieceContour();
    void computeTheLowestPoint();
};


#endif //EDGEDETECTION_PIECEIMAGEMODEL_H
