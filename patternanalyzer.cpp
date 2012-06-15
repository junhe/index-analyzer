
#include "patternanalyzer.h"

#include <string.h>
#include <stack>

namespace MultiLevel {
    ////////////////////////////////////////////////////////////////
    //  MISC
    ////////////////////////////////////////////////////////////////
    inline
    off_t sumVector( vector<off_t> seq )
    {
        vector<off_t>::const_iterator iiter;
        
        off_t sum = 0;
        for ( iiter = seq.begin() ;
              iiter != seq.end() ;
              iiter++ )
        {
            sum += *iiter;
        }
        
        return sum;
    }

    vector<off_t> buildDeltas( vector<off_t> seq ) 
    {
        vector<off_t>::iterator it;
        vector<off_t> deltas;
        for ( it = seq.begin() ; it != seq.end() ; it++ )
        {
            if ( it > seq.begin() ) {
                deltas.push_back( *it - *(it-1) );
            }
        }
        //cout << "in builddeltas: " << seq.size() << " " << deltas.size() << endl; 
        return deltas;
    }

    
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
    
    bool isEqual(DeltaNode* a, DeltaNode* b)
    {
        //TODO: implement this
        return a->serialize() == b->serialize();
    }

    bool isEqual(off_t a, off_t b)
    {
        return a==b;
    }

    void appendToBuffer( string &to, const void *from, const int size )
    {
        if ( size > 0 ) { //make it safe
            to.append( (char *)from, size );
        }
    }

    //Note that this function will increase start
    void readFromBuf( string &from, void *to, int &start, const int size )
    {
        //'to' has to be treated as plain memory
        memcpy(to, &from[start], size);
        start += size;
    }

    template <class T>
    void printVector( vector<T> vec )
    {
        typename vector<T>::const_iterator iiter;
        
        cout << "printVector: ";
        for ( iiter = vec.begin() ;
              iiter != vec.end() ;
              iiter++ )
        {
            cout <<  *iiter << ",";
        }
        cout << endl;        
    }

