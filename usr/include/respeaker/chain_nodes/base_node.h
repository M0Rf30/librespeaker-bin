/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */

#ifndef __BASE_NODE_H__
#define __BASE_NODE_H__

#include <cstddef>
#include <cstdint>
#include <memory>
#include <list>
#include <unordered_map>
#include <queue>
#include <string>
#include <utility>
#include <thread>
#include <chrono>
#include <functional>
#include <atomic>
#include <mutex>
#include <condition_variable>

#include "chain_nodes/chain_shared.h"
#include "chain_nodes/mic_type_info.h"

//This is used to validate the parameter of function BindToCore().
#ifndef NUM_CPU_CORE
#define NUM_CPU_CORE 4
#endif

namespace respeaker
{

/** The type of a node */
enum NodeType {
    BASE_NODE = 0, ///< BaseNode
    PULSE_COLLECTOR_NODE = 10, ///< PulseCollectorNode
    ALSA_COLLECTOR_NODE = 11, ///< AlsaCollectorNode
    FILE_COLLECTOR_NODE = 12, ///< FileCollectorNode
    HYBRID_NODE = 20, ///< HybridNode
    SELECTOR_NODE = 21, ///< SelectorNode
    VEP_AEC_BEAMFORMING_NODE = 30, ///< VepAecBeamformingNode
    SNOWBOY_1B_DOA_KWS_NODE = 40, ///< Snowboy1bDoaKwsNode
    SNOWBOY_MANUAL_BEAM_KWS_NODE = 41, ///< SnowboyManKwsNode
    SNOWBOY_MB_DOA_KWS_NODE = 42, ///< SnowboyMbDoaKwsNode
    SNIPS_1B_DOA_KWS_NODE = 43, ///< Snips1bDoaKwsNode
    SNIPS_MANUAL_BEAM_KWS_NODE = 44, ///< SnipsManBeamKwsNode
    ALOOP_OUTPUT_NODE = 50, ///< AloopOutputNode
};

/** The paramenters for a node's input and output block */
struct NodeParameter
{
    NodeType node_type;  ///< The type of a node
    MicType mic_type;    ///< The microphone array type
    size_t block_len_ms; ///< The time length of the block, in milliseconds
    int rate;            ///< The sample rate
    size_t num_channel;  ///< The number of channels
    bool interleaved;    ///< If or not the audio data is interleaved
};

class BaseNode
{
public:
    BaseNode();
    virtual ~BaseNode() = default;

    /** Get the unique ID of a node */
    BaseNode* GetId();

    /**
     * Link up to the upstream node.
     *
     * @param uplink_node - The pointer of the upstream node.
     */
    void Uplink(BaseNode* uplink_node);

    /**
     * Link down to the downstream node.
     *
     * @param downlink_node - The pointer of the downstream node
     */
    void RegisterDownlinkNode(BaseNode* downlink_node);

    /**
     * Get the output paramenter of this node.
     *
     *
     * @return NodeParameter& - The NodeParamenter struct.
     */
    NodeParameter& GetNodeOutputParameter() { return _output_parameter; }

    /**
     * The `shared_data` must be passed in, typically the `shared_data` is created by the `ReSpeaker` supervisor class.
     *
     * @param shared_data - The pointer of the ChainSharedData struct.
     *
     * @return bool - If `false` is returned , this means that the output paramenter of upstream node doesn't meet the
     *         requirement of a particular node.
     */
    bool RecursivelyStartThread(ChainSharedData* shared_data);

    bool RecursivelyJoinThread();

    /** The derived node must configure the output parameter in this method */
    virtual bool OnStartThread() = 0;

    /**
     * The head node must override this method, to do real audio data fetching
     *
     * @param exit [out] - The exit flag of the ChainSharedData is set, the thread should join now.
     *
     * @return std::string
     */
    virtual std::string FetchBlock(bool& exit);

    /**
     * Every node must implement this method.
     *
     * @param block [in]
     * @param exit [out] - The exit flag of the ChainSharedData is set, the thread should join now.
     *
     * @return std::string
     */
    virtual std::string ProcessBlock(std::string block, bool& exit) = 0;

    /**
     * This method has default implementation, in most times, we can forget this method.
     *
     * @param block [in]
     * @param exit [out] - The exit flag of the ChainSharedData is set, the thread should join now.
     */
    virtual void StoreBlock(std::string block, bool& exit);

    /**
     * Do per node cleanup in this method.
     */
    virtual bool OnJoinThread() = 0;

    virtual void Pause();
    virtual void Resume();

    std::mutex* GetDownlinkDataQueueMutex(BaseNode* downlink_node_id = nullptr);
    std::condition_variable* GetDownlinkDataQueueConditionVar(BaseNode* downlink_node_id = nullptr);
    std::queue<std::string>& GetDownlinkDataQueue(BaseNode* downlink_node_id = nullptr);

    /**
     * Get the average deepth of the output queues.
     * This is supposed to be a debugging method, to see the load of chain.
     */
    int GetQueueDeepth();

    /**
     * Bind the thread to a specified CPU core. Must be callbed before `RecursivelyStartThread`. Don't touch this method
     * unless you know what's happening.
     */
    bool BindToCore(int core_index);

    /**
     * Set the priority of the thread. Must be callbed before `RecursivelyStartThread`.
     *
     * @param priority - [1,99], 99 has highest priority, the default is 50.
     *
     * @return bool
     */
    bool SetThreadPriority(int priority);

private:
    void _InitBeforeStart();
    void _ThreadProc();
    
    std::unique_ptr<std::thread> _thread;
    int _thread_priority;

    std::unordered_map<BaseNode*, std::unique_ptr<std::mutex>> _map_data_queue_mutexes;
    std::unordered_map<BaseNode*, std::unique_ptr<std::condition_variable>> _map_data_queue_cvs;
    std::unordered_map<BaseNode*, std::queue<std::string>> _map_downlink_data_queues;

    bool _paused;
    int _core_index;
    bool _enable_queue_flush;

protected:
    /** The down-node requests data from up-node, the ownership is: down-node explicitly owns up-node */
    BaseNode* _uplink_node = nullptr;

    /** The downlink path is only used to do recursive calls */
    std::list<BaseNode*> _list_downlink_nodes;

    NodeParameter _input_parameter;
    NodeParameter _output_parameter;
    int _system_check = 0;

    ChainSharedData* _chain_shared_data = nullptr;
    bool _is_head = false;
    bool _is_tail = false;

    bool _is_interleaved_after_process = false;
    int _num_frames_for_store_block;
    int _channels_for_store_block;
    int16_t* _buffer_for_store_block;
    std::unique_ptr<int16_t* []>_channels_ptr;

    /**
     * Enable or disable flush the output queue. This method can only be callbed in `ProcessBlock`. If the deepth of queue
     * gets crazy big, this is a disaster, we need to flush the queue anyway to avoid the chain entering unstable.
     */
    void EnableQueueFlush(bool enable);
    void _Interleave(const int16_t* const* deinterleaved, size_t num_frames, size_t num_channels, int16_t* interleaved);
    void _Deinterleave(const int16_t* interleaved, size_t num_frames, size_t num_channels, int16_t* const* deinterleaved);
    void _Deinterleave(const float* interleaved, size_t num_frames, size_t num_channels, float* const* deinterleaved);

};

} // namespace respeaker

#endif // !__NODE_BASE_H__
