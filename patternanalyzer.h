#ifndef __patternanalyzer_h__
#define __patternanalyzer_h__

#include <string>
#include <vector>
#include <iostream>
#include <sstream>

#include <assert.h>

using namespace std;

namespace MultiLevel {

    class PatternAbstract {
        /*
        virtual string show() = 0;
        virtual bool follows( PatternAbstract &other ) = 0;
        virtual string serialize() = 0;
        virtual string deSerialize() = 0;
        */
    };

    // It is a tree structure used to describe patterns
    class DeltaNode: public PatternAbstract {
        public:
            DeltaNode()
                :cnt(1)
            {}

            int cnt; // how many times the node repeats
            vector<DeltaNode *> children;
            vector<off_t> elements;       // one of sizes of 
                                          // children or elements must be 0
            
            
            bool isLeaf() const;
            string show() const;
            void freeChildren();
            void pushElement( off_t elem );
            bool isPopSafe( int limit );
            void popDeltas( int n );
            void popChild();
            void pushChild( DeltaNode *newchild );
            void push( DeltaNode *newchild );
            void push( off_t newelem );
            int getNumOfDeltas() const;
            void assign( vector<DeltaNode *>::const_iterator first,
                         vector<DeltaNode *>::const_iterator last );
            void assign( vector<off_t>::const_iterator first,
                         vector<off_t>::const_iterator last );
    };


    ///////////////////////////////////////////////////////////
    // Pattern Unit
    ///////////////////////////////////////////////////////////
    class PatternUnitAbstract: public PatternAbstract
    {
    };


    class PatternUnit: public PatternAbstract
    {
        public:
            PatternUnit()
                :cnt(0)
            {}
            vector<off_t> seq;
            int cnt;
            string show();
    };

    class PatternHeadUnit: public PatternUnit
    {
        public:
            PatternHeadUnit ()
                :init(0)
            {}
            PatternHeadUnit (off_t x);
            off_t init;
            string show();
    };

    class PatternInitUnit: public PatternUnitAbstract
    {
        public:
            off_t init;
    };

    ///////////////////////////////////////////////////////////

    class PatternBlock: public PatternAbstract 
    {
        public:
            vector<PatternHeadUnit> block;
            string show();
    };

    // A pattern chunk is a pattern with multi levels
    class PatternChunk: public PatternAbstract
    {
        public:
            vector<PatternBlock> chunk;
    };

    class PatternChunkList: public PatternAbstract
    {
        public:
            vector<PatternChunk> list; 
    };
    
    class PatternEntry: public PatternAbstract
    {
        public:
            pid_t original_chunk_id;
            pid_t new_chunk_id;
            PatternChunk logical_offset;
            PatternChunk length;
            PatternChunk physical_offset;
    };

    class PatternEntryList: public PatternAbstract
    {
        public:
            vector<PatternEntry> list;
    };

    class PatternAnalyzer 
    {
        public:
            PatternChunk generatePatterns( PatternBlock &pblock );
    };

    template <class TYPE>  // TYPE could be off_t or DeltaNode *
    class Tuple {
        public:
            int offset; //note that this is not the 
            // offset when accessing file. But
            // the offset in LZ77 algorithm
            int length; //concept in LZ77
            TYPE next_symbol;

            Tuple() {}
            Tuple(int o, int l, TYPE n); 

            void put(int o, int l, TYPE n);
            bool operator== (const Tuple other);
            // Tell if the repeating sequences are next to each other
            bool isRepeatingNeighbor();
            string show();
    };

    template <class TYPE>
    Tuple <TYPE> searchNeighbor( 
                    vector<TYPE> const &seq,
                    typename vector<TYPE>::const_iterator p_lookahead_win, 
                    int win_size ); 


    template <class TYPE>
    DeltaNode *findPattern( vector<TYPE> const &deltas,
                            int win_size );

    bool isEqual(DeltaNode* a, DeltaNode* b);
    bool isEqual(off_t a, off_t b);

