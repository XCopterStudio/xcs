#ifndef VIDEOWRITER_HPP
#define	VIDEOWRITER_HPP

#include <string>
#include <urbi/uobject.hh>

#include "../writer_common.hpp"
#include "video_file_writer.hpp"
#include "abstract_writer.hpp"


namespace xcs {
namespace nodes {
namespace datalogger {

class VideoWriter : public AbstractWriter {
    unsigned int frameNumber_;
    AVFrame* avframe_;
    std::unique_ptr<VideoFileWriter> videoFileWriter;

public:
    VideoWriter(const std::string &name);
    virtual ~VideoWriter();
    void init(const std::string &videoFile, const unsigned int &width, const unsigned int &height, const std::string &dataName, const TimePoint startTime, std::ofstream* file, std::mutex *lock, ::urbi::UVar &uvar);
    void write(::urbi::UImage image);
};

};
}
}


#endif	/* VIDEOWRITER_HPP */
