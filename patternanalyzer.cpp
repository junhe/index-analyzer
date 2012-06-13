
#include "patternanalyzer.h"

#include <sstream>
#include <assert.h>

namespace MultiLevel {
    ////////////////////////////////////////////////////////////////
    //  MISC
    ////////////////////////////////////////////////////////////////
    PatternBlock buildDeltaBlock( PatternBlock &pblock )
    {
        vector<PatternHeadUnit>::iterator it;
        PatternBlock deltas;
        for ( it = pblock.block.begin();
                it != pblock.block.end();
                it++ ) 
        {
            if ( it != pblock.block.begin() ) {
                PatternHeadUnit phu;
                phu.init = it->init - (it-1)->init;
                deltas.block.push_back( phu );
            }
        }
        return deltas;
    }

    Tuple searchNeighbor( vector<off_t> const &seq,
            vector<off_t>::const_iterator p_lookahead_win, int win_size ) 
    {
        vector<off_t>::const_iterator i;     
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
                if ( *(i+j) != *(p_lookahead_win + j) ) {
                    break;
                }
            }
            if ( j == search_length ) {
                //found a repeating neighbor
                return Tuple(search_length, search_length, 
                        *(p_lookahead_win + search_length));
            }
        }

        //Cannot find a repeating neighbor
        return Tuple(0, 0, *(p_lookahead_win));
    }
