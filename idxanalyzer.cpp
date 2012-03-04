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
    vector<IdxEntry>::iterator iter;
    for (iter = entry_buf.begin() ; iter != entry_buf.end() ; iter++ ) {
        cout << (*iter).Logical_offset << " ";
    }
    cout << endl;
    vector<off_t>::iterator iter2;
    for (iter2 = off_deltas.begin() ; iter2 != off_deltas.end() ; iter2++ ) {
        cout << (*iter2) << " ";
    }
}

void IdxSignature::discoverPattern( vector<off_t> seq )
{
    
}





