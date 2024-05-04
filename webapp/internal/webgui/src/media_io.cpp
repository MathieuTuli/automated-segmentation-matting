#include "media_io.hpp"

#include <vector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

// extern "C" {
// #include <libavformat/avformat.h>
// #include <libavcodec/avcodec.h>
// #include <libavutil/avutil.h>
// #include <libavutil/pixdesc.h>
// #include <libswscale/swscale.h>
// }


void load_buffer(const std::string_view &buffer)
{
    std::vector<uchar> data(buffer.begin(), buffer.end());
    // cv::Mat img = imdecode(data, cv::IMREAD_COLOR);

    // cvtColor(img, img, cv::COLOR_BGR2GRAY);
    // imshow( "Frame", img);
}

void load_file(const char* filename)
{
    //cv::threshold(src_gray, dst, threshold_value, max_binary_value, threshold_type );
    std::cout << filename << std::endl;
    // if image 
    // else if video
}

void read_image(const char* filename) 
{
    cv::imread(filename);
}

void read_video(const char* filename)
{
    cv::VideoCapture cap(filename);
}
