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
// File:    InputHandler.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 6 2005
//
// Description: Create and manage inputs
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INPUTHANDLER
#define NISAC_SIMX_INPUTHANDLER


#include <boost/python.hpp>

#include "simx/Input.h"
#include "simx/InputFactory.h"
#include "simx/type.h"
//#include <boost/make_shared.hpp>

namespace simx {

/// Create and manage inputs, exists inside managers
/// 	ObjectIdent - type identifying a particular descendant class of Entity/Service/Info (not the identifier value)
template<typename ObjectIdent>
class InputHandler
{
    public:
	/// constructor, ProfileSetName is the name of the profile set to look for profiles in
	explicit InputHandler(const std::string& ProfileSetName);
    
	/// function to register a descendant of Input
	template<class InputClass> void registerInput(const ObjectIdent& inputIdent);
    
	/// Creates an input object, given its Class, ProfileID and Input::DataSource
	boost::shared_ptr<Input> createInput(const ObjectIdent& inputIdent, const ProfileID profileId, Input::DataSource& dataSource);
	
  
        /// Creates an input object given Class, ProfileID and Python profile object, python data object
  boost::shared_ptr<Input> createInput(const ObjectIdent& inputIdent, const ProfileID profileId, const PyProfile& profile, const boost::python::object& data);
  

  


    protected:
    private:
	/// loads profile into (empty) Input structure
	void loadProfile( const ProfileID profileId, boost::shared_ptr<Input> input );

         /// loads python profile into empty Input structure
  void loadProfile( const ProfileID profileId, const PyProfile& profile, boost::shared_ptr<Input> input);
  

	/// which profile set to look into for profiles
    	const std::string	fProfileSetName;

	/// type of map that stores info about profiles already read in
	/// - must use pair<ObjectIdent,ProfileID> so that each object type
	/// can have different input type even if they use the same ProfileID
	typedef Loki::AssocVector<std::pair<ObjectIdent, ProfileID>, boost::shared_ptr<Input> >	ProfileMap;
	/// stores information about profiles that were already read in (in appropriate Input objects)
	ProfileMap	fProfileMap;

	/// object that can create new (empty) Inputs of desired type
	InputFactory<ObjectIdent>	fInputFactory;
};

//======================================================================================


/// functions to register Inputs, to be done in .C files for the Service objects
template<typename ObjectIdent>
template<class InputClass>
inline void InputHandler<ObjectIdent>::registerInput(const ObjectIdent& inputIdent)
{
    fInputFactory.Register( inputIdent, inputCreator<InputClass> );
}

template<typename ObjectIdent>
InputHandler<ObjectIdent>::InputHandler(const std::string& profileSetName)
    : fProfileSetName( profileSetName )
{
}

template<typename ObjectIdent>
boost::shared_ptr<Input> InputHandler<ObjectIdent>::createInput(const ObjectIdent& inputIdent, const ProfileID profileId, Input::DataSource& dataSource)
{
    Logger::debug3() << "InputHandler: creating Input " << inputIdent << " " << profileId << std::endl;

    boost::shared_ptr<Input>& input = fProfileMap[ make_pair(inputIdent,profileId) ];
    if( !input )
    {
      Logger::debug3() << "InputHandler: loading in new profile" << std::endl;
	// the Input does not yet have its Profile read in, so do it
	input.reset( fInputFactory.CreateObject(inputIdent, input ) );
	SMART_ASSERT( input );
	
	// read in the profile
	loadProfile( profileId, input );

    }
    
    // make a copy of the Input that already have its profile read in:
    boost::shared_ptr<Input> newInput( fInputFactory.CreateObject(inputIdent, input) );
    SMART_ASSERT( newInput );
    
    // read-in the Data input:
    newInput->readData(dataSource);

    // see if the reading failed:
    if( dataSource.fail() )
    {
	Logger::warn() << "InputHandler: reading data failed when creating Input " << inputIdent 
		       << " with ProfileSetName " << fProfileSetName << std::endl;
    }
    // see if there's something left unread
    ws(dataSource);
    if( !dataSource.eof() )
    {
	Logger::debug2() << "InputHandler: not all data read when creating Input " << inputIdent 
			 << " with ProfileSetName " << fProfileSetName << std::endl;
    }
    
    return newInput;
}


  //for python input
template<typename ObjectIdent>
boost::shared_ptr<Input> InputHandler<ObjectIdent>::createInput( const ObjectIdent& inputIdent, const ProfileID profileId, const PyProfile& profile, const boost::python::object& data )

{
  Logger::debug3() << "InputHandler: creating Input " << inputIdent << " " << profileId << std::endl;

  boost::shared_ptr<Input>& input = fProfileMap[ make_pair(inputIdent,profileId) ];
  if( !input )
    {
      Logger::debug3() << "InputHandler: loading in new profile" << std::endl;
      // the Input does not yet have its Profile read in, so do it
      input.reset( fInputFactory.CreateObject(inputIdent, input ) );
      SMART_ASSERT( input );
      
      // read in the profile
      loadProfile( profileId, profile, input );
    }
  
    // make a copy of the Input that already has its profile read in:
    boost::shared_ptr<Input> newInput( fInputFactory.CreateObject(inputIdent, input) );
    SMART_ASSERT( newInput );
    // set python data object for new input
    newInput->readData( data );
    return newInput;
}
  
} // namespace

#endif // NISAC_SIMX_INPUTHANDLER

