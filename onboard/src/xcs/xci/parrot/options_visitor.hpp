#ifndef OPTIONS_VISITOR_H
#define OPTIONS_VISITOR_H

namespace xcs{
namespace xci{
namespace parrot{

    class OptionVisitor{
    public:
        void visit(struct NavdataDemo* demo);
        void visit(struct NavdataCks* cks);
    };

    class OptionAcceptor{
        virtual void accept(class OptionVisitor &visitor) = 0;
    };

    /*NavdataDemo  navdataDemo;
    NavdataCks  navdataCks;
    NavdataTime  navdataTime;
    NavdataRawMeasures  navdataRawMeasures;
    NavdataPressureRaw  navdataPressureRaw;
    NavdataMagneto  navdataMagneto;
    NavdataWindSpeed  navdataWindSpeed;
    NavdataKalmanPressure  navdataKalmanPressure;
    NavdataZimmu3000  navdataZimmu3000;
    NavdataPhysMeasures  navdataPhysMeasures;
    NavdataGyrosOffsets  navdataGyrosOffsets;
    NavdataEulerAngles  navdataEulerAngles;
    NavdataReferences  navdataReferences;
    NavdataTrims  navdataTrims;
    NavdataRcReferences  navdataRcReferences;
    NavdataPwm  navdataPwm;
    NavdataVisionRaw  navdataVisionRaw;
    NavdataVision  navdataVision;
    NavdataVisionPerf  navdataVisionPerf;
    NavdataTrackersSend  navdataTrackersSend;
    NavdataVisionDetect  navdataVisionDetect;
    NavdataVisionOf  navdataVisionOf;
    NavdataWatchdog  navdataWatchdog;
    NavdataAdcDataFrame  navdataAdcDataFrame;
    NavdataVideoStream  navdataVideoStream;
    NavdataHDVideoStream  navdataHDVideoStream;
    NavdataGames  navdataGames;
    NavdataWifi  navdataWifi;*/

}}}
#endif