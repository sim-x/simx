//--------------------------------------------------------------------------
// File:    ServiceManager.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: November 29 2004
//
// Description: Create and manage services
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_SERVICEMANAGER
#define NISAC_SIMX_SERVICEMANAGER

#include "simx/type.h"
#include "simx/ServiceFactory.h"
#include "simx/ServiceData.h"
#include "simx/InputHandler.h"
#include "simx/Service.h"
#include "simx/Python/PyServiceData.h"

#include "loki/Singleton.h"
#include <boost/python.hpp>

#include <set>
#include <map>
#include <string>

namespace simx {

/// Create and manage services
class ServiceManager
{
    friend ServiceManager& theServiceManager();
    friend class Loki::CreateUsingNew<ServiceManager>;


    public:

	/// Makes ready to create Services, DOES NOT create any, only remebers necessary info
	/// to create it later when asked to
       void prepareServices(const std::string& servFile);
	
	// prepare service from a python object
       void preparePyService(const Python::PyServiceData& );
       
	/// functions to register services, to be done in .C files for the Service objects
	template<class ServiceClass, typename EntityClass, typename InputClass> 
	void registerService(const Service::ClassType& servClass);
    
  /// register python services
  void registerPyService( const Service::ClassType&, const boost::python::object&  );



	/// Creates a service object
	boost::shared_ptr<Service> createService(const ServiceName&, Entity&);


	/// for python services TODO: (PYTHON) make private 
  typedef std::map<Service::ClassType, boost::python::object> PyServiceClassMap;
  PyServiceClassMap fPyObjMap;
	
    private:
	typedef struct ServiceInfoTag_ {
	    ServiceInfoTag_() : fCode(""), fInput() {}
	    ServiceInfoTag_(const Service::ClassType& c, const boost::shared_ptr<Input> i) 
		: fCode(c), fInput(i) {}
	
	    Service::ClassType	fCode;
	    const boost::shared_ptr<Input> 	fInput;
	} ServiceInfoItem;
    
	/// stores pre-processed content of SERVICE_FILENAME file
	typedef std::map<ServiceName, ServiceInfoItem> ServiceInfoMap;
	ServiceInfoMap		fServiceInfoMap;

	
	/// stores the already-registered services
	std::set<std::string>	fRegisteredServices;
	
	/// factory to create service objects
	ServiceFactory		fServiceFactory;

	/// stores info about alread-processed profiles
	InputHandler<Service::ClassType>	fInputHandler;

	/// private so that only singleton (friend) can create it
	ServiceManager();
	~ServiceManager();
	/// unimplemneted
	ServiceManager(const ServiceManager&);
	ServiceManager& operator=(const ServiceManager&);
	
	/// singleton object holding the ServiceManager
	typedef Loki::SingletonHolder<ServiceManager,
                              Loki::CreateUsingNew,
                              Loki::DefaultLifetime> sServiceManager;

};


inline ServiceManager& theServiceManager()
{
    return ServiceManager::sServiceManager::Instance();
}

//======================================================================================

template<class ServiceClass, typename EntityClass, typename InputClass> 
void ServiceManager::registerService(const Service::ClassType& servName)
{
    if( servName==Service::ClassType() )
    {
	std::cerr << "ERROR: ServiceManager: service name '" << servName
	    << "' is invalid" << std::endl;
	throw Common::Exception("Error service registration");
    }

    if( !fRegisteredServices.insert(servName).second )
      {
    	std::cerr << "ERROR: ServiceManager: service " << servName
		  << " is already registered" << std::endl;
    	throw Common::Exception("Error service registration");
      }
    
    fInputHandler.registerInput<InputClass>(servName);
    
    fServiceFactory.Register( servName, serviceCreator<ServiceClass, EntityClass, InputClass> );
    
}


} // namespace

#endif 


