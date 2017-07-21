//
// Created by artin on 05/6/2017.
//

#ifndef EDGEDETECTION_CHESSSQUAREMATRIX_H
#define EDGEDETECTION_CHESSSQUAREMATRIX_H


#include <array>
#include "ChessSquare.h"
#include "ChessTableEdges.h"

class ChessSquareMatrix {

private:

    std::vector<ChessSquare> m_squares; // matrix of chess squares , rows [0,7] ,cols [0,7]
    int matrixSize;

    const char whiteFirstRowPieces[8] = {'R', 'K', 'B', 'Q', 'K', 'B', 'K', 'R'};
    const char whiteSecondRowPieces[1] = {'P'};

    const char blackFirstRowPieces[8] = {'r', 'k', 'b', 'q', 'k', 'b', 'k', 'r'};
    const char blackSecondRowPieces[1] = {'p'};

    const char noPiece = '-';


public:

    ChessTableEdges chessTableEdges;

    ChessSquare &getSquare(size_t i, size_t j) {
        return m_squares[i * matrixSize + j];
    }

    ~ChessSquareMatrix() {
        m_squares.clear();
    }

    ChessSquareMatrix(int size) {
        matrixSize = size;
        for (int i = 0; i < matrixSize * matrixSize; ++i) {
            m_squares.push_back(ChessSquare());
        }
        initBasicModel();
    }

    void initBasicModel();

    ChessSquare searchSquareAfterValues(Point basePoint);

};


#endif //EDGEDETECTION_CHESSSQUAREMATRIX_H
