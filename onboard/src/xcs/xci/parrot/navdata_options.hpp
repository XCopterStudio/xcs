/**
*  \file     navdata_options.hpp
*  \brief    Navdata options
*  \author   Ondrej Pilat <ondrap@ondrap.cz>
    based on the navdata_common.h created by Sylvain Gaeremynck <sylvain.gaeremynck@parrot.com>
*/

#ifndef NAVDATA_OPTIONS_H
#define NAVDATA_OPTIONS_H

#include <cstdint>

namespace xcs{
namespace xci{
namespace parrot{

    static const unsigned int NAVDATA_HEADER = 0x55667788;
    static const unsigned int NAVDATA_MAX_SIZE = 4096;
    static const unsigned int NAVDATA_MAX_CUSTOM_TIME_SAVE = 20;

    static const unsigned int GYROS_NUMBER = 3;
    static const unsigned int ACCELEROMETERS_NUMBER = 3;

    /* !!! Warning !!! - changing the value below would break compatibility with older applications
    * DO NOT CHANGE THIS  */
    static const unsigned int NB_NAVDATA_DETECTION_RESULTS = 4;

    // NUMBER OF TRACKERS FOR EACH TRACKING
    static const unsigned int NB_CORNER_TRACKERS_WIDTH = 5;      /* number of trackers in width of current picture */
    static const unsigned int NB_CORNER_TRACKERS_HEIGHT = 4;      /* number of trackers in height of current picture */

    static const unsigned int DEFAULT_NB_TRACKERS_WIDTH = NB_CORNER_TRACKERS_WIDTH + 1;// + NB_BLOCK_TRACKERS_WIDTH)
    static const unsigned int DEFAULT_NB_TRACKERS_HEIGHT = NB_CORNER_TRACKERS_HEIGHT + 1;// + NB_BLOCK_TRACKERS_HEIGHT)

    typedef int32_t bool_t;
    typedef float float32_t;

    enum NavdataHDVideoStates {
        FLAG_NAVDATA_HDVIDEO_STORAGE_FIFO_IS_FULL = (1 << 0),
        FLAG_NAVDATA_HDVIDEO_USBKEY_IS_PRESENT = (1 << 8),
        FLAG_NAVDATA_HDVIDEO_USBKEY_IS_RECORDING = (1 << 9),
        FLAG_NAVDATA_HDVIDEO_USBKEY_IS_FULL = (1 << 10)
    };

    enum NavdataTag{
        NAVDATA_DEMO_TAG = 0,
        NAVDATA_TIME_TAG,
        NAVDATA_RAW_MEASURES_TAG,
        NAVDATA_PHYS_MEASURES_TAG,
        NAVDATA_GYROS_OFFSETS_TAG,
        NAVDATA_EULER_ANGLES_TAG,
        NAVDATA_REFERENCES_TAG,
        NAVDATA_TRIMS_TAG,
        NAVDATA_RC_REFERENCES_TAG,
        NAVDATA_PWM_TAG,
        NAVDATA_ALTITUDE_TAG,
        NAVDATA_VISION_RAW_TAG,
        NAVDATA_VISION_OF_TAG,
        NAVDATA_VISION_TAG,
        NAVDATA_VISION_PERF_TAG,
        NAVDATA_TRACKERS_SEND_TAG,
        NAVDATA_VISION_DETECT_TAG,
        NAVDATA_WATCHDOG_TAG,
        NAVDATA_ADC_DATA_FRAME_TAG,
        NAVDATA_VIDEO_STREAM_TAG,
        NAVDATA_GAMES_TAG,
        NAVDATA_PRESSURE_RAW_TAG,
        NAVDATA_MAGNETO_TAG,
        NAVDATA_WIND_TAG,
        NAVDATA_KALMAN_PRESSURE_TAG,
        NAVDATA_HDVIDEO_STREAM_TAG,
        NAVDATA_WIFI_TAG,
        NAVDATA_ZIMMU_3000_TAG,
        NAVDATA_CKS_TAG = 0xFFFF
    };

