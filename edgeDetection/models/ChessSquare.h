//
// Created by artin on 02/6/2017.
//

#ifndef EDGEDETECTION_CHESSSQUARE_H
#define EDGEDETECTION_CHESSSQUARE_H


#include <opencv2/core/types.hpp>
#include "PieceModel.h"

using namespace std;

class ChessSquare {
public:
    int index;
    int col;
    int row;

    char indexColRow[2]={};

    // 1 4
    // 2 3
    cv::Point2f topLeft;
    cv::Point2f topRight;
    cv::Point2f bottomLeft;
    cv::Point2f bottomRight;

    // 1 - white 0 - black
    int color;
    bool hasPiece;

    // neighbors
    ChessSquare *leftSquare;
    ChessSquare *rightSquare;
    ChessSquare *topSquare;
    ChessSquare *bottomSquare;

    ChessSquare *topLeftSquare;
    ChessSquare *topRightSquare;
    ChessSquare *bottomLeftSquare;
    ChessSquare *bottomRightSquare;

    bool checkIfContainsPiece(cv::Point2f pieceBasePoint);

    PieceModel *piece;
};


#endif //EDGEDETECTION_CHESSSQUARE_H
