#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>

#include "idxanalyzer.h"

using namespace std;

vector<IdxEntry> bufferEntries(ifstream &idx_file);

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    ifstream idx_file;
    vector<IdxEntry> entry_buf;

    idx_file.open("trace.txt");
    if (idx_file.is_open()) {
        cout << "file is open." << endl;
    } else {
        cout << "file is not open." << endl;
    }

    entry_buf = bufferEntries(idx_file);

    cout<<"hello"<<endl;
    return 0;
}

bool getNextEntry( IdxEntry &idx_entry, ifstream &idx_file )
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
    //idx_entry.Logical_offset = atol( tokens[2].c_str() ); 
    sscanf( tokens[2].c_str(), "%lld", &idx_entry.Logical_offset);
    //cout << idx_entry.Logical_offset << endl;
    //idx_entry.Length = atol( tokens[3].c_str() );
    sscanf( tokens[3].c_str(), "%lld", &idx_entry.Length);
    //cout << idx_entry.Length << endl;
    idx_entry.Begin_timestamp = atof( tokens[4].c_str() );
    //cout << idx_entry.Begin_timestamp << endl;
    idx_entry.End_timestamp = atof( tokens[5].c_str() );
    //cout << idx_entry.End_timestamp << endl;
    //idx_entry.Logical_tail = atol( tokens[6].c_str() );
    sscanf( tokens[6].c_str(), "%lld", &idx_entry.Logical_tail);
    //cout << idx_entry.Logical_tail << endl;
    //idx_entry.ID2 = atoi( tokens[7].c_str() );
    //cout << idx_entry.ID2 << endl;
    //idx_entry.Chunk_offset = atoi( tokens[8].c_str() );

    return true;
}

vector<IdxEntry> bufferEntries(ifstream &idx_file)
{
    cout << "i am bufferEntries()" << endl;
    IdxEntry h_entry;
    vector<IdxEntry> entry_buf;
    int bufsize = 86039 ;
    int i;
    off_t pre_l_offset, cur_l_offset; //logical offset
    

    //init the offset deltas
    vector<off_t> off_deltas; //offset[1]-offset[0], offset[2]-offset[1], ... get from entry_buf
    off_deltas.clear();
    pre_l_offset = 0;
    cur_l_offset = 0;
    
    for ( i = 0 ; i < bufsize ; i++ ) {
        if (!getNextEntry( h_entry, idx_file )) {
            //failed to get next entry
            break;
        }
       
        //Debug
        //Filter out only Proc 0
        if ( h_entry.Proc != 0 ) {
            continue;
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
        //cout << (*iter).Logical_offset << " ";
        printf("%lld ", (*iter).Logical_offset );
    }
    cout << endl;
    cout << "==================================================" << endl; 
    vector<off_t>::iterator iter2;
    for (iter2 = off_deltas.begin() ; iter2 != off_deltas.end() ; iter2++ ) {
        cout << (*iter2) << " ";
    }
    cout << endl;
    
    cout << "end of bufferEntries" << endl;
    return entry_buf;
}

