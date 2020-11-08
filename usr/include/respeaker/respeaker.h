/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */


#ifndef __RESPEAKER_H__
#define __RESPEAKER_H__

#include <string>

#include "chain_nodes/base_node.h"
#include "chain_nodes/chain_shared.h"
#include "chain_nodes/direction_manager_node.h"
#include "chain_nodes/hotword_detection_node.h"

/**
 * @mainpage
 *
 * The `librespeaker` is an audio processing library which can perform
 * noise suppression, direction of arrival calculation, beamforming, hotword searching. It reads the microphoone stream
 * from linux sound server, e.g. PulseAudio. It exposes a few APIs which enable users to get indicated when hotword is
 * said and the processed microphone data in PCM format, which then can be sent to cloud services like Alexa for further
 * processing.
 *
 * From the top view, `librespeaker` has two parts:
 * - Audio processing (filter) chain
 * - A supervisor which manages the chain, getting events and outputs from the chain, this supervisor is implemented by
 *   the `ReSpeaker` class.
 *
 * ## Audio processing chain
 *
 * Namely the chain consists of several nodes which link together in a user specified order. A node can be considered as
 * a blackbox which has input and output, and internally applies filter algorithms to the audio stream passing through.
 * The input / output is a `block` of audio data, which is specified by some parameters. See respeaker::NodeParameter.
 * Please note that some nodes may accept particular length of block, e.g. the respeaker::VepAecBeamformingNode accepts
 * only 8 milliseconds block, this is decided by the algorithms. Generally users don't need to specially care about
 * these restrictions, as we utilized the nodes into another application - named `respeakerd`, which is a typicall
 * server application for the ReSpeaker v2 hardware with tuned configurations.
 *
 * The list of nodes:
 * - respeaker::PulseCollectorNode - collect the audio data from PulseAudio.
 * - respeaker::AlsaCollectorNode - collect the audio data from Alsa directly.
 * - respeaker::FileCollectorNode - collect the audio data from a given *.wav file.
 * - respeaker::SelectorNode - select specified channels from the input stream, and output with same rate and format.
 * - respeaker::VepAecBeamformingNode - do beamforming, AEC(acoustic echo cancellation), NR(noise reduction) and 
 *   a part of DOA(direction of arrival) on the input audio stream,output the most proper beam(single-beam) or 
 *   all the beams(multi-beam). These algorithms are provided by Alango.
 * - respeaker::Snowboy1bDoaKwsNode - do single-beam keyword(hotword triggering) search with Snowboy KWS Engine and 
 *   DoA (direction of arrival) and VAD(Voice available detection).
 * - respeaker::SnowboyMbDoaKwsNode - do multi-beam keyword search with Snowboy KWS Engine and DoA and VAD(Voice available detection), 
 *   note that this node only works on ReSpeaker Core v2.
 * - respeaker::SnowboyManKwsNode - this node don't do DoA, you need to select the beam manually to pick the voice audio.
 * - respeaker::Snips1bDoaKwsNode - do single-beam keyword search with Snips KWS Engine and DoA.
 * - respeaker::SnipsManBeamKwsNode - this node don't do DoA, you need to select the beam manually to pick the voice audio.
 * - respeaker::HybridNode - provide NS(Noise suppresstion), AGC(Automatic gain control) and VAD(Voice available detection)
 *   from WebRTC library.
 * - respeaker::AloopOutputNode - output audio stream to a specific Alsa device(eg. "hw:Loopback,0,0").
 * - may have more in the future
 *
 * Each node inherits from one or more interface classes:
 * - respeaker::BaseNode - defines the basic behaviours of a node, including fetch/process/store data block, thread
 *   start/stop, etc...
 * - respeaker::DirectionManagerNode - defines an interface of getting DoA result and setting direction.
 * - respeaker::HotwordDetectionNode - defines an interface of getting hotword trigger event.
 *
 * The nodes are linked together by calling the `Uplink` method, please see the examples to know how to link up.
 *
 * There's a data structure shared between all the nodes of the chain - respeaker::ChainSharedData. This structure is
 * used to pass status through the chain, the status are:
 * - bool exit_flag - a flag to indicate that all the threads in the chain should exit immediately
 * - ChainState state - the state of the chain, please see respeaker::ChainState
 * - bool vad_node_present - to indicate if there's a node with ability to detect voice activity in the chain
 * - bool vad - if the last processed block of the node who can do Vad contains active voice
 *
 * The items in this shared structure are partially used by internal, part of them are exposed to APIs for users to
 * call, with the supervisor methods.
 *
 *
 * ## Supervisor - the `ReSpeaker` class
 *
 * After creation of the chain, we need to register the chain into a supervisor - respeaker::ReSpeaker. The supervisor
 * exposes the methods to user calls. Users don't need to directly operate the nodes then. This is done by registering
 * the following node pointers:
 * - The pointer of the head node
 * - The pointer of the output node - The chain may have branches, saying the chain may have more than one tails, so
 *   it's necessary to tell the supervisor which tail is to output the processed audio stream.
 * - The pointer of the node which does DoA
 * - The pointer of the node which can output the event of hotword triggering
 *
 * After all these done, we have a topological graph like this (this is an exmaple):
 *
 * ```code
 * +--------------+         +---------------+         +-----------+
 * |collector node| +-----> |processing node+-------> |output node|
 * +------+-------+ |       +---------------+         +-----+-----+
 *        |         |                       |               |
 *        |         |       +------------+  |               |
 *        |         +-----> |event node  |  |               |
 *        |                 +------+-----+  |               |
 *        |                        |        |               |
 *        |                        |        |               |
 * +------v------------------------v--------v---------------v-----+
 * |                         supervisor                           |
 * +--------------------------------------------------------------+
 * ```
 *
 * Then we call the `Start` method of the supervisor to start the threads and processing, and call other methods like
 * `DetectHotword` or `Listen` to get the outputs. Please see respeaker::ReSpeaker to know the methods it provides.
 *
 * ## Next Step
 *
 * Please briefly see each class's methods, and then go to the examples.
 * To compile the examples: g++ EXAMPLE_NAME.cc -o EXAMPLE_NAME -lrespeaker -lsndfile -fPIC -std=c++11 -fpermissive -I/usr/include/respeaker/ -DWEBRTC_LINUX -DWEBRTC_POSIX -DWEBRTC_NS_FLOAT -DWEBRTC_APM_DEBUG_DUMP=0 -DWEBRTC_INTELLIGIBILITY_ENHANCER=0
 */

