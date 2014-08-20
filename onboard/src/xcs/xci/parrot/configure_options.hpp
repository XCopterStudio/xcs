#ifndef CONFIGURE_OPTIONS_H
#define CONFIGURE_OPTIONS_H

namespace xcs{
namespace xci{
namespace parrot{

    //! Available codecs on AR.Drone 2.0. Use only H264_*. 
    enum CodecType {
        NULL_CODEC = 0,
        UVLC_CODEC = 0x20,       // codec_type value is used for START_CODE
        MJPEG_CODEC,                // not used
        P263_CODEC,                 // not used
        P264_CODEC = 0x40,
        MP4_360P_CODEC = 0x80,
        H264_360P_CODEC = 0x81,
        MP4_360P_H264_720P_CODEC = 0x82,
        H264_720P_CODEC = 0x83,
        MP4_360P_SLRS_CODEC = 0x84,
        H264_360P_SLRS_CODEC = 0x85,
        H264_720P_SLRS_CODEC = 0x86,
        H264_AUTO_RESIZE_CODEC = 0x87,    // resolution is automatically adjusted according to bitrate
        MP4_360P_H264_360P_CODEC = 0x88,

    };

    //! AR.Drone 2.0 bitrate control values. 
    enum BitrateControlMode
    {
        VBC_MODE_DISABLED = 0,  /*<! no video bitrate control */
        VBC_MODE_DYNAMIC,       /*<! video bitrate control active */
        VBC_MANUAL              /*<! video bitrate control active */
    };

}}}

#endif