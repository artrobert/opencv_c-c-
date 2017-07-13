//
// Created by artin on 05/6/2017.
//

#include "ChessSquareMatrix.h"

void ChessSquareMatrix::initBasicModel() {
    for (int j = 0; j < 8; j++) {
        for (int i = 0; i < 2; i++) {
            getSquare(i, j).piece->isWhite = true;
            getSquare(i, j).hasPiece = true;
            if (i == 0) {
                getSquare(i, j).piece->type = whiteFirstRowPieces[j];
            } else {
                getSquare(i, j).piece->type = whiteSecondRowPieces[0];
                getSquare(i, j).piece->pieceType = PieceType::pion;
            }
        }

        // Set the free spaces
        for (int i = 2; i < 6; i++) {
            getSquare(i, j).hasPiece = false;
            getSquare(i, j).piece->type = noPiece;
            getSquare(i, j).piece->pieceType = PieceType::freeSpace;
        }


        for (int i = 6; i < 8; i++) {
            getSquare(i, j).hasPiece = true;
            getSquare(i, j).piece->isWhite = false;
            if (i == 6) {
                getSquare(i, j).piece->type = blackSecondRowPieces[0];
                getSquare(i, j).piece->pieceType = PieceType::pion;
            } else {
                getSquare(i, j).piece->type = blackFirstRowPieces[j];
            }
        }
    }

    //white
    getSquare(0, 0).piece->pieceType = PieceType::tura;
    getSquare(0, 7).piece->pieceType = PieceType::tura;

    getSquare(0, 1).piece->pieceType = PieceType::cal;
    getSquare(0, 6).piece->pieceType = PieceType::cal;

    getSquare(0, 2).piece->pieceType = PieceType::nebun;
    getSquare(0, 5).piece->pieceType = PieceType::nebun;

    getSquare(0, 3).piece->pieceType = PieceType::regina;
    getSquare(0, 4).piece->pieceType = PieceType::rege;

    //blacks
    getSquare(7, 0).piece->pieceType = PieceType::tura;
    getSquare(7, 7).piece->pieceType = PieceType::tura;

    getSquare(7, 1).piece->pieceType = PieceType::cal;
    getSquare(7, 6).piece->pieceType = PieceType::cal;

    getSquare(7, 2).piece->pieceType = PieceType::nebun;
    getSquare(7, 5).piece->pieceType = PieceType::nebun;

    getSquare(7, 3).piece->pieceType = PieceType::regina;
    getSquare(7, 4).piece->pieceType = PieceType::rege;
}

ChessSquare ChessSquareMatrix::searchSquareAfterValues(Point basePoint) {
    printf("Piece coordinates: %d %d", basePoint.x, basePoint.y);
    for (int i = 0; i < 8; i++) {
        for (int j = 0; j < 8; j++) {
            ChessSquare square = getSquare(i, j);
            if (square.checkIfContainsPiece(basePoint)) {
                return square;
            }
        }
    }
}
