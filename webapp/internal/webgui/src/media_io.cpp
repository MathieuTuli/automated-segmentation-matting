#include "media_io.hpp"

#include <vector>

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>

extern "C" {
#include <libavformat/avformat.h>
#include <libavcodec/avcodec.h>
#include <libavutil/avutil.h>
#include <libavutil/pixdesc.h>
#include <libswscale/swscale.h>
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
    // AVFormatContext* inctx = nullptr;
    // int ret;
    // ret = avformat_open_input(&inctx, filename.c_str(), nullptr, nullptr);
    // if (ret < 0) {
    //     std::cerr << "fail to avforamt_open_input(\"" << filename << "\"): ret=" << ret;
    // }
    // // retrive input stream information
    // ret = avformat_find_stream_info(inctx, nullptr);
    // if (ret < 0) {
    //     std::cerr << "fail to avformat_find_stream_info: ret=" << ret;
    // }

    // // find primary video stream
    // const AVCodec* vcodec = nullptr;
    // ret = av_find_best_stream(inctx, AVMEDIA_TYPE_VIDEO, -1, -1, &vcodec, 0);
    // if (ret < 0) {
    //     std::cerr << "fail to av_find_best_stream: ret=" << ret;
    // }
    // const int vstrm_idx = ret;
    // AVStream* vstrm = inctx->streams[vstrm_idx];

    // // open video decoder context
    // AVCodecContext *ctx = avcodec_alloc_context3(vcodec);
    // ret = avcodec_open2(ctx, vcodec, nullptr);
    // if (ret < 0) {
    //     std::cerr << "fail to avcodec_open2: ret=" << ret;
    // }

    // // print input video stream informataion
    // std::cout
    //     << "filename: " << filename << "\n"
    //     << "format: " << inctx->iformat->name << "\n"
    //     << "vcodec: " << vcodec->name << "\n"
    //     << "size:   " << ctx->width << 'x' << ctx->height << "\n"
    //     << "fps:    " << av_q2d(ctx->framerate) << " [fps]\n"
    //     << "length: " << av_rescale_q(vstrm->duration, vstrm->time_base, {1,1000}) / 1000. << " [sec]\n"
    //     << "pixfmt: " << av_get_pix_fmt_name(ctx->pix_fmt) << "\n"
    //     << "frame:  " << vstrm->nb_frames << "\n"
    //     << std::flush;

    // // initialize sample scaler
    // const int dst_width = ctx->width;
    // const int dst_height = ctx->height;
    // const AVPixelFormat dst_pix_fmt = AV_PIX_FMT_BGR24;
    // SwsContext* swsctx = sws_getCachedContext(
    //     nullptr, ctx->width, ctx->height, ctx->pix_fmt,
    //     dst_width, dst_height, dst_pix_fmt, SWS_BICUBIC, nullptr, nullptr, nullptr);
    // if (!swsctx) {
    //     std::cerr << "fail to sws_getCachedContext";
    // }
    // std::cout << "output: " << dst_width << 'x' << dst_height << ',' << av_get_pix_fmt_name(dst_pix_fmt) << std::endl;

    // // allocate frame buffer for output
    // AVFrame* frame = av_frame_alloc();
    // std::vector<uint8_t> framebuf(avpicture_get_size(dst_pix_fmt, dst_width, dst_height));
    // avpicture_fill(reinterpret_cast<AVPicture*>(frame), framebuf.data(), dst_pix_fmt, dst_width, dst_height);

    // // decoding loop
    // AVFrame* decframe = av_frame_alloc();
    // unsigned nb_frames = 0;
    // bool end_of_stream = false;
    // int got_pic = 0;
    // AVPacket pkt;
    // do {
    //     if (!end_of_stream) {
    //         // read packet from input file
    //         ret = av_read_frame(inctx, &pkt);
    //         if (ret < 0 && ret != AVERROR_EOF) {
    //             std::cerr << "fail to av_read_frame: ret=" << ret;
    //         }
    //         if (ret == 0 && pkt.stream_index != vstrm_idx)
    //             goto next_packet;
    //         end_of_stream = (ret == AVERROR_EOF);
    //     }
    //     if (end_of_stream) {
    //         // null packet for bumping process
    //         // av_init_packet(&pkt);
    //         pkt.data = nullptr;
    //         pkt.size = 0;
    //     }
    //     // decode video frame
    //     avcodec_decode_video2(ctx, decframe, &got_pic, &pkt);
    //     if (!got_pic)
    //         goto next_packet;
    //     // convert frame to OpenCV matrix
    //     sws_scale(swsctx, decframe->data, decframe->linesize, 0, decframe->height, frame->data, frame->linesize);
    //     {
    //     cv::Mat image(dst_height, dst_width, CV_8UC3, framebuf.data(), frame->linesize[0]);
    //     cv::imshow("press ESC to exit", image);
    //     if (cv::waitKey(1) == 0x1b)
    //         break;
    //     }
    //     std::cout << nb_frames << '\r' << std::flush;  // dump progress
    //     ++nb_frames;
    //     next_packet;
    //     // av_free_packet(&pkt);
    // } while (!end_of_stream || got_pic);
    // std::cout << nb_frames << " frames decoded" << std::endl;

    // av_frame_free(&decframe);
    // av_frame_free(&frame);
    // // avcodec_close(ctx);
    // avformat_close_input(&inctx);
}
