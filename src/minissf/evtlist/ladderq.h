#ifndef _LADDERQ_H
#define _LADDERQ_H

#define LADDERQ_MAX_RUNGS 8
#define LADDERQ_DEFAULT_THRESH 50
#define LADDERQ_POSITION_TOP 1
#define LADDERQ_POSITION_RUNG 2
#define LADDERQ_POSITION_BOTTOM 3

#define UNSORTED_LINKEDLIST_ADD(a, b) \
    if(a) { a->prev = b; b->next = a;} \
    a = b;

#ifndef __MINISSF_SIMEVENT_H__
#error "DO NOT INCLUDE THIS CLASS DIRECTLY"
#endif

namespace minissf
{

    template<typename T> class LadderQueueRung;
    template<typename T> class LadderQueueNode;

    template<typename T>
    //sort event into linked list
    void sorted_add_event(LadderQueueNode<T>* &a, LadderQueueNode<T>* b);

    template<typename T>
    class LadderQueue : public Eventlist<T>
    {
    public:
        LadderQueue();
        virtual ~LadderQueue();

        virtual int size() const 
        { 
            return n_events; 
        }

        virtual SimEvent<T>* getMin() const;
        virtual SimEvent<T>* deleteMin();
        virtual void insert(SimEvent<T>* evt);
        virtual void adjust(SimEvent<T>* evt);
        virtual void cancel(SimEvent<T>* evt);
        virtual void clear();

    protected:
        void cancel_nodelete(LadderQueueNode<T>* le);
        void add_rung(int event_count);
        //used to find(or create, if necesarry) the next bucket that of size less than or equal to the threshold
        int recurse_rungs();
        //updates the min and max timestamps of events in top on this epoch
        void min_max_top(LadderQueueNode<T>* e);

        //various counters
        int n_events;
        int n_top;
        int active_rungs;
        int n_bot;

        //top timestamps
        T top_max_ts;
        T top_min_ts;
        T top_thresh_ts;

        //minimum timestamp of events
        T abs_min_ts;

        //pointers to top, the rungs, and bottom
        LadderQueueNode<T>* top;
        LadderQueueRung<T>** rungs;
        LadderQueueNode<T>* bottom;

        //threshhold of bucket size
        int event_thresh;


    private:
        
    }; /* class LadderQueue */

    //helper rung class
    template<typename T>
    class LadderQueueRung
    {
        friend class LadderQueue<T>;
    public:
        LadderQueueRung();
        ~LadderQueueRung();

    protected:
        //functions for adding and removing events/event lists
        void add_event(LadderQueueNode<T>* le);
        //bucket sort linked list into rung
        void add_event_list(LadderQueueNode<T>* & first);//reference to pointer is capable of changing input pointer
        void reset_buckets(T bucket_w, int bucket_c);
        void clear();

        //function used to find position of an event in this rung
        int event_bucket(LadderQueueNode<T>* le);

        //internal position tracking variable
        int rung_id;

        //actual buckets
        LadderQueueNode<T>** buckets;

        //event counts of each bucket and entire rung
        int* event_counts;
        int total_count;

        //rung variables
        T bucket_width;
        int bucket_count;
        int cur_bucket;
        T r_start;
        T r_cur;

    private:

    }; /* class LadderQueueRung */

     //event class
    template<typename T>
    class LadderQueueNode : public SimEvent<T>
    {
        friend class LadderQueue<T>;
        friend class LadderQueueRung<T>;
        friend void sorted_add_event<T>(LadderQueueNode<T>* &a, LadderQueueNode<T>* b);
    public:
        LadderQueueNode():
        SimEvent<T>(0),
        prev(0),
        next(0),
        position(0),
        rung(0),
        bucket(0)
        {
        }
    
        LadderQueueNode(T ts):
        SimEvent<T>(ts),
        prev(0),
        next(0),
        position(0),
        rung(0),
        bucket(0)
        {
        }

        virtual ~LadderQueueNode()
        {
        }

    protected:
        //linked list variables
        LadderQueueNode<T>* prev;
        LadderQueueNode<T>* next;

        //positioning variable
        int position;
        int rung;
        int bucket;

