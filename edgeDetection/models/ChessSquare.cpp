//
// Created by artin on 02/6/2017.
//

#include <opencv/cv.hpp>
#include "ChessSquare.h"

bool ChessSquare::checkIfContainsPiece(cv::Point2f pieceBasePoint) {
    Mat src = Mat::zeros(Size(800, 600), CV_8UC1);
    vector<Point2f> vert(4);
    vert[0] = topLeft;
    vert[1] = bottomLeft;
    vert[2] = bottomRight;
    vert[3] = topRight;
    line(src, vert[0], vert[1], Scalar(255), 3, 8);
    line(src, vert[1], vert[2], Scalar(255), 3, 8);
    line(src, vert[2], vert[3], Scalar(255), 3, 8);
    line(src, vert[3], vert[0], Scalar(255), 3, 8);
//    namedWindow("ifinside", CV_WINDOW_AUTOSIZE);
//    imshow("ifinside", src);
//    waitKey(500);
    vector<vector<Point> > contours;
    vector<Vec4i> hierarchy;
    findContours(src, contours, hierarchy, RETR_TREE, CHAIN_APPROX_SIMPLE);
    double isInside = pointPolygonTest(contours[0], pieceBasePoint, false);

    return isInside == 1 || isInside == 0;
}


void ChessSquare::initWithPiece(PieceType type, bool isWhite) {
    piece = new PieceModel;
    piece->pieceType = type;
    piece->isWhite = isWhite;
}