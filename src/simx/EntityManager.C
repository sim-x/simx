//--------------------------------------------------------------------------
// File:    EntityManager.C
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 9 2005
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/EntityManager.h"
#include "simx/EntityData.h"
#include "simx/Entity.h"
#include "simx/Controller.h"
#include "simx/writers.h"
#include "simx/logger.h"
#include "simx/control.h"
#include "simx/config.h"
#include "simx/control.h"
#include "simx/InfoManager.h"
#include "simx/LP.h"

#include "File/FileReader.h"

#include <sstream>

#include "simx/Python/PyEntityData.h"
#include "simx/Python/PyEntityInput.h"


using namespace std;
using namespace boost;

namespace simx {

EntityManager::EntityManager()
    :	fEntityPtrMap(),
	fInputHandler("EntityProfile"),
	fEntityCreatorMap(),
	fPyEntityCreator( new PyEntityCreator() ),
	fMyLpPtr( 0 ),
	fControllerPtr()
{
//CANNOT    Logger::debug1() << "EntityManager: in constructor" << endl;

// TODO (Python) : Is there a better place to register PyInput?
  fInputHandler.registerInput<Python::PyEntityInput>("PyEntity");

}

EntityManager::~EntityManager()
{
//CANNOT    Logger::debug1() << "EntityManager: in destructor" << endl;
}


//void EntityManager::getEntityIds(std::list<EntityID>& ids) const {
//   for( EntityPtrMap::const_iterator itr = fEntityPtrMap.begin();
//        itr != fEntityPtrMap.end();  ++itr ) {
//      ids.push_back(itr->first);         
//   } 
//}


