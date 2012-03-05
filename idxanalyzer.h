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


class PatternStack;
class PatternUnit;

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

class PatternStack {
    public:
        PatternStack() {}
        void push( PatternUnit pu ) 
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
            vector<PatternUnit>::reverse_iterator rit;
            
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
        PatternUnit top () 
        {
            assert( the_stack.size() > 0 );
            return the_stack.back();
        }
        
        void show()
        {
            vector<PatternUnit>::const_iterator iter;
            
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
    
    private:
        vector<PatternUnit> the_stack;
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
        off_t Chunk_offset;
};


class IdxSigUnit {
    off_t init_offset;
    vector <off_t> offset_stride;
    off_t init_length;
    vector <off_t> length_stride;
};

// Each index has its own signature
class IdxSignature {
    public:
        IdxSignature():win_size(4) {}
        bool openTraceFile( const char *fpath );
        bool bufferEntries(); // read some entries from trace file and
                              // put them in entry_buf
        void discoverPattern( vector<off_t> const &seq );

    private:
        ifstream idx_file;
        vector<IdxSigUnit> sig_list;
        vector<IdxEntry> entry_buf;
        vector<off_t> off_deltas; //offset[1]-offset[0], offset[2]-offset[1], ... get from entry_buf
        int win_size; //window size
        PatternStack pattern_stack;
        
        Tuple searchNeighbor( vector<off_t> const &seq,
                              vector<off_t>::const_iterator p_lookahead_win ); 
        bool getNextEntry(IdxEntry &idx_entry);
};


#endif

