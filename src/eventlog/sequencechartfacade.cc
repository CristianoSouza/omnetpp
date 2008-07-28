//=========================================================================
//  SEQUENCECHARTFACADE.CC - part of
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

#include <algorithm>
#include <set>
#include <math.h>
#include "lcgrandom.h"
#include "ievent.h"
#include "ieventlog.h"
#include "event.h"
#include "messagedependency.h"
#include "sequencechartfacade.h"

USING_NAMESPACE

static double zero = 0;

static double NaN = zero / zero;

inline bool isNaN(double x) { return x != x; }

SequenceChartFacade::SequenceChartFacade(IEventLog *eventLog) : EventLogFacade(eventLog)
{
    timelineMode = NONLINEAR;
    timelineCoordinateSystemVersion = -1;
    undefineTimelineCoordinateSystem();

    nonLinearMinimumTimelineCoordinateDelta = 0.1;
    setNonLinearFocus(calculateNonLinearFocus());
}

void SequenceChartFacade::synchronize(FileReader::FileChangedState change)
{
    if (change != FileReader::UNCHANGED) {
        EventLogFacade::synchronize(change);

        setNonLinearFocus(calculateNonLinearFocus());

        if (timelineCoordinateOriginEventNumber != -1) {
            IEvent *event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber);

            if (event)
                relocateTimelineCoordinateSystem(event);
            else
                undefineTimelineCoordinateSystem();
        }
    }
}

double SequenceChartFacade::calculateNonLinearFocus()
{
    if (!eventLog->isEmpty()) {
        double lastEventSimulationTime = eventLog->getLastEvent()->getSimulationTime().dbl();
        double firstEventSimulationTime = eventLog->getFirstEvent()->getSimulationTime().dbl();
        double totalSimulationTimeDelta = lastEventSimulationTime - firstEventSimulationTime;

        if (totalSimulationTimeDelta == 0)
            totalSimulationTimeDelta = firstEventSimulationTime;

        if (totalSimulationTimeDelta == 0)
            return 1;
        else
            return totalSimulationTimeDelta / eventLog->getApproximateNumberOfEvents() / 10;
    }
    else
        return 1;
}

void SequenceChartFacade::setNonLinearMinimumTimelineCoordinateDelta(double value)
{
    Assert(value >= 0);
    nonLinearMinimumTimelineCoordinateDelta = value;
}

void SequenceChartFacade::setNonLinearFocus(double nonLinearFocus)
{
    Assert(nonLinearFocus >= 0);
    this->nonLinearFocus = nonLinearFocus;
}

void SequenceChartFacade::undefineTimelineCoordinateSystem()
{
    timelineCoordinateSystemVersion++;
    timelineCoordinateOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = -1;
    timelineCoordinateOriginSimulationTime = -1;
}

void SequenceChartFacade::relocateTimelineCoordinateSystem(IEvent *event)
{
    Assert(event);

    timelineCoordinateSystemVersion++;
    timelineCoordinateOriginEventNumber = timelineCoordinateRangeStartEventNumber = timelineCoordinateRangeEndEventNumber = event->getEventNumber();
    timelineCoordinateOriginSimulationTime = event->getSimulationTime().dbl();
    event->cachedTimelineCoordinate = 0;
    event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
}

void SequenceChartFacade::setTimelineMode(TimelineMode timelineMode)
{
    this->timelineMode = timelineMode;

    if (timelineCoordinateOriginEventNumber != -1)
        relocateTimelineCoordinateSystem(eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber));
}

double SequenceChartFacade::IEvent_getTimelineCoordinate(ptr_t ptr)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinate((IEvent*)ptr);
}

double SequenceChartFacade::getTimelineCoordinateDelta(double simulationTimeDelta)
{
    Assert(nonLinearFocus > 0);

    if (timelineMode == STEP)
        return 1;
    else
        return nonLinearMinimumTimelineCoordinateDelta + (1 - nonLinearMinimumTimelineCoordinateDelta) * atan(fabs(simulationTimeDelta) / nonLinearFocus) / PI * 2;
}

double SequenceChartFacade::getTimelineCoordinate(ptr_t ptr, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    IEVENT_PTR(ptr);
    return getTimelineCoordinate((IEvent *)ptr, lowerTimelineCoordinateCalculationLimit, upperTimelineCoordinateCalculationLimit);
}

