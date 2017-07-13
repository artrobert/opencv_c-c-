//
// Created by artin on 12/7/2017.
//

#ifndef EDGEDETECTION_MOVEVALIDITY_H
#define EDGEDETECTION_MOVEVALIDITY_H


#include "../models/ChessSquare.h"

class MoveValidity {

public:

    static bool checkPawnValidMove(ChessSquare op, ChessSquare mtp, bool isWhite);

    static bool checkKnightValidMove(ChessSquare op, ChessSquare mtp);

    static bool checkRookValidMove(ChessSquare op, ChessSquare mtp);

    static bool checkBishopValidMove(ChessSquare op, ChessSquare mtp);

    static bool checkQueenValidMove(ChessSquare op, ChessSquare mtp);

    static bool checkKingValidMove(ChessSquare op, ChessSquare mtp);

    static bool checkMove(ChessSquare op, ChessSquare mtp);
};


#endif //EDGEDETECTION_MOVEVALIDITY_H
