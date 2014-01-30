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

    static const unsigned int NAVDATA_MAX_SIZE = 4096;

    typedef int32_t bool_t;
    typedef float float32_t;

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

    struct Vector31 {
        union {
            float32_t v[3];

            struct {
                float32_t x;
                float32_t y;
                float32_t z;
            };
        };
    };

    enum NavdataTag{
        NAVDATA_DEMO_TAG=0,
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

    #if defined(_MSC_VER)
        #define _ATTRIBUTE_PACKED_
        /* Asks Visual C++ to pack structures from now on*/
        #pragma pack(1)
    #else
        #define _ATTRIBUTE_PACKED_  __attribute__ ((packed))
    #endif

    struct NavdataOption{
        uint16_t tag;
        uint16_t size;
        #if defined _MSC_VER || defined (__ARMCC_VERSION)
        /* Do not use flexible arrays (C99 feature) with these compilers */
        uint8_t data[1];
        #else
        uint8_t data[];
        #endif
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
    } ;

    /**
    * @brief Minimal navigation data for all flights.
    */
    struct _ATTRIBUTE_PACKED_ NavdataDemo {
        uint16_t tag; /*!< Navdata block ('option') identifier */
        uint16_t size; /*!< set this to the size of this structure */

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
    };

    /*----------------------------------------------------------------------------*/

    /**
    * @brief Last navdata option that *must* be included at the end of all navdata packets
    * + 6 bytes
    */
    struct _ATTRIBUTE_PACKED_ NavdataCks {
        uint16_t tag;
        uint16_t size;

        // Checksum for all navdatas (including options)
        uint32_t cks;
    };

    #if defined(_MSC_VER)
    /* Go back to default packing policy */
        #pragma pack()
    #endif
}}}

#endif