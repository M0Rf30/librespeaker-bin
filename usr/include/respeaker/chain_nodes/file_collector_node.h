/*
 * Copyright (c) 2018 Seeed Technology Co., Ltd.
 *
 * @author JerryYip <chdyejiewei@gmail.com>
 *
 */


#ifndef __FILE_COLLECTOR_NODE_H__
#define __FILE_COLLECTOR_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{

class FileCollectorNode : public BaseNode
{
public:
    /**
     * Create a FileCollectorNode instance. Collect the audio data from a given 8 channels *.wav file.
     *
     * @param wav_file_path - The path of the 8 channels *.wav file.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return FileCollectorNode*
     */
    static FileCollectorNode* Create(std::string wav_file_path, bool output_interleaved=false);

    /**
     * @param wav_file_path - The path of the 8 channels *.wav file.
     * @param block_len_ms - The output block time length, in milliseconds. Default to 8(ms).
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     *
     * @return PulseCollectorNode*
     */
    static FileCollectorNode* Create(std::string pcm_device_name, int block_len_ms, bool output_interleaved=false);

    virtual ~FileCollectorNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string FetchBlock(bool& exit) = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;

    virtual void Pause() = 0;
    virtual void Resume() = 0;
};

}  //namespace

#endif // !__FILE_COLLECTOR_NODE_H__