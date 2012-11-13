//--------------------------------------------------------------------------
// File:    Controller.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Oct 27 2008
//
// Description:
//	Controller is an object with EntityID (! Control::getRank()). 
//	There is exactly one per unix process. They handle waking up InfoManager to read more input.
//	Controller also has an associated CONTROLLER_INPUT file which gets
//	polled for new record every time EventInfo::execute is called. The content
//	is read and interpreted in poll() method
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#ifndef NISAC_SIMX_CONTROLLER
#define NISAC_SIMX_CONTROLLER

#include "simx/Entity.h"
#include "simx/Info.h"
#include "boost/shared_ptr.hpp"

#include <map>
#include <list>
#include <utility>

namespace simx {


class LP;
class Service;

/// An ControlInfo that informs us that someone else has created an entity
struct InfoControllerModifyEntity : public Info
{
    virtual void pack(PackedData&) const;
    virtual void unpack(PackedData&);

    enum {kADD, kREMOVE};
    int					fAction;	///< from enum above
    EntityID				fEntityId;
    Entity::ClassType			fEntityType;
    ProfileID				fProfileId;
    std::string				fUserData;
};


class Controller : public Entity
{
    public:
    
	/// creates an entity on lp and 
	Controller(LP& lp, const EntityInput& input);
	virtual ~Controller();
	
	/// overrides the default, and handles InfoControllerModifyEntity 
	/// and InfoWakeupInfoManager by itself
        virtual void processIncomingInfo( boost::shared_ptr<Info>, const ServiceAddress&) const;


	/// looks for new input on CONTROLLER_INPUT, and if there is something, executes it
	/// called before EACH event execution
	virtual void pollInput(void);

	/// printing function
	virtual void print(std::ostream&) const;


	///COMMANDS:
	/// pauses the simulation on this LP, by waiting in active loop until 'go' command is received on Controller Input
	void pauseSimulation(void);

	/// aborts the simulation
	void abortSimulation(void);

	const std::vector<EntityID>& getNeighbors() const
	{ return fNeighbors; }

    private:
	
	/// runs the proper method given command string read from Controller Input (with possible argument)
	void interpretCommand( const std::string& commmand, const std::string& arg );
    
	std::string	fInputFileName;	///< the file name for input
	int		fInputFd;	///< (readonly, non-blocking) file destrictor of this Controllers Input (named InputFileName)

	/// [11/19/2008 by Guanhua Yan]
	std::string	fOutputFileName;	///< the file name for output
	int		fOutputFd;	///< (write only, non-blocking) file destrictor of this Controllers Output (named OutputFileName)

        bool	fPaused;    ///< ==true iff we're in active loop waiting to restart	
	Time	fRunTill;	//< time when we want to pause (from 'pause' or 'go' commands)

	std::vector<EntityID> fNeighbors; //controller IDs on other LPs. 

};


} // namespace

#endif 


