//
// Created by artin on 03/5/2017.
//

#ifndef EDGEDETECTION_IMAGEDB_H
#define EDGEDETECTION_IMAGEDB_H


class ImageDB {

public:
    static void loadSampleImages();

    static int matchChessPieces(std::vector<cv::Point> incomingContour);

    static void getContourFromMat(const cv::Mat &mat, std::vector<cv::Point> &c);

    static void createDatabase();
};


#endif //EDGEDETECTION_IMAGEDB_H
