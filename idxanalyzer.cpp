#include "idxanalyzer.h"

#include <algorithm>

//for debugging
void printIdxEntries( vector<IdxSigEntry> &idx_entry_list )
{
    vector<IdxSigEntry>::const_iterator iter;

    cout << "this is printIdxEntries" << endl;

    for ( iter = idx_entry_list.begin();
            iter != idx_entry_list.end();
            iter++ )
    {
       cout << "[" << iter->proc << "]" << endl;
       vector<IdxSigUnit>::const_iterator iter;
    }
}

//IdxEntry is designed to mimic the HostEntry in PLFS,
//so later I can easily copy and paste these code to PLFS
//and make it work.
//
//It gets signatures for a proc. 
//TODO:
//But do we really need to separate entries by proc at first?
//Also, have to handle the case that there is only one entry
void IdxSignature::generateIdxSignature(vector<IdxEntry> &entry_buf, 
                                        int proc) 
{
    vector<off_t> logical_offset, length, physical_offset; 
    vector<off_t> logical_offset_delta, 
                    length_delta, 
                    physical_offset_delta; 

    vector<IdxEntry>::const_iterator iter;
    for ( iter = entry_buf.begin() ; 
            iter != entry_buf.end() ;
            iter++ )
    {
        if ( iter->Proc != proc ) {
            continue;
        }

        logical_offset.push_back(iter->Logical_offset);
        length.push_back(iter->Length);
        physical_offset.push_back(iter->Physical_offset);

        if ( iter != entry_buf.begin() ) {
            logical_offset_delta.push_back(
                    iter->Logical_offset - (iter-1)->Logical_offset);
            length_delta.push_back(
                    iter->Length - (iter-1)->Length);
            physical_offset_delta.push_back(
                    iter->Physical_offset - (iter-1)->Physical_offset);
        }
        cout << iter->Physical_offset << " ";
    }

    SigStack<IdxSigUnit> offset_sig = 
        discoverSigPattern(logical_offset_delta, logical_offset);

    //Now, go through offset_sig one by one and build the IdxSigEntry s
    vector<IdxSigEntry>idx_entry_list;
    vector<IdxSigUnit>::const_iterator stack_iter;

    int range_start = 0, range_end; //the range currently processing
    for (stack_iter = offset_sig.begin();
            stack_iter != offset_sig.end();
            stack_iter++ )
    {
        //cout << stack_iter->init << " " ;
        IdxSigEntry idx_entry;
        range_end = range_start + stack_iter->size();
        SigStack<IdxSigUnit> length_stack = 
            discoverSigPattern( 
                    vector<off_t> (length_delta.begin()+range_start,
                                   length_delta.begin()+range_end),
                    vector<off_t> (length.begin()+range_start,
                                   length.begin()+range_end) );
        SigStack<IdxSigUnit> physical_offset_stack = 
            discoverSigPattern( 
                    vector<off_t> (physical_offset_delta.begin()+range_start,
                                   physical_offset_delta.begin()+range_end),
                    vector<off_t> (physical_offset.begin()+range_start,
                                   physical_offset.begin()+range_end) );

        idx_entry.logical_offset = *stack_iter;
        idx_entry.length = length_stack;
        idx_entry.physical_offset = physical_offset_stack;
        
        idx_entry_list.push_back( idx_entry);


        range_start = range_end;
    }
    printIdxEntries(idx_entry_list);
}



//find out pattern of a number sequence 
void IdxSignature::discoverPattern(  vector<off_t> const &seq )
{
    vector<off_t>::const_iterator p_lookahead_win; // pointer(iterator) to the lookahead window
    PatternStack<PatternUnit> pattern_stack;

    p_lookahead_win = seq.begin();
    pattern_stack.clear();

    cout << endl << "this is discoverPattern() :)" << endl;

    while ( p_lookahead_win != seq.end() ) {
        //lookahead window is not empty
        Tuple cur_tuple = searchNeighbor( seq, p_lookahead_win );
        cur_tuple.show();
        if ( cur_tuple.isRepeatingNeighbor() ) {
            if ( pattern_stack.isPopSafe( cur_tuple.length ) ) {
                //safe
                pattern_stack.popElem( cur_tuple.length );

                vector<off_t>::const_iterator first, last;
                first = p_lookahead_win;
                last = p_lookahead_win + cur_tuple.length;

                PatternUnit pu;
                pu.seq.assign(first, last);
                pu.cnt = 2;

                pattern_stack.push( pu );
                p_lookahead_win += cur_tuple.length;
            } else {
                //unsafe
                PatternUnit pu = pattern_stack.top();

                if ( pu.seq.size() == cur_tuple.length ) {
                    //the subseq in lookahead window repeats
                    //the top pattern in stack
                    pu.cnt++;
                    pattern_stack.popPattern();
                    pattern_stack.push(pu);

                    p_lookahead_win += cur_tuple.length;
                } else {
                    //cannot pop out cur_tuple.length elems without
                    //totally breaking any pattern.
                    //So we simply add one elem to the stack
                    PatternUnit pu2;
                    pu2.seq.push_back( *p_lookahead_win );
                    pu2.cnt = 1;
                    pattern_stack.push(pu2);
                    p_lookahead_win++;
                }
            }


        } else {
            //(0,0,x)
            PatternUnit pu;
            pu.seq.push_back(cur_tuple.next_symbol);
            pu.cnt = 1;

            pattern_stack.push(pu); 
            p_lookahead_win++;
        }
        pattern_stack.show();
    }

}

