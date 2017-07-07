//
// Created by artin on 13/6/2017.
//

#ifndef EDGEDETECTION_EDGEPROCESSING_H
#define EDGEDETECTION_EDGEPROCESSING_H


#include <opencv2/core/mat.hpp>
#include "../models/ChessSquareMatrix.h"
#include "../models/ChessTableEdges.h"

class EdgeProcessing {

public :
    static void startProcess(cv::Mat &src, ChessSquareMatrix &squareMatrix,bool virtualizeWithPieces);
};


#endif //EDGEDETECTION_EDGEPROCESSING_H