    ////////////////////////////////////////////////////////////////
    //  DeltaNode
    ////////////////////////////////////////////////////////////////
    bool DeltaNode::isLeaf() const
    {
        // if there's no children, then this node is a leaf
        return children.size() == 0 ;
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
        //cout << "in " << __FUNCTION__ 
        //    << " total: " << total
        //    << " limit: " << limit << endl;
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



    // Only used by popDeltas
    void DeltaNode::deleteIt( DeltaNode * nd ) 
    {
        delete nd;
    }
    
    void DeltaNode::deleteIt( off_t anoff ) 
    {
        return ;
    }

    // pop out n expanded deltas out
    // Caller has to make sure it is safe by calling isPopSafe()
    // THIS JUST FREES THE ONE LOWER LEVEL OF THE TREE
    void DeltaNode::popDeltas( int n )
    {
        assert( isPopSafe(n) );
        
        int poped = 0;
        while ( poped < n ) {
            DeltaNode *ptopop = children.back();
            poped += ptopop->getNumOfDeltas();
            deleteIt(poped);
            children.pop_back();
        }
    }

    // pop out the last child in children[]
    // free its space
    void DeltaNode::popChild()
    {
        if ( children.size() > 0 ) {
            DeltaNode *pchild = children.back();
            pchild->freeChildren();
            children.pop_back();
        }
        return;
    }

    // pushChild will make a node NOT a leaf
    // If this node is an inner node, calling this is safe
    // If this node is a leaf:
    //      If it has elements: NOT SAFE
    //      If it has no elements: safe. And this function will
    //                             make this node a inner node.
    void DeltaNode::pushChild( DeltaNode *newchild )
    {
        assert( elements.size() == 0 );
        children.push_back(newchild);
    }

    void DeltaNode::assign( vector<DeltaNode *>::const_iterator first,
                 vector<DeltaNode *>::const_iterator last )
    {
        children.assign( first, last );
    }

    void DeltaNode::assign( vector<off_t>::const_iterator first,
                 vector<off_t>::const_iterator last )
    {
        elements.assign( first, last );
    }

    void DeltaNode::push( DeltaNode *newchild ) {
        pushChild(newchild);
    }
    
    void DeltaNode::push( off_t newelem ) 
    {
        pushElement(newelem);
    }

    void DeltaNode::pushCopy( DeltaNode *nd )
    {
        string buf = nd->serialize();
        DeltaNode *newone;
        newone->deSerialize(buf);
        pushChild(newone);
    }

    void DeltaNode::pushCopy( off_t elm )
    {
        push(elm);
    }

    string DeltaNode::serialize()
    {
        string buf;
        //cout << "++++++Start Serialzing " << this->show() << endl;
        if ( isLeaf() ) {
            //[L][REP][Pattern Size][elements]
            char buftype = LEAF; //L means leaf
            appendToBuffer(buf, &buftype, sizeof(buftype));
            appendToBuffer(buf, &cnt, sizeof(cnt));
            header_t elembodysize = elements.size() * sizeof(off_t); 
            appendToBuffer(buf, &elembodysize, sizeof(elembodysize));
            if ( elembodysize > 0 ) {
                appendToBuffer(buf, &elements[0], elembodysize);
            }
            //cout << "TYPE:[" << buftype << "] bodysize:" << elembodysize << endl;
            //cout << "++++++End   Serialzing " << this->show() << endl;
            return buf;
        } else {
            //[I][REP][Pattern Size][ [..] [...] ..  ]
            char buftype = INNER; // I means inner
            appendToBuffer(buf, &buftype, sizeof(buftype));
            appendToBuffer(buf, &cnt, sizeof(cnt));
            header_t bodysize = 0; //wait to be updated
            int bodysize_pos = buf.size(); // buf[bodysize_pos] is where
                                           // the bodysize is
            appendToBuffer(buf, &bodysize, sizeof(bodysize));
            
            vector<DeltaNode *>::const_iterator it;
            for ( it =  children.begin() ;
                  it != children.end()   ;
                  it++ )
            {
                string tmpbuf;
                tmpbuf = (*it)->serialize();
                appendToBuffer(buf, tmpbuf.c_str(), tmpbuf.size());
            }
            header_t *psize = (header_t *) &buf[bodysize_pos]; 
            *psize = buf.size() 
                     - (sizeof(buftype) + sizeof(cnt) + sizeof(bodysize));
            //cout << "TYPE:[" << buftype << "] bodysize:" << *psize
            //     << "bodysize_pos" << bodysize_pos
            //     << "buf.size():" << buf.size() << endl;
            //cout << "++++++End   Serialzing " << this->show() << endl;
            return buf;
        }
    }

    void DeltaNode::deSerialize( string buf )
    {
        char buftype;
        header_t bodysize = 0;
        int cur_start = 0;

        //cout << "***** START deSerialize " << endl;

        readFromBuf( buf, &buftype, cur_start, sizeof(buftype));
        readFromBuf( buf, &cnt, cur_start, sizeof(cnt));
        readFromBuf( buf, &bodysize, cur_start, sizeof(bodysize));
      
        //cout << "**TYPE:" << buftype << endl;
        //cout << "cnt:" << cnt << endl;
        //cout << "bodysize:" << bodysize << endl;
        //cout << "bufsize:" << buf.size() << endl;
        
        if ( buftype == LEAF ) {
            //only elements left in buf
            //cout << "--type is leaf" << endl;
            if ( bodysize > 0 ) {
                elements.resize( bodysize/sizeof(off_t) );
                readFromBuf( buf, &elements[0], cur_start, bodysize);
            }
        } else {
            // It is a inner node in buf
            //cout << "--type is inner" << endl;
            while ( cur_start < buf.size() ) {
                //cout << "----START----- cur_start:" << cur_start
                //     << " buf.size():" << buf.size() << endl;
                char btype;
                int bcnt;
                header_t bbodysize;
                int bufsize;

                readFromBuf( buf, &btype, cur_start, sizeof(btype));
                readFromBuf( buf, &bcnt, cur_start, sizeof(bcnt));
                readFromBuf( buf, &bbodysize, cur_start, sizeof(bbodysize));



                bufsize = sizeof(btype) + sizeof(bcnt) 
                          + sizeof(bbodysize) + bbodysize;

                //cout << "--------- TYPE:" << btype << " CNT:" << cnt
                //     << " bodysize:" << bbodysize 
                //     << " bufsize:" << bufsize << endl;
                string localbuf;
                localbuf.resize( bufsize );
                cur_start -= (sizeof(btype) + sizeof(bcnt)
                              + sizeof(bbodysize));
                             
                readFromBuf( buf, &localbuf[0], cur_start, bufsize);

                DeltaNode *newchild = new DeltaNode;
                newchild->deSerialize(localbuf);

                pushChild(newchild);
                //cout << "----END----- cur_start:" << cur_start
                //     << " buf.size():" << buf.size() << endl;
            }
        }
        //cout << this->show() << endl;
        //cout << "***** END   deSerialize " << endl;
    }

    void DeltaNode::init()
    {
        cnt = 1;
    }

    DeltaNode::DeltaNode()
    {
        init();
    }

    // this constructor turns this DeltaNode into
    // a pattern for logical_offset, length, or physical_offset
    // The format is:
    // [init...][Delta...] 
    //                    [Delta...] 
    //                                [Delta...] ...
    // [] is a child. [init] is the only place that can be splitted.
    void DeltaNode::buildPatterns( vector<off_t> seq )
    {
        //cout << "in " << __FUNCTION__ << endl;
        init();
        vector<off_t> deltas;
        // [inits][deltas_pattern]
        DeltaNode *deltas_pattern;
        DeltaNode *inits = new DeltaNode;
        deltas = buildDeltas( seq );
        
        //printVector(deltas);

        deltas_pattern = findPattern( deltas, 6 );
        pushChild(inits);
        pushChild(deltas_pattern);

        // handle the inits
        if ( seq.size() == 0 ) {
            // nothing can be done
            return; 
        }
        int pos = 0;
        inits->pushElement( seq[pos] );
        vector<DeltaNode *>::const_iterator it;
        for ( it =  deltas_pattern->children.begin() ;
              it != deltas_pattern->children.end() ;
              it++ )
        {
            pos += (*it)->getNumOfDeltas();
            //cout << pos << "<" << seq.size() << endl;
            assert( pos < seq.size() );
            inits->pushElement( seq[pos] );
        }
        return;
    }

    // It tries to use LZ77 to find repeating pattern in
    // the children
    void DeltaNode::compressMe()
    {
       
        if ( isLeaf() ) {
            DeltaNode *compressedChild 
                       = findPattern( this->children, 20 );
            freeChildren();
        } else {
            DeltaNode *compressedChild 
                       = findPattern( this->elements, 20 );
            children = compressedChild->children;
        }
    }

    // input is the pos of a delta
    // leaf_pos is the output, it is the position of the leaf
    // it returns the delta sun util pos in the leaf
    LeafTuple DeltaNode::getLeafTupleByPos( const int pos )
    {
        LeafTuple tup(0,0);
        int rpos = pos;
        //cout << __FUNCTION__ << ":" << pos
        //     << "NumOfDeltas:" << this->getNumOfDeltas() << endl;
        assert( rpos <= this->getNumOfDeltas() );

        if ( isLeaf() ) {
            // hit what you want
            if ( rpos == 0 ) {
                tup.leaf_delta_sum = 0;
            } else {
                tup.leaf_delta_sum = this->getDeltaSumUtilPos(rpos - 1);
            }
            tup.leaf_index = 0; // tup.leaf_index can be used as an index
                                // leaves[ leaf_index ] is this one
            return tup;
        } else {
            // [..][..]...
            int num_child_deltas = this->getNumOfDeltas()/cnt;
            
            int num_child_leaves = this->getNumOfLeaves()/cnt;

            assert( num_child_deltas > 0 );
            
            int col = rpos % num_child_deltas;
            int row = rpos / num_child_deltas;

            if ( row > 0 ) {
                tup.leaf_index = (this->getNumOfLeaves()/cnt) * row;
            }

            vector<DeltaNode *>::const_iterator it;
            for ( it = children.begin() ;
                  it != children.end()  ;
                  it++ )
            {
                int sizeofchild = (*it)->getNumOfDeltas();
                if ( col < sizeofchild ) {
                    LeafTuple ltup = (*it)->getLeafTupleByPos( col );
                    tup.leaf_index += ltup.leaf_index;
                    tup.leaf_delta_sum = ltup.leaf_delta_sum;
                    break;
                } else {
                    tup.leaf_index += (*it)->getNumOfLeaves();

                    col -= sizeofchild;
                }          
            }
            return tup;
        }
    }

    int DeltaNode::getNumOfLeaves()
    {
        int sumleaves = 0;

        if ( isLeaf() ) {
            return 1;
        } else {
            vector<DeltaNode *>::const_iterator it;
            
            for ( it = children.begin() ;
                  it != children.end()  ;
                  it++ )
            {
                sumleaves += (*it)->getNumOfLeaves();
            }

            return sumleaves * cnt;
        }
    }

    off_t DeltaNode::getDeltaSumUtilPos( const int pos )
    {
        int rpos = pos;
        assert( rpos < this->getNumOfDeltas() );

        if ( isLeaf() ) {
            off_t delta_sum = 0;
            int col = rpos % elements.size();
            int row = rpos / elements.size();

            assert ( row < cnt );
            off_t elem_sum = sumVector(elements);

            //+1 so include the one pointed by col
            off_t last_row_sum 
                  = sumVector( vector<off_t> (elements.begin(),
                                              elements.begin() + col + 1) ); 
            return elem_sum * row + last_row_sum; 
        } else {
            off_t sum = 0;
            int num_child_deltas = this->getNumOfDeltas()/cnt;
            assert( num_child_deltas > 0 );
            int col = rpos % num_child_deltas;
            int row = rpos / num_child_deltas;

            if ( row > 0 ) {
                off_t childsum = this->getDeltaSum() / cnt; //TODO: justify this
                /*
                vector<DeltaNode *>::const_iterator cit;
                for ( cit = children.begin() ;
                      cit != children.end() ;
                      cit++ )
                {
                    childsum += (*cit)->getDeltaSum();
                }
                */
                sum += childsum * row;
            }

            vector<DeltaNode *>::const_iterator it;
            for ( it = children.begin() ;
                  it != children.end()  ;
                  it++ )
            {
                int sizeofchild = (*it)->getNumOfDeltas();
                if ( col < sizeofchild ) {
                    sum += (*it)->getDeltaSumUtilPos( col );
                    break;
                } else {
                    sum += (*it)->getDeltaSum();
                    col -= sizeofchild;
                }          
            }
            return sum;
        }
    }

    off_t DeltaNode::getDeltaSum()
    {
        if ( isLeaf() ) {
            return sumVector(elements)*cnt;
        } else {
            off_t sum = 0;
            vector<DeltaNode *>::const_iterator it;
            for ( it =  children.begin() ;
                  it != children.end()   ;
                  it++ )
            {
                sum += (*it)->getDeltaSum();
            }
            return sum*cnt;
        }
    }

    // this DeltaNode must have format: [init...][delta...] [delta...]...[last]
    off_t DeltaNode::recoverPos( const int pos )
    {
        // find the ID of leaf who have the pos'th delta in it in last
        // pos = id
        // last--

        vector<off_t> deltalist;
        int leafpos = 0;
        int rpos = pos;
        vector<DeltaNode *>::const_reverse_iterator rit;

        for ( rit =  children.rbegin() ;
              rit != children.rend()   ;
              rit++ )
        {
            //cout << "------------ a loop ------------" <<endl;
            if ( rit + 1 == children.rend() ) {
                // this is the DeltaNode of [init...]
                assert( (*rit)->isLeaf() );
                assert( rpos < (*rit)->elements.size() );
                off_t deltasum = sumVector( deltalist );
                //cout << "in [init..]" << "deltasum:" << deltasum
                //     << "rpos: " << rpos
                //     << "elem[rpos]: " << (*rit)->elements[rpos] << endl;
                return (*rit)->elements[rpos] + deltasum;
            } else {
                // pure delta node
                LeafTuple tup = (*rit)->getLeafTupleByPos(rpos);
                deltalist.push_back( tup.leaf_delta_sum );
                rpos = tup.leaf_index;
                //cout << "tup.leaf_index: " << tup.leaf_index << endl;
                //cout << "delta list:" ;
                vector<off_t>::iterator it;
                for ( it = deltalist.begin();
                      it != deltalist.end();
                      it++ )
                {
                    //cout << *it << " " ;
                }
                //cout << endl;
            }
        }
        assert( 0 );
    }

    DeltaNode::~DeltaNode() 
    {
        //freeChildren(); let the creator decide when to delete
    }

    ////////////////////////////////////////////////////////////////
    //  PatternCombo
    ////////////////////////////////////////////////////////////////
    void PatternCombo::buildFromHostEntries( const vector<HostEntry> &entry_buf, 
                                             const pid_t &proc )
    {
        vector<off_t> h_logical_off, h_length, h_physical_off; 
        vector<HostEntry>::const_iterator iter;
        
        for ( iter = entry_buf.begin() ; 
                iter != entry_buf.end() ;
                iter++ )
        {
            if ( iter->id != proc ) {
                continue;
            }

            h_logical_off.push_back(iter->logical_offset);
            h_length.push_back(iter->length);
            h_physical_off.push_back(iter->physical_offset);
        }
        
        //printVector( h_logical_off );
        //cout << "jjjjjjjjjjjjjjjjjjJ" << endl;
        this->logical_offset.buildPatterns( h_logical_off );
        this->length.buildPatterns( h_length );
        this->physical_offset.buildPatterns( h_physical_off );
        this->original_chunk_id = proc;
    }

    string PatternCombo::show()
    {
        ostringstream oss;
        oss << "[" << original_chunk_id << "] [" << new_chunk_id << "]" << endl;
        oss << "****LOGICAL_OFFSET**** :" << logical_offset.show() << endl;
        oss << "****    LENGTH         :" << length.show() << endl;
        oss << "**** PHYSICAL_OFFSET **:" << physical_offset.show() << endl;
        return oss.str();
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


}


