//=========================================================================
//
//  COBJECT.CC - part of
//                          OMNeT++
//           Discrete System Simulation in C++
//
//   Static members of
//    cObject
//
//   Member functions of
//    cObject   : ultimate base class for most objects
//
//  Author: Andras Varga
//
//=========================================================================

/*--------------------------------------------------------------*
  Copyright (C) 1992-2001 Andras Varga
  Technical University of Budapest, Dept. of Telecommunications,
  Stoczek u.2, H-1111 Budapest, Hungary.

  This file is distributed WITHOUT ANY WARRANTY. See the file
  `license' for details on this and other legal matters.
*--------------------------------------------------------------*/

#include <stdio.h>           // sprintf
#include <string.h>          // strcpy, strlen etc.
#include "csimul.h"
#include "cenvir.h"
#include "macros.h"
#include "cobject.h"
#include "cexception.h"

//==========================================================================
//=== GLOBAL VARIABLES

int   cObject::staticflag;
int   cObject::heapflag;

//=== Registration
Register_Class( cObject )

//==========================================================================
//=== cObject - member functions

static bool _do_find(cObject *obj, bool beg, const char *objname, cObject *&p, bool deep);
static bool _do_list(cObject *obj, bool beg, ostream& s);

char cObject::fullpathbuf[FULLPATHBUF_SIZE];


/*--------------------------------------------------------------*\

   Object ownership/contains relationships:
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   Ownership:
      Exclusive right and duty to delete the child objects.
      Ownership works thru ownerp/prevp/nextp and firstchildp pointers.
   'contains' relationship:
      Only for container classes, e.g. cArray or cQueue. Keeping track
      of contained objects works with another mechanism, NOT the previously
      mentioned ptrs. (E.g., cArray uses a vector, cQueue uses a separate
      list).
   The two mechanisms are INDEPENDENT.
   What cObject does:
      - owner of a new object can be explicitly given, if omitted,
        defaultOwner() will will be used.
      - an object created thru the copy constructor:
          - will have the same owner as original;
          - does not dup() or take objects owned by the original.
      - destructor calls dealloc() for owned objects (see later).
   Objects contained as data members:
      the enclosing object should own them.
   What container objects derived from cObject should do:
      - they use the functions: take(obj), drop(obj), dealloc(obj)
      - when an object is inserted, if takeOwnership() is true, should
        take ownership of object by calling take(obj).
        TAKEOWNERSHIP() DEFAULTS TO true.
      - when an object is removed, they should call drop(obj) for it if
        they were the owner.
      - copy constructor copies should dup() and take ownership of objects
        that were owned by the original.
      - destructor doesn't need not call dealloc() for objects: this will be
        done in cObject's destructor.
   cHead:
      special case: behaves as a container, displaying objects it owns as
      contents.
   --VA
\*-------------------------------------------------------------------*/

#define DETERMINE_STORAGE()  stor = heapflag   ? (heapflag=0, 'D') : \
                                    staticflag ? 'A' : 'S' ;

cObject::cObject(const cObject& obj)
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = opp_strdup( obj.namestr );

    ownerp = NULL;
    setOwner( obj.owner() );
    firstchildp = NULL;

    operator=( obj );
}

cObject::cObject()
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = NULL;

    ownerp = NULL;
    setOwner( defaultOwner() );
    if (storage()!='S')    /* to enable building global cHead lists */
          firstchildp = NULL;
}

cObject::cObject(const char *name)
{
    DETERMINE_STORAGE();
    tkownership = true;
    namestr = opp_strdup( name );

    ownerp = NULL;
    setOwner( defaultOwner() );
    if (storage()!='S')    /* to enable building global cHead lists */
          firstchildp = NULL;
}

cObject::cObject(const char *name, cObject *ownerobj)
{
    DETERMINE_STORAGE();
    namestr = opp_strdup( name );
    tkownership = true;

    ownerp = NULL;
    setOwner( ownerobj );
    if (storage()!='S')    /* to enable building global cHead lists */
          firstchildp = NULL;
}

cObject::~cObject()
{
    delete [] namestr;
    setOwner( NULL );

    /* delete owned objects */
    while (firstchildp!=NULL)
        dealloc( firstchildp );
    ev.objectDeleted( this );
}

void *cObject::operator new(size_t m)
{
    void *p = ::new char[m];
    if (p) heapflag = 1;
    return p;
}

cObject& cObject::operator=(const cObject&)
{
    // ownership not affected
    // name string is NOT copied from other object! (24.02.97 --VA)
    return *this;
}

void cObject::copyNotSupported() const
{
    throw new cException(eCANTCOPY,className(),name());
}

void cObject::info(char *buf)
{
    /* prepare one-line textual info about the object */
    sprintf( buf, "%-12s (%s)",
             fullName() ? fullName():"<noname>", className()
           );
}

void cObject::setOwner(cObject *newowner)
{
    if (ownerp!=NULL)   /* remove from owner's child list */
    {
         if (nextp!=NULL)
              nextp->prevp = prevp;
         if (prevp!=NULL)
              prevp->nextp = nextp;
         if (ownerp->firstchildp==this)
              ownerp->firstchildp = nextp;
         ownerp = NULL;
    }
    if (newowner!=NULL) /* insert into owner's child list as first elem. */
    {
         ownerp = newowner;
         prevp = NULL;
         nextp = ownerp->firstchildp;
         if (nextp!=NULL)
              nextp->prevp = this;
         ownerp->firstchildp = this;
    }
}

