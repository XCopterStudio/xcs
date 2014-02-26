#ifndef OPTIONS_VISITOR_H
#define OPTIONS_VISITOR_H

#include "navdata_options.hpp"
#include <xcs/xci/data_receiver.hpp>

namespace xcs{
namespace xci{
namespace parrot{

    class OptionVisitor{
        DataReceiver& dataReceiver_;
    public:
        OptionVisitor(DataReceiver& dataReceiver) : dataReceiver_(dataReceiver) {};

        virtual void visit(NavdataDemo* demo);
        virtual void visit(NavdataCks* cks);
        virtual void visit(NavdataTime* time);
        virtual void visit(NavdataRawMeasures* rawMeasures);
        virtual void visit(NavdataPressureRaw* pressureRaw);
        virtual void visit(NavdataMagneto* magneto);
        virtual void visit(NavdataWindSpeed* windSpeed);
        virtual void visit(NavdataKalmanPressure* kalmanPressure);
        virtual void visit(NavdataZimmu3000* zimmu3000);
        virtual void visit(NavdataPhysMeasures* physMeasures);
        virtual void visit(NavdataGyrosOffsets* gyrosOffsets);
        virtual void visit(NavdataEulerAngles* eulerAngles);
        virtual void visit(NavdataReferences* references);
        virtual void visit(NavdataTrims* trims);
        virtual void visit(NavdataRcReferences* rcReferences);
        virtual void visit(NavdataPwm* pwm);
        virtual void visit(NavdataAltitude* altitude);
        virtual void visit(NavdataVisionRaw* visionRaw);
        virtual void visit(NavdataVision* vision);
        virtual void visit(NavdataVisionPerf* visionPerf);
        virtual void visit(NavdataTrackersSend* trackersSend);
        virtual void visit(NavdataVisionDetect* visionDetect);
        virtual void visit(NavdataVisionOf* visionOf);
        virtual void visit(NavdataWatchdog* watchdog);
        virtual void visit(NavdataAdcDataFrame* adcDataFrame);
        virtual void visit(NavdataVideoStream* videoStream);
        virtual void visit(NavdataHDVideoStream* hdVideoStream);
        virtual void visit(NavdataGames* games);
        virtual void visit(NavdataWifi* wifi);
    };

    struct OptionAcceptor{
        virtual void accept(class OptionVisitor &visitor){};
    };

    struct NavdataDemoAcceptor : public OptionAcceptor{
        NavdataDemo data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataCksAcceptor : public OptionAcceptor{
        NavdataCks data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataTimeAcceptor : public OptionAcceptor{
        NavdataTime data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataRawMeasuresAcceptor : public OptionAcceptor{
        NavdataRawMeasures data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataPressureRawAcceptor : public OptionAcceptor{
        NavdataPressureRaw data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataMagnetoAcceptor : public OptionAcceptor{
        NavdataMagneto data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataWindSpeedAcceptor : public OptionAcceptor{
        NavdataWindSpeed data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataKalmanPressureAcceptor : public OptionAcceptor{
        NavdataKalmanPressure data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataZimmu3000Acceptor : public OptionAcceptor{
        NavdataZimmu3000 data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataPhysMeasuresAcceptor : public OptionAcceptor{
        NavdataPhysMeasures data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataGyrosOffsetsAcceptor : public OptionAcceptor{
        NavdataGyrosOffsets data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataEulerAnglesAcceptor : public OptionAcceptor{
        NavdataEulerAngles data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataReferencesAcceptor : public OptionAcceptor{
        NavdataReferences data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataTrimsAcceptor : public OptionAcceptor{
        NavdataTrims data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataRcReferencesAcceptor : public OptionAcceptor{
        NavdataRcReferences data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataPwmAcceptor : public OptionAcceptor{
        NavdataPwm data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataAltitudeAcceptor : public OptionAcceptor{
        NavdataAltitude data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVisionRawAcceptor : public OptionAcceptor{
        NavdataVisionRaw data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVisionAcceptor : public OptionAcceptor{
        NavdataVision data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVisionPerfAcceptor : public OptionAcceptor{
        NavdataVisionPerf data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataTrackersSendAcceptor : public OptionAcceptor{
        NavdataTrackersSend data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVisionDetectAcceptor : public OptionAcceptor{
        NavdataVisionDetect data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVisionOfAcceptor : public OptionAcceptor{
        NavdataVisionOf data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataWatchdogAcceptor : public OptionAcceptor{
        NavdataWatchdog data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataAdcDataFrameAcceptor : public OptionAcceptor{
        NavdataAdcDataFrame data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataVideoStreamAcceptor : public OptionAcceptor{
        NavdataVideoStream data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataHDVideoStreamAcceptor : public OptionAcceptor{
        NavdataHDVideoStream data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataGamesAcceptor : public OptionAcceptor{
        NavdataGames data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };

    struct NavdataWifiAcceptor : public OptionAcceptor{
        NavdataWifi data;
        void accept(OptionVisitor &visitor){
            visitor.visit(&data);
        };
    };



}}}
#endif