  //TODO (Python) just use the regular registerEntity function, by
  // overloading
  void EntityManager::registerPyEntity(const Entity::ClassType& name, 
				       const python::object& py_entity_class ) {


    if( name==Entity::ClassType() )
      {
	std::cerr << "ERROR: EntityManager: entity name '" << name
		  << "' is invalid" << std::endl;
	throw Common::Exception("Error entity registration");
      }

    if( !fEntityCreatorMap.insert( std::make_pair( 
	    name,  
	    fPyEntityCreator)).second)
      { 
	std::cerr << "ERROR: EntityManager: Python entity '" << name 
		  << "' is already registered" << std::endl; 
	throw Common::Exception("Error in Python entity registration"); 
      } 
    
    fPyEntityCreator->registerObject( name, py_entity_class );
    
  }


bool EntityManager::createEntity(const EntityID& id, const Entity::ClassType& type, const ProfileID profileId, const std::string data, bool notifyOtherMachines)
{
    Logger::debug3() << "EntityManager::createEntity " << id << " of type " << type << endl;

    // 1) first inform everybody that we're being created
    if( notifyOtherMachines )
    {
	Logger::debug3() << "EntityManager::createEntityFromInput: informing other machines" << endl;
        SMART_VERIFY( fMyLpPtr ).msg("Cannot create new Entities on machines with no LPs");
	
        boost::shared_ptr<InfoControllerModifyEntity> info;
        theInfoManager().createInfo( info );
	info->fAction = InfoControllerModifyEntity::kADD;
        info->fEntityId = id;
        info->fEntityType = type;
	info->fProfileId = profileId;
	info->fUserData = data;

	for( int m=0; m<Control::getNumMachines(); ++m )
        {
    	    if( m == Control::getRank() )
    		continue;
	
	    // send it around to 'no service address' on the Control Entities
	    getController().processOutgoingInfo(info, LP::MINDELAY, EntityID('!', m), ServiceAddress() );
	}
    }
    
    
    // 2) second create:
    stringstream ss(data);
    return createEntityPrivate( id, type, profileId, ss );
}



void EntityManager::createController() {

  const Control::LpPtrMap& lps = Control::getLpPtrMap();
  Logger::debug3() << "EntityManager: 	lps= " << lps << endl;
  
  if( !lps.empty() )
    {
        Logger::debug3() << "EntityManager: creating controller" << endl;
	string controllerServicesStr("");
	Config::gConfig.GetConfigurationValue( ky_CONTROLLER_SERVICES, controllerServicesStr );
	EntityInput controllerInput;
	EntityInput::loadServices( controllerInput, controllerServicesStr );
	fMyLpPtr = (lps.begin())->second;
	SMART_ASSERT( fMyLpPtr );
	fControllerPtr = boost::shared_ptr<Controller>( new Controller( *fMyLpPtr, controllerInput) );	///< create is on _some_ LP in the set
	SMART_ASSERT( fControllerPtr );

	/// remember the pointer just like any other entity
	fEntityPtrMap[ EntityID('!', Control::getRank()) ] = fControllerPtr;
    } else
    {
	Logger::warn() << "EntityManager: no LPs here, so no Controller created" << endl;
    }
  
}

void EntityManager::createEntities(const string& dataFiles)
{
    Logger::debug2() << "EntityManager: in readEntityData, dataFiles= '" << dataFiles << "'" << endl;
   
    /// First of all create the Controller:
    createController();

    stringstream sstr;
    sstr << dataFiles;
    string fileName;
    // read in each file, in order of appearance
    const long long PRINT_INTERVAL = 10000;
    while( sstr >> fileName )
    {
	Logger::info() << "EntityManager: processing file " << fileName << endl;
    
	long long num_entities = 0;
	EntityData::Reader reader(fileName);
	while( reader.MoreData() )
	{

    	    EntityData data = reader.ReadData();
    	    Logger::debug3() << "EntityManager: data=" << data << endl;
	    EntityID 		id = data.getEntityId();	// ID of entity being created
	    Entity::ClassType	type = data.getClassType();	// type of entity being created

	    // create the entity, if is wants to be on this LP (do not inform other machines as they also read the input)
	    createEntityPrivate( id, type, data.getProfileId(), data.getData() );

	    // logging:
	    num_entities += 1;
	    if( num_entities % PRINT_INTERVAL == 0) 
	    {
		Logger::info() << "EntityManager: parsed " << num_entities
            			<< " entities" << endl;    
	    }
	} // while(MoreData)
	
    } // filenames
   Logger::info() << "EntityManager: done creating entities" << endl;
}




LPID EntityManager::findEntityLpId( const EntityID& entId ) const
{
    LPID lpId = LPID(); ///< where the entId will live

    /// call functions that are registered as EntityPlacingFunctions one by one
    /// in the order they were registered
    bool found = false;
    for(EntityPlacingFunctionContainer::const_iterator iter = fEntityPlacingFunctionContainer.begin();
	iter != fEntityPlacingFunctionContainer.end();
	++iter)
    {
	EntityPlacingFunction func = *iter;
	SMART_ASSERT( func ).msg("Invalid EntityPlacingFunction");

	/// stop when the first one returns "true"
	if( (*iter)(entId, lpId) )
	{
	    found = true;
	    break;
	}
    }
    
    /// if none returns true, use the default placing function
    if( !found )
    {
	lpId = defaultEntityPlacingFunction( entId );
    }

    SMART_VERIFY( 0 <= lpId && lpId < Control::getNumLPs() )( entId )( lpId )( Control::getNumLPs() )
	.msg("LPID out of range");

    return lpId;
}

void EntityManager::registerPlacingFunction( bool (*func)(const EntityID&, LPID&) )
{
    SMART_VERIFY( func ).msg("Trying to register an invalid entity PlacingFunction");

    /// see if we already have the pointer registered, issue a warning if so,
    /// and don't register it
    for(EntityPlacingFunctionContainer::const_iterator iter = fEntityPlacingFunctionContainer.begin();
	iter != fEntityPlacingFunctionContainer.end();
	++iter)
    {
	if( *iter == func )
	{
	    Logger::warn() << "EntityManager: trying to register the same entity PlacingFunction more the once" << endl;
	    return;
	}
    }
    
    /// otherwise (if we don't yet have the pointer), store it AT THE END, so
    /// that it gets called last when needed
    fEntityPlacingFunctionContainer.push_back( func );

}



Controller& EntityManager::getController(void) const
{
    SMART_VERIFY( fControllerPtr ).msg("Controller does not exist");
    return *fControllerPtr;
}


LPID EntityManager::defaultEntityPlacingFunction( const EntityID& entId ) const
{
    /// take se second part of EntityID (it's numeric ID) and do modulo number of LPs
    /// NOTE: this MUST work for Controllers! ( EntityId('!', Control::getRank() ) )
    ///    (modulo getNumLPs() does work because numLPs >= numMachines )
    LPID lpId = entId.get<1>() % Control::getNumLPs();

    return lpId;
}


//-------------------------------------------------------------------------------------------------

