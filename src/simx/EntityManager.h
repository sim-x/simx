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
// File:    EntityManager.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 9 2005
//
// Description: Create and manage entities
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_ENTITYMANAGER
#define NISAC_SIMX_ENTITYMANAGER

#include "simx/control.h"
#include "simx/type.h"
#include "simx/EntityFactory.h"
#include "simx/InputHandler.h"
#include "simx/logger.h"

#include "loki/Singleton.h"

#include <list>
#include <set>
#include <string>
#include <map>

#include <boost/python.hpp>



namespace simx {

class Entity;
class EntityData;
class Controller;

  // fwd declaration of PyEntityData
  namespace Python {
  class PyEntityData;
  }
/// Create and manage entities
class EntityManager
{
    friend EntityManager& theEntityManager();
    friend class Loki::CreateUsingNew<EntityManager>;
  

    public:
        /// function for creating one entity given its Id, type, profile and userdata (as string that woudl have been read from ENTITY_FILE)
	/// (the input argument is actually an Info so that the content can be sent to
	/// another machine)
	/// (may NOT be created on any LP on this machine, this depends on 
	/// the recult of entityPlacing function)
	/// - returns 'true' iff the entity was created on this machine (some of local LPs)
	/// - send information about the new entity to all other machines if notifyOtherMachines = true (SHOULD DO!)
        bool createEntity(const EntityID& id, const Entity::ClassType& type, const ProfileID profileId, const std::string data, bool notifyOtherMachines = true);
	
	/// reads in data input file and creates entities (the ones that belong to the lps)
	/// calls createEntity()
	void createEntities(const std::string& dataFile);

	/// set the pointer to an entity given it's ID (given it resides on this machine)
	/// returns false when not found.
	/// the pointer is NOT 0 after the call if the return value is true
	template<class EntityClass> 
	bool getEntity( const EntityID& id, boost::shared_ptr<EntityClass>& ) const;
   
    
	/// LK: should NOT be needed, use probeEntities instead
	/// Fills the list with the IDs of entities on this machine
	///void getEntityIds(std::list<EntityID>& ids) const;

	/// returns an LP where an Entity sits
	LPID findEntityLpId( const EntityID& id ) const;

	/// Registers entity, given its class (and class for its input)
	/// the second parameter is a pointer to a function that 
	/// will be run on every node for every entity on input (regardless of whether it will
	/// be created there or not). May be 0, in which case nothing is run.
	/// EntityID - entity to be create
	/// InputClass& - the same input as the entity's constructor will receive
	template<class EntityClass, class InputClass> 
	void registerEntity(const Entity::ClassType&, void (*)(const EntityID&, const InputClass&) = 0);

  void registerPyEntity( const Entity::ClassType&, const boost::python::object&  );


	/// registers a function that will return LPID where an Entity
	/// lives, given its EntityID. Used both for creating Entities
	/// and subsequently sending Infos.
	/// Multiple functions can be registered, they are called until
	/// one returns true, in the order they were registered. If none
	/// of them returns true (or none is registered), a default placing
	/// function is called.
	///
	/// A placing function description:
	/// param1: ID of an entity to place (input)
	/// param2 LPID where to place it (output) - must be in [0,Control::getNumLPs()] if return is true
	/// return: true iff the function was able to find LPID for the EntityID
	/// (if it cannot place the given EntityID, it must return false)
	void registerPlacingFunction( bool (*)(const EntityID&, LPID&) );

	/// Goes through all entities on this LP, sees if it can be of type T
	/// and on each that is (a descendant of) T, calls member fuction method
	/// - (the second optional argument is passes to the method call as reference
	/// and thus can collect any information needed if method also gets a reference)
	/// - the last argument (with default) is the type of entity (fist element of EntityID)
	/// which should be probed. If left to the default, all entities will be probed
	template<typename T>
	void probeEntities( void (T::*method)(void), boost::tuples::element<0, EntityID>::type = EntityID().get<0>() ) const;
	template<typename T, typename Arg1, typename Arg1a>	///< the Arg1a is because the actual argument (type Arg1a) can be derived from Arg1, but not exactly Arg1
	void probeEntities( void (T::*method)(Arg1), Arg1a&, boost::tuples::element<0, EntityID>::type = EntityID().get<0>() ) const;
	
	
	/// creates controller entity
	void createController();

	/// returns reference to the Controller:
	Controller& getController() const;

  // bool createPyEntity(const EntityID& id, const Entity::ClassType&,
  // 		      const ProfileID, const boost::python::object& ) ;

  bool createPyEntity(const Python::PyEntityData& );

  
    protected:
    private:

	/// like createEntity, but accepts Input for input
	bool createEntityPrivate(const EntityID& id, const Entity::ClassType& type, 
				 const ProfileID profileId, Input::DataSource& data);

  bool createEntityonLP(const EntityID&, const Entity::ClassType&, 
			const ProfileID, const boost::shared_ptr<Input>&);


	/// default function to place entities on LPs
	/// must ALWAYS be able to place an Entity
	LPID defaultEntityPlacingFunction( const EntityID& entId ) const;

	typedef bool (*EntityPlacingFunction)(const EntityID&, LPID&);
	typedef std::vector<EntityPlacingFunction> EntityPlacingFunctionContainer;
	EntityPlacingFunctionContainer	fEntityPlacingFunctionContainer;	///< holds info about which functions to call to find an Entitie's LPID
    
