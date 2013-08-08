#ifndef __MINISSF_BINHEAP_H__
#define __MINISSF_BINHEAP_H__

#ifndef __MINISSF_SIMEVENT_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

#include <assert.h>
#include <string.h>

namespace minissf {

template<typename T> class BinaryHeap;
template<typename T> class BinaryHeapNode;

// here's the eventlist implementation using binary heap
template<typename T>
class BinaryHeap : public Eventlist<T> {
 public:
  // the constructor
  BinaryHeap();

  // the destructor
  virtual ~BinaryHeap();

  // these are methods defined in the eventlist class
  virtual int size() const { return num_items; }
  virtual SimEvent<T>* getMin() const;
  virtual SimEvent<T>* deleteMin();
  virtual void insert(SimEvent<T>* evt);
  virtual void cancel(SimEvent<T>* evt);
  virtual void adjust(SimEvent<T>* evt);
  virtual void clear();

 protected:
  int capacity; // size of the array used to contain the events
  int num_items; // number of bins being used at the moment
  BinaryHeapNode<T>** heap; // the array itself

 protected:
  // remove the event located at the given index
  BinaryHeapNode<T>* remove_node(int index);

  // adjust the position of the node at given index upward (toward the root)
  void adjust_upheap(int index);

  // adjust the position of the node at given index downward (toward the leaves)
  void adjust_downheap(int index);
}; /*class BinaryHeap*/

// here's the event type if using binary heap as eventlist
template<typename T>
class BinaryHeapNode : public SimEvent<T> {
 public:
   BinaryHeapNode(T ts) : SimEvent<T>(ts) {}

 protected:
  template<typename V> friend class BinaryHeap;
  int index; // index into the array to know where this node is at
}; /*class BinaryHeapNode*/

// initial array size for the binary heap
#define BINHEAP_INITIAL_CAPACITY 128

template<typename T>
BinaryHeap<T>::BinaryHeap() : capacity(BINHEAP_INITIAL_CAPACITY), num_items(0)
{
  heap = new BinaryHeapNode<T>*[capacity];
  if(!heap) SSF_THROW("unable to allocate binary heap");
}

template<typename T>
BinaryHeap<T>::~BinaryHeap()
{
  clear();
  delete[] heap;
}

template<typename T>
SimEvent<T>* BinaryHeap<T>::getMin() const
{
  if(!num_items) return 0;
  return (SimEvent<T>*)heap[1];
}

template<typename T>
SimEvent<T>* BinaryHeap<T>::deleteMin()
{
  if(!num_items) return 0;
  BinaryHeapNode<T>* e = remove_node(1);
  e->eventlist = 0;
  return (SimEvent<T>*)e;
}

template<typename T>
void BinaryHeap<T>::insert(SimEvent<T>* evt)
{
  //BinaryHeapNode<T>* e = dynamic_cast<BinaryHeapNode<T>*>(evt); (time consuming)
  BinaryHeapNode<T>* e = (BinaryHeapNode<T>*)evt;
  if(!e) SSF_THROW("attempt to insert a null event");
  if(e->eventlist) SSF_THROW("attempt to insert an already scheduled event");
  num_items++;
  if(num_items == capacity) { // expand the array if no more room left
    capacity <<= 1;
    BinaryHeapNode<T>** newheap = new BinaryHeapNode<T>*[capacity];
    memcpy(newheap, heap, num_items*sizeof(BinaryHeapNode<T>*));
    delete[] heap;
    heap = newheap;
  }
  heap[num_items] = e;
  e->index = num_items;
  adjust_upheap(num_items);
  e->eventlist = this;
}

template<typename T>
void BinaryHeap<T>::cancel(SimEvent<T>* evt)
{
  //BinaryHeapNode<T>* r = dynamic_cast<BinaryHeapNode<T>*>(evt); (time consuming)
  BinaryHeapNode<T>* r = (BinaryHeapNode<T>*)evt;
  if(!r) SSF_THROW("attempt to cancel a null event");
  if(r->eventlist != this) SSF_THROW("attempt to cancel an event not enlisted");
  assert(num_items > 0);
  assert(0 < r->index && r->index <= num_items);
  remove_node(r->index);
  r->eventlist = 0;
  delete r;
}

template<typename T>
void BinaryHeap<T>::adjust(SimEvent<T>* evt)
{
  //BinaryHeapNode<T>* r = dynamic_cast<BinaryHeapNode<T>*>(evt); (time consuming)
  BinaryHeapNode<T>* r = (BinaryHeapNode<T>*)evt;
  if(!r) SSF_THROW("attempt to adjust a null event");
  if(r->eventlist != this) SSF_THROW("attempt to adjust an event not enlisted");
  assert(num_items > 0);
  assert(0 < r->index && r->index <= num_items);

  // move it in either direction, upward or downward
  if(r->index>1 && *heap[r->index] < *heap[r->index>>1])
    adjust_upheap(r->index);
  else adjust_downheap(r->index);
}

template<typename T>
void BinaryHeap<T>::clear()
{
  for(int i=1; i<=num_items; i++) {
    heap[i]->eventlist = 0;
    delete heap[i];
  }
  num_items = 0;
}

template<typename T>
BinaryHeapNode<T>* BinaryHeap<T>::remove_node(int index)
{
  BinaryHeapNode<T>* e = heap[index];
  num_items--;

  // plug the hole with the last item, if there is one
  if(index <= num_items) {
    heap[index] = heap[num_items+1];
    heap[index]->index = index;

    // move it in either direction, upward or downward
    if(index>1 && *heap[index] < *heap[index>>1])
      adjust_upheap(index);
    else adjust_downheap(index);
  }
  return e;
}

template<typename T>
void BinaryHeap<T>::adjust_upheap(int index)
{
  int p = (index>>1);
  while(p && *heap[index] < *heap[p]) {
    BinaryHeapNode<T> *node = heap[p];
    heap[p] = heap[index];
    heap[p]->index = p;
    heap[index] = node;
    node->index = index;
    index = p;
    p = (index>>1);
  }
}

template<typename T>
void BinaryHeap<T>::adjust_downheap(int index)
{
  for(;;) {
    int left = index<<1;
    if(left>num_items) break;
    int right = left+1;
    int minside = left;
    if(right <= num_items && *heap[right] < *heap[left])
      minside = right;
    if(*heap[minside] < *heap[index]) {
      BinaryHeapNode<T> *node = heap[index];
      heap[index] = heap[minside];
      heap[index]->index = index;
      heap[minside] = node;
      node->index = minside;
      index = minside;
    } else break;
  }
}

}; /*namespace minissf*/

#endif /*__MINISSF_BINHEAP_H__*/

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
