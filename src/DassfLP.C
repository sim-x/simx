//--------------------------------------------------------------------------
// File:    DassfLP.C
// Module:  simx
// Author:  K. Bisset
// Created: June 25 2004
//
// @@COPYRIGHT@@
//
//--------------------------------------------------------------------------

#include "simx/DassfLP.h"
#include "simx/LP.h"
#include "simx/EventInfo.h"
#include "simx/DassfEvent.h"
#include "simx/DassfEventInfo.h"
#include "simx/control.h"
#include "simx/Exception.h"
#include "simx/PackedData.h"
#include "simx/constants.h"
#include "simx/logger.h"

#ifdef SIMX_USE_PRIME


#include "Common/backtrace.h"

#include "boost/lexical_cast.hpp"

#include "simx/Messenger.h"

using namespace std;
//using namespace simx::Control;



namespace simx {


DassfLP::DassfLP(const LPID id, LP& lp)
  : fLP(lp)
{

  
//   Logger::debug1()
//     << "LP::LP() " << fId << " " << Control::getRank() << "/" << Control::getNumMachines() << endl;

  const string local = boost::lexical_cast<string>( id );
  for (int i=0; i < Control::getNumLPs(); i++)
  {
    const string remote = boost::lexical_cast<string>(i);
    const string inname = string("I")+local+string(".")+remote;
    const string outname = string("O")+local+string(".")+remote;
//     Logger::debug1() << "Create channels " << inname << " " << outname << endl;

    prime::ssf::inChannel* in = new prime::ssf::inChannel(const_cast<char*>(inname.c_str()), this);
    fIn.push_back(in);

    prime::ssf::outChannel* out = new prime::ssf::outChannel(this, prime::ssf::ltime_t(0));
    SSF_PUBLISH_DML_OUTCHANNEL(out, const_cast<char*>(outname.c_str()));
    fOut.push_back(out);
  }
  // arrays must end with a null pointer
  fIn.push_back(0);
  fOut.push_back(0);

  prime::ssf::Process* p = new prime::ssf::Process(this, (void (prime::ssf::Entity::*)(prime::ssf::Process*))
                                   &DassfLP::process, true);
  p->waitsOn(&fIn[0]);
}

DassfLP::~DassfLP()
{
}

void DassfLP::init()
{
//  Logger::debug1() << "LP::init() " << fId << " " << Control::getRank() << "/" << Control::getNumMachines() << endl;
  LPID local_id = fLP.getId();
  const string local = boost::lexical_cast<string>( fLP.getId() );
  for (int i=0; i < Control::getNumLPs(); ++i)
  {
    const string remote = boost::lexical_cast<string>(i);
    const string inname = string("I")+remote+string(".")+local;
    const string outname = string("O")+remote+string(".")+local;
    if (i == local_id) {
      fOut[i]->mapto(const_cast<char*>(inname.c_str()), LOCAL_MINDELAY );
    } else {
      fOut[i]->mapto(const_cast<char*>(inname.c_str()), LP::MINDELAY );
    }
    //    Logger::debug1() << "Map " << outname << " to " << inname << " " << time << endl;
  }
  
}

void DassfLP::wrapup()
{
//  Logger::debug1() << "LP::wrapup() " << fId << " " << Control::getRank() << "/" << Control::getNumMachines() << endl;
}

void DassfLP::sendDassfEvent(const LPID destLP, prime::ssf::Event* e, const Time delay)
{
  Logger::debug3() << "DassfLP " << fLP.getId() << ": sending event with delay " 
		   << delay << " to LP " << destLP << ", at time " << now() << endl;
  
   Time realDelay;
    SMART_ASSERT( e );
    Time mindelay;
    if (destLP ==  fLP.getId()) {
      mindelay = LOCAL_MINDELAY;
    } else {
      mindelay = LP::MINDELAY;
    }
    if( delay < mindelay )
    {
      realDelay = Time(0);
	Logger::error() << "DassfLP " << fLP.getId() << ": too late sending event with delay "
	    << delay << " at time " << now() << ", will be delivered later" << std::endl;
    }
    else {
      realDelay = delay - mindelay;
    }
    Logger::debug3() << "   realDelay: " << realDelay << std::endl;
    SMART_ASSERT( fOut.size() > static_cast<unsigned int>(destLP) )( fOut.size() )( destLP );
    fOut[destLP]->write( e, realDelay);
}

//! SSF PROCEDURE SIMPLE
void DassfLP::process(prime::ssf::Process* p)
{
  Logger::setTime( now() );
  
  Logger::debug2() << "DassfLP " << fLP.getId() << ": in process() " << Control::getRank() << "/" << Control::getNumMachines()
    << " @" << now() << endl;
  prime::ssf::inChannel** in = p->activeChannels();
  if (in != 0)
  {
    for (int j=0; in[j] != 0; ++j)
    {
      prime::ssf::Event** event = in[j]->activeEvents();
      if (event != 0)
      {
        for (int i=0; event[i] != 0; ++i)
        {
	    DassfEvent* e = dynamic_cast<DassfEvent*>(event[i]);
	    if( !e )
	    {
        	Logger::error() << "DassfLP " << fLP.getId() << ": Event conversion failed "
		    << typeid(event[i]).name() << ", ignored" << endl;
		return;
	    }
    	    Logger::debug2() << "Executing event: "
                << Common::demangle(typeid(*e).name())
                << endl;

	    /// main try{} catch{} loop of simx
	    try {
    		e->execute(fLP);
	    }
	    catch(const Exception& ex)
	    {
		switch( ex.getLevel() )
		{
		    case Exception::kINFO:
			Logger::debug2() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kWARN:
			Logger::warn() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kERROR:
			Logger::error() << "Exception: " << ex.getDescription() << endl;
			break;
		    case Exception::kFATAL:
			Logger::error() << "FATAL Exception: " << ex.getDescription() << endl;
			Logger::failure( "FATAL Exception caught");
			break;
		    default:
			Logger::error() << "(UNKNOWN) Exception: " << ex.getDescription() << endl;
		}
	    }
	    catch(const std::exception& ex)
	    {
		SMART_ASSERT( ex.what() );
		Logger::error() << "std::exception: " << ex.what() << endl;
	    }

        }
      }
    }
  }
  waitOn();
}


} // namespace

#endif // SIMX_USE_PRIME