  // bool EntityManager::createPyEntity( const EntityID& id, const Entity::ClassType& type,
  // 				      const ProfileID profileId, const python::object& data )

  // {
  //   boost::shared_ptr<Input> input( fInputHandler.createInput( "PyEntity", profileId, data ) );
  //   SMART_ASSERT( input );
  //   return createEntityonLP( id, type, profileId, input );
  // }

  


//-------------------------------------------------------------------------------------------------
  bool EntityManager::createPyEntity( const Python::PyEntityData& ent_data )

  {
    boost::shared_ptr<Input> input( fInputHandler.createInput( "PyEntity", ent_data.fProfileId, ent_data.fProfile, ent_data.fData ) );
    SMART_ASSERT( input );
    // return createEntityonLP( id, type, profileId, input );
    return createEntityonLP( ent_data.fEntityId, ent_data.fEntityType, 
			     ent_data.fProfileId, input );
  }

//-------------------------------------------------------------------------------------------------

bool EntityManager::createEntityPrivate(const EntityID& id, const Entity::ClassType& type, 
					const ProfileID profileId, Input::DataSource& data)
{
    Logger::debug3() << "EntityManager::createEntityPrivate " << id << " of type " << type << endl;

    // get input object and fill it with data
    boost::shared_ptr<Input> input( fInputHandler.createInput( type, profileId, data ) );
    SMART_ASSERT( input );

    return createEntityonLP( id, type, profileId, input );
}

//-------------------------------------------------------------------------------------------------

  bool EntityManager::createEntityonLP(const EntityID& id, const Entity::ClassType& type, 
				       const ProfileID profileId, const shared_ptr<Input>& input)
  
  {

    // this variable should be static because it's reused many times
    static const Control::LpPtrMap& lps = Control::getLpPtrMap();

	    

    // obtain the creation object (that will create the Entity)
    EntityCreatorMap::const_iterator pmIter = fEntityCreatorMap.find( type );
    if( pmIter == fEntityCreatorMap.end() )
    {
        Logger::error() << "EntityManager::createEntityonLP: entity " << id
	    << " is of an unknown type " << type << endl;
	return false;
    }
    SMART_ASSERT( pmIter->second );
    const BaseEntityCreator& creator = *pmIter->second;


    // run the pre-creating function, if any 
    // (may not have been registered, in which case the call has no effect)
    // - must be done before findEntityLpId, because the pre-creator may decide its output
    creator.preCreate( id, *input );

    // find out which LP this entity will be created at:
    LPID lpId = findEntityLpId( id );
	
    // now create it if it is supposed to be here
    bool created = false;
    Control::LpPtrMap::const_iterator lpIter = lps.find(lpId);
    if( lpIter != lps.end() )
    {
        Logger::debug3() << "EntityManager: creating Entity " << id << endl;

	created = true;

        // the entity will reside on this LP
        SMART_ASSERT( lpIter->second );
	LP& lp = *(lpIter->second);

	// actually create the entity:
	boost::shared_ptr<Entity> entity = creator.create( id, lp, *input, type);

	// now remember where this entity is:
	if( !fEntityPtrMap.insert( make_pair(
		id,
		entity
	    ) ).second )
	{
	    Logger::warn() << "EntityManager: redefining entity " << id << endl;
	}

    } else
    {
    	Logger::debug3() << "EntityManager: NOT creating Entity " << id << endl;

    }
    
    return created;

}




} // namespace
