#include "navdata_parser.hpp"

using namespace xcs::xci::parrot;

uint32_t NavdataProcess::computeChecksum(Navdata* navdata, const size_t size){
    uint8_t* data = (uint8_t*)navdata;
    uint32_t checksum = 0;
    size_t dataSize = size - sizeof (NavdataCks);

    for (unsigned int i = 0; i < dataSize; i++) {
        checksum += (uint32_t)data[i];
    }

    return checksum;
}

std::vector<NavdataOption*> NavdataProcess::parse(xcs::xci::parrot::Navdata* navdata, uint32_t navdataCks, unsigned int lenght){
    unsigned int optionsLenght = lenght - sizeof(Navdata) + sizeof(NavdataOption*);
    unsigned int index = 0;
    std::vector<NavdataOption*> options;
    NavdataOption* optionPtr = &navdata->options[0];
    while(index < optionsLenght){
        switch (optionPtr->tag){
        case NAVDATA_DEMO_TAG:{
            NavdataDemo* demo = new NavdataDemo;
            *demo = *reinterpret_cast<NavdataDemo*>(optionPtr);
            options.push_back(demo);
            }break;
        case NAVDATA_TIME_TAG:{
            NavdataTime* time = new NavdataTime;
            *time = *reinterpret_cast<NavdataTime*>(optionPtr);
            options.push_back(time);
            }break;
        case NAVDATA_RAW_MEASURES_TAG:{
            NavdataRawMeasures* rawMeasures = new NavdataRawMeasures;
            *rawMeasures = *reinterpret_cast<NavdataRawMeasures*>(optionPtr);
            options.push_back(rawMeasures);
            }break;
        case NAVDATA_PHYS_MEASURES_TAG:{
            NavdataPhysMeasures* physMeasures = new NavdataPhysMeasures;
            *physMeasures = *reinterpret_cast<NavdataPhysMeasures*>(optionPtr);
            options.push_back(physMeasures);
            }break;
        case NAVDATA_GYROS_OFFSETS_TAG:{
            NavdataGyrosOffsets* gyrosOffsets = new NavdataGyrosOffsets;
            *gyrosOffsets = *reinterpret_cast<NavdataGyrosOffsets*>(optionPtr);
            options.push_back(gyrosOffsets);
            }break;
        case NAVDATA_EULER_ANGLES_TAG:{
            NavdataEulerAngles* eulerAngles = new NavdataEulerAngles;
            *eulerAngles = *reinterpret_cast<NavdataEulerAngles*>(optionPtr);
            options.push_back(eulerAngles);
            }break;
        case NAVDATA_REFERENCES_TAG:{
            NavdataReferences* references = new NavdataReferences;
            *references = *reinterpret_cast<NavdataReferences*>(optionPtr);
            options.push_back(references);
            }break;
        case NAVDATA_TRIMS_TAG:{
            NavdataTrims* trims = new NavdataTrims;
            *trims = *reinterpret_cast<NavdataTrims*>(optionPtr);
            options.push_back(optionPtr);
            }break;
        case NAVDATA_RC_REFERENCES_TAG:{
            NavdataRcReferences* rcReferences = new NavdataRcReferences;
            *rcReferences = *reinterpret_cast<NavdataRcReferences*>(optionPtr);
            options.push_back(rcReferences);
            }break;
        case NAVDATA_PWM_TAG:{
            NavdataPwm* pwm = new NavdataPwm;
            *pwm = *reinterpret_cast<NavdataPwm*>(optionPtr);
            options.push_back(pwm);
            }break;
        case NAVDATA_ALTITUDE_TAG:{
            NavdataAltitude* altitude = new NavdataAltitude;
            *altitude = *reinterpret_cast<NavdataAltitude*>(optionPtr);
            options.push_back(altitude);
            }break;
        case NAVDATA_VISION_RAW_TAG:{
            NavdataVisionRaw* visionRaw = new NavdataVisionRaw;
            *visionRaw = *reinterpret_cast<NavdataVisionRaw*>(optionPtr);
            options.push_back(visionRaw);
            }break;
        case NAVDATA_VISION_OF_TAG:{
            NavdataVisionOf* visionOf = new NavdataVisionOf;
            *visionOf = *reinterpret_cast<NavdataVisionOf*>(optionPtr);
            options.push_back(visionOf);
            }break;
        case NAVDATA_VISION_TAG:{
            NavdataVision* vision = new NavdataVision;
            *vision = *reinterpret_cast<NavdataVision*>(optionPtr);
            options.push_back(vision);
            }break;
        case NAVDATA_VISION_PERF_TAG:{
            NavdataVisionPerf* visionPerf = new NavdataVisionPerf;
            *visionPerf = *reinterpret_cast<NavdataVisionPerf*>(optionPtr);
            options.push_back(visionPerf);
            }break;
        case NAVDATA_TRACKERS_SEND_TAG:{
            NavdataTrackersSend* trackersSend = new NavdataTrackersSend;
            *trackersSend = *reinterpret_cast<NavdataTrackersSend*>(optionPtr);
            options.push_back(trackersSend);
            }break;
        case NAVDATA_VISION_DETECT_TAG:{
            NavdataVisionDetect* visionDetect = new NavdataVisionDetect;
            *visionDetect = *reinterpret_cast<NavdataVisionDetect*>(optionPtr);
            options.push_back(visionDetect);
            }break;
        case NAVDATA_WATCHDOG_TAG:{
            NavdataWatchdog* watchdog = new NavdataWatchdog;
            *watchdog = *reinterpret_cast<NavdataWatchdog*>(optionPtr);
            options.push_back(watchdog);
            }break;
        case NAVDATA_ADC_DATA_FRAME_TAG:{
            NavdataAdcDataFrame* adcDataFrame = new NavdataAdcDataFrame;
            *adcDataFrame = *reinterpret_cast<NavdataAdcDataFrame*>(optionPtr);
            options.push_back(adcDataFrame);
            }break;
        case NAVDATA_VIDEO_STREAM_TAG:{
            NavdataVideoStream* videoStream = new NavdataVideoStream;
            *videoStream = *reinterpret_cast<NavdataVideoStream*>(optionPtr);
            options.push_back(videoStream);
            }break;
        case NAVDATA_GAMES_TAG:{
            NavdataGames* games = new NavdataGames;
            *games = *reinterpret_cast<NavdataGames*>(optionPtr);
            options.push_back(games);
            }break;
        case NAVDATA_PRESSURE_RAW_TAG:{
            NavdataPressureRaw* pressure = new NavdataPressureRaw;
            *pressure = *reinterpret_cast<NavdataPressureRaw*>(optionPtr);
            options.push_back(pressure);
            }break;
        case NAVDATA_MAGNETO_TAG:{
            NavdataMagneto* magneto = new NavdataMagneto;
            *magneto = *reinterpret_cast<NavdataMagneto*>(optionPtr);
            options.push_back(magneto);
            }break;
        case NAVDATA_WIND_TAG:{
            NavdataWindSpeed* wind = new NavdataWindSpeed;
            *wind = *reinterpret_cast<NavdataWindSpeed*>(optionPtr);
            options.push_back(wind);
            }break;
        case NAVDATA_KALMAN_PRESSURE_TAG:{
            NavdataKalmanPressure* kalmanPressure = new NavdataKalmanPressure;
            *kalmanPressure = *reinterpret_cast<NavdataKalmanPressure*>(optionPtr);
            options.push_back(kalmanPressure);
            }break;
        case NAVDATA_HDVIDEO_STREAM_TAG:{
            NavdataHDVideoStream* hdVideoStream = new NavdataHDVideoStream;
            *hdVideoStream = *reinterpret_cast<NavdataHDVideoStream*>(optionPtr);
            options.push_back(hdVideoStream);
            }break;
        case NAVDATA_WIFI_TAG:{
            NavdataWifi* wifi = new NavdataWifi;
            *wifi = *reinterpret_cast<NavdataWifi*>(optionPtr);
            options.push_back(wifi);
            }break;
        case NAVDATA_ZIMMU_3000_TAG:{
            NavdataZimmu3000* zimu = new NavdataZimmu3000;
            *zimu = *reinterpret_cast<NavdataZimmu3000*>(optionPtr);
            options.push_back(zimu);
            }break;
        case NAVDATA_CKS_TAG:{
            NavdataCks* cks = new NavdataCks;
            *cks = *reinterpret_cast<NavdataCks*>(optionPtr);
            if (navdataCks != cks->cks){
                for(auto option : options)
                    delete option;

                options.clear();
            }else{
                options.push_back(cks);
            }}break;
        default:
            break;
        }

        index += optionPtr->size;
        optionPtr = (NavdataOption*)(((uint8_t*)optionPtr) + optionPtr->size);
    }

    return options;
}