cObject *cObject::defaultOwner() const
{
    return simulation.localList();
}

void cObject::deleteChildren()
{
    bool nothing;                           // a bit difficult, because
    do {                                    // deleting a container object
         nothing = true;                    // may add new items
         cObject *t, *p = firstchildp;
         while (p)
         {
             t=p; p=p->nextp;
             if (t->storage()=='D')
                {delete t; nothing = false;}
         }
    } while (!nothing);
}

void cObject::destructChildren()
{
    while (firstchildp)
    {
       stor = firstchildp->storage();
       if (stor == 'D')
          delete firstchildp;
       else if (stor == 'A')
          firstchildp->destruct();
       else  /* stor == 'S' */
          firstchildp->setOwner( NULL );
    }
}

const char *cObject::fullPath() const
{
    return fullPath(fullpathbuf,FULLPATHBUF_SIZE);
}

const char *cObject::fullPath(char *buffer, int bufsize) const
{
    // check we got a decent buffer
    if (!buffer || bufsize<4)
    {
        if (buffer) buffer[0]='\0';
        return "(fullPath(): no or too small buffer)";
    }

    // append parent path + "."
    char *buf = buffer;
    if (owner()!=NULL)
    {
       owner()->fullPath(buf,bufsize);
       int len = strlen(buf);
       buf+=len;
       bufsize-=len;
       *buf++ = '.';
       bufsize--;
    }

    // append our own name
    opp_strprettytrunc(buf, fullName(), bufsize-1);
    return buffer;
}


/*------------------------------------------------------------------------*

 The forEach() mechanism
 ~~~~~~~~~~~~~~~~~~~~~~~
  o  The forEach() mechanism implemented in OMNeT++ is very special and
     slightly odd. The passed function is called for each object twice:
     once on entering and once on leaving the object. In addition, after
     the first ('entering') call to the function, it signals with its return
     value whether it wants to go deeper in the contained objects or not.
     Functions passed to forEach() will use static variables to store other
     necessary information. (Yes, this limits their recursive use :-( ).
  o  forEach() takes a function do_fn (of DoItFunc type) with 2 arguments:
     a (cObject *) and a (bool). First, forEach() should call do_fn with
     (this,true) to inform it about entering the object. Then, if this call
     returned true, it must call forEach(do_fn) for every contained object.
     Finally, it must call do_fn with (this,false) to let do_fn know that
     there's no more contained object.
  o  Functions using forEach() work in the following way: they call do_fn
     with (NULL,false,<additional args>) to initialize the static variables
     inside the function. Then they call forEach( (DoItFunc)do_fn ) for the
     given object. Finally, read the results by calling do_fn(NULL, false,
     <additional args>). DoItFuncs mustn't call themselves recursively!
  --VA
     ( yeah, I know this all is kind of weird, but changing it would take
       quite some work --VA )
 *------------------------------------------------------------------------*/

void cObject::forEach( ForeachFunc do_fn )
{
    do_fn(this,true);
    do_fn(this,false);
}

void cObject::writeTo(ostream& os)
{
    os << "(" << className() << ") `" << fullPath() << "' begin\n";
    writeContents( os );
    os << "end\n\n";
}

void cObject::writeContents(ostream& os)
{
    //os << "  objects:\n";
    _do_list( NULL, false, os );                    // prepare do_list
    forEach( (ForeachFunc)_do_list );
}

cObject *cObject::findObject(const char *objname, bool deep)
{
    cObject *p;
    _do_find( NULL, false, objname, p, deep ); // give 'objname' and 'deep' to do_find
    forEach( (ForeachFunc)_do_find );          // perform search
    _do_find( NULL, false, objname, p, deep ); // get result into p
    return p;
}

TInspector *cObject::inspector(int type, void *data)
{
    cInspectorFactory *p = findInspectorFactory(inspectorFactoryName());
    if (!p)
    {
        throw new cException("Inspector factory object '%s' for class '%s' not found",
                         inspectorFactoryName(), className());
        return NO(TInspector);
    }
    return p->createInspectorFor(this,type,data);
}

int cObject::cmpbyname(cObject *one, cObject *other)
{
    return opp_strcmp(one->namestr, other->namestr);
}

static bool _do_find(cObject *obj, bool beg, const char *objname, cObject *&p, bool deep)
{
    static const char *name_str;
    static cObject *r;
    static int ctr;
    static bool deepf;
    if (!obj)
    {
        name_str = objname;
        p = r;
        r = NULL;
        deepf = deep;
        ctr = 0;
        return true;
    }
    if (beg && obj->isName(name_str)) r=obj;
    return deepf || ctr==0;
}

static bool _do_list(cObject *obj, bool beg, ostream& s)
{
    static char buf[256];
    static int ctr;       // static is very important here!!!
    static ostream *os;
    if (!obj)
    {        // setup call
        ctr = 0;
        os = &s;
        return true;
    }

    if (beg)
    {
        if (ctr)
        {
            //*os << "  (" << obj->className() << ") `" << obj->name() << "'\n";
            obj->info(buf);
            *os << "   " << buf << "\n";
        }
        return ctr++ == 0;       // only one level!
    }
    else
        return true;
}

