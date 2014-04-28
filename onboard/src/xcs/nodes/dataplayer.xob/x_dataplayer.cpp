#include "x_dataplayer.hpp"

XStart(XDataplayer);

namespace xcs::nodes;

XDataplayer::~XDataplayer() {
    file_.close();
}

void XDataplayer::init(const std::string &file) {
    file_.open(file.c_str());

    if (!file_.is_open()) {
        send("throw \"Dataplayer cannot open file: " + file + "\";");
        return;
    }

    loadHeader();
}

void XDataplayer::loadHeader() {
    
}

void XDataplayer::loop() {
    // load initial record

    while (1) {
        // parse record

        // notify record (if video, take it from video buffer)

        // read next record        

        // sleep until next record (consider already passed time)
    }
}