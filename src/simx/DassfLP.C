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
// File:    DassfLP.C
// Module:  simx
// Author:  K. Bisset, Sunil Thulasidasan
// Created: June 25 2004
//
// @@
//
//--------------------------------------------------------------------------

#ifdef SIMX_USE_PRIME

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




#include "Common/backtrace.h"

#include "boost/lexical_cast.hpp"

#include "simx/Messenger.h"

using namespace std;

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
    //minissf::inChannel* in = new minissf::inChannel(const_cast<char*>(inname.c_str()), this);
    // in miniSSF, the inChannel constructor signature is reversed.
    minissf::inChannel* in = new minissf::inChannel( this,const_cast<char*>(inname.c_str()));
     //fIn.push_back(in);
    fIn.push_back( (fInSet.insert(in)).first );
     //minissf::outChannel* out = new minissf::outChannel(this, minissf::ltime_t(0));
    minissf::outChannel* out = new minissf::outChannel(this);
      //SSF_PUBLISH_DML_OUTCHANNEL(out, const_cast<char*>(outname.c_str()));
    //fOut.push_back(out);
    fOut.push_back( (fOutSet.insert(out)).first );
  }
    // arrays must end with a null pointer
  //fIn.push_back(0);
  //fOut.push_back(0);

  // minissf::Process* p = 
  //new minissf::Process(this,(void (minissf::Entity::*)(minissf::Process*))
  //                               &DassfLP::process, true);
    minissf::Process* p = new EventProcess(this);
   // p->waitsOn(&fIn[0]);
  //p->waitsOn(fIn);
  p->waitsOn(fInSet);
}

//------------------------ EMBEDDED CODE STARTS ----------------------------//
// define the procedure class for action
class _ssf_procedure_EventProcess_action : public minissf::Procedure { 
 public: 
  _ssf_procedure_EventProcess_action() : minissf::Procedure(0, 0) {} 
}; 
// the _ssf_create_procedure_* method is generated for each procedure
minissf::Procedure* EventProcess::_ssf_create_procedure_action() { 
  return new _ssf_procedure_EventProcess_action(); 
}
// function body is transformed
void EventProcess::action() { 
  minissf::Process* _ssf_proc = this; 
  if(!_ssf_proc->top_stack()) { 
    _ssf_proc->push_stack(_ssf_create_procedure_action()); 
  } 
  if(!_ssf_proc || !_ssf_proc->in_procedure_context()) minissf::ThrowableStream("DassfLP.C", 98, "action") << "improper procedure call"; // PROPER_PROCEDURE(_ssf_proc); 
  _ssf_procedure_EventProcess_action* _ssf_pframe = (_ssf_procedure_EventProcess_action*)_ssf_proc->top_stack(); 
  switch(_ssf_pframe->entry_point) { 
    case 1: goto _ssf_sync1; 
  }

  //  #line 100 "DassfLP_cmt_strip.C"

{ _ssf_pframe->entry_point = 1; _ssf_pframe->call_procedure(  ((DassfLP*)owner())->_ssf_create_procedure_process(this)); if(_ssf_pframe->call_suspended()) { return; _ssf_sync1: ; } }
{ _ssf_pframe->call_return(); return; }}
//------------------------ EMBEDDED CODE ENDS ----------------------------//
//#line 103 "DassfLP_cmt_strip.C"


DassfLP::~DassfLP()
{
}

void DassfLP::init()
{
#ifdef DEBUG
  Logger::debug3() << "DassfLP init()" << endl;
#endif
} 

