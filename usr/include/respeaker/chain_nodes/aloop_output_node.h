/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author JerryYip <chdyejiewei@gmail.com>
 *
 */


#ifndef __ALOOP_OUTPUT_NODE_H__
#define __ALOOP_OUTPUT_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{

class AloopOutputNode : public BaseNode
{
public:
    /**
     * Create a AloopOutputNode instance. Output audio stream to a specific Alsa device(eg. "hw:Loopback,0,0"). 
     * In this case, the third party ASR engine(eg. Snips, AVS) can capture the processed audio stream from Alsa
     * (eg. "hw:Loopback,1,0") directly. Need to run "sudo modprobe snd-aloop" to enable aloop module first.
     * 
     * Note that the sample rate and channel number of the audio stream, are the same as the uplink node. And the format is 
     * fixed to S16_LE. And the period/buffer time of Alsa PCM handle is set to 32/128 ms by default. Use large period/buffer time
     * will get large latency but good cpu usage, use low period/buffer time will get superb latency but heavy cpu usage.
     * For more information about period/buffer time, please check this: https://www.alsa-project.org/alsa-doc/alsa-lib/_2test_2latency_8c-example.html
     *
     * @param output_device_name - Users can obtain this by `aplay -L`. 
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return AloopOutputNode*
     */
    static AloopOutputNode* Create(std::string output_device_name, bool output_interleaved=false);

    /**
     * @param period_time_ms - Set period time of Alsa PCM handle, period time should be less than buffer time, default to 32ms
     * @param buffer_time_ms - Set buffer time of Alsa PCM handle, default to 128ms
     *
     * @return AloopOutputNode*
     */
    static AloopOutputNode* Create(std::string output_device_name, int period_time_ms, int buffer_time_ms, bool output_interleaved=false);

    virtual ~AloopOutputNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string FetchBlock(bool& exit) = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;

    /**
     * Set the "max block delay time" of this node. The "max block delay time" means the max acceptable latency from the queue of uplink node.
     * As we known, this node get audio data(we call it "block") from the output queue of its uplink node. When the queue contains some blocks, 
     * for example 10 blocks, and each block has 40ms audio data, that means this aloop node has 10*40ms=400ms "block delay time". Under a high
     * "block delay time"(eg. more than 400ms), your assistant will become "slow" and easy to make mistake. So we need to drain(drop) the audio data 
     * in queue when this situation happens(when the length of audio data is larger than the "max block delay time").
     * Note that this situation will not always happen. 
     * Note that this function should be called after OnStartThread().
     * 
     * @param ms - "max block delay time" in ms. Defalut to (5 * block_len_ms of uplink NodeParameter)ms.
     *             The minimum value is (2 * block_len_ms of uplink NodeParameter)ms.
     *
     * @return int - The actually "max block delay time" in ms.
     */
    virtual int SetMaxBlockDelayTime(int ms) = 0;
};

}  //namespace

#endif // !__ALOOP_OUTPUT_NODE_H__