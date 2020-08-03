#ifndef LOOPEVENT_H
#define LOOPEVENT_H

#include <Callback.h>

typedef bool (&LoopEventHandler)(unsigned long timestamp);

class LoopEvent
{
public:
    LoopEvent(LoopEventHandler &handler, unsigned int intervalMs) : _handler(handler), _intervalMs(intervalMs) {}
    ~LoopEvent()
    {
    }

    void setNextExecute(unsigned long nextExecution)
    {
        this->_nextExecution = nextExecution;
    }

    bool execute(unsigned long timestamp)
    {
        if (this->_intervalMs == 0 || timestamp > this->_nextExecution)
        {
            this->_nextExecution = timestamp + this->_intervalMs;
            return this->_handler(timestamp);
        }

        return true;
    }

private:
    unsigned int _intervalMs;
    unsigned long _nextExecution = 0;
    LoopEventHandler _handler;
};

class SlotLoopEvent
{
    Slot<unsigned long> *_slot;
    unsigned int _intervalMs;
    unsigned long _nextExecution = 0;

public:
    SlotLoopEvent(const Slot<unsigned long> &slot, unsigned int intervalMs) : _slot(slot.clone()), _intervalMs(intervalMs)
    {
    }

    // Since the loop takes copies of all the input slots via clone() it needs to clean up after itself when being destroyed.
    virtual ~SlotLoopEvent()
    {
        delete this->_slot;
    }

    void setNextExecute(unsigned long nextExecution)
    {
        this->_nextExecution = nextExecution;
    }

    bool execute(unsigned long timestamp)
    {
        if (this->_intervalMs == 0 || timestamp > this->_nextExecution)
        {
            this->_nextExecution = timestamp + this->_intervalMs;
            (*this->_slot)(timestamp);
        }

        return true;
    }
};

#endif