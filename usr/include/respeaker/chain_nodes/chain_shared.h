/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */


#ifndef __CHAIN_SHARED_H__
#define __CHAIN_SHARED_H__

#include <mutex>

namespace respeaker
{

/** The state of the state machine of the chain */
enum ChainState{
    WAIT_TRIGGER_QUIETLY, ///< the device is waiting for trigger and its playback is paused or muted.
    WAIT_TRIGGER_WITH_BGM, ///< the device is waiting for trigger, its playback is playing sound
    LISTEN_QUIETLY, ///< the device is listening user's commands, its playback is paused or muted, the chain will skip some algorithms, like hotword detection
    LISTEN_WITH_BGM ///< the device is listening user's commands, its playback is playing sound
};

/** The data structure which is shared between all the nodes of the chain. */
struct ChainSharedData
{
    ChainSharedData() : exit_flag(false),
                        state(WAIT_TRIGGER_QUIETLY),
                        vad_node_present(false),
                        vad(false),
                        vep_freeze(false){};
    std::mutex mutex_exit_flag;
    bool exit_flag;         ///< Exit flag for joining the thread.

    std::mutex mutex_state;
    ChainState state;       ///< The state machine of the chain.

    std::mutex mutex_vad;
    bool vad_node_present;  ///< If the Vad (voice available detection) node is present.
    bool vad;               ///< If the last processed block of the node who can do Vad contains valid voice.

    std::mutex mutex_vep_freeze;
    bool vep_freeze;        ///< For the Vep lib.
};

}  // namespace respeaker
#endif // !__CHAIN_SHARED_H__
