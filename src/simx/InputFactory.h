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
// File:    InputFactory.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 6 2005
//
// Description: 
//
// @@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_INPUTFACTORY
#define NISAC_SIMX_INPUTFACTORY

#include "simx/logger.h"

#include "loki/Factory.h"
#include "Common/Assert.h"

#include "boost/lexical_cast.hpp"
#include "boost/shared_ptr.hpp"

namespace simx {

class Input;
class InputData;
class ProfileInput;

  // Handler for unknown Index identifier
  template <typename IdentifierType, class AbstractProduct>
  struct UnknownIndexInput
  {
    struct Exception : public std::exception
    {
    public:
      Exception(IdentifierType d)
        {
	    fMsg = std::string("Unknown input: <") + boost::lexical_cast<std::string>(d) + ">";
        }
      ~Exception() throw() {}
      const char* what() const throw() {return fMsg.c_str();}
    private:
      std::string fMsg;
    };
        
    static AbstractProduct* OnUnknownType(IdentifierType d)
      {
        throw Exception(d);
      }
  };


/// generic input-creation function
/// possibly makes a copy of orig
template<class Product>
Input* inputCreator(boost::shared_ptr<Input> orig)
{
    if( orig )
    {
	boost::shared_ptr<Product> productOrig( boost::dynamic_pointer_cast<Product>(orig) );
	if( !productOrig )
	{
	    Logger::error() << "InputHandler: cannot make a copy for Input of type " 
		<< typeid(Product).name() << " from type " << typeid(*orig).name() 
		<< std::endl;
	    return new Product();
	} else
	{
	    return new Product(*productOrig);
	}
    } else
    {
	return new Product();
    }
}


/// A InputFactory is a Loki Factory - owned by InputHandler object
///
/// This factory stores a map of input identifiers to input creation
/// functions.
template<class ObjectIdent> 
class InputFactory 
    : public Loki::Factory<Input,
                       ObjectIdent,
		       // this is the creator's signature:
                       Input* (*)(boost::shared_ptr<Input>),
                       UnknownIndexInput>
{};


} // namespace
#endif // NISAC_SIMX_INPUTFACTORY
