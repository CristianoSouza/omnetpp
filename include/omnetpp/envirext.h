//==========================================================================
//  ENVIREXT.H - part of
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2015 Andras Varga
  Copyright (C) 2006-2015 OpenSim Ltd.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#ifndef __OMNETPP_ENVIREXT_H
#define __OMNETPP_ENVIREXT_H

#include <iostream>
#include "simkerneldefs.h"
#include "opp_string.h"
#include "simtime_t.h"
#include "cobject.h"
#include "simtime_t.h"
#include "opp_string.h"
#include "clifecyclelistener.h"

namespace omnetpp {


class cModule;
class cStatistic;
class cEvent;
class cMessage;
class cGate;

/**
 * @brief Abstract base class for output vector managers for cEnvir.
 * cIOutputVectorManagers are plugins into the Envir user
 * interface library (src/envir) that handle recording the output for
 * cOutVectors.
 *
 * The default output vector manager is cFileOutputVectorManager, defined in the
 * Envir library.
 *
 * To change the way output vectors are written, subclass cIOutputVectorManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * outputvectormanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cIOutputVectorManager : public cObject, public cISimulationLifecycleListener
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cIOutputVectorManager(const cIOutputVectorManager&);
    cIOutputVectorManager& operator=(const cIOutputVectorManager&);

  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and flush(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIOutputVectorManager() {}

    /**
     * Destructor.
     */
    virtual ~cIOutputVectorManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Output vectors. */
    //@{

    /**
     * This method is called internally by the Envir library when a cOutVector object registers itself.
     * The return value is a handle of type void*; this handle is passed to
     * record() to identify the vector.
     */
    virtual void *registerVector(const char *modulename, const char *vectorname) = 0;

    /**
     * This method is called internally by the Envir library when a cOutVector object deregisters itself.
     */
    virtual void deregisterVector(void *vechandle) = 0;

    /**
     * This method is called internally by the Envir library when an attribute of the
     * cOutVector object is set.
     */
    virtual void setVectorAttribute(void *vechandle, const char *name, const char *value) = 0;

    /**
     * This method is called internally by the Envir library when a cOutVector object
     * writes a value into the output vector. The return value should be true if the
     * data was actually recorded, and false if it was not recorded (because of filtering, etc.)
     */
    virtual bool record(void *vechandle, simtime_t t, double value) = 0;

    /**
     * Returns the output vector file name. Returns nullptr if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Force writing out all buffered output.
     */
    virtual void flush() = 0;
    //@}
};


/**
 * @brief Abstract base class for handling recording of output scalar data.
 * cIOutputScalarManagers are plugins into the Envir user interface
 * library (src/envir) that ultimately handle calls to the
 * cComponent::recordScalar() family of functions.
 *
 * The default output scalar manager is cFileOutputScalarManager, defined
 * in the Envir library.
 *
 * To change the way scalar results are recorded, subclass cIOutputScalarManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * outputscalarmanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cIOutputScalarManager : public cObject, public cISimulationLifecycleListener
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cIOutputScalarManager(const cIOutputScalarManager&);
    cIOutputScalarManager& operator=(const cIOutputScalarManager&);

  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun(), endRun() and flush(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIOutputScalarManager() {}

    /**
     * Destructor.
     */
    virtual ~cIOutputScalarManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Opens collecting. Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Closes collecting. Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Scalar statistics. */
    //@{

    /**
     * Records a double scalar result, in a default configuration into the scalar result file.
     */
    virtual void recordScalar(cComponent *component, const char *name, double value, opp_string_map *attributes=nullptr) = 0;

    /**
     * Records a histogram or statistic object into the scalar result file.
     * This operation may invoke the transform() method on the histogram object.
     */
    virtual void recordStatistic(cComponent *component, const char *name, cStatistic *statistic, opp_string_map *attributes=nullptr) = 0;

    /**
     * Returns the output scalar file name. Returns nullptr if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;

    /**
     * Force writing out all buffered output.
     */
    virtual void flush() = 0;
    //@}
};


