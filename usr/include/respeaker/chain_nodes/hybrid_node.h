/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */



#ifndef __HYBRID_NODE_H__
#define __HYBRID_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{
/**
 * The HybirdNode can do the following: NS(Noise suppresstion), AGC(Automatic gain control) 
 * and (VAD)Voice available detection. These algorithms are all from the famous open-source library: WebRTC.
 * You can find its source code at https://chromium.googlesource.com/external/webrtc/
 * 
 */
class HybridNode : public BaseNode
{
public:
    /**
     * To know the description of the `level`s and `agc_type` , please see the comments of Create*Only methods.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data
     */
    static HybridNode* Create(bool enable_ns, int ns_level,
                              int agc_type, int agc_level,
                              bool enable_vad, int vad_sensitivity,
                              bool output_interleaved=false);

    /**
     * @param ns_level - [0, 3], the higher the more noise suppression
     */
    static HybridNode* CreateNsOnly(int ns_level = 0, bool output_interleaved=false);

    /**
     * @param agc_type - [1, 3], 1 is for AGC_DIGITAL_FIXED, 2 is for AGC_DIGITAL_ADAPTIVE, and 3 is for
     *                           AGC_ANALOG_ADAPTIVE.
     * @param agc_level - [0, 31], Sets the target peak |level| (or envelope) of the AGC in dBFs (decibels
     *                             from digital full-scale). The convention is to use positive values. For
     *                             instance, passing in a value of 10 corresponds to -10 dBFs, or a target
     *                             level 10 dB below full-scale.
     *                           
     */
    static HybridNode* CreateAgcOnly(int agc_type = 3, int agc_level = 10, bool output_interleaved=false);

    /**
     * @param vad_sesitivity - [0, 3] the higher the more sensitive(more noise being detected as voice)
     */
    static HybridNode* CreateVadOnly(int vad_sensitivity = 0, bool output_interleaved=false);

    virtual ~HybridNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string FetchBlock(bool& exit) = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;


};

}  //namespace

#endif // !__HYBRID_NODE_H__

