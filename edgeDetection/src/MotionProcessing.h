//
// Created by artin on 23/6/2017.
//

#ifndef EDGEDETECTION_MOTIONPROCESSING_H
#define EDGEDETECTION_MOTIONPROCESSING_H

extern void setLearningBeforeStart();
extern void setLearningAfterStart();

class MotionProcessing {

public:
    static bool watchMotion(cv::Mat &frame, cv::Mat &frameMogMask, const char *frameNr,cv::Mat &motionResult);
};


#endif //EDGEDETECTION_MOTIONPROCESSING_H
