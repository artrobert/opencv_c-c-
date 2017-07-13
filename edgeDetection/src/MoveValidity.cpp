//
// Created by artin on 12/7/2017.
//

#include "MoveValidity.h"

bool MoveValidity::checkMove(ChessSquare op, ChessSquare mtp) {
    switch (op.piece->pieceType) {
        case PieceType::pion:
            return checkPawnValidMove(op, mtp, op.piece->isWhite);
        case PieceType::tura:
            return checkRookValidMove(op, mtp);
        case PieceType::cal:
            return checkKnightValidMove(op, mtp);
        case PieceType::nebun:
            return checkBishopValidMove(op, mtp);
        case PieceType::regina:
            return checkQueenValidMove(op, mtp);
        case PieceType::rege:
            return checkKingValidMove(op, mtp);
        default:
            return false;
    }
}

bool MoveValidity::checkPawnValidMove(ChessSquare op, ChessSquare mtp, bool isWhite) {
    int rowMoved = 0;
    if (isWhite) {
        // if white, he can go +1/+2 squares because the 0,0 square start from the white side
        rowMoved = mtp.row - op.row;
    } else {
        // if black, he can go -1/-2 square because black are on row 7/8
        // so we switch the positions to get a positive number
        rowMoved = op.row - mtp.row;
    }

    // moved 1 or 2 squares
    if (op.col == mtp.col) {
        if (rowMoved == 1 || rowMoved == 2) {
            return true;
        }
    } else if ((op.col + 1 == mtp.col || op.col - 1 == mtp.col) && mtp.hasPiece) {
        // if is the attacking he will go left/right with 1 square and top with 1 square
        if (rowMoved == 1) {
            return true;
        }
    }
    return false;
}

bool MoveValidity::checkKnightValidMove(ChessSquare op, ChessSquare mtp) {
    int rowMoved = op.row - mtp.row;

    int colMoved = op.col - mtp.col; // neg values will tell us if it was left, poz values will tell right

    // move long i=i +/- 2  j=j +/- 1 OR move short i=i +/- 1  j=j +/- 2
    return (abs(rowMoved) == 2 && abs(colMoved) == 1) || (abs(rowMoved) == 1 && abs(colMoved) == 2);
}

bool MoveValidity::checkRookValidMove(ChessSquare op, ChessSquare mtp) {
    return (op.col == mtp.col && op.row != mtp.row) || (op.row == mtp.row && op.col != mtp.col);
}

bool MoveValidity::checkBishopValidMove(ChessSquare op, ChessSquare mtp) {
    int rowMoved = op.row - mtp.row;
    int colMoved = op.col - mtp.col;

    return abs(rowMoved) == abs(colMoved); // if the number of rows equals the number of cols (diagonal movement)
}

bool MoveValidity::checkQueenValidMove(ChessSquare op, ChessSquare mtp) {
    return checkKnightValidMove(op, mtp) || checkBishopValidMove(op, mtp);
}

bool MoveValidity::checkKingValidMove(ChessSquare op, ChessSquare mtp) {
    int rowMoved = op.row - mtp.row;
    int colMoved = op.col - mtp.col;

    return (abs(rowMoved) == 0 || abs(rowMoved == 1)) && (abs(colMoved) == 0 || abs(colMoved) == 1);
}