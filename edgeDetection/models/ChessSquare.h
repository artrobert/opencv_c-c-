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

    char indexColRow[2] = {};

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
    ChessSquare *leftSquare = NULL;
    ChessSquare *rightSquare = NULL;
    ChessSquare *topSquare = NULL;
    ChessSquare *bottomSquare = NULL;

    ChessSquare *topLeftSquare = NULL;
    ChessSquare *topRightSquare = NULL;
    ChessSquare *bottomLeftSquare = NULL;
    ChessSquare *bottomRightSquare = NULL;

    bool checkIfContainsPiece(cv::Point2f pieceBasePoint);

    PieceModel *piece;

    void initWithPiece(PieceType type, bool isWhite);

    bool operator<(const ChessSquare &square) const {
        return index < square.index;
    }


    ChessSquare() {
        piece = new PieceModel;
    }

};


#endif //EDGEDETECTION_CHESSSQUARE_H
