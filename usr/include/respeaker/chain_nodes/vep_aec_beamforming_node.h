/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author Jerry Yip <chdyejiewei@gmail.com>
 *
 */



#ifndef __VEP_AEC_BEAMFORMING_NODE_H__
#define __VEP_AEC_BEAMFORMING_NODE_H__

#include "chain_nodes/base_node.h"
#include "chain_nodes/mic_type_info.h"

namespace respeaker
{

/**
 * This VepAecBeamformingNode provides beamforming, AEC, NR and DOA algorithms from Alango.
 */
class VepAecBeamformingNode : public BaseNode
{
public:
    /**
     * Create a VepAecBeamformingNode instance. Do beamforming, AEC(acoustic echo cancellation), NR(noise reduction) and a part of DOA(direction of arrival) on the input audio stream,
     * output the most proper beam(single-beam) or all the beams(multi-beam).
     * 
     * @param mic_type - Specify the microphone type.
     * @param is_single_beam_output - Specify output single-beam(the most proper beam) or multi-beam(all the beams). When set to 
     *                                output single-beam, this node actually outputs 3 channels data, the first channel is the 
     *                                most proper beam channel, the second channel is reference channel, and the third channel 
     *                                is some vep scores. 
     *                                When set to output multi-beam, this node outputs all the beams and a reference channel and
     *                                a vep-score channel.
     * @param ref_channel_index - Please note that the channel index starts from `0`. For ReSpeaker v2, it's better to
     *                            specify `6` here.
     *
     * @return VepAecBeamformingNode*
     */
    static VepAecBeamformingNode* Create(MicType mic_type , bool is_single_beam_output, int ref_channel_index);

    /**
     * @param mic_type - Specify the microphone type.
     * @param is_single_beam_output - Specify output single-beam(the most proper beam) or multi-beam(all the beams).
     * @param ref_channel_index - Please note that the channel index starts from `0`. For ReSpeaker v2, it's better to
     *                          specify `6` here.
     * @param enable_wav_log - Set it true to save all the input channels and output beams into wav files(for debug), default to false.
     *
     * @return VepAecBeamformingNode*
     */
    static VepAecBeamformingNode* Create(MicType mic_type , bool is_single_beam_output, int ref_channel_index,  bool enable_wav_log);

    virtual ~VepAecBeamformingNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string ProcessBlock(std::string block, bool &exit) = 0;
    virtual bool OnJoinThread() = 0;


    /**
     * Set the angle for microphone 0, if it's not `0` for the board. Only works for circular microphone array.
     *
     * @param angle - degree
     */
    virtual void SetAngleForMic0(int angle) = 0;

};

}  //namespace

#endif