    struct Matrix33 {
        float32_t m11;
        float32_t m12;
        float32_t m13;
        float32_t m21;
        float32_t m22;
        float32_t m23;
        float32_t m31;
        float32_t m32;
        float32_t m33;
    };

    struct Vector31{
        float32_t x;
        float32_t y;
        float32_t z;
    };

    struct Vector21{
            float32_t x;
            float32_t y;
    };

    struct ScreenPoint{
        int32_t x;
        int32_t y;
    };

    #if defined(_MSC_VER)
        #define _ATTRIBUTE_PACKED_
        /* Asks Visual C++ to pack structures from now on*/
        #pragma pack(1)
    #else
        #define _ATTRIBUTE_PACKED_  __attribute__ ((packed))
    #endif

    struct _ATTRIBUTE_PACKED_ NavdataOption{
        uint16_t tag; /*!< Navdata block ('option') identifier */
        uint16_t size; /*!< set this to the size of this structure */

        //#if defined _MSC_VER || defined (__ARMCC_VERSION)
        ///* Do not use flexible arrays (C99 feature) with these compilers */
        //uint8_t data[1];
        //#else
        //uint8_t data[];
        //#endif
    };

    /**
    * @brief Navdata structure sent over the network.
    */
    struct _ATTRIBUTE_PACKED_ Navdata{
        uint32_t header; /*!< Always set to NAVDATA_HEADER */
        uint32_t ardrone_state; /*!< Bit mask built from def_ardrone_state_mask_t */
        uint32_t sequence; /*!< Sequence number, incremented for each sent packet */
        bool_t vision_defined;

        NavdataOption options[1];
    };

    /**
    * @brief Minimal navigation data for all flights.
    */
    struct _ATTRIBUTE_PACKED_ NavdataDemo : public NavdataOption {
        uint32_t ctrl_state; /*!< Flying state (landed, flying, hovering, etc.) defined in CTRL_STATES enum. */
        uint32_t vbat_flying_percentage; /*!< battery voltage filtered (mV) */

        float32_t theta; /*!< UAV's pitch in milli-degrees */
        float32_t phi; /*!< UAV's roll  in milli-degrees */
        float32_t psi; /*!< UAV's yaw   in milli-degrees */

        int32_t altitude; /*!< UAV's altitude in centimeters */

        float32_t vx; /*!< UAV's estimated linear velocity */
        float32_t vy; /*!< UAV's estimated linear velocity */
        float32_t vz; /*!< UAV's estimated linear velocity */

        uint32_t num_frames; /*!< streamed frame index */ // Not used -> To integrate in video stage.

        // Camera parameters compute by detection
        Matrix33 detection_camera_rot; /*!<  Deprecated ! Don't use ! */
        Vector31 detection_camera_trans; /*!<  Deprecated ! Don't use ! */
        uint32_t detection_tag_index; /*!<  Deprecated ! Don't use ! */

        uint32_t detection_camera_type; /*!<  Type of tag searched in detection */

        // Camera parameters compute by drone
        Matrix33 drone_camera_rot; /*!<  Deprecated ! Don't use ! */
        Vector31 drone_camera_trans; /*!<  Deprecated ! Don't use ! */

        /*void accept(class OptionVisitor &visitor){
            visitor.visit(this);
        };*/
    };

    /*----------------------------------------------------------------------------*/

    /**
    * @brief Last navdata option that *must* be included at the end of all navdata packets
    * + 6 bytes
    */
    struct _ATTRIBUTE_PACKED_ NavdataCks : public NavdataOption {
        // Checksum for all navdatas (including options)
        uint32_t cks;
    };

    /*----------------------------------------------------------------------------*/

    /**
    * @brief Timestamp
    * + 6 bytes
    */
    struct _ATTRIBUTE_PACKED_ NavdataTime : public NavdataOption {
        uint32_t time; /*!< 32 bit value where the 11 most significant bits represents the seconds, and the 21 least significant bits are the microseconds. */
    };



    /*----------------------------------------------------------------------------*/

