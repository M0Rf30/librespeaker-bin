/*
 * Copyright (c) 2017 Seeed Technology Co., Ltd.
 *
 * @author Jack Shao <jacky.shaoxg@gmail.com>
 *
 */


#ifndef __DIRECTION_MANAGER_NODE_H__
#define __DIRECTION_MANAGER_NODE_H__

namespace respeaker
{
/**
 * Nodes who can do DoA should inherit from this class.
 */
class DirectionManagerNode
{
public:
    /**
     * Subclass should implement this method.
     */
    virtual int GetDirection() = 0;

    /**
     * Subclass should implement this method. (Not used, reserve)
     */
    virtual void SetDirection(int dir) = 0;
};

}  //namespace

#endif // !__DIRECTION_MANAGER_NODE_H__