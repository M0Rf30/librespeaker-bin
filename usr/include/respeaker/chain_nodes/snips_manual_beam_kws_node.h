/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author Jerry Yip <chdyejiewei@gmail.com>
 *
 */

#ifndef __SNIPS_MANUAL_BEAM_KWS_NODE_H__
#define __SNIPS_MANUAL_BEAM_KWS_NODE_H__

#include "chain_nodes/base_node.h"
#include "chain_nodes/direction_manager_node.h"
#include "chain_nodes/hotword_detection_node.h"

namespace respeaker
{

/**
 * SnipsManBeamKwsNode is a node for manual doa. Design for user who need to get the speaker direction 
 * with other method(e.g. with camera) and set that direction to this node to pick the beam
 * in that direction.
 * Please note that, this node can only uplink to VepAecBeamformingNode, and is_single_beam_output should be false.
 */
class SnipsManBeamKwsNode : public BaseNode, public DirectionManagerNode, public HotwordDetectionNode
{
public:
    /**
     * @param snips_model_path - The path of snips model.
     * @param snips_sensitivity - [0.1, 0.9] in float format. The higher the sensitivity, the higher the trigger rate.
     *                              Default to 0.5.
     * @param enable_agc - enable Automatic Gain Control.
     * @param enable_kws - enable Keyword Spotting.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *                          At most time, we don't have to set it true in this node. Because the output data
     *                          has only one channel(beam). 
     *
     * @return SnipsManBeamKwsNode*
     */
    static SnipsManBeamKwsNode* Create(std::string snips_model_path,
                                      float snips_sensitivity,
                                      bool enable_agc, 
                                      bool enable_kws,
                                      bool output_interleaved=false);

    virtual ~SnipsManBeamKwsNode() = default;

    /**
     * Time length over which we can confirm that trigger has been post for all beams,
     * then we begin to scoring the triggerd beams, finally calculated the target beam
     *
     * @param ms - milliseconds
     */
    virtual void SetTriggerPostConfirmThresholdTime(int ms) = 0;

    /**
     * Whether or not do AEC when the state machine is in LISTEN* state.
     *
     * @param do_aec_when_listen - The default is true.
     */
    virtual void SetDoAecWhenListen(bool do_aec_when_listen) = 0;

    /**
     * @see ReSpeaker::SetChainState
     *
     */
    virtual void DisableAutoStateTransfer() = 0;

    /**
     * @param dbfs - [0, 31], Sets the target peak |level| (or envelope) of the AGC in dBFs (decibels
     *               from digital full-scale). The convention is to use positive values. For
     *               instance, passing in a value of 3 corresponds to -3 dBFs, or a target
     *               level 3 dB below full-scale. The default is 3.
     */
    virtual void SetAgcTargetLevelDbfs(int dbfs) = 0;

    virtual bool OnStartThread() = 0;
    virtual std::string ProcessBlock(std::string block, bool &exit) = 0;
    virtual bool OnJoinThread() = 0;

    /**
     * Get the most possible direction of arrival(DOA).
     * At manual doa mode, return the manual set direction.
     *
     */
    virtual int GetDirection() = 0;

    /**
     * Doesn't work now. Please use SetBeanNum() to select the output beam.
     */
    virtual void SetDirection(int dir) = 0;


    /**
     * Set the output beam number at manual doa mode.
     * 
     * @param num - For CIRCULAR_6MIC_7BEAM, [0, 6]; For LINEAR_6MIC_8BEAM, [0, 7]; For LINEAR_4MIC_1BEAM, [0, 0];
     *
     */
    virtual bool SetBeamNum(int num) = 0;
    virtual int HotwordDetected() = 0;

};

}   // namespace respeaker

#endif // __SNIPS_MANUAL_BEAM_KWS_NODE_H__