    private:

    }; /* class LadderQueueEvent */
    
    template<typename T>  
    LadderQueue<T>::LadderQueue():
    n_events(0),
    n_top(0),
    active_rungs(0),
    n_bot(0),
    top_max_ts(0),
    top_min_ts(0),
    top_thresh_ts(0),
    top(0),
    rungs(0),
    bottom(0),
    event_thresh(LADDERQ_DEFAULT_THRESH)
    {
        this->rungs = new LadderQueueRung<T>*[LADDERQ_MAX_RUNGS];
        if(!this->rungs)
	  SSF_THROW("failed to allocate memmory for rungs");

        int x = 0;
        while(x < LADDERQ_MAX_RUNGS)
        {
            this->rungs[x] = new LadderQueueRung<T>();
            this->rungs[x]->rung_id = x;
            if(!this->rungs[x])
	      SSF_THROW("failed to allocate memmory for a rung");
            ++x;
        }
    }

    template<typename T>
    LadderQueue<T>::~LadderQueue()
    {
        this->clear();

        if(this->rungs)
        {
            int x = 0;
            while(x < LADDERQ_MAX_RUNGS)
            {
                if(this->rungs[x])
                {
                    delete this->rungs[x];
                    this->rungs[x] = 0;
                }
                ++x;
            }

            delete[] this->rungs;
            this->rungs = 0;
        }
    }

    template<typename T>
    //this won't necesarily run in constant time, only use if absolutely necesarry
    SimEvent<T>* LadderQueue<T>::getMin() const
    {
        if(this->bottom)
            return this->bottom;

        LadderQueueNode<T>* checker = 0;
        if(this->active_rungs > 0)
        {

            
            LadderQueueRung<T>* low_rung = this->rungs[this->active_rungs - 1];
            //find lowest nonempty bucket;
            int x = 0;
            while(low_rung->event_counts[low_rung->cur_bucket] == 0)
            {
                ++x;
                if(x >= low_rung->bucket_count)
                {
		  SSF_THROW("active rung low rung is empty");
                }
            }
            
            checker = low_rung->buckets[x];   
        }
        else
        {
            checker = this->top;
        }
        
        
        //because this function is const, constant time is not guaranteed as a bucket or top may be overfilled
        LadderQueueNode<T>* min = checker;
        while(checker)
        {
            if(checker < min)
                min = checker;

            checker = checker->next;
        }
        return min;
    }

    template<typename T>
    SimEvent<T>* LadderQueue<T>::deleteMin()
    {
        if(this->n_events <= 0)
            return 0;

        LadderQueueNode<T>* oldMin = 0;

        //move events around until bottom is not empty
        while(!this->bottom)
        {
            if(this->active_rungs > 0)
            {
                int b = this->recurse_rungs();

                //sort bucket b into bottom
                LadderQueueNode<T>* move = 0;
                while(this->rungs[this->active_rungs - 1]->buckets[b])
                {
                    move = this->rungs[this->active_rungs - 1]->buckets[b];

                    this->rungs[this->active_rungs - 1]->buckets[b] = move->next;
                    move->next = 0;

                    if(this->rungs[this->active_rungs - 1]->buckets[b])
                        this->rungs[this->active_rungs - 1]->buckets[b]->prev = 0;
    
                    sorted_add_event(this->bottom, move);
                }

                //update counts
                this->n_bot = this->rungs[this->active_rungs - 1]->event_counts[b];
                this->rungs[this->active_rungs - 1]->total_count -= this->rungs[this->active_rungs - 1]->event_counts[b];
                this->rungs[this->active_rungs - 1]->event_counts[b] = 0;

                //set new positions
                LadderQueueNode<T>* psetter = this->bottom;
                while(psetter)
                {
                    psetter->position = LADDERQ_POSITION_BOTTOM;
                    psetter = psetter->next;
                }

                //invalidate rung if it is empty, otherwise prep for next opperation
                while(this->active_rungs > 0 &&
                        this->rungs[this->active_rungs - 1]->total_count <= 0)
                {
                    --this->active_rungs;
                }

                if(this->active_rungs > 0)
                    this->recurse_rungs();
            }
            else //no rungs or bottom, create a new rung(or possibly bottom), also known as a new epoch
            {
                //all TSes are the same, no sorts required
                if(this->top_max_ts - this->top_min_ts == 0)
                {
                    this->bottom = this->top;
                    this->n_bot = this->n_top;
                    this->n_top = 0;
            
                    //update positions
                    LadderQueueNode<T>* psetter = this->bottom;
                    while(psetter)
                    {
                        psetter->position = LADDERQ_POSITION_BOTTOM;
                        psetter = psetter->next;
                    }
                }
                else
                {
                    //create a new rung, bucket sort top into it
                    this->rungs[0]->reset_buckets((this->top_max_ts - this->top_min_ts)/static_cast<T>(this->n_top), this->n_top + 1);
                    this->rungs[0]->r_start = this->rungs[0]->r_cur = this->top_min_ts;
                    this->top_thresh_ts = this->top_max_ts;
                    this->active_rungs = 1;

                    this->rungs[0]->add_event_list(this->top);

                    this->n_top = 0;
                    this->top_max_ts = this->top_min_ts = 0;
                }
            }
        }


        //once a bottom list exists, dequeue first event in it
        oldMin = this->bottom;
        this->bottom = this->bottom->next;
        if(this->bottom)
            this->bottom->prev = 0;
        --this->n_bot;
        --this->n_events;

        oldMin->eventlist = 0;
        oldMin->position = 0;
        oldMin->rung = 0;
        oldMin->bucket = 0;
        oldMin->next = 0;
        return oldMin;
    }

