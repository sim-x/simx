// Copyright (c) 2012. Los Alamos National Security, LLC. 

// This material was produced under U.S. Government contract DE-AC52-06NA25396
// for Los Alamos National Laboratory (LANL), which is operated by Los Alamos 
// National Security, LLC for the U.S. Department of Energy. The U.S. Government 
// has rights to use, reproduce, and distribute this software.  

// NEITHER THE GOVERNMENT NOR LOS ALAMOS NATIONAL SECURITY, LLC MAKES ANY WARRANTY, 
// EXPRESS OR IMPLIED, OR ASSUMES ANY LIABILITY FOR THE USE OF THIS SOFTWARE.  
// If software is modified to produce derivative works, such modified software should
// be clearly marked, so as not to confuse it with the version available from LANL.

// Additionally, this library is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License v 2.1 as published by the 
// Free Software Foundation. Accordingly, this library is distributed in the hope that 
// it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See LICENSE.txt for more details.

//--------------------------------------------------------------------------
// File:    ServiceManager.C
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: November 29 2004
//
// @@
//
//--------------------------------------------------------------------------

#include <boost/python.hpp>

#include "simx/ServiceManager.h"

#include "simx/Entity.h"
#include "simx/Input.h"
#include "simx/logger.h"

#include "simx/Python/PyEntity.h"
#include "simx/Python/PyService.h"
#include "simx/Python/PyServiceInput.h"
#include "simx/Python/PyServiceFactory.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace simx {

ServiceManager::ServiceManager()
    :	fInputHandler("ServiceProfile")
{
//CANNOT    Logger::debug1() << "ServiceManager: in constructor" << endl;

  //TODO (Python): Is there a better place to register PyServiceInput?
  fInputHandler.registerInput<Python::PyServiceInput>("PyService");

}

ServiceManager::~ServiceManager()
{
//CANNOT    Logger::debug1() << "ServiceManager: in destructor" << endl;
  // vector<shared_ptr<Service> >::iterator iter;
  // for (iter = fPyServices.begin(); iter != fPyServices.end(); ++iter )
  //   delete *iter;
  //  fPyServices.clear();
  
}

  // TODO (PYTHON) : template specialization of registerService here instead?
  void ServiceManager::registerPyService(const Service::ClassType& name,
					 const python::object& py_service_class) {

    if( name==Service::ClassType() )
    {
	std::cerr << "ERROR: ServiceManager: service name '" << name
	    << "' is invalid" << std::endl;
	throw Common::Exception("Error service registration");
    }

     if( !fRegisteredServices.insert(name).second )
    {
	std::cerr << "ERROR: ServiceManager: service " << name
	    << " is already registered" << std::endl;
	throw Common::Exception("Error service registration");
    }
    
     //fInputHandler.registerInput<InputClass>(name);
     
     if (! fPyObjMap.insert( make_pair( name, py_service_class)).second) {
      cerr << "ERROR: ServiceManager.C: Python service class '" << name 
		<< "' is already registered" << endl; 
      throw Common::Exception("Error in Python service class registration");

     }

     fServiceFactory.Register( name, serviceCreator<Python::PyService, 
						    Python::PyEntity, 
						    Python::PyServiceInput>); 
  }

void ServiceManager::prepareServices(const string& serviceFiles)
{
    Logger::debug2() << "ServiceManager: in prepareServices, serviceFiles= '" << serviceFiles << "'" << endl;

    stringstream sstr;
    sstr << serviceFiles;
    string servFile;
    // read all service files
    while( sstr >> servFile )
    {
	Logger::debug3() << "ServiceManager: processing file " << servFile << endl;
    
        ServiceData::Reader reader(servFile);
        while (reader.MoreData())
        {
    	    ServiceData data = reader.ReadData();
    
            Logger::debug3() << "ServiceManager: preparing service, data=" << data << endl;
    		
    	    // get input object and fill it with data
    	    boost::shared_ptr<Input> input = fInputHandler.createInput( data.getClassType(), data.getProfileId(), data.getData() );
    	    SMART_ASSERT( input );
    
    	    // store the info for later use
	    pair<ServiceInfoMap::iterator, bool> insRes = fServiceInfoMap.insert( 
    							    make_pair( 
    		    						data.getName(),
    		    						ServiceInfoItem(data.getClassType(), input)
    							    )  );
    	    if( !insRes.second )
    	    {
    	        Logger::warn() << "ServiceManager: service with name '" 
    		    << data.getName() << "' (ClassType=" 
    		    << data.getClassType() << ") collides with another" << endl;
    	    }
	} // while(MoreData)
    } // (filenames)
}

//----------------------------------------------------------------------------------------

//  void ServiceManager::preparePyService(const python::object& serv_obj) {
    void ServiceManager::preparePyService(const Python::PyServiceData& serv_data) {
    
    Logger::debug2() << "ServiceManager: in preparePyService " << endl;
    try {
      // ServiceName name = python::extract<ServiceName>(serv_obj.attr("name_"));
      // Service::ClassType type = python::extract<Service::ClassType>(serv_obj.attr("type_"));
      // ProfileID id =  python::extract<ProfileID>(serv_obj.attr("profile_id_"));
      // string data = python::extract<string>
      // (serv_obj.attr("data_"));
      // stringstream ss(data);
      Logger::debug3() << "ServiceManager: preparing service, data=" << serv_data.fName
		       << " "
		       << serv_data.fClassType << " " << serv_data.fProfileId
		       << endl;
      boost::shared_ptr<Input> input = fInputHandler.createInput( "PyService",
								  serv_data.fProfileId,
								  serv_data.fProfile,
								  serv_data.fData);
      SMART_ASSERT( input );
      
      // store the info for later use
      pair<ServiceInfoMap::iterator, bool> insRes = 
	fServiceInfoMap.insert( make_pair( serv_data.fName,
					   ServiceInfoItem(serv_data.fClassType, 
							   input)));
      if( !insRes.second )
	{
	  Logger::warn() << "ServiceManager: service with name '" 
			 << serv_data.fName << "' (ClassType=" 
			 << serv_data.fClassType << ") collides with another" << endl;
	}
    }
    catch(...) {
      PyErr_Print();
      PyErr_Clear();
    }
  }


//----------------------------------------------------------------------------------------

boost::shared_ptr<Service> ServiceManager::createService(const ServiceName& name, Entity& ent)
{
    Logger::debug2() << "ServiceManager: creating service name=" << name << " on entity " << ent.getId() << endl;

    ServiceInfoMap::const_iterator iter = fServiceInfoMap.find( name );
    if( iter == fServiceInfoMap.end() )
    {
	Logger::error() << "ServiceManager: service name=" << name << " does not exist" << endl;
	return boost::shared_ptr<Service>();
    }
    
    const ServiceName& nameArg = iter->first;	///< this reference will be valid all the time
    const ServiceInfoItem& info = iter->second;
    SMART_ASSERT( info.fInput );
    boost::shared_ptr<Service> 
      service( fServiceFactory.CreateObject(info.fCode, nameArg, 
				      ent, *info.fInput, info.fCode) );
    
    
    SMART_ASSERT( service );
    return service;
}

} // namespace

