#include "LoopEvent.h"

struct LoopEvent::Impl
{
    Impl(unsigned int intervalMs, loopEventHandler handler) : _intervalMs(intervalMs), _handler(handler)
    {
    }
    ~Impl()
    {
        _handler = 0;
    }

    bool execute(unsigned int timestamp);

    unsigned int _intervalMs;
    unsigned long _nextExecution = 0;
    loopEventHandler _handler;
};

LoopEvent::LoopEvent(unsigned int intervalMs, loopEventHandler handler) : _pimpl(new Impl(intervalMs, handler))
{
}

LoopEvent::~LoopEvent()
{
    _pimpl->~Impl();
}

bool LoopEvent::execute(unsigned int timestamp)
{
    if (timestamp > _pimpl->_nextExecution){
        _pimpl->_nextExecution = timestamp + _pimpl->_intervalMs;
        return _pimpl->_handler(timestamp);
    }

    return true;
}