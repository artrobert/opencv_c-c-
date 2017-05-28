//
// Created by artin on 14/5/2017.
//

#ifndef EDGEDETECTION_PIECEMODEL_H
#define EDGEDETECTION_PIECEMODEL_H

#include <vector>
#include "PieceType.h"
#include "PieceContour.h"

class PieceModel {
public:
    PieceType pieceType; // the type of the piece
    std::vector<PieceContour> contours; // the contours of the piece (from different angles)
};


#endif //EDGEDETECTION_PIECEMODEL_H
