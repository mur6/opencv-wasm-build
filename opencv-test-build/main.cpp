#include <iostream>
#include <opencv2/core/core.hpp>

int main(int argc, char *argv[])
{
    // 64F, channels=10, 3x3 の2次元配列（行列）
    cv::Mat m1(3, 3, CV_64FC(10));
    cv::Mat m2(3, 3, CV_MAKETYPE(CV_64F, 10));

    CV_Assert(m1.type() == m2.type());
    std::cout << "mat1/mat2" << std::endl;
    std::cout << "  dims: " << m1.dims << ", depth(byte/channel):" << m1.elemSize1()
              << ", channels: " << m1.channels() << std::endl;

    // 64F, channles=1, 2x2x3x4 の4次元配列
    const int sizes[] = {2, 2, 3, 4};
    cv::Mat m3(sizeof(sizes) / sizeof(int), sizes, CV_64FC1);

    CV_Assert(m3.dims == sizeof(sizes) / sizeof(sizes[0]));
    CV_Assert(m3.rows == -1); // 3次元以上のMatでは，
    CV_Assert(m3.cols == -1); // 常にrows==cols==-1

    std::cout << "mat3" << std::endl;
    std::cout << "  dims: " << m3.dims << ", depth(byte/channel):" << m3.elemSize1()
              << ", channels: " << m3.channels() << std::endl;
}
