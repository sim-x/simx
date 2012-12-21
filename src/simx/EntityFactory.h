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
// File:    EntityFactory.h
// Module:  simx
// Author:  Lukas Kroc, Sunil Thulasidasan
// Created: Feb 9 2005
//
// Description: 
//
// @@
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


  // for creating objects from their class-name strings
  virtual boost::shared_ptr<Entity> create( const EntityID& id, LP& lp, 
					    const Input& input, 
					    const Entity::ClassType& type ) const;

  // for creating objects directly from python class objects
  virtual boost::shared_ptr<Entity> create( const EntityID& id, LP& lp,
					    const Input& input,
					    const boost::python::object& py_class ) const;


};

} // namespace

#endif 

