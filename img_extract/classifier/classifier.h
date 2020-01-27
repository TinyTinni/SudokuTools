#pragma once

#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>

class classifier
{
    cv::dnn::Net m_net;
public:
    classifier();
    struct output
    {
        int number;
        double confidence;
    };

    output classify(const cv::Mat& img);
};