/**
 * @brief Abstract base class for snapshot managers. cISnapshotManagers are plugins
 * into the Envir user interface library (src/envir) that provide an output
 * stream into which snapshots are written (see cSimulation::snapshot()).
 *
 * The default output snapshot manager is cFileSnapshotManager, defined in the
 * Envir library.
 *
 * To create your own snaphot manager class, subclass cISnapshotManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * snapshotmanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cISnapshotManager : public cObject, public cISimulationLifecycleListener
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cISnapshotManager(const cISnapshotManager&);
    cISnapshotManager& operator=(const cISnapshotManager&);

  protected:
    /**
     * A cISimulationLifecycleListener method. Delegates to startRun() and endRun(); override if needed.
     */
    virtual void lifecycleEvent(SimulationLifecycleEventType eventType, cObject *details) override;

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cISnapshotManager() {}

    /**
     * Destructor.
     */
    virtual ~cISnapshotManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * Called at the beginning of a simulation run.
     */
    virtual void startRun() = 0;

    /**
     * Called at the end of a simulation run.
     */
    virtual void endRun() = 0;
    //@}

    /** @name Snapshot management */
    //@{
    /**
     * Returns a stream where a snapshot can be written.
     */
    virtual std::ostream *getStreamForSnapshot() = 0;

    /**
     * Releases a stream after a snapshot was written.
     */
    virtual void releaseStreamForSnapshot(std::ostream *os) = 0;

    /**
     * Returns the snapshot file name. Returns nullptr if this object is not
     * producing file output.
     */
    virtual const char *getFileName() const = 0;
    //@}
};


/**
 * @brief Abstract base class for eventlog managers for cEnvir. cIEventlogManagers
 * are plugins into the Envir user interface library (src/envir) that handle
 * the recording of simulation history into an event log.
 *
 * The default eventlog manager is EventlogFileManager, defined in the
 * Envir library.
 *
 * To change the way event logs are written, subclass cIEventlogManager,
 * register your new class with the Register_Class() macro, then select it
 * by adding the following to <tt>omnetpp.ini</tt>:
 *
 * <pre>
 * [General]
 * eventlogmanager-class="MyClass"
 * </pre>
 *
 * @ingroup EnvirExtensions
 */
class SIM_API cIEventlogManager : public cObject
{
  private:
    // copy constructor and assignment unsupported, make them inaccessible and also leave unimplemented
    cIEventlogManager(const cIEventlogManager&);
    cIEventlogManager& operator=(const cIEventlogManager&);

  public:
    /** @name Constructor, destructor */
    //@{

    /**
     * Constructor.
     */
    explicit cIEventlogManager() {}

    /**
     * Destructor.
     */
    virtual ~cIEventlogManager() {}
    //@}

    /** @name Controlling the beginning and end of collecting data. */
    //@{

    /**
     * (Re)starts eventlog recording. Whether eventlog recording is enabled by
     * default depends on the eventlog manager (e.g. omnetpp.ini configuration
     * options). This function may be called several times (from the user
     * interface) during the simulation, but only between events.
     * The eventlog manager is expected to produce output starting from the
     * next event.
     */
    virtual void startRecording() = 0;

    /**
     * Temporarily stops eventlog recording. See startRecording() for more details.
     */
    virtual void stopRecording() = 0;

    /**
     * Forces writing out all buffered output.
     */
    virtual void flush() = 0;
    //@}

    /** @name Functions called from cEnvir's similar functions */
    //@{
    virtual void simulationEvent(cEvent *event) = 0;
    virtual void bubble(cComponent *component, const char *text) = 0;
    virtual void messageScheduled(cMessage *msg) = 0;
    virtual void messageCancelled(cMessage *msg) = 0;
    virtual void beginSend(cMessage *msg) = 0;
    virtual void messageSendDirect(cMessage *msg, cGate *toGate, simtime_t propagationDelay, simtime_t transmissionDelay) = 0;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate) = 0;
    virtual void messageSendHop(cMessage *msg, cGate *srcGate, simtime_t propagationDelay, simtime_t transmissionDelay) = 0;
    virtual void endSend(cMessage *msg) = 0;
    virtual void messageCreated(cMessage *msg) = 0;
    virtual void messageCloned(cMessage *msg, cMessage *clone) = 0;
    virtual void messageDeleted(cMessage *msg) = 0;
    virtual void moduleReparented(cModule *module, cModule *oldparent, int oldId) = 0;
    virtual void componentMethodBegin(cComponent *from, cComponent *to, const char *methodFmt, va_list va) = 0;
    virtual void componentMethodEnd() = 0;
    virtual void moduleCreated(cModule *newmodule) = 0;
    virtual void moduleDeleted(cModule *module) = 0;
    virtual void gateCreated(cGate *newgate) = 0;
    virtual void gateDeleted(cGate *gate) = 0;
    virtual void connectionCreated(cGate *srcgate) = 0;
    virtual void connectionDeleted(cGate *srcgate) = 0;
    virtual void displayStringChanged(cComponent *component) = 0;
    virtual void logLine(const char *prefix, const char *line, int lineLength) = 0;
    virtual void stoppedWithException(bool isError, int resultCode, const char *message) = 0;
    //@}
};



}  // namespace omnetpp


#endif


