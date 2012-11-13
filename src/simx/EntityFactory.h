//--------------------------------------------------------------------------
// File:    EntityFactory.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 9 2005
//
// Description: 
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_ENTITYFACTORY
#define NISAC_SIMX_ENTITYFACTORY

#include "simx/Entity.h"
#include "simx/ExceptionInvalidEntityInput.h"

#include "Config/Configuration.h"
#include "Common/Assert.h"
#include <boost/python.hpp>

namespace simx {

class LP;

/// base entity-placement and creation object
class BaseEntityCreator
{
    public:
  virtual ~BaseEntityCreator() {}
	virtual void preCreate(const EntityID&, const Input& input) const = 0;	///< method that gets called for EVERY input entity on EVERY node
  virtual boost::shared_ptr<Entity> create(const EntityID&, LP&, const Input& input, const Entity::ClassType& type = "") const = 0;	///< method that actually creates it
    private:
};

// generic entity-creation function
template<class EntityClass, class InputClass>
class DerivedEntityCreator : public BaseEntityCreator
{
    private:
	typedef void (*PreCreatorPtr)(const EntityID&, const InputClass&);
	const PreCreatorPtr fPreCreator;	///< pointer to the function that will do stuff with input entities on all nodes
	
    public:
	explicit DerivedEntityCreator(PreCreatorPtr preCreatePtr)
	    : fPreCreator( preCreatePtr ) { }	///< preCreatePtr MAY be NULL
	
	virtual void preCreate(const EntityID& id, const Input& input) const
	{
	    /// if no pre-creator registered, there's nothing to do
	    if( !fPreCreator )
	    {
		return;
	    }
	
	    SMART_ASSERT( fPreCreator );
	    const InputClass* in = dynamic_cast<const InputClass*>(&input);
	    if( !in )
	    {
		Logger::error() << "EntityCreator: invalid InputClass passed to placement of "
		    << typeid(EntityClass).name() << ", expected " << typeid(InputClass).name()
		    << ", received " << typeid(input).name() << std::endl;
		throw ExceptionInvalidEntityInput();
	    }
	    (*fPreCreator)(id, *in);
	}

  virtual boost::shared_ptr<Entity> create(const EntityID& id, LP& lp, const Input& input, 
					   const Entity::ClassType& type) const
	{
	    const InputClass* in = dynamic_cast<const InputClass*>(&input);
	    if( !in )
	    {
		Logger::error() << "EntityCreator: invalid InputClass passed to creation of "
		    << typeid(EntityClass).name() << ", expected " << typeid(InputClass).name()
		    << ", received " << typeid(input).name() << std::endl;
		throw ExceptionInvalidEntityInput();
	    }

	    return boost::shared_ptr<Entity>( new EntityClass( id, lp, *in ) );
	}


};


class PyEntityCreator : public BaseEntityCreator {

  private:
    //TODO: PYTHON: should we store pointers to objects here? How to make it safe, then?
  typedef std::map<Entity::ClassType, boost::python::object> PyEntityClassMap;
    PyEntityClassMap fPyObjMap;

  public:
    explicit PyEntityCreator()
       { }
    virtual void preCreate( const EntityID& id, const Input& input ) const
    {
      // nothing here for now
    }
  
  void registerObject( const Entity::ClassType& name, 
		       const boost::python::object& py_entity_class );

  
  virtual boost::shared_ptr<Entity> create( const EntityID& id, LP& lp, 
					    const Input& input, 
					    const Entity::ClassType& type ) const;

};

} // namespace

#endif 

