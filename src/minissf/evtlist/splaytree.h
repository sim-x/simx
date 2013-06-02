#ifndef __MINISSF_SPLAYTREE_H__
#define __MINISSF_SPLAYTREE_H__

#ifndef __MINISSF_SIMEVENT_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

#include <assert.h>

namespace minissf {

template<typename T> class SplayTree;
template<typename T> class SplayTreeNode;

// here's an eventlist implementation using the splay tree
template<typename T>
class SplayTree : public Eventlist<T> {
 public:
  // the constructor
  SplayTree() : num_items(0), root(0), min_node(0) {}
  
  // the destructor
  virtual ~SplayTree() { clear(); }

  // these are methods defined in the eventlist class
  virtual int size() const { return num_items; }
  virtual SimEvent<T>* getMin() const { return (SimEvent<T>*)min_node; }
  virtual SimEvent<T>* deleteMin();
  virtual void insert(SimEvent<T>* evt);
  virtual void adjust(SimEvent<T>* evt);
  virtual void cancel(SimEvent<T>* evt);
  virtual void clear();

 protected:
  int num_items; // total number of tree nodes
  SplayTreeNode<T>* root; // point to the root of the splay tree
  SplayTreeNode<T>* min_node; // point to the tree node that has the smallest timestamp

 protected:
  // remove the tree node that has the smallest timestamp
  SplayTreeNode<T>* remove_min();

  // reclaim a sub-tree rooted by the given node
  void reclaim_tree(SplayTreeNode<T>* node);

  // the famous splay operation
  void splay(SplayTreeNode<T>* node);

}; /*class SplayTree*/

// here's the event type if using splay tree as eventlist
template<typename T>
class SplayTreeNode : public SimEvent<T> {
 public:
  // the constructor
  SplayTreeNode(T ts) : SimEvent<T>(ts), parent(0), left(0), right(0) {}

 protected:
  template<typename V> friend class SplayTree;

