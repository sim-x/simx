//--------------------------------------------------------------------------
// $Id: InfoHandler.h,v 1.11 2008/10/17 19:11:49 sunil Exp $
//--------------------------------------------------------------------------
// File:    InfoHandler.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Jan 11 2005
//
// Description: 
//	Object that hold info about which method to call to handle an Info message
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INFOHANDLER
#define NISAC_SIMX_INFOHANDLER

#include "simx/Info.h"
#include "simx/logger.h"

namespace simx {

  class InfoHandler;

  /// returns an object identical to InfoClass, but overrides getInfoHandler function
  /// which returns on object that returns info about the InfoClass type
  template<class InfoClass>
  class InfoHandlerWrapper : public InfoClass
  {
  public:
    InfoHandlerWrapper();
    InfoHandlerWrapper(const InfoClass& x);	///< copy-constructor
	    
    virtual ~InfoHandlerWrapper();
    virtual const InfoHandler& getInfoHandler() const;
  private:
  };

  class Service;

  /// Base class for all Info handlers 
  class InfoHandler
  {
  public:
    virtual ~InfoHandler() = 0;
    virtual boost::shared_ptr<Info> create() const = 0;
    virtual boost::shared_ptr<Info> create(const Info&) const = 0;	///< for copy-constructing
    virtual void execute(boost::shared_ptr<Service>, boost::shared_ptr<Info>, bool=true) const = 0;
    //execute an out-of-band info ( controlInfo )
    virtual void executeControl( boost::shared_ptr<Service>, boost::shared_ptr<Info>, bool=true) const = 0;
    virtual Info::ClassType getClassType() const = 0;
    virtual size_t getByteSize() const = 0;
  };

  /// object that stores information about which handler (member function)
  /// of Recipient to call to handle an particular InfoClass type
  template<class InfoClass>
  class InfoHandlerDerived : public InfoHandler
  {
  private:
    const Info::ClassType		fClassType;

  public:
    /// constructor
    explicit InfoHandlerDerived(const Info::ClassType type);
    virtual ~InfoHandlerDerived();
	
    /// creation function (empty objects)
    virtual boost::shared_ptr<Info> create() const;

    /// creation function (copy construction)
    virtual boost::shared_ptr<Info> create(const Info&) const;

    /// takes a particular instance of a recipient and calls its appropriate
    /// handler with the info as an argument
    /// If the recipient is not derived from InfoRecipient<InfoClass>, then it is an error
    /// and an error message is output iff the doError flag is true
    /// If doError flag is false, then it is not an error, only a trial was made
    virtual void execute(boost::shared_ptr<Service> baseService, boost::shared_ptr<Info> baseInfo, bool doError) const;
    /// execute an out-of-band info ( controlInfo )
    virtual void executeControl( boost::shared_ptr<Service> baseService, 
				 boost::shared_ptr<Info> baseInfo, bool doError ) const;
    
    /// returns class type of the entry
    virtual Info::ClassType getClassType() const;
	
    /// returns sizeof(InfoClass)
    virtual size_t getByteSize() const;
  };

  //============================================================================

  // needed to print InfoHandlerMap in InfoManager
  inline std::ostream& operator<<(std::ostream& os, const InfoHandler& ih)
  {
    os << "IH";
    return os;
  }

  inline std::ostream& operator<<(std::ostream& os, const InfoHandler* ih)
  {
    if(ih) { os << *ih; }
    else { os << "NULL"; }
    return os;
  }

} // namespace

#endif 

//--------------------------------------------------------------------------
//--------------------------------------------------------------------------
