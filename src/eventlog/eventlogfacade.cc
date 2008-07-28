//=========================================================================
//  EVENTLOGFACADE.CC - part of
//                  OMNeT++/OMNEST
//           Discrete System Simulation in C++
//
//  Author: Levente Meszaros
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2006-2008 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "eventlog.h"
#include "filteredevent.h"
#include "filteredeventlog.h"
#include "eventlogfacade.h"

USING_NAMESPACE

EventLogFacade::EventLogFacade(IEventLog *eventLog)
{
    Assert(eventLog);
    this->eventLog = eventLog;
}

void EventLogFacade::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED)
        return eventLog->synchronize(change);
}

IEvent* EventLogFacade::IEvent_getEvent(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return (IEvent*)ptr;
}

ptr_t EventLogFacade::IEvent_getNonFilteredEvent(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    IEvent *ievent = (IEvent*)ptr;

    Event *event = dynamic_cast<Event *>(ievent);
    if (event)
        return ptr;
    else
        return IEvent_getNonFilteredEvent((ptr_t)((FilteredEvent *)ievent)->getEvent());
}

ptr_t EventLogFacade::IEvent_getEventForEventNumber(eventnumber_t eventNumber)
{
    return (ptr_t)eventLog->getEventForEventNumber(eventNumber);
}

ptr_t EventLogFacade::IEvent_getNonFilteredEventForEventNumber(eventnumber_t eventNumber)
{
    EventLog *nonFilteredEventLog = dynamic_cast<EventLog *>(eventLog);

    if (nonFilteredEventLog)
        return (ptr_t)nonFilteredEventLog->getEventForEventNumber(eventNumber);
    else
        return (ptr_t)((FilteredEventLog *)eventLog)->getEventLog()->getEventForEventNumber(eventNumber);
}

ptr_t EventLogFacade::IEvent_getPreviousEvent(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getPreviousEvent();
}

ptr_t EventLogFacade::IEvent_getNextEvent(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getNextEvent();
}

eventnumber_t EventLogFacade::IEvent_getEventNumber(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getEventNumber();
}

simtime_t EventLogFacade::IEvent_getSimulationTime(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime();
}

double EventLogFacade::IEvent_getSimulationTimeAsDouble(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getSimulationTime().dbl();
}

int EventLogFacade::IEvent_getModuleId(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getModuleId();
}

int EventLogFacade::IEvent_getNumCauses(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getCauses()->size();
}

int EventLogFacade::IEvent_getNumConsequences(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->getConsequences()->size();
}

ptr_t EventLogFacade::IEvent_getCause(ptr_t ptr, int index)
{
    IEVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getCauses()->at(index);
}

ptr_t EventLogFacade::IEvent_getConsequence(ptr_t ptr, int index)
{
    IEVENT_PTR(ptr);
    return (ptr_t)((IEvent*)ptr)->getConsequences()->at(index);
}

bool EventLogFacade::IEvent_isSelfMessageProcessingEvent(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return ((IEvent*)ptr)->isSelfMessageProcessingEvent();
}

EventLogEntry *EventLogFacade::EventLogEntry_getEventLogEntry(ptr_t ptr)
{
    EVENT_LOG_ENTRY_PTR(ptr);
    return (EventLogEntry*)ptr;
}

int EventLogFacade::EventLogEntry_getContextModuleId(ptr_t ptr)
{
    EVENT_LOG_ENTRY_PTR(ptr);
    return ((EventLogEntry*)ptr)->contextModuleId;
}

bool EventLogFacade::BeginSendEntry_isSelfMessage(ptr_t ptr)
{
    BEGIN_SEND_ENTRY_PTR(ptr);
    BeginSendEntry *beginSendEntry = (BeginSendEntry *)ptr;
    return beginSendEntry->getEvent()->isSelfMessage(beginSendEntry);
}

int EventLogFacade::BeginSendEntry_getMessageId(ptr_t ptr)
{
    BEGIN_SEND_ENTRY_PTR(ptr);
    BeginSendEntry *beginSendEntry = (BeginSendEntry *)ptr;
    return beginSendEntry->messageId;
}

ptr_t EventLogFacade::BeginSendEntry_getEndSendEntry(ptr_t ptr)
{
    BEGIN_SEND_ENTRY_PTR(ptr);
    BeginSendEntry *beginSendEntry = (BeginSendEntry *)ptr;
    return (ptr_t)beginSendEntry->getEvent()->getEndSendEntry(beginSendEntry);
}

simtime_t EventLogFacade::BeginSendEntry_getTransmissionDelay(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    BeginSendEntry *beginSendEntry = (BeginSendEntry *)ptr;
    return beginSendEntry->getEvent()->getTransmissionDelay(beginSendEntry);
}

bool EventLogFacade::EndSendEntry_isReceptionStart(ptr_t ptr)
{
    END_SEND_ENTRY_PTR(ptr);
    EndSendEntry *endSendEntry = (EndSendEntry *)ptr;
    return endSendEntry->isReceptionStart;
}

const char *EventLogFacade::ModuleCreatedEntry_getModuleFullPath(ptr_t ptr)
{
    std::string fullPath;
    MODULE_CREATED_ENTRY_PTR(ptr);
    ModuleCreatedEntry *moduleCreatedEntry = (ModuleCreatedEntry *)ptr;

    while (moduleCreatedEntry) {
        fullPath = moduleCreatedEntry->fullName + fullPath;
        moduleCreatedEntry = eventLog->getModuleCreatedEntry(moduleCreatedEntry->parentModuleId);
        if (moduleCreatedEntry)
            fullPath = "." + fullPath;
    }

    return eventLogStringPool.get(fullPath.c_str());
}

IMessageDependency *EventLogFacade::IMessageDependency_getMessageDependency(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return (IMessageDependency*)ptr;
}

const char *EventLogFacade::IMessageDependency_getMessageName(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getBeginSendEntry()->messageFullName;
}

bool EventLogFacade::IMessageDependency_getIsReuse(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getIsReuse();
}

bool EventLogFacade::IMessageDependency_isFilteredMessageDependency(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return dynamic_cast<FilteredMessageDependency*>((IMessageDependency*)ptr);
}

ptr_t EventLogFacade::IMessageDependency_getCauseEvent(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return (ptr_t)((IMessageDependency*)ptr)->getCauseEvent();
}

ptr_t EventLogFacade::IMessageDependency_getConsequenceEvent(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return (ptr_t)((IMessageDependency*)ptr)->getConsequenceEvent();
}

ptr_t EventLogFacade::IMessageDependency_getBeginSendEntry(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return (ptr_t)((IMessageDependency*)ptr)->getBeginSendEntry();
}

simtime_t EventLogFacade::IMessageDependency_getCauseSimulationTime(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getCauseSimulationTime();
}

simtime_t EventLogFacade::IMessageDependency_getConsequenceSimulationTime(ptr_t ptr)
{
    IMESSAGE_DEPENDENCY_PTR(ptr);
    return ((IMessageDependency*)ptr)->getConsequenceSimulationTime();
}

const char *EventLogFacade::FilteredMessageDependency_getBeginMessageName(ptr_t ptr)
{
    FILTERED_MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getBeginMessageDependency()->getBeginSendEntry()->messageFullName;
}

const char *EventLogFacade::FilteredMessageDependency_getEndMessageName(ptr_t ptr)
{
    FILTERED_MESSAGE_DEPENDENCY_PTR(ptr);
    return ((FilteredMessageDependency*)ptr)->getEndMessageDependency()->getBeginSendEntry()->messageFullName;
}
