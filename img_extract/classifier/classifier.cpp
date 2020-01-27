#include "classifier.h"

classifier::classifier()
{
    m_net = cv::dnn::readNet(RES_PATH"mnist_cnn.onnx");
}

classifier::output classifier::classify(const cv::Mat& img)
{
    cv::Mat blob;
    cv::dnn::blobFromImage(img, blob);
    m_net.setInput(blob);
    cv::Mat prob = m_net.forward();


    cv::Point classIdPoint;
    output o;
    cv::minMaxLoc(prob.reshape(1, 1), 0, &o.confidence, 0, &classIdPoint);
    o.number = classIdPoint.x;

    return o;
}