    /**
    * @brief Raw sensors measurements
    */
    struct _ATTRIBUTE_PACKED_ NavdataRawMeasures : public NavdataOption{
        uint16_t raw_accs[ACCELEROMETERS_NUMBER]; // filtered accelerometers
        int16_t raw_gyros[GYROS_NUMBER]; // filtered gyrometers
        int16_t raw_gyros_110[2]; // gyrometers  x/y 110 deg/s
        uint32_t vbat_raw; // battery voltage raw (mV)
        uint16_t us_debut_echo;
        uint16_t us_fin_echo;
        uint16_t us_association_echo;
        uint16_t us_distance_echo;
        uint16_t us_courbe_temps;
        uint16_t us_courbe_valeur;
        uint16_t us_courbe_ref;
        uint16_t flag_echo_ini;
        // TODO:   uint16_t  frame_number; // from ARDrone_Magneto
        uint16_t nb_echo;
        uint32_t sum_echo;
        int32_t alt_temp_raw;
        int16_t gradient;
    };

    // split next struc into magneto_navdata_t and pressure_navdata_t

    struct _ATTRIBUTE_PACKED_ NavdataPressureRaw : public NavdataOption{
        int32_t up;
        int16_t ut;
        int32_t Temperature_meas;
        int32_t Pression_meas;
    };

    struct _ATTRIBUTE_PACKED_ NavdataMagneto : public NavdataOption{
        int16_t mx;
        int16_t my;
        int16_t mz;
        Vector31 magneto_raw; // magneto in the body frame, in mG
        Vector31 magneto_rectified;
        Vector31 magneto_offset;
        float32_t heading_unwrapped;
        float32_t heading_gyro_unwrapped;
        float32_t heading_fusion_unwrapped;
        char magneto_calibration_ok;
        uint32_t magneto_state;
        float32_t magneto_radius;
        float32_t error_mean;
        float32_t error_var;
    };

    struct _ATTRIBUTE_PACKED_ NavdataWindSpeed : public NavdataOption{
        float32_t wind_speed; // estimated wind speed [m/s]
        float32_t wind_angle; // estimated wind direction in North-East frame [rad] e.g. if wind_angle is pi/4, wind is from South-West to North-East
        float32_t wind_compensation_theta;
        float32_t wind_compensation_phi;
        float32_t state_x1;
        float32_t state_x2;
        float32_t state_x3;
        float32_t state_x4;
        float32_t state_x5;
        float32_t state_x6;
        float32_t magneto_debug1;
        float32_t magneto_debug2;
        float32_t magneto_debug3;
    };

    struct _ATTRIBUTE_PACKED_ NavdataKalmanPressure : public NavdataOption{
        float32_t offset_pressure;
        float32_t est_z;
        float32_t est_zdot;
        float32_t est_bias_PWM;
        float32_t est_biais_pression;
        float32_t offset_US;
        float32_t prediction_US;
        float32_t cov_alt;
        float32_t cov_PWM;
        float32_t cov_vitesse;
        bool_t bool_effet_sol;
        float32_t somme_inno;
        bool_t flag_rejet_US;
        float32_t u_multisinus;
        float32_t gaz_altitude;
        bool_t Flag_multisinus;
        bool_t Flag_multisinus_debut;
    };

    // TODO: depreciated struct ? remove it ?

    struct _ATTRIBUTE_PACKED_ NavdataZimmu3000 : public NavdataOption{
        int32_t vzimmuLSB;
        float32_t vzfind;

    };

    struct _ATTRIBUTE_PACKED_ NavdataPhysMeasures : public NavdataOption{
        float32_t accs_temp;
        uint16_t gyro_temp;
        float32_t phys_accs[ACCELEROMETERS_NUMBER];
        float32_t phys_gyros[GYROS_NUMBER];
        uint32_t alim3V3; // 3.3volt alim [LSB]
        uint32_t vrefEpson; // ref volt Epson gyro [LSB]
        uint32_t vrefIDG; // ref volt IDG gyro [LSB]
    };

