//
// Created by artin on 03/5/2017.
//

#ifndef EDGEDETECTION_IMAGEDB_H
#define EDGEDETECTION_IMAGEDB_H


#include "models/PieceContour.h"
#include "models/PieceType.h"

class ImageDB {

public:

    static const int maxLayers = 4;

    static const CvSize imageResizeSize;

    static const int imageResizeHeight = 800;
    static const int imageResizeWidth = 600;

    // The closest to the camera and the most harder to find
    static const int layer0 = imageResizeHeight / maxLayers * 1;
    static const int layer1 = imageResizeHeight / maxLayers * 2;
    static const int layer2 = imageResizeHeight / maxLayers * 3;
    static const int layer3 = imageResizeHeight / maxLayers * 4;


    static void loadSampleImages();

    static void createDatabase();

    static PieceContour getContourFromMat(const cv::Mat &mat);

    static PieceType matchChessPieces(PieceContour pieceIncoming);
};


#endif //EDGEDETECTION_IMAGEDB_H
