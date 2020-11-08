/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */



#ifndef __PULSE_COLLECTOR_NODE_H__
#define __PULSE_COLLECTOR_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{

class PulseCollectorNode : public BaseNode
{
public:
    /**
     * Create a PulseCollectorNode instance. Collect 8 channels audio data stream from PulseAudio server.
     *
     * @param source_name - Users can obtain this by `pactl list sources`. 
     * @param rate - Set the recording sample rate of your board, and PulseCollectorNode will resample the recordings to 16KHz.
     *               For now, the sample rate is fixed in 48000(Hz). User should not change this value.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return PulseCollectorNode*
     */
    static PulseCollectorNode* Create(std::string source_name, int rate, bool output_interleaved=false);

    /**
     * @param source_name - Users can obtain this by `pactl list sources`.
     * @param block_len_ms - The output block time length, in milliseconds. For now, block_len_ms is fixed in 8(ms).
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return PulseCollectorNode*
     */
    static PulseCollectorNode* Create_48Kto16K(std::string source_name, int block_len_ms, bool output_interleaved=false);

    /**
     * @param source_name - Users can obtain this by `pactl list sources`.
     * @param rate - Set the recording sample rate of your board, and PulseCollectorNode will resample the recordings to 16KHz.
     *               For now, the sample rate is fixed in 48000(Hz). User should not change this value.
     * @param block_len_ms - The output block time length, in milliseconds. For now, block_len_ms is fixed in 8(ms).
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return PulseCollectorNode*
     */
    static PulseCollectorNode* Create(std::string source_name, int rate, int block_len_ms, bool output_interleaved=false);

    virtual ~PulseCollectorNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string FetchBlock(bool& exit) = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;

    virtual void Pause() = 0;
    virtual void Resume() = 0;
};

}  //namespace

#endif // !__PULSE_COLLECTOR_NODE_H__