//
// Created by rartin on 22/2/2017.
//

#ifndef EDGEDETECTION_CANNYTRESH_H
#define EDGEDETECTION_CANNYTRESH_H


#include <opencv2/core/mat.hpp>

class cannyTresh {

public:
    static int callCanny(cv::Mat &src);
};


#endif //EDGEDETECTION_CANNYTRESH_H
