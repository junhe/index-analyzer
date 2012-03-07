#ifndef __idxanalyzer_h__
#define __idxanalyzer_h__

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <unistd.h>
#include <stdio.h>


using namespace std;

#define off_t long long int


//used to describe a single pattern that found
//This will be saved in the stack
class PatternUnit {
    public:
        vector<off_t> seq;
        int cnt; //count of repeatition
        
        PatternUnit() {}
        PatternUnit( vector<off_t> sq, int ct )
            :seq(sq),cnt(ct)
        {}
        //return number of elements in total
        int size() 
        {
            return seq.size()*cnt;
        }
};

//This is used to describ a single repeating
//pattern, but with starting value
class IdxSigUnit: public PatternUnit {
    public:
        off_t init; // the initial value of 
                    // logical offset, length, or physical offset
};

//This is the new entry for the new index
//file using I/O signature. It corresponds to
//HostEntry in old PLFS.
//
//Damn, where can I put the time stamp :(
class IdxSigEntry {
    public:
        int proc;
        IdxSigUnit logical_offset;
        vector<IdxSigUnit> length;
        vector<IdxSigUnit> physical_offset;
};

template <class T> // T can be PatternUnit or IdxSigUnit
class PatternStack {
    public:
        PatternStack() {}
        void push( T pu ) 
        {
            the_stack.push_back(pu);
        }
        
        void clear() 
        {
            the_stack.clear();
        }

        //if popping out t elem breaks any patterns
        bool isPopSafe( int t ) 
        {
            typename vector<T>::reverse_iterator rit;
            
            int total = 0;
            rit = the_stack.rbegin();
            while ( rit != the_stack.rend()
                    && total < t )
            {
                total += rit->size();
                rit++;
            }
            return total == t;
        }

        //return false if it is not safe
        //t is number of elements, not pattern unit
        bool popElem ( int t )
        {
            if ( !isPopSafe(t) ) {
                return false;
            }

            int total = 0; // the number of elem already popped out
            while ( !the_stack.empty() && total < t ) {
                total += top().size();
                the_stack.pop_back();
            }
            assert( total == t );

            return true;
        }

        //pop out one pattern
        void popPattern () 
        {
            the_stack.pop_back();
        }
        
        //make sure the stack is not empty before using this
        T top () 
        {
            assert( the_stack.size() > 0 );
            return the_stack.back();
        }
        
        virtual void show()
        {
            typename vector<T>::const_iterator iter;
            
            for ( iter = the_stack.begin();
                    iter != the_stack.end();
                    iter++ )
            {
                vector<off_t>::const_iterator off_iter;
                for ( off_iter = (iter->seq).begin();
                        off_iter != (iter->seq).end();
                        off_iter++ )
                {
                    cout << *off_iter << ", ";
                }
                cout << "^" << iter->cnt << endl;
            }
        }
    
    protected:
        vector<T> the_stack;
};

template <class T>
class SigStack: public PatternStack <T> 
{
    virtual void show()
    {
        typename vector<T>::const_iterator iter;

        for ( iter = this->the_stack.begin();
                iter != this->the_stack.end();
                iter++ )
        {
            vector<off_t>::const_iterator off_iter;
            cout << iter->init << "- " ;
            for ( off_iter = (iter->seq).begin();
                    off_iter != (iter->seq).end();
                    off_iter++ )
            {
                cout << *off_iter << ", ";
            }
            cout << "^" << iter->cnt << endl;
        }
    }

};

class Tuple {
    public:
        int offset; //note that this is not the 
        // offset when accessing file. But
        // the offset in LZ77 algorithm
        int length; //concept in LZ77
        off_t next_symbol;

        Tuple() {}
        Tuple(int o, int l, off_t n) {
            offset = o;
            length = l;
            next_symbol = n;
        }

        void put(int o, int l, off_t n) {
            offset = o;
            length = l;
            next_symbol = n;
        }

        bool operator== (const Tuple other) {
            if (offset == other.offset 
                    && length == other.length
                    && next_symbol == other.next_symbol)
            {
                return true;
            } else {
                return false;
            }
        }

        // Tell if the repeating sequences are next to each other
        bool isRepeatingNeighbor() {
            return (offset == length && offset > 0);
        }

        void show() {
            cout << "(" << offset 
                << ", " << length
                << ", " << next_symbol << ")" << endl;
        }
};


class IdxEntry {
    public:
        int Proc;
#define ID_WRITE 0
#define ID_READ  1
        int ID; //either ID_WRITE or ID_READ
        off_t Logical_offset;
        off_t Length;
        double Begin_timestamp;
        double End_timestamp;
        off_t Logical_tail;
        int ID_2;
        off_t Physical_offset;
};


// Each index has its own signature
class IdxSignature {
    public:
        IdxSignature():win_size(4) {} 
        void discoverPattern( vector<off_t> const &seq );
        void discoverSigPattern( vector<off_t> const &seq,
                vector<off_t> const &orig );
        //It takes in a entry buffer like in PLFS,
        //analyzes it and generate Index Signature Entries
        void generateIdxSignature(vector<IdxEntry> &entry_buf, int proc);
    private:
        vector<IdxEntry> entry_buf;
        int win_size; //window size

        Tuple searchNeighbor( vector<off_t> const &seq,
                vector<off_t>::const_iterator p_lookahead_win ); 
};


#endif