/**
 * namesapce respeaker, the only namespace exists in this software.
 */
namespace respeaker
{
/* the values are copied from <log4cplus/loglevel.h> */
enum LogLevel {
    OFF_LOG_LEVEL = 60000, /** Turn off all logs */
    FATAL_LOG_LEVEL = 50000, /** Only print fatal logs */
    ERROR_LOG_LEVEL = 40000, /** Only print error logs */
    WARN_LOG_LEVEL = 30000, /** Only print warnning logs */
    INFO_LOG_LEVEL = 20000, /** Print info level logs */
    DEBUG_LOG_LEVEL = 10000, /** Print debug level logs */
    TRACE_LOG_LEVE = 0 /** Print all level logs, including some trace information */
};



/**
 * The `ReSpeaker` class is a container/supervisor of node classes.
 *
 * `ReSpeaker` maintains the following nodes:
 * - Head node - which typically is a collector node and used to access the whole chain
 * - DirectionManager node - this is an interface class, nodes derived from it can report DoA direction or set DoA direction
 * - HotwordDetection node - this is an interface class, nodes derived from it can report hotword trigger event
 * - Output node - from which we fetch final stream
 */
class ReSpeaker
{
public:
    /**
     * Create an instance of `ReSpeaker` class. This is a static method, you may call this using:
     * ```cpp
     * ReSpeaker::Create(INFO_LOG_LEVEL)
     * ```
     *
     * @param log_level - One of respeaker::LogLevel, the default is DEBUG_LOG_LEVEL.
     *
     * @return ReSpeaker* - The pointer of the instance.
     */
    static ReSpeaker* Create(LogLevel log_level = DEBUG_LOG_LEVEL);

