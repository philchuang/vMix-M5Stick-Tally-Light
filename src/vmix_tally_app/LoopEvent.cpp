#include "LoopEvent.h"

struct LoopEvent::Impl
{
    Impl(LoopEventHandler &handler, unsigned int intervalMs) : _handler(handler), _intervalMs(intervalMs)
    {
    }
    ~Impl()
    {
        _handler = 0;
    }

    bool execute(unsigned int timestamp);

    unsigned int _intervalMs;
    unsigned long _nextExecution = 0;
    LoopEventHandler _handler;
};

LoopEvent::LoopEvent(LoopEventHandler &handler, unsigned int intervalMs) : _pimpl(new Impl(handler, intervalMs))
{
}

LoopEvent::~LoopEvent()
{
    _pimpl->~Impl();
}

void LoopEvent::setNextExecute(unsigned long nextExecution)
{
    _pimpl->_nextExecution = nextExecution;
}

bool LoopEvent::execute(unsigned long timestamp)
{
    if (_pimpl->_intervalMs == 0 || timestamp > _pimpl->_nextExecution)
    {
        _pimpl->_nextExecution = timestamp + _pimpl->_intervalMs;
        return _pimpl->_handler(timestamp);
    }

    return true;
}