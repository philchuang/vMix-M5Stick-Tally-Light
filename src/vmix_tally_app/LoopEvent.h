#ifndef LOOPEVENT_H
#define LOOPEVENT_H

typedef bool (&LoopEventHandler)(unsigned long timestamp);

class LoopEvent
{
public:
    LoopEvent(LoopEventHandler &handler, unsigned int intervalMs) : _handler(handler), _intervalMs(intervalMs) {}
    ~LoopEvent() {}

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

#endif