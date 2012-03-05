#include "idxanalyzer.h"

#include <algorithm>
#include <iterator>
#include <sstream>

bool IdxSignature::openTraceFile( const char *fpath )
{
    idx_file.open(fpath);
    if (idx_file.is_open()) {
        cout << "file is open." << endl;
        return true;
    } else {
        cout << "file is not open." << endl;
        return false;
    }
}

bool IdxSignature::getNextEntry( IdxEntry &idx_entry )
{
    string line;

    if ( !idx_file.is_open() ) {
        cout << "Trace file is not opened." << endl;
        return false;
    }

    if ( !idx_file.good() ) {
        cout << "The file is not good for reading" << endl;
        return false;
    }

    getline(idx_file, line);
    //cout << line << endl;

    vector<string> tokens;
    vector<string>::iterator iter;
    istringstream iss(line);
    copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter<vector<string> >(tokens));

    //for ( iter = tokens.begin() ; iter != tokens.end() ; iter++ ) {
    //    cout << *iter << endl;
    //}
    idx_entry.Proc = atoi( tokens[0].c_str() );
    //cout << idx_entry.Proc << endl;
    idx_entry.ID = (tokens[1] == string("w")) ? ID_WRITE:ID_READ;
    //cout << idx_entry.ID << endl;
    idx_entry.Logical_offset = atol( tokens[2].c_str() ); 
    //cout << idx_entry.Logical_offset << endl;
    idx_entry.Length = atol( tokens[3].c_str() );
    //cout << idx_entry.Length << endl;
    idx_entry.Begin_timestamp = atof( tokens[4].c_str() );
    //cout << idx_entry.Begin_timestamp << endl;
    idx_entry.End_timestamp = atof( tokens[5].c_str() );
    //cout << idx_entry.End_timestamp << endl;
    idx_entry.Logical_tail = atol( tokens[6].c_str() );
    //cout << idx_entry.Logical_tail << endl;
    //idx_entry.ID2 = atoi( tokens[7].c_str() );
    //cout << idx_entry.ID2 << endl;
    //idx_entry.Chunk_offset = atoi( tokens[8].c_str() );

    return true;
}

bool IdxSignature::bufferEntries()
{
    sig_list.clear();
    IdxEntry h_entry;
    int bufsize = 16;
    int i;
    off_t pre_l_offset, cur_l_offset; //logical offset

    //init the offset deltas
    off_deltas.clear();
    pre_l_offset = 0;
    cur_l_offset = 0;
    
    for ( i = 0 ; i < bufsize ; i++ ) {
        if (!getNextEntry( h_entry )) {
            //failed to get next entry
            break;
        }
        
        pre_l_offset = cur_l_offset;
        cur_l_offset = h_entry.Logical_offset;
        if ( i > 0 ) {
            //push offset[i]-offset[i-1]
            off_deltas.push_back( cur_l_offset - pre_l_offset );
        }

        entry_buf.push_back(h_entry);
    }



    /*
    vector<IdxEntry>::iterator iter;
    for (iter = entry_buf.begin() ; iter != entry_buf.end() ; iter++ ) {
        cout << (*iter).Logical_offset << " ";
    }
    cout << endl;
    */
    vector<off_t>::iterator iter2;
    for (iter2 = off_deltas.begin() ; iter2 != off_deltas.end() ; iter2++ ) {
        cout << (*iter2) << " ";
    }
    cout << endl;
    
    discoverPattern(off_deltas);
}

//find out pattern of a number sequence 
void IdxSignature::discoverPattern(  vector<off_t> const &seq )
{
    vector<off_t>::const_iterator p_lookahead_win; // pointer(iterator) to the lookahead window
    
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

Tuple IdxSignature::searchNeighbor( vector<off_t> const &seq,
                                    vector<off_t>::const_iterator p_lookahead_win ) 
{
    vector<off_t>::const_iterator i;     
    int j;
    cout << " I am in searchNeighbor() " << endl;
    
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


