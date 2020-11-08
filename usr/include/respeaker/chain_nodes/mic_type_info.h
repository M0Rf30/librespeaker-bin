/**
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author Jerry Yip <chdyejiewei@gmail.com>
 *
 */


#ifndef __MIC_TYPE_INFO_H__
#define __MIC_TYPE_INFO_H__

#include <string>

namespace respeaker
{

/** The type of microphone array */
enum MicType {
    CIRCULAR_6MIC_7BEAM = 0, ///< Corresponding hardware: ReSpeaker Core v2, ReSpeaker 6-Mic Circular Array Kit for Raspberry Pi
    LINEAR_6MIC_8BEAM, ///< Corresponding hardware: Coming soon, NOT SUPPORTED YET
    LINEAR_4MIC_1BEAM, ///< Corresponding hardware: ReSpeaker 4-Mic Linear Array Kit for Raspberry Pi, NOT SUPPORTED YET
    CIRCULAR_4MIC_9BEAM, ///< Corresponding hardware: ReSpeaker 4-Mic Array for Raspberry Pi, NOT SUPPORTED YET
};

/** The data structure which contains the information of a microphone array */
typedef struct MicTypeInfo_
{
    MicTypeInfo_() : num_of_mics(0),
                     num_of_beams(0),
                     num_of_directional_beams(0),
                     num_of_auxiliary_beams(0),
                     num_of_central_mics(0),
                     geometries(0),
                     pick_up_voice_degree(0),
                     frame(0),
                     init_flag(false){};

    int num_of_mics;                    ///< This number doesn't contain ref-channel, the ref-channel needs to specify when create node.
    int num_of_beams;                   ///< Equal to (num_of_directional_beams + num_of_auxiliary_beams)
    int num_of_directional_beams;       ///< Number of directional beamforming beams 
    int num_of_auxiliary_beams;         ///< Number of auxiliary non-beamforming beams
    int num_of_central_mics;            ///< For some kinds of circular microphone array, there is 1 central microphone on the plane.
    int geometries;                     ///< 0 for circular, 1 for linear
    int pick_up_voice_degree;           ///< Circular Mic Array can do 360 degree voice pick-up, while Linear Array can do 180 degree voice pick-up.
    int frame;                          ///< size of int16 buffer: mic/spk/mout, should be equal to num_of_beams * frame_cnt_per_beam(=128)
    bool init_flag;                     ///< Set this flag to true when finish setting params

    MicType mic_type;                   ///< Type of microphone

} MicTypeInfo;

/**
 * Set data structrue MicTypeInfo
 *
 * @param mic_type - The MicType you want to set.
 * @param mic_type_info - A reference of MicTypeInfo
 *
 * @return bool
 */
static bool SetMicTypeInfo(MicType mic_type, MicTypeInfo& mic_type_info) {
    bool is_set = false;

    switch(mic_type) {
        case CIRCULAR_6MIC_7BEAM:
            mic_type_info.num_of_mics = 6;
            mic_type_info.num_of_beams = 7;
            mic_type_info.num_of_directional_beams = 6;
            mic_type_info.num_of_auxiliary_beams = 1;
            mic_type_info.geometries = 0;
            mic_type_info.num_of_central_mics = 0;
            mic_type_info.pick_up_voice_degree = 360;
            mic_type_info.mic_type = CIRCULAR_6MIC_7BEAM;
            mic_type_info.init_flag = true;
            // Vep has fixed 8ms block size, so 128 = 16K / 1000 * 8
            // "*2" to ensure enough space 
            mic_type_info.frame = mic_type_info.num_of_beams * 128 * 2;    
            is_set = true;
            break;

        case LINEAR_6MIC_8BEAM:
            mic_type_info.num_of_mics = 6;
            mic_type_info.num_of_beams = 8;
            mic_type_info.num_of_directional_beams = 7;
            mic_type_info.num_of_auxiliary_beams = 1;
            mic_type_info.geometries = 1;
            mic_type_info.num_of_central_mics = 0;
            mic_type_info.pick_up_voice_degree = 180;
            mic_type_info.mic_type = LINEAR_6MIC_8BEAM;
            mic_type_info.init_flag = true;
            // Vep has fixed 8ms block size, so 128 = 16K / 1000 * 8
            // "*2" to ensure enough space 
            mic_type_info.frame = mic_type_info.num_of_beams * 128 * 2;                
            is_set = true;
            break;

        case LINEAR_4MIC_1BEAM:
            mic_type_info.num_of_mics = 4;
            mic_type_info.num_of_beams = 1;
            mic_type_info.num_of_directional_beams = 1;
            mic_type_info.num_of_auxiliary_beams = 0;
            mic_type_info.geometries = 1;
            mic_type_info.num_of_central_mics = 0;
            mic_type_info.pick_up_voice_degree = 180;
            mic_type_info.mic_type = LINEAR_4MIC_1BEAM;
            mic_type_info.init_flag = true;
            // Vep has fixed 8ms block size, so 128 = 16K / 1000 * 8
            // "*2" to ensure enough space 
            mic_type_info.frame = mic_type_info.num_of_beams * 128 * 2;                
            is_set = true;
            break;

        case CIRCULAR_4MIC_9BEAM:
            mic_type_info.num_of_mics = 4;
            mic_type_info.num_of_beams = 9;
            mic_type_info.num_of_directional_beams = 8;
            mic_type_info.num_of_auxiliary_beams = 1;
            mic_type_info.geometries = 0;
            mic_type_info.num_of_central_mics = 0;
            mic_type_info.pick_up_voice_degree = 360;
            mic_type_info.mic_type = CIRCULAR_4MIC_9BEAM;
            mic_type_info.init_flag = true;
            // Vep has fixed 8ms block size, so 128 = 16K / 1000 * 8
            // "*2" to ensure enough space 
            mic_type_info.frame = mic_type_info.num_of_beams * 128 * 2;                
            is_set = true;
            break;

        default:
            break;
    }

    return is_set;
}

/**
 * Get MicType from string
 *
 * @param mic_type_str - A string name of MicType
 *
 * @return MicType
 */
static MicType StringToMicType(std::string mic_type_str) {

    if (mic_type_str == "LINEAR_6MIC_8BEAM") {
        return LINEAR_6MIC_8BEAM;
    }
    else if (mic_type_str == "LINEAR_4MIC_1BEAM") {
        return LINEAR_4MIC_1BEAM;
    }
    else if (mic_type_str == "CIRCULAR_6MIC_7BEAM") {
        return CIRCULAR_6MIC_7BEAM;
    }
    else if (mic_type_str == "CIRCULAR_4MIC_9BEAM") {
        return CIRCULAR_4MIC_9BEAM;
    }
    else {
        // default type
        return CIRCULAR_6MIC_7BEAM;
    }
}


} // respeaker


#endif //__MIC_TYPE_INFO_H__