    /**
     * Create an instance of `ReSpeaker` class. This is a static method, you may call this using:
     * ```cpp
     * ReSpeaker::Create(INFO_LOG_LEVEL)
     * ```
     *
     * @param log_level - One of respeaker::LogLevel
     * @param log_conf_file - The path to the configuration file, which follows the syntax of log4cplus.
     *
     * @return ReSpeaker*
     */
    static ReSpeaker* Create(LogLevel log_level, std::string log_conf_file);

    virtual ~ReSpeaker() = default;

    /**
     * Set the log level after creation of the ReSpeaker instance.
     *
     * @param log_level - One of respeaker::LogLevel
     */
    virtual void SetLogLevel(LogLevel log_level) = 0;

    /**
     * Register the head node into the ReSpeaker supervisor.
     *
     * @param head_node - The pointer to the head node
     */
    virtual void RegisterChainByHead(BaseNode* head_node) = 0;

    /**
     * Register the node which does DoA
     *
     * @param dir_manager_node - The pointer of the node
     */
    virtual void RegisterDirectionManagerNode(DirectionManagerNode* dir_manager_node) = 0;

    /**
     * Register the node which does hotword search.
     *
     * @param hotword_detection_node - The pointer of the node
     */
    virtual void RegisterHotwordDetectionNode(HotwordDetectionNode* hotword_detection_node) = 0;

    /**
     * Register the output node
     *
     * @param output_node - The pointer of the node
     */
    virtual void RegisterOutputNode(BaseNode* output_node) = 0;

    /**
     * Start the theads
     *
     * @param interrupt - The pointer to a boolean variable, when the value of this variable comes to `true`, the loops
     *                  inside will be interupted immediately.
     *
     * @return bool - If `false` is returned, it means the parameter of the input of some node doesn't meet the requirements
     *         of that node.
     */
    virtual bool Start(bool* interrupt) = 0;

    /**
     * Stop the threads with safe. Please note that there might be some delay before all the threads join, since some of
     * them will actually exit when they come active from sleeping.
     */
    virtual bool Stop() = 0;

    /**
     * Check if the keyword has been detected.
     * Need to iterate this function quickly to drain the stream buffer if you don't call the `Listen` method to drain it.
     *
     * @return int - Whether or not the keyword has been detected. -2: Silence. -1: Error. 0: No event. 1: Hotword 1 triggered.
     *             2: Hotword 2 triggered. 3 or More: Hotword 3 or other index of the hotword triggered.
     */
    virtual int DetectHotword() = 0;

    /**
     * Check if the keyword has been detected.
     * Need to iterate this function quickly to drain the stream buffer if you don't call the `Listen` method to drain it.
     *
     * @param detected [Out] - Whether or not the keyword has been detected. -2: Silence. -1: Error. 0: No event. 1: Hotword 1 triggered.
     *                       2: Hotword 2 triggered. 3 or More: Hotword 3 or other index of the hotword triggered.
     *
     * @return const std::string - The audio block being processed is returned.
     */
    virtual const std::string DetectHotword(int& detected) = 0;

    /**
     * Fetch the stream data for a duration of `block_time_length_ms` millisenconds.
     *
     * @param block_time_length_ms - How many milliseconds of data you want to fetch.
     *
     * @return const string - the stream data block
     */
    virtual const std::string Listen(int block_time_length_ms) = 0;

    /**
     * Fetch the stream data, the length is the output node's default output length.
     * @return const string - the stream data block
     */
    virtual const std::string Listen() = 0;

