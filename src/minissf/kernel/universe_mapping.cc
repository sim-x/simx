#include <ctype.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#include "kernel/universe.h"
#include "ssf.h"

namespace minissf {

#define MAX_MPI_BUFSIZ 4096
#define MAX_MPI_NNAMES 100
#define MAX_MPI_NMAPS 100

SET(Timeline*) Universe::created_timelines;
MAP(PAIR(int,int),Stargate*) Universe::created_stargates;
SET(Entity*) Universe::orphan_entities;
VECTOR(Entity*) Universe::created_entities;
MAP(STRING,inChannel*) Universe::local_icmap;
MAP(STRING,int) Universe::remote_icmap;
VECTOR(inChannel*) Universe::named_inchannels_vector;
int* Universe::timeline_scans = 0;
Universe::MapRequest* Universe::mapreq_head = 0;
Universe::MapRequest* Universe::mapreq_tail = 0;
MAP(int,VECTOR(PAIR(MapInport*,Stargate*))*) Universe::starmap;

void Universe::assign_timeline(Timeline* tmln) {
  timelines.insert(tmln);
  tmln->settle_universe(this);
}

Timeline* Universe::create_timeline() { 
  Timeline* timeline = new Timeline;
  created_timelines.insert(timeline);
  return timeline;
}

void Universe::delete_timeline(Timeline* timeline) {
  created_timelines.erase(timeline);
  delete timeline;
}

void Universe::register_stargate(Stargate* sg) {
  created_stargates.insert(MAKE_PAIR(MAKE_PAIR(sg->source_timeline_id, sg->target_timeline_id), sg));
}

Stargate* Universe::find_stargate(int src, int tgt) {
  MAP(PAIR(int,int),Stargate*)::iterator iter = created_stargates.find(MAKE_PAIR(src,tgt));
  if(iter != created_stargates.end()) return (*iter).second;
  else return 0;
}

void Universe::register_orphan_entity(Entity* ent) {
  orphan_entities.insert(ent);
  created_entities.push_back(ent);
}

void Universe::deregister_orphan_entity(Entity* ent) {
  orphan_entities.erase(ent);
}

void Universe::register_named_inchannel(inChannel* ic) {
  if(!local_icmap.insert(MAKE_PAIR(ic->name, ic)).second)
    SSF_THROW("duplicate inchannel name: " << ic->name);
  named_inchannels_vector.push_back(ic);
}

int Universe::timeline_to_machine(int sno) {
  assert(timeline_scans);
  for(int i=0; i<args_nmachs-1; i++)
    if(sno < timeline_scans[i]) return i;
  assert(sno < timeline_scans[args_nmachs-1]);
  return args_nmachs-1;
}

void Universe::add_mapping(outChannel* oc, inChannel* ic, VirtualTime delay)
{
  MapRequest* req = new MapRequest(oc, ic, delay);
  if(!mapreq_head) mapreq_head = mapreq_tail = req;
  else { mapreq_tail->next = req; mapreq_tail = req; }
}

void Universe::add_mapping(outChannel* oc, STRING icname, VirtualTime delay)
{
  MapRequest* req = new MapRequest(oc, icname, delay);
  if(!mapreq_head) mapreq_head = mapreq_tail = req;
  else { mapreq_tail->next = req; mapreq_tail = req; }
}

void Universe::synchronize_inchannel_names()
{
  if(args_nmachs == 1) { // nothing nneds to be done really if sequential
    named_inchannels_vector.clear();
    return; 
  }

#ifdef HAVE_MPI_H
  int nnames = named_inchannels_vector.size();
  /*
  if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
    printf(">> [%d] synchronize %d ic names\n", args_rank, nnames);
    for(int h=0; h<nnames; h++)
      printf(">>  %d => %s\n", h, named_inchannels_vector[h]->name.c_str());
  }
  */
  int maxnnames;
  ssf_mpi_allreduce(&nnames, &maxnnames, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  /*
  if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) 
    printf(">>  max names = %d\n", maxnnames);
  */
  if(maxnnames == 0) return;

  char* sendbuf = new char[MAX_MPI_BUFSIZ]; assert(sendbuf);
  int* rcnts = new int[args_nmachs]; assert(rcnts);
  int* rdisp = new int[args_nmachs]; assert(rdisp);
  register int i, j, k;

  // pack and send MAX_MPI_NNAMES names at a time
  for(k=0; k<maxnnames; k+=MAX_MPI_NNAMES) {
    int sendpos = 0;
    int nn = k<nnames ? nnames-k : 0; if(nn > MAX_MPI_NNAMES) nn = MAX_MPI_NNAMES;
    ssf_mpi_pack(&nn, 1, MPI_INT, sendbuf, MAX_MPI_BUFSIZ, &sendpos, MPI_COMM_WORLD);
    if(nn > 0) {
      for(i=k; i<k+nn; i++) {	
	int icnamelen = (int)named_inchannels_vector[i]->name.length();
	int sn = named_inchannels_vector[i]->entity_owner->timeline->serialno;
	ssf_mpi_pack(&icnamelen, 1, MPI_INT, sendbuf, MAX_MPI_BUFSIZ, &sendpos, MPI_COMM_WORLD);
	ssf_mpi_pack((char*)named_inchannels_vector[i]->name.c_str(), icnamelen, MPI_UNSIGNED_CHAR, 
		     sendbuf, MAX_MPI_BUFSIZ, &sendpos, MPI_COMM_WORLD);
	ssf_mpi_pack(&sn, 1, MPI_INT, sendbuf, MAX_MPI_BUFSIZ, &sendpos, MPI_COMM_WORLD);
      }
    }
    ssf_mpi_allgather(&sendpos, 1, MPI_INT, rcnts, 1, MPI_INT, MPI_COMM_WORLD);
    rdisp[0] = 0;
    for(i=1; i<args_nmachs; i++)
      rdisp[i] = rdisp[i-1]+rcnts[i-1];
    int rsize = rdisp[args_nmachs-1]+rcnts[args_nmachs-1];
    /*
    if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
      printf(">> [%d] sendpos=%d\n", args_rank, sendpos);
      for(i=0; i<args_nmachs; i++)
	printf(">> [%d] rcnts[%d]=%d, rdisp[%d]=%d\n", args_rank,
	       i, rcnts[i], i, rdisp[i]);
      //printf("%d: send data:\n", args_rank);
      //for(i=0; i<sendpos; i++) printf("%02x ", unsigned(sendbuf[i]&0xff)); 
      //printf("\n");
    }
    */
    char* recvbuf = new char[rsize]; assert(recvbuf);
    ssf_mpi_allgatherv(sendbuf, sendpos, MPI_PACKED,
		       recvbuf, rcnts, rdisp, MPI_PACKED, MPI_COMM_WORLD);
    //if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
    //  printf("%d: recv data:\n", args_rank);
    //  for(i=0; i<rsize; i++) printf("%02x ", unsigned(recvbuf[i]&0xff));
    //  printf("\n");
    //}

    for(i=0; i<args_nmachs; i++) {
      if(i == args_rank) continue;
      char* buf = &recvbuf[rdisp[i]];
      int bufsiz = rcnts[i];
      int pos = 0;
      int nnames;
      ssf_mpi_unpack(buf, bufsiz, &pos, &nnames, 1, MPI_INT, MPI_COMM_WORLD);
      /*
      if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	printf(">> [%d] got %d public names from %d\n", args_rank, nnames, i);
      }
      */
      for(j=0; j<nnames; j++) {
	int icnamelen;
	ssf_mpi_unpack(buf, bufsiz, &pos, &icnamelen, 1, MPI_INT, MPI_COMM_WORLD);
	char* icname = new char[icnamelen+1]; assert(icname);
	int sn;
	ssf_mpi_unpack(buf, bufsiz, &pos, icname, icnamelen, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	ssf_mpi_unpack(buf, bufsiz, &pos, &sn, 1, MPI_INT, MPI_COMM_WORLD);
	icname[icnamelen] = 0;

	if(!remote_icmap.insert(std::make_pair(icname, sn)).second)
	  SSF_THROW("duplicate inchannel name: " << icname);
	assert(timeline_to_machine(sn) == i);
	/*
	if((args_debug_mask&DEBUG_FLAG_MPIMSG) != 0) {
	  printf(">>   %d: %s => %d\n", j, icname, sn);
	}
	*/
	delete[] icname;
      }
    }
    delete[] recvbuf;
  }
  named_inchannels_vector.clear();

  delete[] sendbuf;
  delete[] rcnts;
  delete[] rdisp;
#endif
}

void Universe::wire_up_channels()
{
  int rmap_cnt = 0;
  MapRequest* rmap_head = 0;
  MapRequest* rmap_tail = 0;

  MapRequest* node = mapreq_head;
  while(node) {
    MapRequest* req = node;
    node = req->next;
    if(req->ic) { 
      map_local_local(req->oc, req->ic, req->delay); 
      delete req;
    } else {
      MAP(STRING,inChannel*)::iterator iter = local_icmap.find(req->icname);
      if(iter != local_icmap.end()) {
	map_local_local(req->oc, (*iter).second, req->delay);
	delete req;
      } else {
	MAP(STRING,int)::iterator iter = remote_icmap.find(req->icname);
	if(iter == remote_icmap.end()) 
	  SSF_THROW("mapto unknown inchannel: " << req->icname);
	// delay becomes initial min delay or extra delay!!!
	map_local_remote(req->oc, (*iter).second, req->delay); 

	// add to the link for further operations
	rmap_cnt++;
	req->next = 0;
	if(!rmap_head) rmap_head = rmap_tail = req;
	else { rmap_tail->next = req; rmap_tail = req; }
      }
    }
  }
  mapreq_head = mapreq_tail = 0;
  if(args_nmachs == 1) {
    assert(!rmap_cnt);
    local_icmap.clear();
    return;
  }

#ifdef HAVE_MPI_H
  register int i, k;

  int maxmaps;
  ssf_mpi_allreduce(&rmap_cnt, &maxmaps, 1, MPI_INT, MPI_MAX, MPI_COMM_WORLD);
  if(maxmaps == 0) return;

  char* sendbuf = new char[args_nmachs*MAX_MPI_BUFSIZ]; assert(sendbuf);
  char** sbuf = new char*[args_nmachs]; assert(sbuf);
  int* sdisp = new int[args_nmachs]; assert(sdisp);
  for(i=0; i<args_nmachs; i++) {
    sdisp[i] = i*MAX_MPI_BUFSIZ;
    sbuf[i] = &sendbuf[sdisp[i]];
  }
  int* sendcnt = new int[args_nmachs]; assert(sendcnt);
  int* recvcnt = new int[args_nmachs]; assert(recvcnt);
  int* rdisp = new int[args_nmachs]; assert(rdisp);

  // pack and send MAX_MPI_NMAPS mappings at a time
  for(k=0; k<maxmaps; k += MAX_MPI_NMAPS) {
    memset(sendcnt, 0, args_nmachs*sizeof(int));

    int nn = 0;
    while(rmap_head && nn<MAX_MPI_NMAPS) {
      MapRequest* req = rmap_head;
      rmap_head = req->next;
      nn++;

      int sn = 0;
      MAP(STRING,int)::iterator iter = remote_icmap.find(req->icname);
      if(iter != remote_icmap.end()) sn = (*iter).second;
      else assert(0);

      int mach = timeline_to_machine(sn);
      int tmlnid = req->oc->entity_owner->timeline->serialno;
      int outport = req->oc->portno;
      int icnamelen = (int)req->icname.length();
      int64 mytime = (int64)req->delay; // extra delay here!

      ssf_mpi_pack(&tmlnid, 1, MPI_INT, sbuf[mach], MAX_MPI_BUFSIZ, &sendcnt[mach], MPI_COMM_WORLD);
      ssf_mpi_pack(&outport, 1, MPI_INT, sbuf[mach], MAX_MPI_BUFSIZ, &sendcnt[mach], MPI_COMM_WORLD);
      ssf_mpi_pack(&mytime, 1, MPI_LONG_LONG_INT, sbuf[mach], MAX_MPI_BUFSIZ, 
		   &sendcnt[mach], MPI_COMM_WORLD);
      ssf_mpi_pack(&icnamelen, 1, MPI_INT, sbuf[mach], MAX_MPI_BUFSIZ, &sendcnt[mach], MPI_COMM_WORLD);
      ssf_mpi_pack((char*)req->icname.c_str(), icnamelen, MPI_UNSIGNED_CHAR, 
		   sbuf[mach], MAX_MPI_BUFSIZ, &sendcnt[mach], MPI_COMM_WORLD);
      delete req;
    }
    
    ssf_mpi_alltoall(sendcnt, 1, MPI_INT, recvcnt, 1, MPI_INT, MPI_COMM_WORLD);

    rdisp[0] = 0;
    for(i=1; i<args_nmachs; i++)
      rdisp[i] = rdisp[i-1]+recvcnt[i-1];
    int rsize = rdisp[args_nmachs-1]+recvcnt[args_nmachs-1];
    char* recvbuf = new char[rsize]; assert(recvbuf);
    ssf_mpi_alltoallv(sendbuf, sendcnt, sdisp, MPI_PACKED,
		      recvbuf, recvcnt, rdisp, MPI_PACKED, MPI_COMM_WORLD);

    for(i=0; i<args_nmachs; i++) {
      if(i == args_rank) continue;

      char* rbuf = &recvbuf[rdisp[i]];
      int bufsiz = recvcnt[i];
      int pos = 0;

      while(pos < bufsiz) {
	int tmlnid, outport, icnamelen;
	int64 mytime;

	ssf_mpi_unpack(rbuf, bufsiz, &pos, &tmlnid, 1, MPI_INT, MPI_COMM_WORLD);
	ssf_mpi_unpack(rbuf, bufsiz, &pos, &outport, 1, MPI_INT, MPI_COMM_WORLD);
	ssf_mpi_unpack(rbuf, bufsiz, &pos, &mytime, 1, MPI_LONG_LONG_INT, MPI_COMM_WORLD);
	ssf_mpi_unpack(rbuf, bufsiz, &pos, &icnamelen, 1, MPI_INT, MPI_COMM_WORLD);

	char* icname = new char[icnamelen+1]; assert(icname);
	ssf_mpi_unpack(rbuf, bufsiz, &pos, icname, icnamelen, MPI_UNSIGNED_CHAR, MPI_COMM_WORLD);
	icname[icnamelen] = 0;

	MAP(STRING,inChannel*)::iterator iter = local_icmap.find(icname);
	if(iter == local_icmap.end()) SSF_THROW("can't find inchannel: " << icname);
	map_remote_local(tmlnid, outport, (*iter).second, mytime);

	delete[] icname;
      }
    }
    delete[] recvbuf;
  }

  delete[] sendbuf;
  delete[] sbuf;
  delete[] sdisp;
  delete[] rdisp;
  delete[] sendcnt;
  delete[] recvcnt;

  local_icmap.clear();
  remote_icmap.clear();
#endif
}

void Universe::map_local_local(outChannel* oc, inChannel* ic, VirtualTime delay)
{
  MapInport* inport = new MapInport(ic);
  delay += oc->channel_delay;
  /*
  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf(">> [%d] map_local_local(%d=>%d): (channel+mapping) delay=%lg\n", 
	   args_rank, oc->entity_owner->timeline->serialno, 
	   ic->entity_owner->timeline->serialno, delay.second());
  }
  */

  // try find an existing outport at oc
  DEQUE(MapOutport*)::iterator iter;
  for(iter = oc->outports.begin(); iter != oc->outports.end(); iter++) {
    if(!(*iter)->stargate && // no stargate means it's on the same timeline
       oc->entity_owner->timeline == ic->entity_owner->timeline) break;
    else if((*iter)->stargate &&  // stargate to another timeline on the same machine
	    (*iter)->stargate->target_timeline &&
	    (*iter)->stargate->target_timeline == ic->entity_owner->timeline) break;
  }
  MapOutport* outport;
  if(iter != oc->outports.end()) outport = *iter;
  else outport = 0;

  if(!outport) { // if outport does not exist, create one
    if(oc->entity_owner->timeline == ic->entity_owner->timeline) {
      outport = new MapOutport(0, inport, delay);
    } else {
      // connecting separate timelines on the same machine
      Stargate* sg = find_stargate(oc->entity_owner->timeline->serialno, 
				   ic->entity_owner->timeline->serialno);
      if(!sg) {
	sg = new Stargate(oc->entity_owner->timeline, ic->entity_owner->timeline);
	assert(sg);
      }
      sg->set_delay(delay);
      outport = new MapOutport(sg, inport, delay); assert(outport);
    }
    assert(outport);
    oc->outports.push_back(outport);
  } else { // if the outport exists, adjust min delay and insert at right place
    VirtualTime x = delay - outport->min_offset;
    if(delay < outport->min_offset) {
      outport->min_offset = delay;
      if(outport->stargate) outport->stargate->set_delay(delay);
    }

    MapInport** p = &outport->inport;
    while((*p) && (*p)->extra_delay < x) {
      x -= (*p)->extra_delay;
      p = &(*p)->next;
    }
    inport->next = *p;
    if(inport->next) inport->next->extra_delay -= x;
    *p = inport; inport->extra_delay = ((x>0)?x:VirtualTime(0));
  }
}

void Universe::map_local_remote(outChannel* oc, int timelineno, VirtualTime& delay)
{
  delay += oc->channel_delay;
  /*
  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf(">> [%d] map_local_remote(%d=>%d): (channel+mapping) delay=%lg\n", 
	   args_rank, oc->entity_owner->timeline->serialno, 
	   timelineno, delay.second());
  }
  */

  // try find an existing outport at oc
  DEQUE(MapOutport*)::iterator iter;
  for(iter = oc->outports.begin(); iter != oc->outports.end(); iter++) {
    if((*iter)->stargate && !(*iter)->stargate->target_timeline &&
       (*iter)->stargate->target_timeline_id == timelineno) break;
  }
  MapOutport* outport;
  if(iter != oc->outports.end()) outport = *iter;
  else outport = 0;

  if(!outport) { // if outport does not exist, create one
    Stargate* sg = find_stargate(oc->entity_owner->timeline->serialno, timelineno);
    if(!sg) {
      sg = new Stargate(oc->entity_owner->timeline, timelineno, oc->portno);
      assert(sg);
    }
    outport = new MapOutport(sg, 0, delay); assert(outport);
    sg->set_delay(delay);
    oc->outports.push_back(outport);
    // the first channel map, delay is the initial min delay!
  } else { // if outport exists, adjust min delay
    assert(outport->stargate);
    VirtualTime x = delay-outport->min_offset;
    if(delay < outport->min_offset) {
      outport->min_offset = delay;
      outport->stargate->set_delay(delay);
    }
    delay = x; // subsequent channel map, delay is the extra delay (which may be negative)
  }
}

void Universe::map_remote_local(int tmlnid, int outport, inChannel* ic, VirtualTime xdelay)
{
  /*
  if((args_debug_mask&DEBUG_FLAG_LPSCHED) != 0) {
    printf(">> [%d] map_remote_local(%d=>%d): xdelay=%lg\n", args_rank, tmlnid, 
	   ic->entity_owner->timeline->serialno, xdelay.second());
  }
  */

  MAP(int,VECTOR(PAIR(MapInport*,Stargate*))*)::iterator iter = 
    starmap.find(outport);
  if(iter == starmap.end()) { // if we can't find an existing map
    VECTOR(PAIR(MapInport*,Stargate*))* vec = 
      new VECTOR(PAIR(MapInport*,Stargate*));
    assert(vec);
    Stargate* sg = find_stargate(tmlnid, ic->entity_owner->timeline->serialno);
    if(!sg) {
      sg = new Stargate(tmlnid, outport, ic->entity_owner->timeline); 
      assert(sg);
    }
    sg->set_delay(xdelay); // first time, xdelay is min_delay (not extra)
    MapInport* inport = new MapInport(ic);
    vec->push_back(MAKE_PAIR(inport,sg));
    starmap.insert(MAKE_PAIR(outport,vec));
  } else {
    VECTOR(PAIR(MapInport*,Stargate*))* vec = (*iter).second;
    assert(vec);
    for(int i=0; i<(int)vec->size(); i++) {
      Stargate* sg = vec->at(i).second; assert(sg);
      if(sg->target_timeline == ic->entity_owner->timeline) {
	// found the existing mapping
	if(xdelay < 0) {
	  sg->set_delay(sg->min_delay-xdelay); // subsequent times, xdelay is the extra
	}
	MapInport* inport = new MapInport(ic);
	MapInport** p = &vec->at(i).first;
	while((*p) && (*p)->extra_delay < xdelay) {
	  xdelay -= (*p)->extra_delay;
	  p = &(*p)->next;
	}
	inport->next = *p;
	if(inport->next) inport->next->extra_delay -= xdelay;
	*p = inport; inport->extra_delay = ((xdelay>0)?xdelay:VirtualTime(0));
	return;
      }
    }
    // if no such target timeline is found
    Stargate* sg = find_stargate(tmlnid, ic->entity_owner->timeline->serialno);
    if(!sg) {
      sg = new Stargate(tmlnid, outport, ic->entity_owner->timeline); 
      assert(sg);
    }
    sg->set_delay(xdelay); // first time, xdelay is min_delay (not extra)
    MapInport* inport = new MapInport(ic);
    vec->push_back(MAKE_PAIR(inport,sg));
  }
}

}; /*namespace minissf*/

/*
 * Copyright (c) 2011-2013 Florida International University.
 *
 * Permission is hereby granted, free of charge, to any individual or
 * institution obtaining a copy of this software and associated
 * documentation files (the "software"), to use, copy, modify, and
 * distribute without restriction.
 *
 * The software is provided "as is", without warranty of any kind,
 * express or implied, including but not limited to the warranties of
 * merchantability, fitness for a particular purpose and
 * noninfringement.  In no event shall Florida International
 * University be liable for any claim, damages or other liability,
 * whether in an action of contract, tort or otherwise, arising from,
 * out of or in connection with the software or the use or other
 * dealings in the software.
 *
 * This software is developed and maintained by
 *
 *   Modeling and Networking Systems Research Group
 *   School of Computing and Information Sciences
 *   Florida International University
 *   Miami, Florida 33199, USA
 *
 * You can find our research at http://www.primessf.net/.
 */
