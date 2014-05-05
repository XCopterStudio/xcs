#include "x_dataplayer.hpp"

#include <xcs/logging.hpp>
#include <xcs/nodes/xobject/x.hpp>
#include <xcs/nodes/xobject/x_type.hpp>

#include <sstream>
#include <thread>

using namespace xcs::nodes;
using namespace std;

const std::string XDataplayer::CMD_PLAY = "Play";
const std::string XDataplayer::CMD_PAUSE = "Pause";

XDataplayer::XDataplayer(const std::string& name) :
  xcs::nodes::XObject(name),
  command("COMMAND"),
  seek("INTEGER"),
  isPlaying_(false) {
    XBindFunction(XDataplayer, init);
    XBindVarF(command, &XDataplayer::onCommand);
}

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

    dataLoopThread_ = move(thread(&XDataplayer::loop, this));
}

void XDataplayer::loadHeader() {
    while (true) {
        string line;
        getline(file_, line);
        if (line.empty()) {
            break;
        }
        stringstream sline(line);
        string check, name, semanticType, synType;
        sline >> check >> name >> semanticType >> synType;

        if (check != "register") {
            continue;
        }

        SimpleXVar &xvar = dataReceiver_.registerOutput(name, XType(synType, semanticType, XType::DATAFLOWTYPE_XVAR));
        XBindVarRename(xvar, name);
        
        cerr << "Got: " << name << endl;

    }

    XCS_LOG_INFO("Loaded header...");
}

void XDataplayer::loop() {
    // load initial record

    while (1) {
        this_thread::sleep_for(chrono::milliseconds(100));
        if (!isPlaying_) {
            continue;
        }
        if (file_.eof()) {
            break;
        }

        string line;
        getline(file_, line);
        stringstream sline(line);
        string timestamp, name, rest;
        sline >> timestamp >> name;
        getline(sline, rest);
        
        dataReceiver_.notify(name, stod(rest));
        //cerr << "Got " << timestamp << ":" << name << ", rest:" << rest << endl;
        
        // parse record

        // notify record (if video, take it from video buffer)

        // read next record        

        // sleep until next record (consider already passed time)
    }
}

void XDataplayer::onCommand(const std::string &command) {
    if (command == CMD_PLAY) {
        isPlaying_ = true;
    } else if (command == CMD_PAUSE) {
        isPlaying_ = false;
    }
}

XStart(XDataplayer);
