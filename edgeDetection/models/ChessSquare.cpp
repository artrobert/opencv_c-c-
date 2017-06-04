//
// Created by artin on 02/6/2017.
//

#include "ChessSquare.h"

bool ChessSquare::checkIfContainsPiece(cv::Point2f pieceBasePoint) {
    return pieceBasePoint.x > topLeft.x && pieceBasePoint.x > bottomLeft.x // in the RIGHT of the left edge
           && pieceBasePoint.x < topRight.x && pieceBasePoint.x < bottomRight.x // in the left of the right edge
           && pieceBasePoint.y > topLeft.y && pieceBasePoint.y > topRight.y // below the top edge
           && pieceBasePoint.y < bottomLeft.y && pieceBasePoint.y < bottomRight.y; // above the bottom edge
}
