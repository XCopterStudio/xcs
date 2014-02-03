#include <iostream>

#include "options_visitor.hpp"

using namespace xcs::xci::parrot;

void OptionVisitor::visit(NavdataDemo* demo){
    printf("\r Navdata demo [%f,%f,%f]",demo->phi,demo->theta,demo->psi);
}

void OptionVisitor::visit(NavdataCks* cks){

}

void OptionVisitor::visit(NavdataTime* time){}

void OptionVisitor::visit(NavdataRawMeasures* rawMeasures){}

void OptionVisitor::visit(NavdataPressureRaw* pressureRaw){}

void OptionVisitor::visit(NavdataMagneto* magneto){}

void OptionVisitor::visit(NavdataWindSpeed* windSpeed){}

void OptionVisitor::visit(NavdataKalmanPressure* kalmanPressure){}

void OptionVisitor::visit(NavdataZimmu3000* zimmu3000){}

void OptionVisitor::visit(NavdataPhysMeasures* physMeasures){}

void OptionVisitor::visit(NavdataGyrosOffsets* gyrosOffsets){}

void OptionVisitor::visit(NavdataEulerAngles* eulerAngles){}

void OptionVisitor::visit(NavdataReferences* references){}

void OptionVisitor::visit(NavdataTrims* trims){}

void OptionVisitor::visit(NavdataRcReferences* rcReferences){}

void OptionVisitor::visit(NavdataPwm* pwm){}

void OptionVisitor::visit(NavdataAltitude* altitude){}

void OptionVisitor::visit(NavdataVisionRaw* visionRaw){}

void OptionVisitor::visit(NavdataVision* vision){}

void OptionVisitor::visit(NavdataVisionPerf* visionPerf){}

void OptionVisitor::visit(NavdataTrackersSend* trackersSend){}

void OptionVisitor::visit(NavdataVisionDetect* visionDetect){}

void OptionVisitor::visit(NavdataVisionOf* visionOf){}

void OptionVisitor::visit(NavdataWatchdog* watchdog){}

void OptionVisitor::visit(NavdataAdcDataFrame* adcDataFrame){}

void OptionVisitor::visit(NavdataVideoStream* videoStream){}

void OptionVisitor::visit(NavdataHDVideoStream* hdVideoStream){}

void OptionVisitor::visit(NavdataGames* games){}

void OptionVisitor::visit(NavdataWifi* wifi){}
