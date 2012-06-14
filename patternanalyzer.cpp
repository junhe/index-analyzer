
#include "patternanalyzer.h"


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
    
    bool isEqual(DeltaNode* a, DeltaNode* b)
    {
        //TODO: implement this
        return true;     
    }

    bool isEqual(off_t a, off_t b)
    {
        return a==b;
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