    /**
     * Fetch a whole data chunk of a sentence which is splited out by a `cmd_timeout_ms` silence gap and which has the
     * maximum length of `max_cmd_time_ms`.
     *
     * @return const string - the stream data chunk
     */
    virtual const std::string ListenToSilence(int cmd_silence_gap_ms = 3000,
                                              int cmd_max_timeout_ms = 10000) = 0;

    /**
     *
     * @param DirectionReportCallback - The callback will be running in the same thread as the caller's thread, typically
     *                                the main thread of an application.
     * @param cmd_silence_gap_ms - How many milliseconds of silence will be waited before we consider the sentence is over.
     * @param cmd_max_timeout_ms - Over this length of time, the sentence will be cut even there's still valid voice.
     *
     * @return const std::string - the stream data chunk
     */
    virtual const std::string ListenToSilence(void (*DirectionReportCallback)(int),
                                             int cmd_silence_gap_ms = 3000,
                                             int cmd_max_timeout_ms = 10000) = 0;

    /**
     * Get the direction of DoA.
     *
     * @return int - The degree of direction, [0, 360]
     */
    virtual int GetDirection() = 0;

    /**
     * Set the direction of DoA. (Not used, reserve)
     *
     * @param dir - The degree of direction, [0, 360]
     */  
    virtual void SetDirection(int dir) = 0;

    /**
     * Get if there is active voice at the current time in the processed stream.
     * 
     * @return bool - When someone is speaking, it returns true. 
     */
    virtual bool GetVad() =  0;

    /**
     * Set the state machine of the audio processing chain. The state of the chain will be changed automatically to
     * `LISTEN_QUIETLY` or `LISTEN_WITH_BGM` by the hot word detection node when hot word triggered. Users should however
     * pay attention to switch the state of the chain back to `WAIT_TRIGGER_QUIETLY` or `WAIT_TRIGGER_WITH_BGM` when
     * listened audio has been processed, uploaded to cloud ASR for an example, otherwise the trigger isn't gonna to happen
     * again. This is usefully when users implement application logic like this:
     * - wait trigger
     * - when triggered, fetch audio block by `HotwordDection` or `Listen`, upload audio to cloud ASR, in the background,
     *   some algorithms may be skipped to lower the CPU load since the chain knows it doen't need to do those algorithms(
     *   e.g. hotword searching) by sensing the state machine.
     * - cloud ASR return `stop capture` event
     * - `SetChainState` back to WAIT_TRIGGER_*
     *
     * @note
     * In some cases, if the cloud ASR can accept constant audio stream, users can disable the automatic state transfer by
     * calling `DisableAutoStateTransfer` of the hotword detection node if provided. After disabled, there's no need to call
     * this method in anywhere.
     *
     * @param state - enum respeaker::ChainState in chain_shared.h
     *
     * @see respeaker::ChainState
     */
    virtual void SetChainState(ChainState state) = 0;

    /**
     * Pause the collector node
     */
    virtual void Pause() = 0;

    /**
     * Resume the audio collection of the collector node
     */
    virtual void Resume() = 0;

    virtual ChainSharedData* GetChainSharedDataPtr() = 0;

    /**
     * Get the number of channels of the output node.
     */
    virtual size_t GetNumOutputChannels() = 0;

