#ifndef SLOTLOOPEVENT_H
#define SLOTLOOPEVENT_H

#include <Callback.h>

class SlotLoopEvent
{
    Slot<unsigned long> *_slot;
    // if _intervalMs = 0, execute all the time, if _intervalMs < 0 then execute only when setNextExecute is used
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

    void execute(unsigned long timestamp)
    {
        if (this->_intervalMs == 0 || (timestamp > this->_nextExecution && this->_nextExecution >= 0))
        {
            if (this->_intervalMs < 0)
            {
                this->_nextExecution = -1;
            }
            else
            {
                this->_nextExecution = timestamp + this->_intervalMs;
            }

            (*this->_slot)(timestamp);
        }
    }
};

#endif