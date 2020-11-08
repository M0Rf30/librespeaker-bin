/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author JerryYip <chdyejiewei@gmail.com>
 *
 */


#ifndef __ALSA_COLLECTOR_NODE_H__
#define __ALSA_COLLECTOR_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{

class AlsaCollectorNode : public BaseNode
{
public:
    /**
     * Create a AlsaCollectorNode instance. Collect 8 channels audio data stream from Alsa directly.
     *
     * @param pcm_device_name - Users can obtain this by `arecord -L`.
     * @param rate - Set the recording sample rate of your board, and AlsaCollectorNode will resample the recordings to 16KHz. 
     *               For now, the sample rate is fixed in 48000(Hz). User should not change this value.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return AlsaCollectorNode*
     */
    static AlsaCollectorNode* Create(std::string pcm_device_name, int rate, bool output_interleaved=false);

    /**
     * @param pcm_device_name - Users can obtain this by `arecord -L`.
     * @param rate - Set the recording sample rate of your board, and AlsaCollectorNode will resample the recordings to 16KHz. 
     *               For now, the sample rate is fixed in 48000(Hz). User should not change this value.
     * @param block_len_ms - The output block time length, in milliseconds. For now, block_len_ms is fixed in 8(ms).
     *                       User should not change this value.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return AlsaCollectorNode*
     */
    static AlsaCollectorNode* Create(std::string pcm_device_name, int rate, int block_len_ms, bool output_interleaved=false);

    virtual ~AlsaCollectorNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string FetchBlock(bool& exit) = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;

    virtual void Pause() = 0;
    virtual void Resume() = 0;
};

}  //namespace

#endif // !__ALSA_COLLECTOR_NODE_H__