    /** Get the number of rate of the output node. */
    virtual int GetNumOutputRate() = 0;

};

/** @example pulse_base_test.cc
 * This is an exmaple that shows the basic usage of the `PulseCollectorNode` node, the `SelectorNode` node and the
 * `ReSpeaker` supervisor.
 */

/** @example pulse_agc_test.cc
 * This is an exmaple that shows the basic usage of the `PulseCollectorNode` node, the `SelectorNode` node, the
 * `HybridNode` node and the `ReSpeaker` supervisor.
 */

/** @example alsa_base_test.cc
 * This is an exmaple that shows the basic usage of the `AlsaCollectorNode` node, the `SelectorNode` node, and the 
 * `ReSpeaker` supervisor.
 */

/** @example alsa_agc_test.cc
 * This is an exmaple that shows the basic usage of the `AlsaCollectorNode` node, the `SelectorNode` node, the
 * `HybridNode` node and the `ReSpeaker` supervisor.
 */

/** @example file_base_test.cc
 * This is an exmaple that shows the basic usage of the `FileCollectorNode` node and the `ReSpeaker` supervisor.
 */

/** @example pulse_snowboy_1b_test.cc
 * This is an exmaple that shows how to use the `VepAecBeamformingNode` node, the `Snowboy1bDoaKwsNode` node and the
 * `ReSpeaker` supervisor, to make a simple snowboy KWS demo. This example supports all the microphone type, 
 * keyword "alexa" and "snowboy", adjustable target gain level and wav log. 
 */

/** @example pulse_snowboy_1b_multi_hotword_vad_test.cc
 * This is an exmaple that shows how to setup multi-hotword and get vad status with `Snowboy1bDoaKwsNode` node.
 */

/** @example pulse_snowboy_mb_test.cc
 * This is an exmaple that shows how to use the `VepAecBeamformingNode` node, the `SnowboyMbDoaKwsNode` node and the
 * `ReSpeaker` supervisor. Different with `Snowboy1bDoaKwsNode`, `SnowboyMbDoaKwsNode` provides more accurate DoA, 
 * hence it requires more calculation. For now, this example only supports CIRCULAR_6MIC_7BEAM on ReSpeaker Core v2. 
 */

/** @example pulse_snowboy_manual_beam_test.cc
 * This is an exmaple that shows how to use the `VepAecBeamformingNode` node, the `SnowboyManKwsNode` node and the
 * `ReSpeaker` supervisor, to select the beam manually.
 */

/** @example pulse_snips_1b_test.cc
 * This is an exmaple that shows how to use the `VepAecBeamformingNode` node, the `Snips1bDoaKwsNode` node and the
 * `ReSpeaker` supervisor, to make a simple snips KWS demo. This example supports all the microphone type, 
 * keyword "hey-snips", adjustable target gain level and wav log. 
 */

/** @example pulse_snips_manual_beam_test.cc
 * This is an exmaple that shows how to use the `VepAecBeamformingNode` node, the `SnipsManBeamKwsNode` node and the
 * `ReSpeaker` supervisor. The `SnipsManBeamKwsNode` node is similar to the `SnowboyManKwsNode` node.
 */

/** @example file_1beam_test.cc
 * This is an exmaple that shows how to read a 8 channels wav file, send the recording stream to `VepAecBeamformingNode`
 * node and detect hotword from the output beam. This example supports all the microphone type, keyword "alexa", "snowboy" and 
 * "heysnips", adjustable target gain level and wav log. 
 */

/** @example file_novep_1b_test.cc
 * This example is similar to "file_1beam_test.cc", but it just selects 1 channel of the wav file and detects hotword,
 * rather than send all the channels to `VepAecBeamformingNode` and detect the output beam.
 */

/** @example file_snowboy_mb_test.cc
 * This is an exmaple that shows how to read a 8 channels wav file, send the recording stream to `VepAecBeamformingNode`
 * node and detect hotword with `SnowboyMbDoaKwsNode` from the output beam.
 */


/** @example alsa_aloop_test.cc
 *  This is an example that shows how to redirect the processed audio stream into a specific Alsa device(Loopback PCM).
 *  To run this example, you have to run 'sudo modprobe snd-aloop' first. And make sure "pulseaudio" doesn't start, then  
 *  you can use "arecord -Dhw:Loopback,1,0 -c 1 -r 16000 -f S16_LE loop_test.wav" to arecord the processed audio stream.
 *  Further more, you can setup a third party voice assistant to capture voice from "hw:Loopback,1,0", to run the assistant directly.
 */

/** @example alsa_snips_1b_test.cc
 * This is an exmaple that shows how to use the `AlsaCollectorNode` node with `VepAecBeamformingNode` node and `Snips1bDoaKwsNode` node.
 */

} //namespace

#endif //__RESPEAKER_H__