//find out pattern of a number sequence(deltas) with its
//original sequence
SigStack<IdxSigUnit> IdxSignature::discoverSigPattern( vector<off_t> const &seq,
                                    vector<off_t> const &orig )
{
    // pointer(iterator) to the lookahead window, bot should move together
    vector<off_t>::const_iterator p_lookahead_win, 
                                  p_lookahead_win_orig; 
    SigStack<IdxSigUnit> pattern_stack;

    p_lookahead_win = seq.begin();
    p_lookahead_win_orig = orig.begin();
    pattern_stack.clear();

    cout << endl << "this is discoverPattern() :)" << endl;

    while ( p_lookahead_win != seq.end() ) {
        //lookahead window is not empty
        Tuple cur_tuple = searchNeighbor( seq, p_lookahead_win );
        cur_tuple.show();
        if ( cur_tuple.isRepeatingNeighbor() ) {
            if ( pattern_stack.isPopSafe( cur_tuple.length ) ) {
                //safe
                pattern_stack.popElem( cur_tuple.length );

                vector<off_t>::const_iterator first, last;
                first = p_lookahead_win;
                last = p_lookahead_win + cur_tuple.length;

                IdxSigUnit pu;
                pu.seq.assign(first, last);
                pu.cnt = 2;
                pu.init = *(p_lookahead_win_orig - cur_tuple.length);

                pattern_stack.push( pu );
                p_lookahead_win += cur_tuple.length;
            } else {
                //unsafe
                IdxSigUnit pu = pattern_stack.top();

                if ( pu.seq.size() == cur_tuple.length ) {
                    //the subseq in lookahead window repeats
                    //the top pattern in stack.
                    //initial remains the same.
                    pu.cnt++;
                    pattern_stack.popPattern();
                    pattern_stack.push(pu);

                    p_lookahead_win += cur_tuple.length;
                    p_lookahead_win_orig += cur_tuple.length;
                } else {
                    //cannot pop out cur_tuple.length elems without
                    //totally breaking any pattern.
                    //So we simply add one elem to the stack
                    IdxSigUnit pu;
                    pu.seq.push_back( *p_lookahead_win );
                    pu.init = *p_lookahead_win_orig;
                    pu.cnt = 1;
                    pattern_stack.push(pu);
                    p_lookahead_win++;
                    p_lookahead_win_orig++;
                }
            }
        } else {
            //(0,0,x)
            IdxSigUnit pu;
            pu.seq.push_back(cur_tuple.next_symbol);
            pu.init = *p_lookahead_win_orig;
            pu.cnt = 1;

            pattern_stack.push(pu); 
            p_lookahead_win++;
            p_lookahead_win_orig++;
        }
        pattern_stack.show();
    }
    return pattern_stack;
}

Tuple IdxSignature::searchNeighbor( vector<off_t> const &seq,
        vector<off_t>::const_iterator p_lookahead_win ) 
{
    vector<off_t>::const_iterator i;     
    int j;
    cout << "------------------- I am in searchNeighbor() " << endl;

    //i goes left util the begin or reaching window size
    int distance = 0;
    i = p_lookahead_win;
    while ( i != seq.begin() && distance < win_size ) {
        i--;
        distance++;
    }
    //termination: i == seq.begin() or distance == win_size

    //print out search buffer and lookahead buffer
    cout << "search buf: " ;
    vector<off_t>::const_iterator k;
    for ( k = i ; k != p_lookahead_win ; k++ ) {
        cout << *k << " ";
    }
    cout << endl;

    cout << "lookahead buf: " ;
    vector<off_t>::const_iterator p;
    p = p_lookahead_win;
    for ( p = p_lookahead_win ; 
            p != seq.end() && p - p_lookahead_win < win_size ; 
            p++ ) {
        cout << *p << " ";
    }
    cout << endl;

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