void DassfLP::mapChannels()
{
#ifdef DEBUG
  Logger::debug3() << "DassfLP: mapping channels" << endl;
#endif
  LPID local_id = fLP.getId();
  const string local = boost::lexical_cast<string>( fLP.getId() );
  for (int i=0; i < Control::getNumLPs(); ++i)
  {
    const string remote = boost::lexical_cast<string>(i);
    const string inname = string("I")+remote+string(".")+local;
    const string outname = string("O")+remote+string(".")+local;
    if (i == local_id) {
      (*fOut[i])->mapto(const_cast<char*>(inname.c_str()), LOCAL_MINDELAY );
    } else {
      (*fOut[i])->mapto(const_cast<char*>(inname.c_str()), LP::MINDELAY );
    }
    
  }
  //cout <<" dasslp, mindelay is" << LP::MINDELAY << endl;
}

void DassfLP::wrapup()
{
  //do we need to do anything here?
}

void DassfLP::sendDassfEvent(const LPID destLP, minissf::Event* e, const Time delay)
{
#ifdef DEBUG
  Logger::debug3() << "DassfLP " << fLP.getId() << ": sending event with delay " 
		   << delay << " to LP " << destLP << ", at time " << now() << endl;
#endif
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
    //Logger::debug3() << "   realDelay: " << realDelay << std::endl;
    SMART_ASSERT( fOut.size() > static_cast<unsigned int>(destLP) )( fOut.size() )( destLP );
    (*fOut[destLP])->write( e, realDelay);
}
 

  // Below is old dassf/prime-ssf code for processing events. With minissf things are 
  // quite a bit different -ST.

// void DassfLP::process(minissf::Process* p)
// {
//   Logger::setTime( now() );
  
//   Logger::debug2() << "DassfLP " << fLP.getId() << ": in process() " << Control::getRank() << "/" << Control::getNumMachines()
//     << " @" << now() << endl;
//   minissf::inChannel** in = p->activeChannels();
//   if (in != 0)
//   {
//     for (int j=0; in[j] != 0; ++j)
//     {
//       minissf::Event** event = in[j]->activeEvents();
//       if (event != 0)
//       {
//         for (int i=0; event[i] != 0; ++i)
//         {
// 	    DassfEvent* e = dynamic_cast<DassfEvent*>(event[i]);
// 	    if( !e )
// 	    {
//         	Logger::error() << "DassfLP " << fLP.getId() << ": Event conversion failed "
// 		    << typeid(event[i]).name() << ", ignored" << endl;
// 		return;
// 	    }
//     	    Logger::debug2() << "Executing event: "
//                 << Common::demangle(typeid(*e).name())
//                 << endl;

// 	    /// main try{} catch{} loop of simx
// 	    try {
//     		e->execute(fLP);
// 	    }
// 	    catch(const Exception& ex)
// 	    {
// 		switch( ex.getLevel() )
// 		{
// 		    case Exception::kINFO:
// 			Logger::debug2() << "Exception: " << ex.getDescription() << endl;
// 			break;
// 		    case Exception::kWARN:
// 			Logger::warn() << "Exception: " << ex.getDescription() << endl;
// 			break;
// 		    case Exception::kERROR:
// 			Logger::error() << "Exception: " << ex.getDescription() << endl;
// 			break;
// 		    case Exception::kFATAL:
// 			Logger::error() << "FATAL Exception: " << ex.getDescription() << endl;
// 			Logger::failure( "FATAL Exception caught");
// 			break;
// 		    default:
// 			Logger::error() << "(UNKNOWN) Exception: " << ex.getDescription() << endl;
// 		}
// 	    }
// 	    catch(const std::exception& ex)
// 	    {
// 		SMART_ASSERT( ex.what() );
// 		Logger::error() << "std::exception: " << ex.what() << endl;
// 	    }

//         }
//       }
//     }
//   }
//   waitOn();
// }
 
// ssf procedure
 