    // Input:  vector<T> deltas
    // Output: DeltaNode describing patterns in deltas
    //
    // deltas is got by a sequence of inits. Later the output can be
    // used to combine inits with deltas
    // This function finds out the pattern by LZ77 modification
    template <class TYPE>
    DeltaNode* findPattern( vector<TYPE> const &deltas, int win_size )
    {
        typename vector<TYPE>::const_iterator lookahead_win_start; 
        DeltaNode *pattern_node = new DeltaNode;
        pattern_node->cnt = 1; //remind you that this level does not repeat

        lookahead_win_start = deltas.begin();
        
        while ( lookahead_win_start != deltas.end() ) {
            //lookahead window is not empty
            Tuple<TYPE> cur_tuple = searchNeighbor( deltas, 
                                              lookahead_win_start, 
                                              win_size );
            //cur_tuple.show();
            if ( cur_tuple.isRepeatingNeighbor() ) {
                if ( pattern_node->isPopSafe( cur_tuple.length ) ) {
                    //safe
                    pattern_node->popDeltas( cur_tuple.length );

                    typename vector<TYPE>::const_iterator first, last;
                    first = lookahead_win_start;
                    last = lookahead_win_start + cur_tuple.length;

                    DeltaNode *combo_node = new DeltaNode;  // TODO: now only work for off_t
                    combo_node->assign(first, last);
                    combo_node->cnt = 2;

                    pattern_node->pushChild( combo_node  );
                    lookahead_win_start += cur_tuple.length;
                } else {
                    //unsafe
                    assert(pattern_node->children.size() > 0); // window moved,
                                                               // so some patterns must be in children
                    DeltaNode *lastchild = pattern_node->children.back();
                    
                    // check if the last child(pattern) can be used to 
                    // represent the repeating neighbors
                    int pattern_length = 0; //how many non-expanded deltas in lastchild
                    if ( lastchild->isLeaf() ) {
                        pattern_length = lastchild->elements.size();
                    } else {
                        // lastchild is a inner node
                        vector<DeltaNode *>::const_iterator it;
                        for ( it =  lastchild->children.begin() ;
                              it != lastchild->children.end() ;
                              it++ )
                        {
                            pattern_length += (*it)->getNumOfDeltas();
                        }
                    }

                    if ( pattern_length == cur_tuple.length ) {
                        //the subdeltas in lookahead window repeats
                        //the last pattern in pattern_node
                        lastchild->cnt++;

                        lookahead_win_start += cur_tuple.length;
                    } else {
                        //cannot pop out cur_tuple.length elems without
                        //totally breaking any pattern.
                        //So we simply add one elem to the stack
                        DeltaNode *newchild = new DeltaNode;
                        newchild->push( *lookahead_win_start );
                        newchild->cnt = 1;

                        pattern_node->pushChild(newchild);
                        
                        lookahead_win_start++;
                    }
                }
            } else {
                //(0,0,x)
                DeltaNode *newchild = new DeltaNode;
                newchild->push( cur_tuple.next_symbol );
                newchild->cnt = 1;
                pattern_node->pushChild(newchild);
                
                lookahead_win_start++;
            }
        }
      
        /*
        SigStack<IdxSigUnit> pattern_node_compressed;
        vector<IdxSigUnit>::iterator it;
        for ( it = pattern_node->the_stack.begin();
              it != pattern_node->the_stack.end();
              it++ )
        {
            it->compressRepeats();
            if (pattern_node_compressed.the_stack.empty()) {
                //mlog(IDX_WARN, "Empty");
                pattern_node_compressed.the_stack.push_back(*it);
            } else {
                bool ret;
                ret = pattern_node_compressed.the_stack.back().append(*it);
                if (ret == false) {
                    pattern_node_compressed.the_stack.push_back(*it);
                }
            }
            //ostringstream oss;
            //oss << pattern_node_compressed.show();
            ////mlog(IDX_WARN, "%s", oss.str().c_str());
        }
        */
        
        return pattern_node;
    }

    ////////////////////////////////////////////////////////////////
    //  Tuple : the concept in LZ77
    ////////////////////////////////////////////////////////////////
    template <class TYPE>
    Tuple<TYPE>::Tuple(int o, int l, TYPE n) {
        offset = o;
        length = l;
        next_symbol = n;
    }
    
    template <class TYPE>
    void Tuple<TYPE>::put(int o, int l, TYPE n) {
        offset = o;
        length = l;
        next_symbol = n;
    }

    template <class TYPE>
    bool Tuple<TYPE>::operator== (const Tuple other) {
        if (offset == other.offset 
                && length == other.length
                && isEqual( next_symbol == other.next_symbol ) )
        {
            return true;
        } else {
            return false;
        }
    }

    template <class TYPE>
    bool Tuple<TYPE>::isRepeatingNeighbor() {
        return (offset == length && offset > 0);
    }
    
    template <class TYPE>
    string Tuple<TYPE>::show() {
        ostringstream showstr;
        showstr << "(" << offset 
            << ", " << length
            << ", " << next_symbol << ")" << endl;
        return showstr.str();
    }


    template <class TYPE>
    Tuple <TYPE> searchNeighbor( 
                    vector<TYPE> const &seq,
                    typename vector<TYPE>::const_iterator p_lookahead_win, 
                    int win_size ) 
    {
        typename vector<TYPE>::const_iterator i;     
        int j;

        //i goes left util the begin or reaching window size
        i = p_lookahead_win;
        int remain = seq.end() - p_lookahead_win;
        while ( i != seq.begin() 
                && (p_lookahead_win - i) < win_size
                && (p_lookahead_win - i) < remain ) {
            i--;
        }

        //i points to a element in search buffer where matching starts
        //j is the iterator from the start to the end of search buffer to compare
        for ( ; i != p_lookahead_win ; i++ ) {
            int search_length = p_lookahead_win - i;
            for ( j = 0 ; j < search_length ; j++ ) {
                if ( !isEqual( *(i+j), *(p_lookahead_win + j)) ) { //TODO: need to impliment comparison
                    break;
                }
            }
            if ( j == search_length ) {
                //found a repeating neighbor
                return Tuple<TYPE>(search_length, search_length, 
                        *(p_lookahead_win + search_length));
            }
        }

        //Cannot find a repeating neighbor
        return Tuple<TYPE>(0, 0, *(p_lookahead_win));
    }


}

#endif
