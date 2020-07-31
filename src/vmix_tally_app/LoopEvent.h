#ifndef LOOPEVENT_H
#define LOOPEVENT_H

typedef bool (&LoopEventHandler)(unsigned long timestamp);

class LoopEvent
{
public:
    LoopEvent(LoopEventHandler &handler, unsigned int intervalMs);
    ~LoopEvent();

    void setNextExecute(unsigned long nextExecution);
    bool execute(unsigned long timestamp);

private:
    class Impl;
    Impl *_pimpl;
};

#endif