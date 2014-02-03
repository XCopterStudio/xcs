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

std::vector<OptionAcceptor*> NavdataProcess::parse(xcs::xci::parrot::Navdata* navdata, uint32_t navdataCks, unsigned int lenght){
    unsigned int optionsLenght = lenght - sizeof(Navdata);
    unsigned int index = 0;
    std::vector<OptionAcceptor*> options;
    NavdataOption* optionPtr = navdata->options;
    while(index < optionsLenght){
        switch (optionPtr->tag){
        case NAVDATA_DEMO_TAG:{
            NavdataDemoAcceptor* demo = new NavdataDemoAcceptor;
            demo->data = *reinterpret_cast<NavdataDemo*>(optionPtr);
            options.push_back(demo);
            }break;
        case NAVDATA_TIME_TAG:{
            NavdataTimeAcceptor* time = new NavdataTimeAcceptor;
            time->data = *reinterpret_cast<NavdataTime*>(optionPtr);
            options.push_back(time);
            }break;
        case NAVDATA_RAW_MEASURES_TAG:{
            NavdataRawMeasuresAcceptor* rawMeasures = new NavdataRawMeasuresAcceptor;
            rawMeasures->data = *reinterpret_cast<NavdataRawMeasures*>(optionPtr);
            options.push_back(rawMeasures);
            }break;
        case NAVDATA_PHYS_MEASURES_TAG:{
            NavdataPhysMeasuresAcceptor* physMeasures = new NavdataPhysMeasuresAcceptor;
            physMeasures->data = *reinterpret_cast<NavdataPhysMeasures*>(optionPtr);
            options.push_back(physMeasures);
            }break;
        case NAVDATA_GYROS_OFFSETS_TAG:{
            NavdataGyrosOffsetsAcceptor* gyrosOffsets = new NavdataGyrosOffsetsAcceptor;
            gyrosOffsets->data = *reinterpret_cast<NavdataGyrosOffsets*>(optionPtr);
            options.push_back(gyrosOffsets);
            }break;
        case NAVDATA_EULER_ANGLES_TAG:{
            NavdataEulerAnglesAcceptor* eulerAngles = new NavdataEulerAnglesAcceptor;
            eulerAngles->data = *reinterpret_cast<NavdataEulerAngles*>(optionPtr);
            options.push_back(eulerAngles);
            }break;
        case NAVDATA_REFERENCES_TAG:{
            NavdataReferencesAcceptor* references = new NavdataReferencesAcceptor;
            references->data = *reinterpret_cast<NavdataReferences*>(optionPtr);
            options.push_back(references);
            }break;
        case NAVDATA_TRIMS_TAG:{
            NavdataTrimsAcceptor* trims = new NavdataTrimsAcceptor;
            trims->data = *reinterpret_cast<NavdataTrims*>(optionPtr);
            options.push_back(trims);
            }break;
        case NAVDATA_RC_REFERENCES_TAG:{
            NavdataRcReferencesAcceptor* rcReferences = new NavdataRcReferencesAcceptor;
            rcReferences->data = *reinterpret_cast<NavdataRcReferences*>(optionPtr);
            options.push_back(rcReferences);
            }break;
        case NAVDATA_PWM_TAG:{
            NavdataPwmAcceptor* pwm = new NavdataPwmAcceptor;
            pwm->data = *reinterpret_cast<NavdataPwm*>(optionPtr);
            options.push_back(pwm);
            }break;
        case NAVDATA_ALTITUDE_TAG:{
            NavdataAltitudeAcceptor* altitude = new NavdataAltitudeAcceptor;
            altitude->data = *reinterpret_cast<NavdataAltitude*>(optionPtr);
            options.push_back(altitude);
            }break;
        case NAVDATA_VISION_RAW_TAG:{
            NavdataVisionRawAcceptor* visionRaw = new NavdataVisionRawAcceptor;
            visionRaw->data = *reinterpret_cast<NavdataVisionRaw*>(optionPtr);
            options.push_back(visionRaw);
            }break;
        case NAVDATA_VISION_OF_TAG:{
            NavdataVisionOfAcceptor* visionOf = new NavdataVisionOfAcceptor;
            visionOf->data = *reinterpret_cast<NavdataVisionOf*>(optionPtr);
            options.push_back(visionOf);
            }break;
        case NAVDATA_VISION_TAG:{
            NavdataVisionAcceptor* vision = new NavdataVisionAcceptor;
            vision->data = *reinterpret_cast<NavdataVision*>(optionPtr);
            options.push_back(vision);
            }break;
        case NAVDATA_VISION_PERF_TAG:{
            NavdataVisionPerfAcceptor* visionPerf = new NavdataVisionPerfAcceptor;
            visionPerf->data = *reinterpret_cast<NavdataVisionPerf*>(optionPtr);
            options.push_back(visionPerf);
            }break;
        case NAVDATA_TRACKERS_SEND_TAG:{
            NavdataTrackersSendAcceptor* trackersSend = new NavdataTrackersSendAcceptor;
            trackersSend->data = *reinterpret_cast<NavdataTrackersSend*>(optionPtr);
            options.push_back(trackersSend);
            }break;
        case NAVDATA_VISION_DETECT_TAG:{
            NavdataVisionDetectAcceptor* visionDetect = new NavdataVisionDetectAcceptor;
            visionDetect->data = *reinterpret_cast<NavdataVisionDetect*>(optionPtr);
            options.push_back(visionDetect);
            }break;
        case NAVDATA_WATCHDOG_TAG:{
            NavdataWatchdogAcceptor* watchdog = new NavdataWatchdogAcceptor;
            watchdog->data = *reinterpret_cast<NavdataWatchdog*>(optionPtr);
            options.push_back(watchdog);
            }break;
        case NAVDATA_ADC_DATA_FRAME_TAG:{
            NavdataAdcDataFrameAcceptor* adcDataFrame = new NavdataAdcDataFrameAcceptor;
            adcDataFrame->data = *reinterpret_cast<NavdataAdcDataFrame*>(optionPtr);
            options.push_back(adcDataFrame);
            }break;
        case NAVDATA_VIDEO_STREAM_TAG:{
            NavdataVideoStreamAcceptor* videoStream = new NavdataVideoStreamAcceptor;
            videoStream->data = *reinterpret_cast<NavdataVideoStream*>(optionPtr);
            options.push_back(videoStream);
            }break;
        case NAVDATA_GAMES_TAG:{
            NavdataGamesAcceptor* games = new NavdataGamesAcceptor;
            games->data = *reinterpret_cast<NavdataGames*>(optionPtr);
            options.push_back(games);
            }break;
        case NAVDATA_PRESSURE_RAW_TAG:{
            NavdataPressureRawAcceptor* pressure = new NavdataPressureRawAcceptor;
            pressure->data = *reinterpret_cast<NavdataPressureRaw*>(optionPtr);
            options.push_back(pressure);
            }break;
        case NAVDATA_MAGNETO_TAG:{
            NavdataMagnetoAcceptor* magneto = new NavdataMagnetoAcceptor;
            magneto->data = *reinterpret_cast<NavdataMagneto*>(optionPtr);
            options.push_back(magneto);
            }break;
        case NAVDATA_WIND_TAG:{
            NavdataWindSpeedAcceptor* wind = new NavdataWindSpeedAcceptor;
            wind->data = *reinterpret_cast<NavdataWindSpeed*>(optionPtr);
            options.push_back(wind);
            }break;
        case NAVDATA_KALMAN_PRESSURE_TAG:{
            NavdataKalmanPressureAcceptor* kalmanPressure = new NavdataKalmanPressureAcceptor;
            kalmanPressure->data = *reinterpret_cast<NavdataKalmanPressure*>(optionPtr);
            options.push_back(kalmanPressure);
            }break;
        case NAVDATA_HDVIDEO_STREAM_TAG:{
            NavdataHDVideoStreamAcceptor* hdVideoStream = new NavdataHDVideoStreamAcceptor;
            hdVideoStream->data = *reinterpret_cast<NavdataHDVideoStream*>(optionPtr);
            options.push_back(hdVideoStream);
            }break;
        case NAVDATA_WIFI_TAG:{
            NavdataWifiAcceptor* wifi = new NavdataWifiAcceptor;
            wifi->data = *reinterpret_cast<NavdataWifi*>(optionPtr);
            options.push_back(wifi);
            }break;
        case NAVDATA_ZIMMU_3000_TAG:{
            NavdataZimmu3000Acceptor* zimu = new NavdataZimmu3000Acceptor;
            zimu->data = *reinterpret_cast<NavdataZimmu3000*>(optionPtr);
            options.push_back(zimu);
            }break;
        case NAVDATA_CKS_TAG:{
            NavdataCksAcceptor* cks = new NavdataCksAcceptor;
            cks->data = *reinterpret_cast<NavdataCks*>(optionPtr);
            if (navdataCks != cks->data.cks){
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