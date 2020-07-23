#ifndef LOOPEVENT_H
#define LOOPEVENT_H

typedef bool (*loopEventHandler)(unsigned int timestamp);

class LoopEvent
{
public:
    LoopEvent(unsigned int intervalMs, loopEventHandler handler);
    ~LoopEvent();

    bool execute(unsigned int timestamp);

private:
    class Impl;
    Impl *_pimpl;
};

#endif