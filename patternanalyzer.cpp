
#include "patternanalyzer.h"

#include <string.h>

namespace MultiLevel {
    ////////////////////////////////////////////////////////////////
    //  MISC
    ////////////////////////////////////////////////////////////////

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
            poped += ptopop->getNumOfDeltas();
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

    string DeltaNode::serialize()
    {
        string buf;
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
            *psize = buf.size() - bodysize_pos;
            return buf;
        }
    }

    void DeltaNode::deSerialize( string buf )
    {
        char buftype;
        header_t bodysize = 0;
        int cur_start = 0;

        readFromBuf( buf, &buftype, cur_start, sizeof(buftype));
        readFromBuf( buf, &cnt, cur_start, sizeof(cnt));
        readFromBuf( buf, &bodysize, cur_start, sizeof(bodysize));
        
        if ( buftype == LEAF ) {
            //only elements left in buf
            if ( bodysize > 0 ) {
                elements.resize( bodysize/sizeof(off_t) );
                readFromBuf( buf, &elements[0], cur_start, bodysize);
            }
        } else {
            // It is a inner node in buf
            while ( cur_start < buf.size() ) {
                char btype;
                int bcnt;
                header_t bbodysize;
                int bufsize;

                readFromBuf( buf, &btype, cur_start, sizeof(btype));
                readFromBuf( buf, &bcnt, cur_start, sizeof(bcnt));
                readFromBuf( buf, &bbodysize, cur_start, sizeof(bbodysize));
                bufsize = sizeof(btype) + sizeof(bcnt) 
                          + sizeof(bbodysize) + bbodysize;

                string localbuf;
                localbuf.resize( bufsize );
                cur_start -= (sizeof(btype) + sizeof(bcnt)
                              + sizeof(bbodysize));
                             
                readFromBuf( buf, &localbuf[0], cur_start, bufsize);

                DeltaNode *newchild = new DeltaNode;
                newchild->deSerialize(localbuf);

                pushChild(newchild);
            }
        }
    }

    void DeltaNode::init()
    {
        cnt = 1;
    }

    void DeltaNode::DeltaNode()
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
    DeltaNode::DeltaNode( vector<off_t> seq )
    {
        init();
        vector<off_t> deltas;
        // [inits][deltas_pattern]
        DeltaNode *deltas_pattern;
        DeltaNode *inits = new DeltaNode;
        deltas = buildDeltas( seq );

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
        for ( it =  children.begin() ;
              it != children.end() ;
              it++ )
        {
            pos += (*it)->getNumOfDeltas();
            assert( pos < seq.size() );
            inits->pushElement( seq[pos] );
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


}


