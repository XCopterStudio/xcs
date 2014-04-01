#include <iostream>

#include "basic.hpp"
#include "options_visitor.hpp"
#include <xcs/nodes/xci.xob/structs/cartesian_vector.hpp>
#include <xcs/nodes/xci.xob/structs/eulerian_vector.hpp>


using namespace xcs::xci::parrot;
using namespace xcs::nodes::xci;

void OptionVisitor::visit(NavdataDemo* demo){
    /*dataReceiver_.notify("psi", miliDegreesToRadias(demo->psi));
    dataReceiver_.notify("theta", miliDegreesToRadias(demo->theta));
    dataReceiver_.notify("phi", miliDegreesToRadias(demo->phi));
    dataReceiver_.notify("velocityX", demo->vx);
    dataReceiver_.notify("velocityY", demo->vy);
    dataReceiver_.notify("velocityZ", demo->vz);*/
    CartesianVectorChronologic rotation(miliDegreesToRadias(demo->phi),
        miliDegreesToRadias(demo->theta),
        miliDegreesToRadias(demo->psi),
        0);
    dataReceiver_.notify("rotation", rotation);

    EulerianVectorChronologic velocity(demo->vx / 1000.0,
        demo->vy / 1000.0,
        demo->vz / 1000.0,
        0);
    dataReceiver_.notify("velocity", velocity);

    dataReceiver_.notify("altitude", demo->altitude / 1000.0);
    dataReceiver_.notify("battery", demo->vbat_flying_percentage / 100.0);
}

void OptionVisitor::visit(NavdataCks* cks){

}

void OptionVisitor::visit(NavdataTime* time){

}

void OptionVisitor::visit(NavdataRawMeasures* rawMeasures){
}

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
