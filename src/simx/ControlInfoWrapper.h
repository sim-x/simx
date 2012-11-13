//--------------------------------------------------------------------------
// File:    ControlInfoWrapper.h
// Module:  simx
// Author:  Sunil Thulasidasan
// Created: Oct 09 2008
//
// Description: 
//	Wrapper class around an Info that is sent out-of-band. Will hold
//      information such as time sent, destination entity service etc.
//      //TODO: mechanism to delay acting on this info if the simulation clock
//      //TODO: on receiving LP is out of sync with message time.    
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_CONTROL_INFO_WRAPPER_H
#define NISAC_SIMX_CONTROL_INFO_WRAPPER_H

#include "simx/type.h"
#include "simx/Info.h"
#include "simx/InfoManager.h"
#include "simx/logger.h"


namespace simx {

  class ControlInfoWrapper

  {

  public:

    ControlInfoWrapper();
    void setSentTime( const Time time );
    Time getSentTime() const;
    
    //packing and unpacking 
    void pack(PackedData&) const;
    void unpack(PackedData&);

    void print(std::ostream& os) const;
    //void execute(LP& lp);
    void execute ();

    const EntityID& getDestEntity() const;
    const ServiceAddress& getDestService() const;

    void setTo( const EntityID entId, const ServiceAddress servAddr );
    void setDelay(const Time time );
    void setInfo(const boost::shared_ptr<const Info> info );
    
    const Time& getDelay() const;

    void setSrcLP( const LPID );
    LPID getSrcLP() const;
    void setDestLP( const LPID );
    LPID getDestLP() const;

  private:
    
    EntityID fDestEntity;
    ServiceAddress fDestService;
    Time fSentTime;
    Time fDelay; //in case a delay needs to be specified.

    boost::shared_ptr<const Info> fInfo; /* pointer to actual info; 
					    Just as in-band sending, we use Info class
					    to hold the data */
    LPID fSrcLP;
    LPID fDestLP;

  };

  inline std::ostream& operator<<(std::ostream& os, const ControlInfoWrapper& c )
  {
    c.print( os );
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const ControlInfoWrapper* cp )
  {
    if(cp) { cp->print(os); }
    else { os << "NULL"; }
    return os;
  }

}//namespace
#endif
