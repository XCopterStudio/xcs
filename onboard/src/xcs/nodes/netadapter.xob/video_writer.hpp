#ifndef VIDEOWRITER_HPP
#define	VIDEOWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "video_file_writer.hpp"

namespace xcs {
namespace nodes {

class VideoWriter {
    unsigned int frameNumber_;
    AVFrame* avframe_;
    VideoFileWriter* videoFileWriter_;

public:
    VideoWriter(const std::string &videoFile, const std::string &mimetype, const unsigned int &width, const unsigned int &height);
    virtual ~VideoWriter();

    void write(::urbi::UImage image);
};

}
}


#endif	/* VIDEOWRITER_HPP */

