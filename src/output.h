//--------------------------------------------------------------------------
// File:    output.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Feb 13 2005
//
// Description: Manages output functionality
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_OUTPUT
#define NISAC_SIMX_OUTPUT

#include "simx/type.h"
#include "simx/control.h"
#include <fstream>

namespace simx {

class Entity;
class Service;

/// Manages output functionality
namespace Output
{
    /// structure used to output anything
    struct OutputStream 
    {
	~OutputStream();
	std::ofstream fStream; 
    };
	
    /// initializaton function
    void init(const std::string& fileName);
	
    /// returns an OutputStream object that can be used for outputtting things
    OutputStream& output(const Entity&, const OutputRecordType, Control::eSimPhase = Control::kPhaseRun );
    OutputStream& output(const Service&, const OutputRecordType, Control::eSimPhase = Control::kPhaseRun );
		
    //=====================================================================

    /// generic operator<< function for output purposes
    template<class Type>
    Output::OutputStream& operator<<(Output::OutputStream& os, const Type& obj)
    {
	os.fStream << "\t";
	os.fStream << obj;
	return os;
    }
    
} // Output namespace


} // simx namespace

#endif

