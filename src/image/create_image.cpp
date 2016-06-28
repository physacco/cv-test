// Copyright: This program is released into the public domain.

#include <stdio.h>
#include <vector>
#include <stdexcept>
#include <opencv2/opencv.hpp>

#define clamp(x) cv::saturate_cast<uchar>(x)

uchar lerp(int i, int range, uchar max) {
    float ratio = static_cast<float>(i) / static_cast<float>(range);
    return static_cast<uchar>(ratio * max);
}

void fill_image(cv::Mat* img) {
    cv::Mat& mat = *img;
    for (int i = 0; i < mat.rows; ++i) {
        for (int j = 0; j < mat.cols; ++j) {
            cv::Vec4b& rgba = mat.at<cv::Vec4b>(i, j);
            rgba[0] = UCHAR_MAX;  // blue
            rgba[1] = clamp(lerp(i, mat.rows - 1, UCHAR_MAX));  // green
            rgba[2] = clamp(lerp(j, mat.cols - 1, UCHAR_MAX));  // red
            rgba[3] = clamp(0.5 * (rgba[1] + rgba[2]));  // alpha
        }
    }
}

int main(int argv, char** argc) {
    const char* filename = "output.png";

    // Create a 640x480 image with alpha channel
    cv::Mat img(480, 640, CV_8UC4);  // initialized as transparent black
    fill_image(&img);

    std::vector<int> compression_params;
    compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
    compression_params.push_back(9);

    try {
        cv::imwrite(filename, img, compression_params);
    } catch (std::runtime_error& ex) {
        fprintf(stderr, "Exception converting image to PNG format: %s\n",
                ex.what());
        return 1;
    }

    fprintf(stdout, "Image saved to %s.\n", filename);
    return 0;
}
