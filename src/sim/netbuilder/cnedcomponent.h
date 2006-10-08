//==========================================================================
// CNEDCOMPONENT.H -
//
//                     OMNeT++/OMNEST
//            Discrete System Simulation in C++
//
//==========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 2002-2005 Andras Varga

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/


#ifndef __CNEDCOMPONENT_H
#define __CNEDCOMPONENT_H

#include "defs.h"
#include "nedcomponent.h"
#include "globals.h"
#include "cobject.h"
#include "cpar.h"
#include "cgate.h"
#include <string>
#include <vector>
#include <map>

class cPar;
class cProperties;


/**
 * Represents the NED declaration of modules, module interfaces,
 * channels and channel interfaces. It stores gates and parameters;
 * submodules are NOT stored.
 *
 * Inherited parameters and gates are included. Values (parameters,
 * gate sizes) are converted into cPar form and stored in cPar form.
 *
 * cNEDComponents are used during network setup (and dynamic module
 * creation) to add gates and parameters to the freshly created module
 * object, and also to verify that module parameters set correctly.
 *
 * cNEDComponents may be created either dynamically or by the NED compiler,
 * from NED declarations of modules and channels. When a module or channel
 * needs to be created, the appropriate cNEDComponent object
 * is looked up from the list, and the component's parameters (and gates)
 * are created according to the description in the cNEDComponent
 * object.
 *
 * @ingroup Internals
 */
class SIM_API cNEDComponent : public cNoncopyableObject, public NEDComponent
{
  public:
    /**
     * Describes a parameter declaration with its value
     */
    struct ParamDescription
    {
        std::string name;
        cPar::Type type;
        bool isVolatile;
        cPar *value;  // NULL if there's no default value
        cProperties *properties;
        std::string declaredOn;

        ParamDescription();
        ParamDescription deepCopy() const;
    };

    /**
     * Describes a gate declaration, with its size if vector
     */
    struct GateDescription
    {
        std::string name;
        cGate::Type type; // input, output, inout
        bool isVector;
        cPar *gatesize; // NULL if not vector or gatesize unspecified
        cProperties *properties;
        std::string declaredOn;

        GateDescription();
        GateDescription deepCopy() const;
    };

  protected:
    typedef std::vector<std::string> StringVector;
    typedef std::map<std::string,int> StringToIntMap;

    // inheritance
    StringVector extendsnames;
    StringVector interfacenames;

    // properties
    cProperties *props;

    // parameters
    typedef std::vector<ParamDescription> ParamDescriptions;
    ParamDescriptions params;
    StringToIntMap paramNameMap;

    // gates
    typedef std::vector<GateDescription> GateDescriptions;
    GateDescriptions gates;
    StringToIntMap gateNameMap;

    // locking
    bool locked;

  public:
    /** @name Constructors, destructor, assignment */
    //@{

    /**
     * Constructor.
     */
    cNEDComponent(const char *name, NEDElement *tree);

    /**
     * Destructor.
     */
    virtual ~cNEDComponent();
    //@}

    /** @name Redefined cObject methods */
    //@{
    /**
     * Produces a one-line description of object contents into the buffer passed as argument.
     */
    virtual std::string info() const;

    /**
     * Returns detailed info including gates and parameters in a multi-line string.
     */
    virtual std::string detailedInfo() const;
    //@}

    /** @name Setup */
    //@{
    /**
     * Adds a parameter to the declaration. The contained cPar and cProperties
     * objects in the struct will be taken over by cNEDComponent (and not
     * duplicated/copied).
     */
    virtual void addPar(const ParamDescription& paramDesc);

    /**
     * Adds a gate to the declaration. The contained cPar and cProperties
     * objects in the struct will be taken over by cNEDComponent (and not
     * duplicated/copied).
     */
    virtual void addGate(const GateDescription& gateDesc);

    /**
     * Sets the properties in this declaration. The properties object cannot be
     * modified afterwards.
     */
    virtual void setProperties(cProperties *props);

    /**
     * Add an "extends" name (super component) to this declaration.
     */
    virtual void addExtendsName(const char *name);

    /**
     * Add a "like" interface to this declaration.
     */
    virtual void addInterfaceName(const char *name);

    /**
     * Call lock() after setup is done.
     */
    void lock() {locked = true;}
    //@}

    /** @name Inheritance */
    //@{
    /**
     * Returns the number of "extends" names.
     */
    int numExtendsNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth "extends" name (k=0..numExtendsNames()-1).
     */
    const char *extendsName(int k) const;

    /**
     * Returns the number of interfaces.
     */
    int numInterfaceNames() const  {return interfacenames.size();}

    /**
     * Returns the name of the kth interface (k=0..numInterfaceNames()-1).
     */
    const char *interfaceName(int k) const;
    //@}

    /** @name Properties */
    //@{
    /**
     * Return the properties for this component. The properties are locked against
     * modification, use dup() to create a modifiable copy.
     */
    virtual cProperties *properties();
    //@}

    /** @name Parameters */
    //@{
    /**
     * Returns total number of the parameters.
     */
    virtual int numPars() const;

    /**
     * Returns the name of the kth parameter.
     */
    virtual const char *paramName(int k) const {return paramDescription(k).name.c_str();}

    /**
     * Returns the description of the kth parameter.
     * Throws an error if the parameter does not exist.
     */
    virtual const ParamDescription& paramDescription(int k) const;

    /**
     * Returns the description of the ith parameter.
     * Throws an error if the parameter does not exist.
     */
    virtual const ParamDescription& paramDescription(const char *name) const;

    /**
     * Returns index of the parameter specified with its name.
     * Returns -1 if the object doesn't exist.
     */
    virtual int findPar(const char *parname) const;

    /**
     * Check if a parameter exists.
     */
    bool hasPar(const char *s) const {return findPar(s)>=0;}

    /**
     * Sets the parameter value.
     */
    virtual void setParamValue(const char *paramName, cPar *value);
    //@}

    /** @name Gates */
    //@{
    /**
     * Returns the number of gates
     */
    virtual int numGates() const;

    /**
     * Returns the name of the kth gate.
     */
    virtual const char *gateName(int k) const {return gateDescription(k).name.c_str();}

    /**
     * Returns the description of the kth gate.
     * Throws an error if the gate does not exist.
     */
    virtual const GateDescription& gateDescription(int k) const;

    /**
     * Returns the description of the ith gate.
     * Throws an error if the gate does not exist.
     */
    virtual const GateDescription& gateDescription(const char *name) const;

    /**
     * Returns index of the given gate (0..numGates()), or -1 if not found
     */
    virtual int findGate(const char *name) const;

    /**
     * Check if a gate exists.
     */
    bool hasGate(const char *s) const  {return findGate(s)>=0;}

    /**
     * Sets the gate vector size.
     */
    virtual void setGateSize(const char *gateName, cPar *gateSize);
    //@}
};

#endif


