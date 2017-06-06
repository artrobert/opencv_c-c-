//
// Created by rartin on 01/3/2017.
//

#ifndef EDGEDETECTION_EDGEDETECTING_H
#define EDGEDETECTION_EDGEDETECTING_H

#include <opencv2/core/matx.hpp>
#include "models/ChessSquareMatrix.h"

class EdgeDetecting {

public:

    static void startProcess(cv::Mat &src,ChessSquareMatrix &squareMatrix);
};


#endif //EDGEDETECTION_EDGEDETECTING_H
