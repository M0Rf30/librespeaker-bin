/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author Jerry Yip <chdyejiewei@gmail.com>
 *
 */



#ifndef __SNOWBOY_1B_DOA_KWS_NODE_H__
#define __SNOWBOY_1B_DOA_KWS_NODE_H__

#include "chain_nodes/base_node.h"
#include "chain_nodes/direction_manager_node.h"
#include "chain_nodes/hotword_detection_node.h"

namespace respeaker
{

/**
 * Please note that, this node can only uplink to VepAecBeamformingNode, and is_single_beam_output should be true. 
 * Snowboy1bDoaKwsNode only accepts 3 channels data, the first channel is a beam channel, the second channel is 
 * reference channel, and the third channel is some vep scores.
 * This node also provides VAD(Voice available detection).
 */
class Snowboy1bDoaKwsNode : public BaseNode, public DirectionManagerNode, public HotwordDetectionNode
{
public:
    /**
     * Create a Snowboy1bDoaKwsNode instance. Do single-beam keyword(hotword triggering) search with Snowboy KWS Engine 
     * and DoA (direction of arrival).
     * 
     * @param snowboy_resource_path - The path of snowboy resource.
     * @param snowboy_model_path - The path of snowboy model.
     * @param snowboy_sensitivity - [0.1, 0.9], but in string format. The higher the sensitivity, the higher the trigger rate.
     *                              Default to 0.5. 
     *
     * @return Snowboy1bDoaKwsNode*
     */
    static Snowboy1bDoaKwsNode* Create(std::string snowboy_resource_path,
                                 std::string snowboy_model_path,
                                 std::string snowboy_sensitivity);

    /**
     * @param snowboy_resource_path - The path of snowboy resource.
     * @param snowboy_model_path - The path of snowboy model.
     * @param snowboy_sensitivity - [0.1, 0.9], but in string format. The higher the sensitivity, the higher the trigger rate.
     *                              Default to 0.5. 
     * @param underclocking_count - the count of input block this node will wait before processing and passing down.
     */
    static Snowboy1bDoaKwsNode* Create(std::string snowboy_resource_path,
                                 std::string snowboy_model_path,
                                 std::string snowboy_sensitivity,
                                 int underclocking_count);

    /**
     * @param snowboy_resource_path - The path of snowboy resource.
     * @param snowboy_model_path - The path of snowboy model.
     * @param snowboy_sensitivity - [0.1, 0.9], but in string format. The higher the sensitivity, the higher the trigger rate.
     *                              Default to 0.5. 
     * @param underclocking_count - the count of input block this node will wait before processing and passing down.
     * @param enable_agc - enable automatic gain control.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *                          At most time, we don't have to set it true in this node. Because the output data
     *                          has only one channel(beam). 
     *
     * @return Snowboy1bDoaKwsNode*
     */
    static Snowboy1bDoaKwsNode* Create(std::string snowboy_resource_path,
                                    std::string snowboy_model_path,
                                    std::string snowboy_sensitivity,
                                    int underclocking_count,
                                    bool enable_agc, 
                                    bool output_interleaved=false);

    virtual ~Snowboy1bDoaKwsNode() = default;

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

    /**
     *  Whether or not update direction when hotword doesn't trigger. 
     *  @param auto_doa_update - Default is false.
     */
    virtual void SetAutoDoaUpdate(bool auto_doa_update) = 0;

    virtual bool OnStartThread() = 0;
    virtual std::string ProcessBlock(std::string block, bool &exit) = 0;
    virtual bool OnJoinThread() = 0;

    virtual int GetDirection() = 0;
    virtual void SetDirection(int dir) = 0;
    virtual int HotwordDetected() = 0;

};

}  //namespace

#endif  // __SNOWBOY_1B_DOA_KWS_NODE_H__