double SequenceChartFacade::getTimelineCoordinate(IEvent *event, double lowerTimelineCoordinateCalculationLimit, double upperTimelineCoordinateCalculationLimit)
{
    Assert(event);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion) {
        double timelineCoordinate;

        switch (timelineMode) {
            case SIMULATION_TIME:
                timelineCoordinate = event->getSimulationTime().dbl() - timelineCoordinateOriginSimulationTime;
                break;
            case EVENT_NUMBER:
                timelineCoordinate = event->getEventNumber() - timelineCoordinateOriginEventNumber;
                break;
            case STEP:
            case NONLINEAR:
                {
                    IEvent *previousEvent = NULL;
                    // do we go forward from end or backward from start of known range
                    bool forward = event->getEventNumber() > timelineCoordinateRangeEndEventNumber;
                    IEvent *currentEvent = eventLog->getEventForEventNumber(forward ? timelineCoordinateRangeEndEventNumber : timelineCoordinateRangeStartEventNumber);

                    Assert(event->getEventNumber() < timelineCoordinateRangeStartEventNumber || timelineCoordinateRangeEndEventNumber < event->getEventNumber());

                    // TODO: LONG RUNNING OPERATION
                    // does a linear search towards the event to calculate the non linear timeline coordinate
                    do {
                        Assert(currentEvent);
                        previousEvent = currentEvent;
                        currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();
                        Assert(currentEvent);

                        double previousSimulationTime = previousEvent->getSimulationTime().dbl();
                        double previousTimelineCoordinate = previousEvent->cachedTimelineCoordinate;
                        double simulationTime = currentEvent->getSimulationTime().dbl();
                        double timelineCoordinateDelta = getTimelineCoordinateDelta(simulationTime - previousSimulationTime);

                        if (forward) {
                            timelineCoordinate = previousTimelineCoordinate + timelineCoordinateDelta;

                            if (timelineCoordinate > upperTimelineCoordinateCalculationLimit)
                                return NaN;
                        }
                        else {
                            timelineCoordinate = previousTimelineCoordinate - timelineCoordinateDelta;

                            if (timelineCoordinate < lowerTimelineCoordinateCalculationLimit)
                                return NaN;
                        }

                        currentEvent->cachedTimelineCoordinate = timelineCoordinate;
                        currentEvent->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
                    }
                    while (currentEvent != event);

                    if (forward)
                        timelineCoordinateRangeEndEventNumber = event->getEventNumber();
                    else
                        timelineCoordinateRangeStartEventNumber = event->getEventNumber();
                }
                break;
            default:
                throw opp_runtime_error("Unknown timeline mode");
        }

        event->cachedTimelineCoordinate = timelineCoordinate;
        event->cachedTimelineCoordinateSystemVersion = timelineCoordinateSystemVersion;
    }

    return event->cachedTimelineCoordinate;
}

double SequenceChartFacade::getCachedTimelineCoordinate(IEvent *event)
{
    Assert(event);
    Assert(timelineCoordinateSystemVersion != -1);
    Assert(timelineCoordinateOriginEventNumber != -1);

    if (this->timelineCoordinateSystemVersion > event->cachedTimelineCoordinateSystemVersion)
        return -1;
    else
        return event->cachedTimelineCoordinate;
}

IEvent *SequenceChartFacade::getEventForNonLinearTimelineCoordinate(double timelineCoordinate, bool &forward)
{
    Assert(timelineCoordinateOriginEventNumber != -1);
    IEvent *timelineCoordinateRangeStartEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeStartEventNumber);
    IEvent *timelineCoordinateRangeEndEvent = eventLog->getEventForEventNumber(timelineCoordinateRangeEndEventNumber);
    IEvent *currentEvent;

    Assert(timelineCoordinateRangeStartEvent && timelineCoordinateRangeEndEvent);

    if (timelineCoordinate <= getTimelineCoordinate(timelineCoordinateRangeStartEvent)) {
        forward = false;
        currentEvent = timelineCoordinateRangeStartEvent;
    }
    else if (getTimelineCoordinate(timelineCoordinateRangeEndEvent) <= timelineCoordinate) {
        forward = true;
        currentEvent = timelineCoordinateRangeEndEvent;
    }
    else {
        forward = true;
        currentEvent = timelineCoordinateRangeStartEvent;
    }

    // TODO: LONG RUNNING OPERATION
    // does a linear search towards requested non linear timeline coordinate
    while (currentEvent && (forward ? getTimelineCoordinate(currentEvent) < timelineCoordinate :
                                      timelineCoordinate <= getTimelineCoordinate(currentEvent)))
        currentEvent = forward ? currentEvent->getNextEvent() : currentEvent->getPreviousEvent();

    return currentEvent;
}

