//
// Created by rartin on 20/2/2017.
//

#ifndef EDGEDETECTION_IMAGEPREPARATION_H
#define EDGEDETECTION_IMAGEPREPARATION_H


class imagePreparation {



public:

    static cv::Mat readImage();

    static cv::Mat convertImageGreyscale(cv::Mat &src);

    static cv::Mat blurImage(cv::Mat &src);

    static cv::Mat dilationImage(cv::Mat &src);

    std::vector<cv::Point> getCornerPoints(cv::Mat &src);

    cv::Mat warpImage(cv::Mat &image, std::vector<cv::Point> &points);

    static cv::Mat CannyThreshold(cv::Mat &src);

};


#endif //EDGEDETECTION_IMAGEPREPARATION_H
