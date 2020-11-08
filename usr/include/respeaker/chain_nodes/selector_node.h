/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */



#ifndef __SELECTOR_NODE_H__
#define __SELECTOR_NODE_H__

#include "chain_nodes/base_node.h"

namespace respeaker
{

class SelectorNode : public BaseNode
{
public:
    /**
     * Select specified channels from the input stream, and output with same rate and format.
     * 
     * @param channel_indexes - Please note that the channel index starts from `0`.
     * @param output_interleaved - Set it true to output interleaved data, false to output deinterleaved data.
     * 
     * @return SelectorNode*
     */
    static SelectorNode* Create(std::vector<int> channel_indexes, bool output_interleaved = false);

    virtual ~SelectorNode() = default;

    virtual bool OnStartThread() = 0;
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;
    virtual bool OnJoinThread() = 0;

};

}  //namespace

#endif // !__SELECTOR_NODE_H__