    template<typename T>
    void LadderQueue<T>::insert(SimEvent<T>* evt)
    {
        LadderQueueNode<T>* le = dynamic_cast<LadderQueueNode<T>*>(evt);
        if(!le)
	  SSF_THROW("attempted to insert null event");

        if(le->eventlist)
	  SSF_THROW("attempt to insert an already scheduled event in an event list");

        //if event is far off, simply add to top
        if(le->time() >= this->top_thresh_ts)
        {
            UNSORTED_LINKEDLIST_ADD(this->top, le)
            this->min_max_top(le);
            le->position = LADDERQ_POSITION_TOP;
            ++this->n_top;
        }
        else
        {
            //check if event should be in rungs or bottom
            int x = 0;
            T ts = le->time(); 
            while((ts < this->rungs[x]->r_cur) &&
                  (x <= this->active_rungs - 1))
                ++x;

            if(x <= this->active_rungs - 1)
            {
                //add to correct rung if it exists
                le->position = LADDERQ_POSITION_RUNG;
                this->rungs[x]->add_event(le);
            }
            else
            {
                if(this->n_bot + 1 <= this->event_thresh)
                {

                    //sort new event into bottom
                    le->position = LADDERQ_POSITION_BOTTOM;
                    sorted_add_event(this->bottom, le);
                    ++n_bot;
                }
                else
                {   
                    //rewind current rung to it's start and resort bottom into it
                    le->position = LADDERQ_POSITION_RUNG;
                    this->rungs[this->active_rungs - 1]->r_cur = this->rungs[this->active_rungs - 1]->r_start;
                    UNSORTED_LINKEDLIST_ADD(this->bottom, le)
                    this->rungs[this->active_rungs - 1]->add_event_list(this->bottom);
                    this->n_bot = 0;
               }
            }
        }
        
        le->eventlist = this;
        ++this->n_events;
    }

    template<typename T>
    void LadderQueue<T>::adjust(SimEvent<T>* evt)
    {
        LadderQueueNode<T>* le = dynamic_cast<LadderQueueNode<T>*>(evt);

        if(le->eventlist != this)
	  SSF_THROW("tried to adjust event not in eventlist");

        //cancel event and reinsert it
        this->cancel_nodelete(le);
        this->insert(le);

    }

    template<typename T>
    void LadderQueue<T>::cancel(SimEvent<T>* evt)
    {
        LadderQueueNode<T>* le = dynamic_cast<LadderQueueNode<T>*>(evt);
        this->cancel_nodelete(le);
        delete le;

    }

