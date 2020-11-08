/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */



#ifndef __HOTWORD_DETECTION_NODE_H__
#define __HOTWORD_DETECTION_NODE_H__

namespace respeaker
{
/**
 * Nodes who can do hotword detection should inherit from this class.
 */
class HotwordDetectionNode
{
public:
    /**
     * Subclass should implement this method.
     */
    virtual int HotwordDetected() = 0;
};

}  //namespace

#endif // !__HOTWORD_DETECTION_NODE_H__