IEvent *SequenceChartFacade::getLastEventNotAfterTimelineCoordinate(double timelineCoordinate)
{
    if (eventLog->isEmpty())
        return NULL;

    switch (timelineMode) {
        case SIMULATION_TIME:
            return eventLog->getLastEventNotAfterSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
        case EVENT_NUMBER:
            {
                eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;

                if (eventNumber < 0)
                    return NULL;
                else
                    return eventLog->getLastEventNotAfterEventNumber(eventNumber);
            }
        case STEP:
        case NONLINEAR:
            {
                bool forward;
                IEvent *currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, forward);
                currentEvent = forward ? (currentEvent ? currentEvent->getPreviousEvent() : eventLog->getLastEvent()) : currentEvent;

                Assert(!currentEvent || getTimelineCoordinate(currentEvent) <= timelineCoordinate);
                return currentEvent;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

IEvent *SequenceChartFacade::getFirstEventNotBeforeTimelineCoordinate(double timelineCoordinate)
{
    if (eventLog->isEmpty())
        return NULL;

    switch (timelineMode) {
        case SIMULATION_TIME:
            return eventLog->getFirstEventNotBeforeSimulationTime(getSimulationTimeForTimelineCoordinate(timelineCoordinate));
        case EVENT_NUMBER:
            {
                eventnumber_t eventNumber = (eventnumber_t)floor(timelineCoordinate) + timelineCoordinateOriginEventNumber;

                if (eventNumber < 0)
                    return eventLog->getFirstEvent();
                else
                    return eventLog->getFirstEventNotBeforeEventNumber(eventNumber);
            }
        case STEP:
        case NONLINEAR:
            {
                bool forward;
                IEvent *currentEvent = getEventForNonLinearTimelineCoordinate(timelineCoordinate, forward);
                currentEvent = forward ? currentEvent : (currentEvent ? currentEvent->getNextEvent() : eventLog->getFirstEvent());

                Assert(!currentEvent || getTimelineCoordinate(currentEvent) >= timelineCoordinate);
                return currentEvent;
            }
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }
}

void SequenceChartFacade::extractSimulationTimesAndTimelineCoordinates(
    IEvent *event, IEvent *&nextEvent,
    double &eventSimulationTime, double &eventTimelineCoordinate,
    double &nextEventSimulationTime, double &nextEventTimelineCoordinate,
    double &simulationTimeDelta, double &timelineCoordinateDelta)
{
    // if before the first event
    if (event) {
        eventSimulationTime = event->getSimulationTime().dbl();
        eventTimelineCoordinate = getTimelineCoordinate(event);
    }
    else {
        eventSimulationTime = 0;
        IEvent *firstEvent = eventLog->getFirstEvent();
        eventTimelineCoordinate = getTimelineCoordinate(firstEvent);

        if (timelineMode == EVENT_NUMBER)
            eventTimelineCoordinate -= 1;
        else
            eventTimelineCoordinate -= getTimelineCoordinateDelta(firstEvent->getSimulationTime().dbl());
    }

    // linear approximation between two enclosing events
    nextEvent = event ? event->getNextEvent() : eventLog->getFirstEvent();

    if (nextEvent) {
        nextEventSimulationTime = nextEvent->getSimulationTime().dbl();
        nextEventTimelineCoordinate = getTimelineCoordinate(nextEvent);

        simulationTimeDelta = nextEventSimulationTime - eventSimulationTime;
        timelineCoordinateDelta = nextEventTimelineCoordinate - eventTimelineCoordinate;
    }
}

double SequenceChartFacade::getSimulationTimeForTimelineCoordinate(double timelineCoordinate, bool upperLimit)
{
    Assert(!isNaN(timelineCoordinate));

    if (eventLog->isEmpty())
        return 0;

    double simulationTime;

    switch (timelineMode)
    {
        case SIMULATION_TIME:
            {
                double lastEventSimulationTime = eventLog->getLastEvent()->getSimulationTime().dbl();
                simulationTime = std::max(0.0, std::min(lastEventSimulationTime, timelineCoordinate + timelineCoordinateOriginSimulationTime));
            }
            break;
        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR:
            {
                IEvent *nextEvent;
                double eventSimulationTime, eventTimelineCoordinate;
                double nextEventSimulationTime, nextEventTimelineCoordinate;
                double simulationTimeDelta, timelineCoordinateDelta;

                IEvent *event = getLastEventNotAfterTimelineCoordinate(timelineCoordinate);
                extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                                                             eventSimulationTime, eventTimelineCoordinate,
                                                             nextEventSimulationTime, nextEventTimelineCoordinate,
                                                             simulationTimeDelta, timelineCoordinateDelta);

                if (nextEvent) {
                    if (timelineCoordinateDelta == 0) {
                        // IMPORTANT NOTE: this is just an approximation
                        if (upperLimit)
                            simulationTime = nextEventSimulationTime;
                        else
                            simulationTime = eventSimulationTime;
                    }
                    else {
                        timelineCoordinate = std::max(eventTimelineCoordinate, std::min(nextEventTimelineCoordinate, timelineCoordinate));
                        simulationTime = eventSimulationTime + simulationTimeDelta * (timelineCoordinate - eventTimelineCoordinate) / timelineCoordinateDelta;
                    }
                }
                else
                    simulationTime = eventSimulationTime;
            }
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    Assert(!isNaN(simulationTime));
    Assert(simulationTime >= 0);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime().dbl());

    return simulationTime;
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTime(double simulationTime, bool upperLimit)
{
    Assert(!isNaN(simulationTime));

    if (eventLog->isEmpty())
        return 0;

    Assert(simulationTime >= 0);
    Assert(simulationTime <= eventLog->getLastEvent()->getSimulationTime().dbl());

    double timelineCoordinate;

    switch (timelineMode)
    {
        case SIMULATION_TIME:
            timelineCoordinate = simulationTime - timelineCoordinateOriginSimulationTime;
            break;
        case EVENT_NUMBER:
        case STEP:
        case NONLINEAR:
            {
                IEvent *nextEvent;
                double eventSimulationTime, eventTimelineCoordinate;
                double nextEventSimulationTime, nextEventTimelineCoordinate;
                double simulationTimeDelta, timelineCoordinateDelta;

                IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);
                extractSimulationTimesAndTimelineCoordinates(event, nextEvent,
                                                             eventSimulationTime, eventTimelineCoordinate,
                                                             nextEventSimulationTime, nextEventTimelineCoordinate,
                                                             simulationTimeDelta, timelineCoordinateDelta);

                if (nextEvent) {
                    if (simulationTimeDelta == 0) {
                        // IMPORTANT NOTE: this is just an approximation
                        if (upperLimit)
                            timelineCoordinate = nextEventTimelineCoordinate;
                        else
                            timelineCoordinate = eventTimelineCoordinate;
                    }
                    else {
                        simulationTime = std::max(eventSimulationTime, std::min(nextEventSimulationTime, simulationTime));
                        timelineCoordinate = eventTimelineCoordinate + timelineCoordinateDelta * (simulationTime - eventSimulationTime) / simulationTimeDelta;
                    }
                }
                else
                    timelineCoordinate = eventTimelineCoordinate;
            }
            break;
        default:
            throw opp_runtime_error("Unknown timeline mode");
    }

    Assert(!isNaN(timelineCoordinate));

    return timelineCoordinate;
}

double SequenceChartFacade::getTimelineCoordinateForSimulationTimeAndEventInModule(simtime_t simulationTime, int moduleId)
{
    IEvent *event = eventLog->getLastEventNotAfterSimulationTime(simulationTime);

    while (event && event->getSimulationTime() == simulationTime) {
        if (event->getModuleId() == moduleId)
            return getTimelineCoordinate(event);

        event = event->getNextEvent();
    }

    return getTimelineCoordinateForSimulationTime(simulationTime.dbl());
}

std::vector<ptr_t> *SequenceChartFacade::getIntersectingMessageDependencies(ptr_t startEventPtr, ptr_t endEventPtr)
{
    std::set<ptr_t> messageDependencies;
    IEvent *startEvent = (IEvent *)startEventPtr;
    IEvent *endEvent = (IEvent *)endEventPtr;
    Assert(startEvent);
    Assert(endEvent);
    eventnumber_t startEventNumber = startEvent->getEventNumber();

    // TODO: LONG RUNNING OPERATION
    // this might take a while if start and end events are far away from each other
    // if not, then some dependencies will not be included
    for (IEvent *event = startEvent;; event = event->getNextEvent()) {
        IMessageDependencyList *causes = event->getCauses();

        for (IMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
            IMessageDependency *messageDependency = *it;

            if (messageDependency->getCauseEventNumber() < startEventNumber)
                messageDependencies.insert((ptr_t)messageDependency);
        }

        IMessageDependencyList *consequences = event->getConsequences();

        for (IMessageDependencyList::iterator it = consequences->begin(); it != consequences->end(); it++)
            messageDependencies.insert((ptr_t)*it);

        if (event == endEvent)
            break;
    }

    std::vector<ptr_t> *result = new std::vector<ptr_t>;
    result->resize(messageDependencies.size());
    std::copy(messageDependencies.begin(), messageDependencies.end(), result->begin());

    return result;
}

std::vector<int> SequenceChartFacade::getApproximateMessageDependencyCountAdjacencyMatrix(std::map<int, int> *moduleIdToAxisIndexMap, int numberOfSamples, int messageSendWeight, int messageReuseWeight)
{
    LCGRandom lcgRandom;
    std::vector<int> adjacencyMatrix;
    std::set<int> axisIndexSet;
    std::map<eventnumber_t, IEvent *> eventNumberToEventMap;

    for (std::map<int, int>::iterator it = moduleIdToAxisIndexMap->begin(); it != moduleIdToAxisIndexMap->end(); it++)
        axisIndexSet.insert(it->second);

    int numberOfAxes = axisIndexSet.size();
    adjacencyMatrix.resize(numberOfAxes * numberOfAxes);

    for (int i = 0; i < numberOfSamples; i++)
    {
        // draw random
        double percentage = lcgRandom.next01();
        IEvent *event = eventLog->getApproximateEventAt(percentage);
        eventNumberToEventMap[event->getEventNumber()] = event;

        // look before origin
        if (timelineCoordinateOriginEventNumber != -1) {
            if (timelineCoordinateOriginEventNumber - i >= 0) {
                event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber - i);
                if (event)
                    eventNumberToEventMap[event->getEventNumber()] = event;
            }

            // look after origin
            event = eventLog->getEventForEventNumber(timelineCoordinateOriginEventNumber + i);
            if (event)
                eventNumberToEventMap[event->getEventNumber()] = event;
        }
    }

    for (std::map<eventnumber_t, IEvent *>::iterator it = eventNumberToEventMap.begin(); it != eventNumberToEventMap.end(); it++) {
        IEvent *event = it->second;
        IMessageDependencyList *causes = event->getCauses();

        for (IMessageDependencyList::iterator it = causes->begin(); it != causes->end(); it++) {
            IMessageDependency *messageDependency = *it;
            IEvent *causeEvent = messageDependency->getCauseEvent();
            IEvent *consequenceEvent = messageDependency->getConsequenceEvent();
            int weight = messageDependency->getIsReuse() ? messageReuseWeight : messageSendWeight;

            if (causeEvent && consequenceEvent && weight != 0) {
                int causeModuleId = causeEvent->getModuleId();
                int consequenceModuleId = consequenceEvent->getModuleId();

                std::map<int, int>::iterator causeModuleIdIt = moduleIdToAxisIndexMap->find(causeModuleId);
                std::map<int, int>::iterator consequenceModuleIdIt = moduleIdToAxisIndexMap->find(consequenceModuleId);

                if (causeModuleIdIt !=  moduleIdToAxisIndexMap->end() && consequenceModuleIdIt != moduleIdToAxisIndexMap->end())
                    adjacencyMatrix.at(causeModuleIdIt->second * numberOfAxes + consequenceModuleIdIt->second) += weight;
            }
        }
    }

    return adjacencyMatrix;
}
