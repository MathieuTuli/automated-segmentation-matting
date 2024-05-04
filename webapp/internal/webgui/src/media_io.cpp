#define STB_IMAGE_IMPLEMENTATION
#include "media_io.hpp"

#include <vector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

#include "third_party/stb/stb_image.h"

// extern "C" {
// #include <libavformat/avformat.h>
// #include <libavcodec/avcodec.h>
// #include <libavutil/avutil.h>
// #include <libavutil/pixdesc.h>
// #include <libswscale/swscale.h>
// }


bool load_file(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
    //cv::threshold(src_gray, dst, threshold_value, max_binary_value, threshold_type );
    std::cout << filename << std::endl;
    // if image 
    // else if video
    // bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
    // Load from file
    int image_width = 0;
    int image_height = 0;
    unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
    if (image_data == NULL)
        return false;

    // Create a OpenGL texture identifier
    GLuint image_texture;
    glGenTextures(1, &image_texture);
    glBindTexture(GL_TEXTURE_2D, image_texture);

    // Setup filtering parameters for display
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // This is required on WebGL for non power-of-two textures
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE); // Same

    // Upload pixels into texture
#if defined(GL_UNPACK_ROW_LENGTH) && !defined(__EMSCRIPTEN__)
    glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
    stbi_image_free(image_data);

    *out_texture = image_texture;
    *out_width = image_width;
    *out_height = image_height;

    std::cout << "done" << std::endl;
    return true;
}


bool load_buffer(const std::string_view &buffer, GLuint* out_texture, int* out_width, int* out_height)
{
    std::vector<uchar> data(buffer.begin(), buffer.end());
    // cv::Mat img = imdecode(data, cv::IMREAD_COLOR);
    return false;
}

void read_image(const char* filename) 
{
    cv::imread(filename);
}

void read_video(const char* filename)
{
    cv::VideoCapture cap(filename);
}
