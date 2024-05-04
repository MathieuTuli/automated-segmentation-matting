#ifndef MEDIA_IO_H
#define MEDIA_IO_H
#include <string_view>

void load_file(const char * filename);
void load_buffer(const std::string_view &buffer);
void read_image(const char* filename);
void read_video(const char * filename);

#endif