/*
    // Input:  vector<T> deltas
    // Output: DeltaNode describing patterns in deltas
    //
    // deltas is got by a sequence of inits. Later the output can be
    // used to combine inits with deltas
    // This function finds out the pattern by LZ77 modification
    DeltaNode findPattern( vector<off_t> const &deltas, int win_size )
    {
        vector<off_t>::const_iterator lookahead_win_start, 
        DeltaNode pattern_node;

        lookahead_win_start = deltas.begin();
        
        while ( lookahead_win_start != deltas.end() ) {
            //lookahead window is not empty
            Tuple cur_tuple = searchNeighbor( deltas, 
                                              lookahead_win_start, 
                                              win_size );
            //cur_tuple.show();
            if ( cur_tuple.isRepeatingNeighbor() ) {
                if ( pattern_stack.isPopSafe( cur_tuple.length ) ) {
                    //safe
                    pattern_stack.popElem( cur_tuple.length );

                    vector<off_t>::const_iterator first, last;
                    first = lookahead_win_start;
                    last = lookahead_win_start + cur_tuple.length;

                    IdxSigUnit pu;
                    pu.deltas.assign(first, last);
                    pu.cnt = 2;
                    pu.init = *(lookahead_win_start_orig - cur_tuple.length);

                    pattern_stack.push( pu );
                    lookahead_win_start += cur_tuple.length;
                    lookahead_win_start_orig += cur_tuple.length;
                } else {
                    //unsafe
                    IdxSigUnit pu = pattern_stack.top();

                    if ( pu.deltas.size() == cur_tuple.length ) {
                        //the subdeltas in lookahead window repeats
                        //the top pattern in stack.
                        //initial remains the same.
                        pu.cnt++;
                        pattern_stack.popPattern();
                        pattern_stack.push(pu);
                        pu.init = *lookahead_win_start_orig; //should delete this. keep if only for 
                        //tmp debug.

                        lookahead_win_start += cur_tuple.length;
                        lookahead_win_start_orig += cur_tuple.length;
                    } else {
                        //cannot pop out cur_tuple.length elems without
                        //totally breaking any pattern.
                        //So we simply add one elem to the stack
                        IdxSigUnit pu;
                        pu.deltas.push_back( *lookahead_win_start );
                        pu.init = *lookahead_win_start_orig;
                        pu.cnt = 1;
                        pattern_stack.push(pu);
                        lookahead_win_start++;
                        lookahead_win_start_orig++;
                    }
                }
            } else {
                //(0,0,x)
                IdxSigUnit pu;
                pu.deltas.push_back(cur_tuple.next_symbol);
                pu.init = *lookahead_win_start_orig;
                pu.cnt = 1;

                pattern_stack.push(pu); 
                lookahead_win_start++;
                lookahead_win_start_orig++;
            }
        }
       
        if ( lookahead_win_start_orig < orig.end() ) {
            assert(lookahead_win_start_orig + 1 == orig.end());
            IdxSigUnit pu;
            pu.init = *lookahead_win_start_orig;
            pu.cnt = 0;

            pattern_stack.push(pu); 
        }
       
        SigStack<IdxSigUnit> pattern_stack_compressed;
        vector<IdxSigUnit>::iterator it;
        for ( it = pattern_stack.the_stack.begin();
              it != pattern_stack.the_stack.end();
              it++ )
        {
            it->compressRepeats();
            if (pattern_stack_compressed.the_stack.empty()) {
                //mlog(IDX_WARN, "Empty");
                pattern_stack_compressed.the_stack.push_back(*it);
            } else {
                bool ret;
                ret = pattern_stack_compressed.the_stack.back().append(*it);
                if (ret == false) {
                    pattern_stack_compressed.the_stack.push_back(*it);
                }
            }
            //ostringstream oss;
            //oss << pattern_stack_compressed.show();
            ////mlog(IDX_WARN, "%s", oss.str().c_str());
        }
        

        if ( pattern_stack.size() != orig.size() ) {
            ostringstream oss;
            oss<< "pattern_stack.size() != orig.size() in"
                   << __FUNCTION__ << pattern_stack.size() 
                   << "," << orig.size() << endl;
            oss << "deltas.size():" << deltas.size() << endl;
            oss << pattern_stack.show() << endl;
            vector<off_t>::const_iterator it;
            for ( it = orig.begin();
                  it != orig.end();
                  it++ )
            {
                oss << *it << ",";
            }
            oss << endl;
            //mlog(IDX_ERR, "%s", oss.str().c_str());
            exit(-1);
        }

        return pattern_stack_compressed;
    }
*/


    ////////////////////////////////////////////////////////////////
    //  DeltaNode
    ////////////////////////////////////////////////////////////////
    bool DeltaNode::isLeaf() const
    {
        // if there's no children, then this node is a leaf
        return children.size() == 0;
    }

    string DeltaNode::show() const
    {
        ostringstream oss;

        if ( this->isLeaf() ) {
            oss << "(" ;
            vector<off_t>::const_iterator it;
            for ( it = elements.begin() ;
                  it != elements.end()  ;
                  it++ )
            {
                oss << *it;
                if ( it+1 != elements.end() ) {
                    oss << ",";
                }
            }
            oss << ")" ;
            oss << "^" << cnt;
        } else {
            // Not a leaf, recurse
            oss << "[";
            vector<DeltaNode*>::const_iterator it;
            for ( it =  children.begin() ;
                  it != children.end()   ;
                  it++ )
            {
                oss << (*it)->show();
                if ( it+1 != children.end() ) {
                    oss << ";";
                }
            }
            oss << "]";
            oss << "^" << cnt;
        }

        return oss.str();
    }

    // Delete all children of this node
    // This makes this node become a leaf
    void DeltaNode::freeChildren() 
    {
        vector<DeltaNode*>::const_iterator it;
        while ( children.size() > 0 )
        {
            popChild();   
        }
        return;
    }

    // Caller to make sure it is a leaf
    void DeltaNode::pushElement( off_t elem )
    {
        assert( isLeaf() ); // only leaf can have elements
        elements.push_back(elem);
    }

    bool DeltaNode::isPopSafe( int limit )
    {
        // limit is the number of deltas to pop out
        assert( !this->isLeaf() ); //assume this is not a leaf
                                   //since it is used for storing patterns
        vector<DeltaNode *>::reverse_iterator rit;
        int total = 0;

        rit = children.rbegin();
        while ( rit != children.rend() && total < limit ) 
        {
            total += (*rit)->getNumOfDeltas();
            rit++;
        }
        return total == limit; //exactly limit of deltas can be poped out
    }

    // To get how many deltas is in this node
    // if all patterns in this node is expanded
    int DeltaNode::getNumOfDeltas() const
    {
        
        if ( this->isLeaf() ) {
            int total = 0;
            total = elements.size() * cnt;
            return total;
        } else {
            int total = 0;
            vector<DeltaNode *>::const_iterator it;
            
            for ( it =  children.begin() ;
                  it != children.end() ;
                  it++ )
            {
                total += (*it)->getNumOfDeltas();
            }
            total *= cnt; // the whole pattern may repeat
            return total;
        }
    }

    // pop out n expanded deltas out
    // Caller has to make sure it is safe by calling isPopSafe()
    void DeltaNode::popDeltas( int n )
    {
        assert( isPopSafe(n) );
        
        int poped = 0;
        while ( poped < n ) {
            DeltaNode *ptopop = children.back();
            poped += pchild->getNumOfDeltas();
            popChild();
        }
    }

    // pop out the last child in children[]
    // free its space
    void DeltaNode::popChild()
    {
        if ( children.size() > 0 ) {
            DeltaNode *pchild = children.back();
            pchild->freeChildren();
            children.pop();
        }
        return;
    }


    ////////////////////////////////////////////////////////////////
    //  PatternUnit
    ////////////////////////////////////////////////////////////////
    string PatternUnit::show() 
    {
        ostringstream oss;

        if ( seq.size() == 0 || cnt == 0 )
            return oss.str();

        oss << ",(";
        vector<off_t>::iterator it;
        for ( it = seq.begin();
                it != seq.end();
                it++ )
        {
            oss << *it ;
            if ( it + 1 != seq.end() ) {
                // not the last one
                oss << ",";
            }
        }
        oss << ")";
        oss << "^" << cnt;
        return oss.str();       
    }


    ////////////////////////////////////////////////////////////////
    //  PatternHeadUnit
    ////////////////////////////////////////////////////////////////
    string PatternHeadUnit::show()
    {
        ostringstream oss;
        oss << init;
        oss << PatternUnit::show();
        return oss.str();       
    }

    PatternHeadUnit::PatternHeadUnit (off_t x)
        :init(x)
    {
    }



    ////////////////////////////////////////////////////////////////
    //  PatternBlock
    ////////////////////////////////////////////////////////////////

    string PatternBlock::show()
    {
        ostringstream oss;

        oss << "[";
        vector<PatternHeadUnit>::iterator it;
        for ( it = block.begin();
                it != block.end();
                it++ )
        {
            oss << it->show();
            if ( it+1 != block.end() ) {
                oss << ";";
            }
        }
        oss << "]";
        return oss.str();
    }



    ////////////////////////////////////////////////////////////////
    //  PatternAnalyzer
    ////////////////////////////////////////////////////////////////

    // input: many PatternUnit (just offset, or PatternHeadUnit)
    // output: a PatternChunk describing the pattern:
    //            with two blocks if pattern found
    //            with only one if not found
    PatternChunk PatternAnalyzer::generatePatterns( PatternBlock &pblock )
    {

    }

    ////////////////////////////////////////////////////////////////
    //  Tuple : the concept in LZ77
    ////////////////////////////////////////////////////////////////
    Tuple::Tuple(int o, int l, off_t n) {
        offset = o;
        length = l;
        next_symbol = n;
    }
    
    void Tuple::put(int o, int l, off_t n) {
        offset = o;
        length = l;
        next_symbol = n;
    }

    bool Tuple::operator== (const Tuple other) {
        if (offset == other.offset 
                && length == other.length
                && next_symbol == other.next_symbol)
        {
            return true;
        } else {
            return false;
        }
    }

    bool Tuple::isRepeatingNeighbor() {
        return (offset == length && offset > 0);
    }
    
    string Tuple::show() {
        ostringstream showstr;
        showstr << "(" << offset 
            << ", " << length
            << ", " << next_symbol << ")" << endl;
        return showstr.str();
    }
}