    struct _ATTRIBUTE_PACKED_ NavdataGyrosOffsets : public NavdataOption{
        float32_t offset_g[GYROS_NUMBER];
    };

    struct _ATTRIBUTE_PACKED_ NavdataEulerAngles : public NavdataOption{
        float32_t theta_a;
        float32_t phi_a;
    };

    struct _ATTRIBUTE_PACKED_ NavdataReferences : public NavdataOption{
        int32_t ref_theta;
        int32_t ref_phi;
        int32_t ref_theta_I;
        int32_t ref_phi_I;
        int32_t ref_pitch;
        int32_t ref_roll;
        int32_t ref_yaw;
        int32_t ref_psi;

        float32_t vx_ref;
        float32_t vy_ref;
        float32_t theta_mod;
        float32_t phi_mod;

        float32_t k_v_x;
        float32_t k_v_y;
        uint32_t k_mode;

        float32_t ui_time;
        float32_t ui_theta;
        float32_t ui_phi;
        float32_t ui_psi;
        float32_t ui_psi_accuracy;
        int32_t ui_seq;
    };

    struct _ATTRIBUTE_PACKED_ NavdataTrims : public NavdataOption{
        float32_t angular_rates_trim_r;
        float32_t euler_angles_trim_theta;
        float32_t euler_angles_trim_phi;
    };

    struct _ATTRIBUTE_PACKED_ NavdataRcReferences : public NavdataOption{
        int32_t rc_ref_pitch;
        int32_t rc_ref_roll;
        int32_t rc_ref_yaw;
        int32_t rc_ref_gaz;
        int32_t rc_ref_ag;
    };

    struct _ATTRIBUTE_PACKED_ NavdataPwm : public NavdataOption{
        uint8_t motor1;
        uint8_t motor2;
        uint8_t motor3;
        uint8_t motor4;
        uint8_t sat_motor1;
        uint8_t sat_motor2;
        uint8_t sat_motor3;
        uint8_t sat_motor4;
        float32_t gaz_feed_forward;
        float32_t gaz_altitude;
        float32_t altitude_integral;
        float32_t vz_ref;
        int32_t u_pitch;
        int32_t u_roll;
        int32_t u_yaw;
        float32_t yaw_u_I;
        int32_t u_pitch_planif;
        int32_t u_roll_planif;
        int32_t u_yaw_planif;
        float32_t u_gaz_planif;
        uint16_t current_motor1;
        uint16_t current_motor2;
        uint16_t current_motor3;
        uint16_t current_motor4;
        //WARNING: new navdata (FC 26/07/2011)
        float32_t altitude_prop;
        float32_t altitude_der;
    };

    struct NavdataAltitude : public NavdataOption{
        int32_t altitude_vision;
        float32_t altitude_vz;
        int32_t altitude_ref;
        int32_t altitude_raw;

        float32_t obs_accZ;
        float32_t obs_alt;
        Vector31 obs_x;
        uint32_t obs_state;
        Vector21 est_vb;
        uint32_t est_state;
    };

    struct _ATTRIBUTE_PACKED_ NavdataVisionRaw : public NavdataOption{
        float32_t vision_tx_raw;
        float32_t vision_ty_raw;
        float32_t vision_tz_raw;
    };

    struct _ATTRIBUTE_PACKED_ NavdataVision : public NavdataOption{
        uint32_t vision_state;
        int32_t vision_misc;
        float32_t vision_phi_trim;
        float32_t vision_phi_ref_prop;
        float32_t vision_theta_trim;
        float32_t vision_theta_ref_prop;

        int32_t new_raw_picture;
        float32_t theta_capture;
        float32_t phi_capture;
        float32_t psi_capture;
        int32_t altitude_capture;
        uint32_t time_capture; // time in TSECDEC format (see config.h)
        Vector31 body_velocities;

        float32_t delta_phi;
        float32_t delta_theta;
        float32_t delta_psi;

        uint32_t gold_defined;
        uint32_t gold_reset;
        float32_t gold_x;
        float32_t gold_y;
    };