	typedef std::map<EntityID,boost::shared_ptr<Entity> >	EntityPtrMap;
	EntityPtrMap	fEntityPtrMap;	///< stores entities that sit on this machine

	/// produces Inputs (from input stream) to give to an Entity constructor
	InputHandler<Entity::ClassType>	fInputHandler;
	
	/// stores info about how to create each Entity object
	typedef Loki::AssocVector<
		    Entity::ClassType, 
		    boost::shared_ptr<BaseEntityCreator>
		> EntityCreatorMap;	///< assoc vector of both functions needed to create an entity
	EntityCreatorMap	fEntityCreatorMap;	///< replaces entity factory


  boost::shared_ptr<PyEntityCreator> fPyEntityCreator;
  //PyEntityCreator* fPyEntityCreator;

	/// some LP that's on this machine:
	LP*	fMyLpPtr;
	
	/// controller:
	boost::shared_ptr<Controller>	fControllerPtr;

	/// private so that only singleton (friend) can creat it
	EntityManager();
	~EntityManager();
	/// unimplemented
	EntityManager(const EntityManager&);
	EntityManager& operator=(const EntityManager&);

	/// singleton object holding EntityManager
	typedef Loki::SingletonHolder<EntityManager,
                              Loki::CreateUsingNew,
                              Loki::DefaultLifetime> sEntityManager;

};

inline EntityManager& theEntityManager()
{
    return EntityManager::sEntityManager::Instance();
}

//=====================================================================

template<class EntityClass, class InputClass>
void EntityManager::registerEntity(const Entity::ClassType& name, 
				   void (*preCreator)(const EntityID&, const InputClass&) )
{
//TODO: add a check (compile time at best) that EntityClass is derived from Entity etc...

    if( name==Entity::ClassType() )
    {
	std::cerr << "ERROR: EntityManager: entity name '" << name
	    << "' is invalid" << std::endl;
	throw Common::Exception("Error entity registration");
    }

  /*   if( !fEntityCreatorMap.insert( std::make_pair( */
/* 	    name, */
/* 	    new DerivedEntityCreator<EntityClass, InputClass>(preCreator) */
/* 	) ).second ) */
/*     { */
/* 	std::cerr << "ERROR: EntityManager: entity '" << name */
/* 	    << "' is already registered" << std::endl; */
/* 	throw Common::Exception("Error entity registration"); */
/*     } */

    // register the Input object:
 
    //fInputHandler.registerInput<InputClass>(name);
}





template<class EntityClass>
bool EntityManager::getEntity( const EntityID& id, boost::shared_ptr<EntityClass>& ptr ) const
{
    EntityPtrMap::const_iterator iter = fEntityPtrMap.find( id );
    if( iter == fEntityPtrMap.end() )
    {
	return false;
    } else
    {
	boost::shared_ptr<EntityClass> ent = boost::dynamic_pointer_cast<EntityClass>(iter->second);
	if( !ent )
	{
	    Logger::warn() << "EntityManager: cannot cast entity " << id
		<< " to type " << typeid(EntityClass).name() << std::endl;
	    return false;
	}
	
	ptr = ent;
	SMART_ASSERT( ptr );
	return true;
    }
}

template<typename T>
void EntityManager::probeEntities( void (T::*method)(void), boost::tuples::element<0, EntityID>::type eType ) const
{
    Logger::debug3() << "EntityManager: probingEntities() with " << method << std::endl;
    SMART_ASSERT( method ).msg("Cannot probe with NULL member pointer");
    
    
    for(EntityPtrMap::const_iterator iter = fEntityPtrMap.begin();
	iter != fEntityPtrMap.end();  
	++iter ) 
    {
	// only look at entities of the right kind
	if( eType != EntityID().get<0>() && eType != iter->first.get<0>() )
	    continue;
    
	// get the pointer to a (possibly derived from Entity) class T
	boost::shared_ptr<T> entPtr = boost::dynamic_pointer_cast<T>( iter->second );
	
	// if success, then run method (otherwise the object was not derived from T, and will not be able to respond)
	if( entPtr )
	{
	    ((*entPtr).*method)();
	}
    }
}


//template<typename T, typename Arg1, typename Arg1a>
template<typename T, typename Arg1, typename Arg1a>
void EntityManager::probeEntities( void (T::*method)(Arg1), Arg1a& arg1, boost::tuples::element<0, EntityID>::type eType ) const
{
    Logger::debug3() << "EntityManager: probingEntities(Arg) with " << method << std::endl;
    SMART_ASSERT( method ).msg("Cannot probe with NULL member pointer");
    
    
    for(EntityPtrMap::const_iterator iter = fEntityPtrMap.begin();
	iter != fEntityPtrMap.end();  
	++iter ) 
    {
	// only look at entities of the right kind
	if( eType != EntityID().get<0>() && eType != iter->first.get<0>() )
	    continue;
    
	// get the pointer to a (possibly derived from Entity) class T
	boost::shared_ptr<T> entPtr = boost::dynamic_pointer_cast<T>( iter->second );
	
	// if success, then run method (otherwise the object was not derived from T, and will not be able to respond)
	if( entPtr )
	{
	    ((*entPtr).*method)(arg1);
	}
    }
}



} // namespace
#endif // NISAC_SIMX_ENTITYMANAGER
