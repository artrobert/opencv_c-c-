//
// Created by artin on 05/6/2017.
//

#ifndef EDGEDETECTION_CHESSSQUAREMATRIX_H
#define EDGEDETECTION_CHESSSQUAREMATRIX_H


#include <array>
#include "ChessSquare.h"

class ChessSquareMatrix {
    std::vector<ChessSquare> m_squares; // matrix of chess squares , rows [0,7] ,cols [0,7]
    int matrixSize;

public:

    ChessSquare & getSquare(size_t i, size_t j){
        return m_squares[ i * matrixSize + j];
    }
    ~ChessSquareMatrix(){
        m_squares.clear();
    }
    ChessSquareMatrix(int size){
        matrixSize=size;
        for (int i = 0; i < matrixSize*matrixSize; ++i) {
            m_squares.push_back(ChessSquare());
        }
    }
};


#endif //EDGEDETECTION_CHESSSQUAREMATRIX_H
