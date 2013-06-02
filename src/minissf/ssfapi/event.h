/**
 * \file event.h
 * \brief Header file for the event class.
 *
 * This header file contains the definition of the event class;
 * Users do not need to include this header file directly; it is
 * included from ssf.h.
 */

#ifndef __MINISSF_EVENT_H__
#define __MINISSF_EVENT_H__

#include "ssfapi/ssf_common.h"
#include "ssfapi/compact_datatype.h" // no longer required

namespace minissf {

/**
 * \brief The factory method for an event.
 *
 * An event factory is a user-defined callback function that takes a
 * byte array and returns a newly created event object. A callback
 * function of this type must be provided and registered for each
 * user-defined event subclass, using SSF_DECLARE_EVENT and
 * SSF_REGISTER_EVENT macros. The kernel will map the name of the
 * event class to its factory method so that it can instantiate an
 * event object whenever the event is traveling across address space
 * boundaries. This is for serialization. The factory method should
 * take as parameter a pointer to the buffer and a size; the buffer is
 * filled up using the pack() method; the user is expected to restore
 * the data fields of the derived event class in the factory method.
 */
typedef Event* (*EventFactory)(char* buf, int siz);

/**
 * \brief Declare a user-defined event class.
 *
 * This macro must be used \b inside every user-defined event
 * subclass. Internally, this macro creates an identifier for the
 * event, which will be used when the simulator needs to serialize and
 * ship the event across the address space. The identifier of the
 * event will be used at the receiving end to reconstruct the event
 * object and deserialize the data fields.
 *
 * \param classname the name of the event class (do not use quotes)
 */
#define SSF_DECLARE_EVENT(classname) \
  static int event_class_ident_##classname; \
  virtual int event_class_ident() { return event_class_ident_##classname; }

/**
 * \brief Register a user-defined event class.
 *
 * This macro is expected to be used in the source code where the
 * event class (in particular, the event factory method of the event
 * class) is defined. Every user-defined event subclass must register
 * itself using this macro. The factory method should be a static
 * member of the event subclass.
 *
 * \param classname is the name of the event class (do not quotes).
 * \param eventfactory points to the factory method of the event
 *
 * The following example describes the use of the event factory method
 * and the two macros for declaring and registering an event subclass:
\verbatim
In myevent.h file:
  class MyEvent : public Event {
    int int_data;
    ...
    static MyEvent* my_event_factory(char* buf, int siz);
    virtual int pack(char* buf, int siz) {
      CompactDataType* cdata = new CompactDataType;
      cdata->add_int(int_data);
      ...
      return cdata->pack_and_delete(buf, siz);
    }
    SSF_DECLARE_EVENT(MyEvent);
  };

In myevent.cc file:
  MyEvent* MyEvent::my_event_factory(char* buf, int siz) {
     MyEvent* newevt = new MyEvent;
     CompactDataType* cdata = new CompactDataType;
     cdata->unpack(buf, siz);
     cdata->get_int(&newevt->int_data);
     ...
     delete cdata;
     return newevt;
  }
  SSF_REGISTER_EVENT(MyEvent, MyEvent::my_event_factory);
\endverbatim
 *
 */
#define SSF_REGISTER_EVENT(classname,eventfactory) \
  int classname::event_class_ident_##classname = \
    minissf::Event::register_event(#classname, eventfactory);

/**
 * \brief Messages sent across channels.
 *
 * An event is a message that is sent from one entity to another
 * through the channels. An event written to an outchannel will be
 * delivered by the system to all inchannels that are mapped to the
 * outchannel with a delay that is the sum of the channel delay
 * (defined by the outchannel), the mapping delay (defined during
 * mapping), and the per-write delay (provided at the time when the
 * write method is called). A user-defined event that carries useful
 * data must derive from this base event class.
 */
class Event {
 public:
  /** \brief The constructor of an event. */
  Event() {}

  /**
   * \brief The copy constructor.
   *
   * The copy constructor is used to make an explicit copy of the
   * instance. The method takes necessary steps to copy the internal
   * member data. All derived event classes must provide their own
   * copy constructor that overload this method, if any member data in
   * the subclass needs to be copied.
   *
   * \param evt a reference to the event that needs to be copied
   *
   * \b IMPORTANT: in C++, since a copy constructor is treated just
   * like all other constructors, in the derived event class, the user
   * needs to explictly call the copy constructor of the base
   * class. Otherwise, the default constructor Event() will be called,
   * which does not really copy the data fields. As a rule of thumb,
   * we recommend that one should \b always call the superclass's copy
   * constructor.
   */
  Event(const Event& evt) {}

  /** \brief The event destructor. */
  virtual ~Event() {}

  /**
   * \brief Clone an event.
   *
   * The clone method is used to make an explicit copy of an event
   * object. The user is expected to provide such a method in the
   * derived event class. Whenever it is called, a new instance of the
   * event is returned.  Normally the overloaded method can be
   * implemented to simply use the copy constructor:
\verbatim
Event* myEvent::clone() { return new myEvent(*this); }
\endverbatim
   * The method is called by the kernel when the simulator decides it
   * needs to copy an event object, for example, when shipping the
   * event across the address space boundaries. Failing to provide
   * such a method will cause a derived event object to be sent
   * incorrectly on distributed platforms.
   */
  virtual Event* clone();

  /**
   * \brief Pack the event into a compact form to be send to a
   * remotely machine.
   *
   * This method is used to serialize the event object. The user is
   * responsible to pack the data fields in the derived event object
   * when the event is about to be shipped by the kernel to a remote
   * machine in a distributed simulation environment. The packed
   * (serialized) data will be unpacked (deserialized) at the
   * destination machine. This is done by the kernel calling the
   * factory method of the corresponding event class, which should
   * have already been registered using SSF_DECLARE_EVENT and
   * SSF_REGISTER_EVENT macros. The pack method returns the size of
   * the byte array after the serialization.  At the event base class,
   * the method does nothing other than returning zero as a special
   * case since the base event class does not have data that needs to
   * be serialized.
   */
  virtual int pack(char* buf, int siz) { return 0; }

 public: 
  // all event must be declared this way, including this base class
  SSF_DECLARE_EVENT(Event);

  // public methods used internally for event class registration
  static Event* create_registered_event(int id, char* buf, int siz);
  static int register_event(const char* classname, EventFactory factory);
  static Event* create_baseclass_event(char* buf, int siz);
  static SET(STRING)* registered_event_names;
  static VECTOR(EventFactory)* registered_event_factories;
}; /*class Event*/

}; /*namespace minissf*/

#endif /*__MINISSF_EVENT_H__*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