  SplayTreeNode<T>* parent;
  SplayTreeNode<T>* left;
  SplayTreeNode<T>* right;
}; /*class SplayTreeNode*/

// these macros are used internally
#define SPLAYTREE_UP(t)    ((t)->parent)
#define SPLAYTREE_UPUP(t)  ((t)->parent->parent)
#define SPLAYTREE_LEFT(t)  ((t)->left)
#define SPLAYTREE_RIGHT(t) ((t)->right)
#define SPLAYTREE_RROTATE(n,p,g) \
  if((SPLAYTREE_LEFT(p) = SPLAYTREE_RIGHT(n))) SPLAYTREE_UP(SPLAYTREE_RIGHT(n)) = p; \
  SPLAYTREE_RIGHT(n) = p; SPLAYTREE_UP(n) = g;  SPLAYTREE_UP(p) = n;
#define SPLAYTREE_LROTATE(n,p,g) \
  if((SPLAYTREE_RIGHT(p) = SPLAYTREE_LEFT(n))) SPLAYTREE_UP(SPLAYTREE_LEFT(n)) = p; \
  SPLAYTREE_LEFT(n) = p; SPLAYTREE_UP(n) = g;  SPLAYTREE_UP(p) = n;

template<typename T>
SimEvent<T>* SplayTree<T>::deleteMin()
{
  if(!num_items) return 0;
  SplayTreeNode<T>* e = remove_min();
  num_items--;
  e->eventlist = 0;
  return (SimEvent<T>*)e;
}

template<typename T>
void SplayTree<T>::insert(SimEvent<T>* evt)
{
  //SplayTreeNode<T>* e = dynamic_cast<SplayTreeNode<T>*>(evt); (time consuming)
  SplayTreeNode<T>* e = (SplayTreeNode<T>*)evt;
  if(!e) SSF_THROW("attempt to insert a null event");
  if(e->eventlist) SSF_THROW("attempt to insert an already scheduled event");
  
  SplayTreeNode<T>* n = root;
  SPLAYTREE_RIGHT(e) = SPLAYTREE_LEFT(e) = 0;
  if(n) {
    for(;;) {
      if(*n <= *e) {
	if(SPLAYTREE_RIGHT(n)) n = SPLAYTREE_RIGHT(n);
	else {
	  SPLAYTREE_RIGHT(n) = e;
	  SPLAYTREE_UP(e) = n;
	  break;
	}
      } else {
	if(SPLAYTREE_LEFT(n)) n = SPLAYTREE_LEFT(n);
	else {
	  if(min_node == n) min_node = e;
	  SPLAYTREE_LEFT(n) = e; 
	  SPLAYTREE_UP(e) = n;
	  break;
	}
      }
    }
    splay(e);
    root = e;
  } else {
    root = min_node = e;
    SPLAYTREE_UP(e) = 0;
  }

  num_items++;
  e->eventlist = this;
}

template<typename T>
void SplayTree<T>::cancel(SimEvent<T>* evt)
{
  //SplayTreeNode<T>* r = dynamic_cast<SplayTreeNode<T>*>(evt); (time consuming)
  SplayTreeNode<T>* r = (SplayTreeNode<T>*)evt;
  if(!r) SSF_THROW("attempt to cancel a null event");
  if(r->eventlist != this) SSF_THROW("attempt to cancel an event not enlisted");
  assert(num_items > 0);

  if(r == min_node) remove_min();
  else {
    SplayTreeNode<T>* n; SplayTreeNode<T>* p;
    if((n = SPLAYTREE_LEFT(r))) {
      SplayTreeNode<T>* t;
      if((t = SPLAYTREE_RIGHT(r))) {
	SPLAYTREE_UP(n) = 0;
	for(; SPLAYTREE_RIGHT(n); n = SPLAYTREE_RIGHT(n));
	splay(n);
	SPLAYTREE_RIGHT(n) = t;
	SPLAYTREE_UP(t) = n;
      }
      SPLAYTREE_UP(n) = SPLAYTREE_UP(r);
    } else if((n = SPLAYTREE_RIGHT(r))) {
      SPLAYTREE_UP(n) = SPLAYTREE_UP(r);
    }
    if((p = SPLAYTREE_UP(r))) {
      if(r == SPLAYTREE_LEFT(p)) SPLAYTREE_LEFT(p) = n;
      else SPLAYTREE_RIGHT(p) = n;
      if(n) {
	splay(p);
	root = p;
      }
    } else root = n;
  }

  num_items--;
  r->eventlist = 0;
  delete r;
}

template<typename T>
void SplayTree<T>::adjust(SimEvent<T>* evt)
{
  //SplayTreeNode<T>* r = dynamic_cast<SplayTreeNode<T>*>(evt); (time consuming)
  SplayTreeNode<T>* r = (SplayTreeNode<T>*)evt;
  if(!r) SSF_THROW("attempt to adjust a null event");
  if(r->eventlist != this) SSF_THROW("attempt to adjust an event not enlisted");
  assert(num_items > 0);

  // simply cancel the event ...
  if(r == min_node) remove_min();
  else {
    SplayTreeNode<T>* n; SplayTreeNode<T>* p;
    if((n = SPLAYTREE_LEFT(r))) {
      SplayTreeNode<T>* t;
      if((t = SPLAYTREE_RIGHT(r))) {
	SPLAYTREE_UP(n) = 0;
	for(; SPLAYTREE_RIGHT(n); n = SPLAYTREE_RIGHT(n));
	splay(n);
	SPLAYTREE_RIGHT(n) = t;
	SPLAYTREE_UP(t) = n;
      }
      SPLAYTREE_UP(n) = SPLAYTREE_UP(r);
    } else if((n = SPLAYTREE_RIGHT(r))) {
      SPLAYTREE_UP(n) = SPLAYTREE_UP(r);
    }
    if((p = SPLAYTREE_UP(r))) {
      if(r == SPLAYTREE_LEFT(p)) SPLAYTREE_LEFT(p) = n;
      else SPLAYTREE_RIGHT(p) = n;
      if(n) {
	splay(p);
	root = p;
      }
    } else root = n;
  }

  // and then re-insert the event
  SplayTreeNode<T>* n = root;
  SPLAYTREE_RIGHT(r) = SPLAYTREE_LEFT(r) = 0;
  if(n) {
    for(;;) {
      if(*n <= *r) {
	if(SPLAYTREE_RIGHT(n)) n = SPLAYTREE_RIGHT(n);
	else {
	  SPLAYTREE_RIGHT(n) = r;
	  SPLAYTREE_UP(r) = n;
	  break;
	}
      } else {
	if(SPLAYTREE_LEFT(n)) n = SPLAYTREE_LEFT(n);
	else {
	  if(min_node == n) min_node = r;
	  SPLAYTREE_LEFT(n) = r; 
	  SPLAYTREE_UP(r) = n;
	  break;
	}
      }
    }
    splay(r);
    root = r;
  } else {
    root = min_node = r;
    SPLAYTREE_UP(r) = 0;
  }
}

template<typename T>
void SplayTree<T>::clear()
{
  if(root) { 
    reclaim_tree(root);
    num_items = 0;
    root = min_node = 0;
  }
}

template<typename T>
SplayTreeNode<T>* SplayTree<T>::remove_min()
{
  SplayTreeNode<T> *r = min_node;
  SplayTreeNode<T> *t, *p;
  if((p = SPLAYTREE_UP(min_node))) {
    if((t = SPLAYTREE_RIGHT(min_node))) {
      SPLAYTREE_LEFT(p) = t;
      SPLAYTREE_UP(t) = p;
      for(; SPLAYTREE_LEFT(t); t = SPLAYTREE_LEFT(t));
      min_node = t;
    } else {
      min_node = SPLAYTREE_UP(min_node);
      SPLAYTREE_LEFT(min_node) = 0;
    }
  } else {
    if((root = SPLAYTREE_RIGHT(min_node))) {
      SPLAYTREE_UP(root) = 0;
      for(t = root; SPLAYTREE_LEFT(t); t = SPLAYTREE_LEFT(t));
      min_node = t;
    } else min_node = 0;
  }
  return r;
}

template<typename T>
void SplayTree<T>::reclaim_tree(SplayTreeNode<T>* n)
{
  SplayTreeNode<T> *l = SPLAYTREE_LEFT(n), *r = SPLAYTREE_RIGHT(n);
  n->eventlist = 0; delete n;
  if(l) reclaim_tree(l);
  if(r) reclaim_tree(r);
}

template<typename T>
void SplayTree<T>::splay(SplayTreeNode<T>* n)
{
  SplayTreeNode<T> *g, *p, *x, *z;
  for(;(p = SPLAYTREE_UP(n));) {
    if(n == SPLAYTREE_LEFT(p)) {
      if(!((g = SPLAYTREE_UPUP(n)))) {
	SPLAYTREE_RROTATE(n,p,g);
      } else if(p == SPLAYTREE_LEFT(g)) {
	if((z = SPLAYTREE_UP(g))) {
	  if(g == SPLAYTREE_LEFT(z))
	    SPLAYTREE_LEFT(z) = n;
	  else
	    SPLAYTREE_RIGHT(z) = n;
	}
	SPLAYTREE_UP(n) = z;
	if((x = SPLAYTREE_LEFT(p) = SPLAYTREE_RIGHT(n)))
	  SPLAYTREE_UP(x) = p;
	SPLAYTREE_RIGHT(n) = p;
	SPLAYTREE_UP(p) = n;
	if((x = SPLAYTREE_LEFT(g) = SPLAYTREE_RIGHT(p)))
	  SPLAYTREE_UP(x) = g;
	SPLAYTREE_RIGHT(p) = g;
	SPLAYTREE_UP(g) = p;
      } else {
	if((z = SPLAYTREE_UP(g))) {
	  if(g == SPLAYTREE_LEFT(z))
	    SPLAYTREE_LEFT(z) = n;
	  else
	    SPLAYTREE_RIGHT(z) = n;
	}
	SPLAYTREE_UP(n) = z;
	if((x = SPLAYTREE_LEFT(p) = SPLAYTREE_RIGHT(n)))
	  SPLAYTREE_RIGHT(n) = SPLAYTREE_UP(x) = p;
	else
	  SPLAYTREE_RIGHT(n) = p;
	if((x = SPLAYTREE_RIGHT(g) = SPLAYTREE_LEFT(n)))
	  SPLAYTREE_LEFT(n) = SPLAYTREE_UP(x) = g;
	else
	  SPLAYTREE_LEFT(n) = g;
	SPLAYTREE_UP(g) = SPLAYTREE_UP(p) = n;
      }
    } else {
      if(!((g = SPLAYTREE_UPUP(n)))) {
	SPLAYTREE_LROTATE(n,p,g);
      } else if(p == SPLAYTREE_RIGHT(g)) {
	if((z = SPLAYTREE_UP(g))) {
	  if(g == SPLAYTREE_RIGHT(z))
	    SPLAYTREE_RIGHT(z) = n;
	  else
	    SPLAYTREE_LEFT(z) = n;
	}
	SPLAYTREE_UP(n) = z;
	if((x = SPLAYTREE_RIGHT(p) = SPLAYTREE_LEFT(n)))
	  SPLAYTREE_UP(x) = p;
	SPLAYTREE_LEFT(n) = p;
	SPLAYTREE_UP(p) = n;
	if((x = SPLAYTREE_RIGHT(g) = SPLAYTREE_LEFT(p)))
	  SPLAYTREE_UP(x) = g;
	SPLAYTREE_LEFT(p) = g;
	SPLAYTREE_UP(g) = p;
      } else {
	if((z = SPLAYTREE_UP(g))) {
	  if(g == SPLAYTREE_RIGHT(z))
	    SPLAYTREE_RIGHT(z) = n;
	  else
	    SPLAYTREE_LEFT(z) = n;
	}
	SPLAYTREE_UP(n) = z;
	if((x = SPLAYTREE_RIGHT(p) = SPLAYTREE_LEFT(n)))
	  SPLAYTREE_LEFT(n) = SPLAYTREE_UP(x) = p;
	else
	  SPLAYTREE_LEFT(n) = p;
	if((x = SPLAYTREE_LEFT(g) = SPLAYTREE_RIGHT(n)))
	  SPLAYTREE_RIGHT(n) = SPLAYTREE_UP(x) = g;
	else
	  SPLAYTREE_RIGHT(n) = g;
	SPLAYTREE_UP(g) = SPLAYTREE_UP(p) = n;
      }
    }
  }
}

}; /*namespace minissf*/

#endif /*__MINISSF_SPLAYTREE_H__*/

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