    template<typename T>
    void LadderQueue<T>::clear()
    {

        //clear top
        while(this->top)
        { 
            LadderQueueNode<T>* del = this->top;
            if(this->top)
                this->top = this->top->next;
            if(del)
            {
                del->eventlist = 0;
                delete del;
            }
        }
        this->n_top = 0;
        
        //clear ladder
        int x = 0;
        while(x < LADDERQ_MAX_RUNGS)
        {
            this->rungs[x]->clear();
            ++x;
        }

        //clear bottom
        while(this->bottom)
        { 
            LadderQueueNode<T>* del = this->bottom;
            if(this->bottom)
                this->bottom = this->bottom->next;
            if(del)
            {
                del->eventlist = 0;
                delete del;
            }
        }

        this->n_bot = 0;
        this->n_events = 0;
    }

    template<typename T>
    void LadderQueue<T>::cancel_nodelete(LadderQueueNode<T>* le)
    {
        if(le->eventlist != this)
	  SSF_THROW("tried to cancel event not in eventlist");

        //update count based on position and remove event from its coresponding list
        if(le->position == LADDERQ_POSITION_TOP)
        {
            --this->n_top;
            if(le->prev)
                le->prev->next = le->next;
            else
                this->top = le->next;
        }
        else if(le->position == LADDERQ_POSITION_RUNG)
        {
            --this->rungs[le->rung]->total_count;
            --this->rungs[le->rung]->event_counts[le->bucket];
            if(le->prev)
                le->prev->next = le->next;
            else
                this->rungs[le->rung]->buckets[le->bucket] = le->next;
        }
        else if(le->position == LADDERQ_POSITION_BOTTOM)
        {
            --this->n_bot;
            if(le->prev)
                le->prev->next = le->next;
            else
                this->bottom = le->next;
        }
        else
	  SSF_THROW("tried to cancel event with invalid position");

        //update counts and clear nodes position
        --this->n_events;
        le->eventlist = 0;
        le->position = 0;
        le->rung = 0;
        le->bucket = 0;
    }
   
    template<typename T>
    void LadderQueue<T>::add_rung(int event_count)
    {
        if(LADDERQ_MAX_RUNGS < this->active_rungs + 1)
        {
            //rungs are maxed, throw
            //if this becomes a problem, increase threshold
	  SSF_THROW("rung overflow");
        }

        //set bucket width to previous rung's bucket width / thresh (aiming for one event per bucket)
        this->rungs[this->active_rungs]->reset_buckets(this->rungs[this->active_rungs - 1]->bucket_width / event_count, event_count + 1);

        //set start and cur of the new rung to cur marking the current bucket
        this->rungs[this->active_rungs]->r_start = this->rungs[this->active_rungs]->r_cur = this->rungs[this->active_rungs - 1]->r_cur;

        ++this->active_rungs;        
    }

    template<typename T>
    int LadderQueue<T>::recurse_rungs()
    {
        LadderQueueRung<T>* low_rung = this->rungs[this->active_rungs - 1];
        //continue until an acceptable bucket is found
        while(low_rung->event_counts[low_rung->cur_bucket] == 0 ||
                low_rung->event_counts[low_rung->cur_bucket] > this->event_thresh)
        {
            //find next nonempty bucket
            while(low_rung->event_counts[low_rung->cur_bucket] == 0)
            {
                ++low_rung->cur_bucket;
                low_rung->r_cur += low_rung->bucket_width;
                if(low_rung->cur_bucket >= low_rung->bucket_count)
                {
                    --this->active_rungs;
                    if(this->active_rungs <= 0)
		      SSF_THROW("recurse_rungs() called on empty rung");
                    
                    low_rung = this->rungs[this->active_rungs - 1];
                    continue;
                }   
            }
            //if bucket is too big, create a new rung for it and sort the bucket into the rung
            if(low_rung->event_counts[low_rung->cur_bucket] > this->event_thresh)
            {
                this->add_rung(low_rung->event_counts[low_rung->cur_bucket]);
                this->rungs[this->active_rungs - 1]->add_event_list(low_rung->buckets[low_rung->cur_bucket]);

                low_rung->buckets[low_rung->cur_bucket] = 0;
                                
                low_rung->total_count -= low_rung->event_counts[low_rung->cur_bucket];
                low_rung->event_counts[low_rung->cur_bucket] = 0;

                low_rung = this->rungs[this->active_rungs - 1];
            }
        }

        return low_rung->cur_bucket;
    }
  