    struct _ATTRIBUTE_PACKED_ NavdataVisionPerf : public NavdataOption{
        float32_t time_szo;
        float32_t time_corners;
        float32_t time_compute;
        float32_t time_tracking;
        float32_t time_trans;
        float32_t time_update;
        float32_t time_custom[NAVDATA_MAX_CUSTOM_TIME_SAVE];
    };

    struct _ATTRIBUTE_PACKED_ NavdataTrackersSend : public NavdataOption{
        int32_t locked[DEFAULT_NB_TRACKERS_WIDTH * DEFAULT_NB_TRACKERS_HEIGHT];
        ScreenPoint point[DEFAULT_NB_TRACKERS_WIDTH * DEFAULT_NB_TRACKERS_HEIGHT];
    };

    struct _ATTRIBUTE_PACKED_ NavdataVisionDetect : public NavdataOption{
        /* !! Change the function 'navdata_server_reset_vision_detect()' if this structure is modified !! */
        uint32_t nb_detected;
        uint32_t type[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t xc[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t yc[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t width[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t height[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t dist[NB_NAVDATA_DETECTION_RESULTS];
        float32_t orientation_angle[NB_NAVDATA_DETECTION_RESULTS];
        Matrix33 rotation[NB_NAVDATA_DETECTION_RESULTS];
        Vector31 translation[NB_NAVDATA_DETECTION_RESULTS];
        uint32_t camera_source[NB_NAVDATA_DETECTION_RESULTS];
    };

    struct _ATTRIBUTE_PACKED_ NavdataVisionOf : public NavdataOption{
        float32_t of_dx[5];
        float32_t of_dy[5];
    };

    struct _ATTRIBUTE_PACKED_ NavdataWatchdog : public NavdataOption{
        int32_t watchdog;
    };

    struct _ATTRIBUTE_PACKED_ NavdataAdcDataFrame : public NavdataOption{
        uint32_t version;
        uint8_t data_frame[32];
    };

    struct _ATTRIBUTE_PACKED_ NavdataVideoStream : public NavdataOption{
        uint8_t quant; // quantizer reference used to encode frame [1:31]
        uint32_t frame_size; // frame size (bytes)
        uint32_t frame_number; // frame index
        uint32_t atcmd_ref_seq; // atmcd ref sequence number
        uint32_t atcmd_mean_ref_gap; // mean time between two consecutive atcmd_ref (ms)
        float32_t atcmd_var_ref_gap;
        uint32_t atcmd_ref_quality; // estimator of atcmd link quality

        // drone2
        uint32_t out_bitrate; // measured out throughput from the video tcp socket
        uint32_t desired_bitrate; // last frame size generated by the video encoder

        // misc temporary data
        int32_t data1;
        int32_t data2;
        int32_t data3;
        int32_t data4;
        int32_t data5;

        // queue usage
        uint32_t tcp_queue_level;
        uint32_t fifo_queue_level;
    };

    struct _ATTRIBUTE_PACKED_ NavdataHDVideoStream : public NavdataOption{
        uint32_t hdvideo_state;
        uint32_t storage_fifo_nb_packets;
        uint32_t storage_fifo_size;
        uint32_t usbkey_size; /*! USB key in kbytes - 0 if no key present */
        uint32_t usbkey_freespace; /*! USB key free space in kbytes - 0 if no key present */
        uint32_t frame_number; /*! 'frame_number' PaVE field of the frame starting to be encoded for the HD stream */
        uint32_t usbkey_remaining_time; /*! time in seconds */
    };

    struct _ATTRIBUTE_PACKED_ NavdataGames : public NavdataOption{
        uint32_t double_tap_counter;
        uint32_t finish_line_counter;
    };

    struct _ATTRIBUTE_PACKED_ NavdataWifi : public NavdataOption{
        uint32_t link_quality;
    };

    #if defined(_MSC_VER)
    /* Go back to default packing policy */
        #pragma pack()
    #endif
}}}

#endif