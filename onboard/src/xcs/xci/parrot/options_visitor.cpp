#include <iostream>

#include "options_visitor.hpp"
#include <xcs/xcs_fce.hpp>
#include <xcs/types/cartesian_vector.hpp>
#include <xcs/types/eulerian_vector.hpp>


using namespace xcs;
using namespace xcs::xci::parrot;

void OptionVisitor::visit(NavdataDemo* demo){
    EulerianVector rotation(miliDegreesToRadias(demo->phi),
        miliDegreesToRadias(demo->theta),
        miliDegreesToRadias(demo->psi));
    dataReceiver_.notify("rotation", rotation);

    CartesianVector velocity(demo->vy / 1000.0,
        demo->vx / 1000.0,
        demo->vz / 1000.0);
    dataReceiver_.notify("velocity", velocity);

    dataReceiver_.notify("altitude", demo->altitude / 1000.0);
    dataReceiver_.notify("battery", demo->vbat_flying_percentage / 100.0);
}

void OptionVisitor::visit(NavdataCks* cks){

}

void OptionVisitor::visit(NavdataTime* time){
    dataReceiver_.notify("internalTimeImu", (time->time >> 21) + ((time->time & 0x001FFFFF) / 1000000.0));
}

void OptionVisitor::visit(NavdataRawMeasures* rawMeasures){
    CartesianVector acceleration(rawMeasures->raw_accs[1],
        rawMeasures->raw_accs[0],
        rawMeasures->raw_accs[2]);
    dataReceiver_.notify("acceleration", acceleration);
    CartesianVector gyro(rawMeasures->raw_gyros[0],
        rawMeasures->raw_gyros[1],
        rawMeasures->raw_gyros[2]);
    dataReceiver_.notify("gyro", gyro);
}

void OptionVisitor::visit(NavdataPressureRaw* pressureRaw){}

void OptionVisitor::visit(NavdataMagneto* magneto){
    CartesianVector acceleration(magneto->my,
        magneto->mx,
        magneto->mz);
    dataReceiver_.notify("magneto", magneto);
}

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

void OptionVisitor::visit(NavdataAltitude* altitude){
    CartesianVector altitudeAll(altitude->altitude_vision,
        altitude->altitude_ref,
        altitude->altitude_raw);
    dataReceiver_.notify("altitudeAll", altitudeAll);
    dataReceiver_.notify("altitudeV", altitude->altitude_vz);
}

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

void OptionVisitor::visit(NavdataWifi* wifi){
    dataReceiver_.notify("wifiQuality", wifi->link_quality);
}