    template<typename T>  
    void LadderQueue<T>::min_max_top(LadderQueueNode<T>* e)
    {
        if(e->time() > this->top_max_ts)
            this->top_max_ts = e->time();

        if(e->time() < this->top_min_ts)
            this->top_min_ts = e->time();
    }

    template<typename T>
    LadderQueueRung<T>::LadderQueueRung():
    buckets(0),
    event_counts(0),
    total_count(0),
    bucket_width(0),
    bucket_count(0),
    cur_bucket(0),
    r_start(0),
    r_cur(0)
    {
    }

    template<typename T>
    LadderQueueRung<T>::~LadderQueueRung()
    {
        this->clear();
        if(this->buckets)
        {
            delete[] this->buckets;
            this->buckets = 0;
        }
        if(this->event_counts)
        {
            delete[] this->event_counts;
            this->event_counts = 0;
        }
    }

    template<typename T>
    void LadderQueueRung<T>::add_event(LadderQueueNode<T>* le)
    {
        int b = this->event_bucket(le);

        //update positon
        le->rung = this->rung_id;
        le->bucket = b;

        UNSORTED_LINKEDLIST_ADD(this->buckets[b], le)

        ++this->event_counts[b];
        ++this->total_count;
    }

    template<typename T>
    void LadderQueueRung<T>::add_event_list(LadderQueueNode<T>* &first)
    {
        LadderQueueNode<T>* adder = first;
        while(adder)
        {
            first = adder->next;

            if(first)
                first->prev = 0;

            adder->next = 0;
            
            this->add_event(adder);
            adder = first;
        }
    }

    template<typename T>
    void LadderQueueRung<T>::reset_buckets(T bucket_w, int bucket_c)
    {
        if(this->total_count)
	  SSF_THROW("tried to reset a rung with active events");

        if(this->buckets)
        {
            delete[] this->buckets;
            this->buckets = 0;
        }
        if(this->event_counts)
        {
            delete[] this->event_counts;
            this->event_counts = 0;
        }

        this->bucket_width = bucket_w;
        this->bucket_count = bucket_c;
        this->buckets = new LadderQueueNode<T>*[this->bucket_count]();
        this->event_counts = new int[this->bucket_count]();
        this->total_count = 0;
        cur_bucket = 0;
        if(!(this->buckets && this->event_counts))
	  SSF_THROW("failed to allocate memmory for buckets");
        
    }

    template<typename T>
    void LadderQueueRung<T>::clear()
    {
        int x = 0;
        while(x < this->bucket_count)
        {
            while(this->buckets[x])
            {
                LadderQueueNode<T>* del = this->buckets[x];

                if(this->buckets[x])
                    this->buckets[x] = this->buckets[x]->next;

                del->eventlist = 0;
                del->position = 0;
                del->rung = 0;
                del->bucket = 0;
                delete del;
            }

            this->event_counts[x] = 0;
            ++x;
        }

        this->total_count = 0;
    }

    template<typename T>
    int LadderQueueRung<T>::event_bucket(LadderQueueNode<T>* e)
    {
        int b = (e->time() - this->r_start) / this->bucket_width;
        if(b < 0 ||
            b > this->bucket_count - 1)
	  SSF_THROW("event bucket out of range");

        return b;
    }

    template<typename T>   
    void sorted_add_event(LadderQueueNode<T>* &a, LadderQueueNode<T>* b)
    {
        if(a)
        {
            LadderQueueNode<T>* place = a;

            while(place->time() < b->time())
            {
                if(!place->next) 
                    break;

                place = place->next;
            }

            if(place->time() < b->time())
            {
                place->next = b;
                b->prev = place;
                return;
            }
            

            if(!place->prev)
                a = b;
            else
            {
                b->prev = place->prev;
                place->prev->next = b;
            }

            place->prev = b;
            b->next = place;
        }
        else
        {
            a = b;
        }
    }

}

#endif
