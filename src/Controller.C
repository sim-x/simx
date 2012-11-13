//--------------------------------------------------------------------------
// File:    Controller.h
// Module:  simx
// Author:  Lukas Kroc
// Created: Oct 27 2008
//
// Description:
//	Controller is an object with EntityID (! Control::getRank()). 
//	There is exactly one per unix process.
//	Controller also has an associated CONTROLLER_INPUT file which gets
//	polled for new record every time Event::execute is called. The content
//	is read and interpreted in poll() method
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------


#include "simx/Controller.h"
#include "simx/control.h"
#include "simx/config.h"
#include "simx/InfoManager.h"
#include "simx/EntityManager.h"
#include "simx/InfoData.h"

#include "Config/Configuration.h"

#include "boost/lexical_cast.hpp"

#include <sstream>
#include <errno.h>
#include <string.h>
#include <fcntl.h>


using namespace std;

namespace simx {


//====================================================================
// InfoControllerAddEntity

void InfoControllerModifyEntity::pack(PackedData& dp) const
{
    dp.add( fAction );
    dp.add( fEntityId );
    dp.add( fEntityType );
    dp.add( fProfileId );
    dp.add( fUserData );
}


void InfoControllerModifyEntity::unpack(PackedData& dp)
{
    dp.get( fAction );
    dp.get( fEntityId );
    dp.get( fEntityType );
    dp.get( fProfileId );
    dp.get( fUserData );
}



//====================================================================
// Controller

Controller::Controller( LP& lp, const EntityInput& input )
    : 	Entity( EntityID('!', Control::getRank() ), lp, input),
	fInputFileName(""),
	fInputFd( -1 ),
	/// [11/19/2008 by Guanhua Yan]
	fOutputFileName(""),
	fOutputFd( -1 ),
	fPaused( false ),
	fRunTill()
{
    Logger::debug2() << "Controller" << getId() << " is being created on rank=" << Control::getRank() << endl;
    
    // create a file for control input if needs be:
    string inputFileName("");
    Config::gConfig.GetConfigurationValue( ky_CONTROLLER_INPUT, inputFileName );
    if( inputFileName != "" )
    {

//	stringstream ss;
//	ss << inputFileName << ":" << Control::getRank() << ":" << Control::getProcessorName();
//	fInputFileName = ss.str();
	fInputFileName = inputFileName;
    
	Logger::debug3() << "Controller opens (creates) input file " << fInputFileName << endl;

// This would create a named PIPE, but that cannot be used on NFS
//	int ret = mkfifo( fInputFileName.c_str(), 0666 );
//	if( ret < 0 )
//	{
//	    Logger::error() << "Controller::Controller: cannot create FIFO '" << fInputFileName << "' " << strerror( errno ) << endl;
//	    errno = 0;
//	}
//	fInputFd = open( fInputFileName.c_str(), O_RDONLY | O_NONBLOCK );

	fInputFd = open( fInputFileName.c_str(), O_RDONLY | O_NONBLOCK | O_CREAT, 0666 );
	if( fInputFd < 0 )
	{
	    Logger::error() << "Controller::Controller: cannot open input '" << fInputFileName << "' " << strerror( errno ) << endl;
	    errno = 0;
	}
    } else
    {
	Logger::debug3() << "Controller does not create any input file" << endl;
    }

    /// [11/19/2008 by Guanhua Yan]
    string outputFileName("");
    Config::gConfig.GetConfigurationValue( ky_CONTROLLER_OUTPUT, outputFileName );
    if( outputFileName != "" ) {
      fOutputFileName = outputFileName;
      Logger::debug3() << "Controller opens (creates) output file " << fOutputFileName << endl;
      
      fOutputFd = open( fOutputFileName.c_str(), O_WRONLY | O_NONBLOCK | O_CREAT | O_TRUNC, 0666 );
      if( fOutputFd < 0 ) {
	Logger::error() << "Controller::Controller: cannot open output '" << fOutputFileName << "' " << strerror( errno ) << endl;
	errno = 0;
      }
    } else {
      Logger::debug3() << "Controller does not create any output file" << endl;
    }

// LK: why is this needed?
   //populate neighbor list
   for ( int i = 0; i < Control::getNumMachines(); ++i )
     {
	if ( i != Control::getRank() )
	  {
	    fNeighbors.push_back( EntityID('!',i));
	  }
     }

    createServices( *this, input.fServices );
}

Controller::~Controller()
{
    Logger::debug3() << "Controller" << getId() << " in destructor" << endl;

    // close the file descriptor if opened
    if( fInputFd >= 0 )
    {
	close( fInputFd );
    }

    if( fOutputFd >= 0 )
    {
	close( fOutputFd );
    }


//    // delete the control input LK: might want to keep it....
//    if( fInputFileName != "" )
//    {
//	unlink( fInputFileName.c_str() );
//    } 
}


void Controller::processIncomingInfo( boost::shared_ptr<Info> info, const ServiceAddress& addr) const
{
    // see if this is the InfoWakeupInfoManager or InfoControllerModifyEntity info:
    // ....they would only be sent to 'no address'....
    SMART_VERIFY( info );
    if( addr == ServiceAddress() )
    {
	// ... and it would be of the right kind
	boost::shared_ptr<InfoWakeupInfoManager> info2 = boost::dynamic_pointer_cast<InfoWakeupInfoManager>( info );
	if( info2 )
	{
	    // wakeup the InfoManager
	    theInfoManager().readDataFile( info2->fFileId );
	    return;
	}
	    
	boost::shared_ptr<InfoControllerModifyEntity> info3 = boost::dynamic_pointer_cast<InfoControllerModifyEntity>( info );
	if( info3 )
	{
	    // create the entity, but do not tell anyone else
	    theEntityManager().createEntity( info3->fEntityId, info3->fEntityType, info3->fProfileId, info3->fUserData, false );
	    return;
	}
	    
	return;
    }
    
    // if not processed yet, use the default functions:
    // need to give up pointer before passing it on, else Entity will croak.
    Entity::processIncomingInfo( giveup_smart_ptr(info), addr);
}


void Controller::pollInput(void)
{
    Logger::debug3() << "Controller polls for input and checks if we need to pause" << endl;
    
    // see if you need to pause according to fRunTill:
    if( fRunTill != Time() && fRunTill <= getNow() )
    {
	// need to pause!
	pauseSimulation();
	return;
    }
    
    if( fInputFd < 0 )
	return;	///< no input file opened
    
    static char c[2] = {0,0};
    ssize_t n = read( fInputFd, c, 1 );
    if( n <= 0 )
	return;	///< no input waiting


    /// something is waiting to be read, 
    /// 1) read until '\n' found
    /// TODO: this can be more efficient with number of calls to read....
    
	string com("");	//< the command
	string arg("");	//< its argument
	string *s = &com;	//< start by building command
	while( n > 0 || errno == EAGAIN )
	{
	    if( n > 0 )
	    {

		if( c[0] == '\n' )
		    break;	///< done with the line

		if( c[0] == ' ' || c[0] == '\t' )
		{
		    s = &arg;	//< next word, we need to read (continue reading) argument
		} 

		// at any rate add the character to the string (command or argument)
	    	*s += c;

	    }
	    errno = 0;	///< clear the errno
	    n = read( fInputFd, c, 1 );
	}

    /// 2) interpret the command
	if( c[0] == '\n' )
	{
	    Logger::debug3() << "Controller::pollInput: read input '" << com << "' with argument '" << arg << "'" << endl;
	    interpretCommand( com, arg );
	} else
	{
	    Logger::warn() << "Controller::pollInput: no '\\n' in input '" << s << "'" << endl;
	}

    // 3) make sure there is nothing more waiting:
	pollInput();

}

void Controller::print(ostream& os) const
{
    os << "Controller(";
    Entity::print(os);
    os << "," << fInputFd << ")";
}


void Controller::pauseSimulation(void)
{
    Logger::debug3() << "Controller::pauseSimulation: wating for 'go' and interpreting other commands...." << endl;
    /// go into an active loop waiting for 'start' to appear on the input:

    /// [11/19/2008 by Guanhua Yan] write the status of the simulation
    if( fOutputFd >= 0) {
      // Aha, we need to write the pausing state to the file. that's pausing.
      write(fOutputFd, "pausing\n", 8);
    }

    SMART_ASSERT( !fPaused );
    fRunTill = Time();
    fPaused = true;    
    while( true )
    {
	pollInput();
	if( !fPaused )
	    break;
	sleep( 1 );
    }
    
    /// [11/19/2008 by Guanhua Yan] write the status of the simulation
    if( fOutputFd >= 0) {
      // Aha, we need to write the running state to the file. that's '0'.
      write(fOutputFd, "running\n", 8);
    }

    Logger::debug3() << "Controller::pauseSimulation: going!" << endl;
}

void Controller::abortSimulation(void)
{
    Logger::debug3() << "Controller::abortSimulation" << endl;

    // do whatever here.... exit() will call the destructors
    exit(1);
}

//======================================================================================

// recognized commands:
// 'pause [time]' - pauses, waiting for 'go' immediately, or just before first event after or at time is processed
// 'go [time]' - restarts this LP, forever or till first event after or at time is processed
// 'info arg..' - creates an Info from 'arg' and call processIncomingInfo if recipient in this process (and time<=current time, otherwise it schedules it)
// 'abort' - aborts the simulation
void Controller::interpretCommand( const string& command, const string& arg )
{
    // stringstream to read argument from
    stringstream ssarg( arg );
    ws( ssarg );

    /// this is where different command are interpreted:
    if( command == "pause" )
    {
	if( fPaused )
	{
	    Logger::warn() << "Controller::interpretCommand simulation already paused" << endl;
	} else
	{
	    if( ssarg.eof() )
	    {
		Logger::debug3() << "Controller::interpretCommand pausing the simulation NOW" << endl;
		pauseSimulation();
	    } else
	    {
		ssarg >> fRunTill;
		Logger::debug3() << "Controller::interpretCommand pausing the simulation at time " << fRunTill << endl;
	    }
	}
    } else
    if( command == "go" )
    {
	Logger::debug3() << "Controller::interpretCommand restarting the simulation" << endl;
	fPaused = false;
	if( !ssarg.eof() )
	{
	    ssarg >> fRunTill;
	    Logger::debug3() << "Controller::interpretCommand .... running till time " << fRunTill << endl;
	}
    } else
    if( command == "info" )
    {
	Logger::debug3() << "Controller::interpretCommand need to process an Info" << endl;
	// TODO: create an info from 'arg' and call the appropriate 'processIncomingInfo' if the recipient is on this LP
	// (do NOT send it, this way it gets procesed immediately)
	
	// read the info data:
	InfoData data;
	data.read( ssarg );
	Logger::debug3() << "Controller::interpretCommand .... processing data " << data << endl;

	// see if it should be created on this lp:
	LPID lpId = theEntityManager().findEntityLpId( data.getEntityId() );
	if( lpId != Control::getRank() )
	{
	    Logger::debug3() << "Controller::interpretCommand .... data for LP " << lpId <<  " which is not us, dropping." << endl;
	    return;
	}

	// create the info:
	boost::shared_ptr<Input> input( theInfoManager().getInputHandler().createInput( data.getClassType(), data.getProfileId(), data.getData() ) );
	boost::shared_ptr<Info> info = boost::dynamic_pointer_cast<Info>( giveup_smart_ptr(input) );
	SMART_ASSERT( info );	/// the object MUST actually be a descendant of Info

	// execute it immediatelly, or send it (depending on time):
	if( data.getTime() < getNow() )
	{
	    // execute it right now:
	    boost::shared_ptr<Entity> entPtr;
	    bool ret = theEntityManager().getEntity( data.getEntityId(), entPtr );
	    if( !ret )
	    {
		Logger::warn() << "Controller::interpretCommand .... cannot find entity " << data.getEntityId() << "on LP " << lpId << endl;
		return;
	    }
	    SMART_ASSERT( entPtr );
	    entPtr->processIncomingInfo( giveup_smart_ptr(info), data.getServiceAddress() );
	} else
	{
	    // send it:
	    processOutgoingInfo( giveup_smart_ptr(info), data.getTime(), data.getEntityId(), data.getServiceAddress() );
	}

//	    // set EventInfo parameters:
//	      Logger::warn() << "InfoManager: Sending event at time " << now
//			     << " which should have been sent at " << data.getTime() 
//			     << " : Check if input is time sorted " << endl;
//	    event.setDelay( max (LOCAL_MINDELAY, data.getTime() - now ));
//	    // send it off directly to the right LP
//	    LP* lpPointer = lpIter->second;
//	    SMART_ASSERT( lpPointer );
//	    lpPointer->sendEventInfo(event);


	
    } else
    if( command == "abort" )
    {
	Logger::debug3() << "Controller::interpretCommand aborting the simulation" << endl;
	abortSimulation();
    } else
    {
	Logger::warn() << "Controller::interpretCommand: cannot interpret command '" << command << "' with argument '" << arg << "'" << endl;
    }
	    

}


} // namespace


