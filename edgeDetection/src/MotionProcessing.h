//
// Created by artin on 23/6/2017.
//

#ifndef EDGEDETECTION_MOTIONPROCESSING_H
#define EDGEDETECTION_MOTIONPROCESSING_H

#include "../models/PieceContour.h"
#include "../models/ChessSquareMatrix.h"

extern void setLearningBeforeStart();
extern void setLearningAfterStart();

class MotionProcessing {

public:

    static void getContourFromMat(const Mat &mat, PieceContour &contour1, PieceContour &contour2);

    static bool watchMotion(Mat frame, Mat frameMogMask, const char *frameNr, ChessSquareMatrix &squareMatrix);
};


#endif //EDGEDETECTION_MOTIONPROCESSING_H
