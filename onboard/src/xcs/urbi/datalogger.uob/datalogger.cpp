#include "datalogger.hpp"

#include <sstream>

#include <boost/bind.hpp>

using namespace std;
using namespace std::chrono;
using namespace ::urbi;
using namespace xcs::nodes;

const char* Datalogger::REGISTER = "register";

// =================================== public functions ====================

DataWriter::DataWriter(const std::string &name) :
XObject(name)
{
}

void DataWriter::init(const std::string &dataName, const TimePoint startTime, std::ofstream* file, ::urbi::UVar &uvar){
    startTime_ = startTime;
    file_ = file;
    dataName_ = dataName;
    UNotifyChange(uvar, &DataWriter::write);
}

void DataWriter::write(urbi::UVar &uvar){
    //cerr << dataName_ << " " << uvar.val() << endl;
    auto time = duration_cast<milliseconds>(highResolutionClock_.now() - startTime_).count();
    *file_ << dataName_ << " " << uvar.val() ;
    *file_ << " timestamp " << time << endl ;
}

// ========

VideoWriter::VideoWriter(const std::string &name) :
XObject(name)
{
    frameNumber_ = 0;
    avframe_ = avcodec_alloc_frame();
}

VideoWriter::~VideoWriter(){
    avcodec_free_frame(&avframe_);
}

void VideoWriter::init(const std::string &videoFile, const unsigned int &width, const unsigned int &height, const std::string &dataName, const TimePoint startTime, std::ofstream* file, ::urbi::UVar &uvar){
    startTime_ = startTime;
    file_ = file;
    dataName_ = dataName;
    videoFileWriter = unique_ptr<VideoFileWriter>(new VideoFileWriter(videoFile,width,height));
    UNotifyThreadedChange(uvar, &VideoWriter::write, ::urbi::LOCK_FUNCTION);
}

void VideoWriter::write(urbi::UImage &image){
    auto time = duration_cast<milliseconds>(highResolutionClock_.now() - startTime_).count();
    *file_ << dataName_ << " " << frameNumber_++;
    *file_ << " timestamp " << time << endl;
    
    //TODO: use frame format
    
    avframe_->width = image.width;
    avframe_->height = image.height;
    avframe_->format = PIX_FMT_BGR24;
    avpicture_fill((AVPicture*)avframe_, image.data, (AVPixelFormat) avframe_->format, image.width, image.height);
    videoFileWriter->writeVideoFrame(*avframe_);

}

// ========

Datalogger::Datalogger(const std::string& name) :
xcs::nodes::XObject(name),
startTime_(Clock().now())
{
    XBindFunction(Datalogger, init);
    XBindFunction(Datalogger, registerData);
    XBindFunction(Datalogger, registerVideo);
}

Datalogger::~Datalogger(){
    file_.close();
}

void Datalogger::init(const std::string &file){
    file_.open(file.c_str());

    if (!file_.is_open()){
        cerr << "Datalogger cannot open file: " << file << endl;
        return;
    }
}

void Datalogger::registerData(const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar){
    if (!file_.is_open()){
        cerr << "File error" << endl;
    }
    else{
        if (semanticType == "video"){
            cerr << "Use registerVideo function for video data!." << endl;
        }
        else{
            file_ << REGISTER << " " << name << " " << semanticType << " " << syntacticType << endl;

            DataWriter* function = new DataWriter(std::string());
            function->init(name, startTime_ ,&file_, uvar);
            dataWriterList_.push_back(std::unique_ptr<DataWriter>(function));
        }
    }
}

void Datalogger::registerVideo(const std::string &videoFile, int width, int height, const std::string &name, const std::string &semanticType, const std::string &syntacticType, ::urbi::UVar &uvar){
    if (!file_.is_open()){
        cerr << "File error" << endl;
    }
    else{
        if (semanticType == "video"){
            file_ << REGISTER << " " << name << " " << semanticType << " " << syntacticType << endl;
            VideoWriter* function = new VideoWriter(std::string());
            function->init(videoFile, width, height, name, startTime_, &file_, uvar);
            videoWriterList_.push_back(std::unique_ptr<VideoWriter>(function));
        }
        else{
            cerr << "Use registerData function for another data than video!" << endl;
        }
    }
}

//XStart(DataWriter);
//XStart(VideoWriter);
XStart(Datalogger);