#ifndef OPTIONS_VISITOR_H
#define OPTIONS_VISITOR_H

#include "navdata_options.hpp"
#include <xcs/xci/data_receiver.hpp>

namespace xcs{
namespace xci{
namespace parrot{
    /*! \brief Navdata option visitor.

        Each option in navdata packet have special visit function. 
        Some visit functions are empty. 
        When you want implement empty visit function do not forget call notify on dataReceiver_ and add new type off sensor in xci_parrot.h .
    */
    class OptionVisitor{
        DataReceiver& dataReceiver_;
    public:
        OptionVisitor(DataReceiver& dataReceiver) : dataReceiver_(dataReceiver) {};

        /// Update rotation, velocity, altitude and battery sensor
        virtual void visit(NavdataDemo* demo);
        /// Empty 
        virtual void visit(NavdataCks* cks);
        /// Update time sensor. Navdata packet creation AR.Drone 2.0 time.
        virtual void visit(NavdataTime* time);
        /// Update raw measurement accelerometer and gyroscope sensor.
        virtual void visit(NavdataRawMeasures* rawMeasures);
        /// Empty
        virtual void visit(NavdataPressureRaw* pressureRaw);
        /// Update raw measurement magnetometer sensor.
        virtual void visit(NavdataMagneto* magneto);
        /// Empty
        virtual void visit(NavdataWindSpeed* windSpeed);
        /// Empty
        virtual void visit(NavdataKalmanPressure* kalmanPressure);
        /// Empty
        virtual void visit(NavdataZimmu3000* zimmu3000);
        /// Empty
        virtual void visit(NavdataPhysMeasures* physMeasures);
        /// Empty
        virtual void visit(NavdataGyrosOffsets* gyrosOffsets);
        /// Empty
        virtual void visit(NavdataEulerAngles* eulerAngles);
        /// Empty
        virtual void visit(NavdataReferences* references);
        /// Empty
        virtual void visit(NavdataTrims* trims);
        /// Empty
        virtual void visit(NavdataRcReferences* rcReferences);
        /// Empty
        virtual void visit(NavdataPwm* pwm);
        //! Update altitudes sensors. 

        //! First is raw, vision and reference altitude.
        //! Second is unknown.
        virtual void visit(NavdataAltitude* altitude);
        /// Empty
        virtual void visit(NavdataVisionRaw* visionRaw);
        /// Empty
        virtual void visit(NavdataVision* vision);
        /// Empty
        virtual void visit(NavdataVisionPerf* visionPerf);
        ///Empty
        virtual void visit(NavdataTrackersSend* trackersSend);
        ///Empty
        virtual void visit(NavdataVisionDetect* visionDetect);
        ///Empty
        virtual void visit(NavdataVisionOf* visionOf);
        ///Empty
        virtual void visit(NavdataWatchdog* watchdog);
        ///Empty
        virtual void visit(NavdataAdcDataFrame* adcDataFrame);
        ///Empty
        virtual void visit(NavdataVideoStream* videoStream);
        ///Empty
        virtual void visit(NavdataHDVideoStream* hdVideoStream);
        ///Empty
        virtual void visit(NavdataGames* games);
        /// Update wifi link quality sensor.
        virtual void visit(NavdataWifi* wifi);
    };

    /*! Base navdata option acceptor.

        For all navdata option from AR.Drone 2.0 was automatically generated derived class.
        Do not change anything in this header file!.
    */
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