//------------------------ EMBEDDED CODE STARTS ----------------------------//
// define the procedure class for process
  class _ssf_procedure_DassfLP_process : public minissf::Procedure 
  { 
  public: 
    minissf::Process* p; // function argument
    _ssf_procedure_DassfLP_process(minissf::ProcedureContainer* _ssf_focus, 
				   minissf::Process* _ssf_local_p,
				   void* _ssf_retaddr) : 
      minissf::Procedure((minissf::ProcedureFunction)&DassfLP::process, 
			 _ssf_focus, _ssf_retaddr, 0), p(_ssf_local_p) {} 
  }; 
  

  // the _ssf_create_procedure_* method is generated for each procedure
  minissf::Procedure* DassfLP::_ssf_create_procedure_process
  (minissf::Process* p, void* retaddr) 
  { 
    return new _ssf_procedure_DassfLP_process(this, p, retaddr); 
  }
  

// function body is transformed
void DassfLP::process(minissf::Process* _ssf_proc) 
{ 
  if(!_ssf_proc->top_stack()) 
    { 
      minissf::ThrowableStream("DassfLP.C", 230, "process") << "not a starting procedure"; 
    } 
  if(!_ssf_proc || !_ssf_proc->in_procedure_context()) 
    minissf::ThrowableStream("DassfLP.C", 230, "process") 
      << "improper procedure call"; // PROPER_PROCEDURE(_ssf_proc); 
  _ssf_procedure_DassfLP_process* _ssf_pframe = 
    (_ssf_procedure_DassfLP_process*)_ssf_proc->top_stack(); 
  switch(_ssf_pframe->entry_point) 
    { 
    case 1: goto _ssf_sync1; 
  }

  //  #line 232 "DassfLP_cmt_strip.C"
 
   // Logger::setTime( now() );
  
   // Logger::debug2() << "DassfLP " << fLP.getId() 
   // 		    << ": in process() " << Control::getRank()
   // 		    << "/" << Control::getNumMachines()
   // 		    << " @" << now() << endl;
   minissf::inChannel* in;
   minissf::Event* event;
   // for(;;) {
   //cout << "here" << endl;
    
   { _ssf_pframe->entry_point = 1;
     _ssf_pframe->p->waitOn();
     if(_ssf_pframe->call_suspended()) 
       { 
	 return;
       _ssf_sync1: ; 
       } 
   }
   //while( (in = _ssf_pframe->p->activeChannel()) != 0 )
   in =  _ssf_pframe->p->activeChannel();
   if ( in != 0 )
     {
       //cout << "here2" << endl;
	
       
       
       //while((event = in->activeEvent()) != 0)
       event = in->activeEvent();
       if ( event != 0 )
	 {
	   Logger::setTime( now() );
#ifdef DEBUG
	   Logger::debug2() << "DassfLP " << fLP.getId() 
			    << ": in process() " << Control::getRank()
			    << "/" << Control::getNumMachines()
			    << " @" << now() << endl;
#endif
	   

	    
	    DassfEvent* e = dynamic_cast<DassfEvent*>(event);
	    if( !e )
	    {
        	Logger::error() << "DassfLP " << fLP.getId() 
				<< ": Event conversion failed "
				<< typeid(event).name() << ", ignored" << endl;
		{ _ssf_pframe->call_return(); return; }
	    }
#ifdef DEBUG
    	    Logger::debug2() << "Executing event: "
			     << Common::demangle(typeid(*e).name())
			     << endl;
#endif
	    
	    try {
	      e->execute(fLP);
	    }
	    catch(const Exception& ex)
	      {
		switch( ex.getLevel() )
		  {
		  case Exception::kINFO:
#ifdef DEBUG
		    Logger::debug2() << "Exception: " << ex.getDescription() << endl;
#endif
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
		    Logger::error() << "(UNKNOWN) Exception: " 
				    << ex.getDescription() << endl;
		  }
	      }
	    catch(const std::exception& ex)
	      {
		SMART_ASSERT( ex.what() );
		Logger::error() << "std::exception: " << ex.what() << endl;
	      }

	  
	  }
      }
    //}
  
   { 
     _ssf_pframe->call_return(); 
     return; 
   }
}
//------------------------ EMBEDDED CODE ENDS ----------------------------//

} 

#endif 
