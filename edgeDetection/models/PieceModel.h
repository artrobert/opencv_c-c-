//
// Created by artin on 14/5/2017.
//

#ifndef EDGEDETECTION_PIECEMODEL_H
#define EDGEDETECTION_PIECEMODEL_H

#include <vector>
#include "PieceType.h"
#include "PieceContour.h"

/**
 * This class will contain info about the PIECE TYPE and POSSIBLE CONTOURS
 */
class PieceModel {
public:
    char type;
    PieceType pieceType; // the type of the piece
    std::vector<PieceContour> contours; // the contours of the piece (from different angles)
    bool isWhite; // the color of the piece
};


#endif //EDGEDETECTION_PIECEMODEL_H
