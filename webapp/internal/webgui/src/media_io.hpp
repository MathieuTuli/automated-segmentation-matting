#ifndef MEDIA_IO_H
#define MEDIA_IO_H
#include <string_view>
#include <GLFW/glfw3.h>

bool load_file(const char * filename, GLuint* out_texture, int* out_width, int* out_height);
bool load_buffer(const std::string_view &buffer, GLuint* out_texture, int* out_width, int* out_height);
void read_image(const char* filename);
void read_video(const char * filename);

#endif
