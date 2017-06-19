//
// Created by artin on 18/6/2017.
//

#ifndef EDGEDETECTION_CHESSTABLEEDGES_H
#define EDGEDETECTION_CHESSTABLEEDGES_H

#include <opencv/cv.h>

using namespace cv;
using namespace std;

class ChessTableEdges {

public:
    // This holds the lines that have a positive angle and are presumed 'vertical'
    vector<Vec4i> verticalPositiveAngleLines;

    // This holds the lines that have a negative angle and are presumed 'horizontal'
    vector<Vec4i> horizontalNegativeAngleLines;

    // This represents the left margin edge
    Vec4i *verticalLeft=NULL;

    // This represents the right margin edge
    Vec4i *verticalRight=NULL;

    // This represents the low margin edge
    Vec4i *horizontalLow=NULL;

    // This represents the high margin edge
    Vec4i *horizontalHigh=NULL;

};


#endif //EDGEDETECTION_CHESSTABLEEDGES_H
