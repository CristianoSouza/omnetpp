//==========================================================================
//  GLOBALS.H - part of
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

#ifndef __OMNETPP_GLOBALS_H
#define __OMNETPP_GLOBALS_H

#include "onstartup.h"
#include "cregistrationlist.h"
#include "cobjectfactory.h"

NAMESPACE_BEGIN

//
// Global objects
//

//< Internal: list in which objects are accumulated if there is no simple module in context.
//< @see cOwnedObject::setDefaultOwner() and cSimulation::setContextModule())
SIM_API extern cDefaultList defaultList;

SIM_API extern cGlobalRegistrationList componentTypes;  ///< List of all component types (cComponentType)
SIM_API extern cGlobalRegistrationList nedFunctions;    ///< List if all NED functions (cNEDFunction and cNEDMathFunction)
SIM_API extern cGlobalRegistrationList classes;         ///< List of all classes that can be instantiated using createOne(); see cObjectFactory and Register_Class() macro
SIM_API extern cGlobalRegistrationList enums;           ///< List of all enum objects (cEnum)
SIM_API extern cGlobalRegistrationList classDescriptors;///< List of all class descriptors (cClassDescriptor)
SIM_API extern cGlobalRegistrationList configOptions;   ///< List of supported configuration options (cConfigOption)
SIM_API extern cGlobalRegistrationList resultFilters;   ///< List of result filters (cResultFilter)
SIM_API extern cGlobalRegistrationList resultRecorders; ///< List of result recorders (cResultRecorder)
SIM_API extern cGlobalRegistrationList messagePrinters; ///< List of message printers (cMessagePrinter)

NAMESPACE_END


#endif
