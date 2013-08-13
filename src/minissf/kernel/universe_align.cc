#include "kernel/universe.h"
#include "ssf.h"

#define METIS_USE_RECURSIVE
extern "C" {
#include "metis/metis.h"
}

namespace minissf {

metis_node_t::metis_node_t(int _id, int _size, Timeline* tm) : 
  id(_id), size(_size), timeline(tm), part(0) {}

metis_node_t::~metis_node_t() { links.clear(); }

void metis_node_t::add_link(metis_node_t* tnode, double delay) 
{
  MAP(metis_node_t*,double)::iterator iter = links.find(tnode);
  if(iter == links.end()) links.insert(MAKE_PAIR(tnode, delay));
  else if(delay < (*iter).second) (*iter).second = delay;
}

metis_graph_t::metis_graph_t() : 
  nlinks(0), min_delay(1e38), max_delay(0) {}

metis_graph_t::~metis_graph_t() 
{
  for(VECTOR(metis_node_t*)::iterator iter = nodes.begin();
      iter != nodes.end(); iter++) delete (*iter);
  nodes.clear();
}

int metis_graph_t::add_node(int size, Timeline* tm) 
{
  int id = (int)nodes.size();
  metis_node_t* node = new metis_node_t(id, size, tm);
  nodes.push_back(node);
  return id;
}

void metis_graph_t::add_link(int idfrom, int idto, double delay) 
{
  assert(0 <= idfrom && idfrom < (int)nodes.size());
  assert(0 <= idto && idto < (int)nodes.size());
  assert(idfrom != idto);
  nodes[idfrom]->add_link(nodes[idto], delay);
  nlinks++;
}

void metis_graph_t::minmax_delays() 
{
  for(VECTOR(metis_node_t*)::iterator niter = nodes.begin();
      niter != nodes.end(); niter++) {
    for(MAP(metis_node_t*,double)::iterator liter = (*niter)->links.begin();
	liter != (*niter)->links.end(); liter++) {
      if(min_delay > (*liter).second) min_delay = (*liter).second;
      if(max_delay < (*liter).second) max_delay = (*liter).second;
    }
  }
}

int metis_graph_t::fx(double delay) 
{
  int ret;
  if(max_delay == min_delay) ret = 0;
  else {
    double x = (max_delay-delay)/(max_delay-min_delay); x += 1.0;
    x = pow(x, 0.25); // make it favorable to large delays
    ret = (int)(x*16384);
  }
  return ret;
}

void metis_graph_t::partition(int nparts) 
{
  minmax_delays();

  int i, n = nodes.size(); 
  idxtype *xadj = new idxtype[n+1];
  xadj[0] = 0;
  idxtype* adjncy = nlinks ? new idxtype[nlinks] : 0;
  idxtype* adjwgt = nlinks ? new idxtype[nlinks] : 0;
  int wgtflag = 3;
  int numflag = 0;
  int options[5]; options[0] = 0;
  int edgecut;
  idxtype* vwgt = new idxtype[n];
  idxtype* part = new idxtype[n];

  float* tpwgts = new float[nparts];
  for(i=0; i<nparts; i++) tpwgts[i] = 1.0/nparts;

  // partition among the machines
  if(nparts == 1) {
    // we don't do anything since the graph is initialized this way
  } else {
    int x = 0;
    VECTOR(metis_node_t*)::iterator iter;
    for(iter = nodes.begin(), i=0; iter != nodes.end(); iter++, i++) {
      metis_node_t* node = (*iter);
      vwgt[i] = node->size;
      if(min_delay < max_delay) {
	for(MAP(metis_node_t*,double)::iterator f_iter = node->links.begin();
	    f_iter != node->links.end(); f_iter++) {
	  idxtype edge = (idxtype)fx((*f_iter).second);
	  adjncy[x] = (*f_iter).first->id;
	  adjwgt[x] = edge;
	  x++;
	}
      }
      xadj[i+1] = x;
    }
    assert(x == 0 || x == nlinks);
    
    if(x == 0) wgtflag = 2;
    if(nparts <= 8) {
      METIS_WPartGraphRecursive(&n, xadj, x>0?adjncy:0, vwgt, x>0?adjwgt:0, &wgtflag,
				&numflag, &nparts, tpwgts, options, &edgecut, part);
    } else {
      METIS_WPartGraphKway(&n, xadj, x>0?adjncy:0, vwgt, x>0?adjwgt:0, &wgtflag,
			   &numflag, &nparts, tpwgts, options, &edgecut, part);
    }
    
    i = 0;
    for(iter = nodes.begin(); iter != nodes.end(); iter++) {
      (*iter)->part = part[i++];
    }
  }

  delete[] xadj;
  if(adjncy) delete[] adjncy;
  if(adjwgt) delete[] adjwgt;
  delete[] tpwgts;
  delete[] vwgt;
  delete[] part;
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
