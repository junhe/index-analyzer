#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>

#include "idxanalyzer.h"

using namespace std;

vector<IdxEntry> bufferEntries(ifstream &idx_file,
                               vector<off_t> &off_deltas);

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    ifstream idx_file;
    vector<IdxEntry> entry_buf;
    vector<off_t> off_deltas;

    idx_file.open("trace.txt");
    if (idx_file.is_open()) {
        cout << "file is open." << endl;
    } else {
        cout << "file is not open." << endl;
    }

    entry_buf = bufferEntries(idx_file, off_deltas);
    cout << "after bufferEntries" << endl;
    //mysig.discoverPattern( off_deltas );
    int proc;
    for ( proc = 0 ; proc < 64 ; proc++ ) {
        mysig.generateIdxSignature(entry_buf, proc);
    }

    cout<<"End of the program"<<endl;
    return 0;
}

bool getNextEntry( IdxEntry &idx_entry, ifstream &idx_file )
{
    string line;

    if ( !idx_file.is_open() ) {
        cout << "Trace file is not opened." << endl;
        fflush(stdout);
        return false;
    }

    if ( idx_file.eof() ) {
        cout << "The file is not good for reading" << endl;
        fflush(stdout);
        return false;
    }

    getline(idx_file, line);

    vector<string> tokens;
    vector<string>::iterator iter;
    istringstream iss(line);
    copy(istream_iterator<string>(iss),
            istream_iterator<string>(),
            back_inserter<vector<string> >(tokens));

    idx_entry.Proc = atoi( tokens[0].c_str() );
    idx_entry.ID = (tokens[1] == string("w")) ? ID_WRITE:ID_READ;
    sscanf( tokens[2].c_str(), "%lld", &idx_entry.Logical_offset);
    sscanf( tokens[3].c_str(), "%lld", &idx_entry.Length);
    idx_entry.Begin_timestamp = atof( tokens[4].c_str() );
    idx_entry.End_timestamp = atof( tokens[5].c_str() );
    sscanf( tokens[6].c_str(), "%lld", &idx_entry.Logical_tail);
    sscanf( tokens[8].c_str(), "%lld", &idx_entry.Physical_offset);
    return true;
}

vector<IdxEntry> bufferEntries(ifstream &idx_file,
                               vector<off_t> &off_deltas
                              )
{
    //cout << "i am bufferEntries()" << endl;
    IdxEntry h_entry;
    IdxEntry &idx_entry = h_entry;
    vector<IdxEntry> entry_buf;
    int bufsize = 100000 ;
    int i;
    off_t pre_l_offset, cur_l_offset; //logical offset
    

    //init the offset deltas
    off_deltas.clear();
    pre_l_offset = 0;
    cur_l_offset = 0;
    
    cout << "before for" << endl;
    for ( i = 0 ; i < bufsize && idx_file.good(); i++ ) {
        cout << i << ".";
        fflush(stdout);
        /*
        if (!getNextEntry( h_entry, idx_file )) {
            //failed to get next entry
            break;
        }
        */
        string line;
        if ( !getline(idx_file, line).good() ) {
            break;
        }

        vector<string> tokens;
        vector<string>::iterator iter;
        istringstream iss(line);
        copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter<vector<string> >(tokens));

        idx_entry.Proc = atoi( tokens[0].c_str() );
        idx_entry.ID = (tokens[1] == string("w")) ? ID_WRITE:ID_READ;
        sscanf( tokens[2].c_str(), "%lld", &idx_entry.Logical_offset);
        sscanf( tokens[3].c_str(), "%lld", &idx_entry.Length);
        idx_entry.Begin_timestamp = atof( tokens[4].c_str() );
        idx_entry.End_timestamp = atof( tokens[5].c_str() );
        sscanf( tokens[6].c_str(), "%lld", &idx_entry.Logical_tail);
        sscanf( tokens[8].c_str(), "%lld", &idx_entry.Physical_offset);
        

        pre_l_offset = cur_l_offset;
        cur_l_offset = h_entry.Logical_offset;
        if ( i > 0 ) {
            //push offset[i]-offset[i-1]
            off_deltas.push_back( cur_l_offset - pre_l_offset );
        }

        entry_buf.push_back(h_entry);
    }
    cout << "after for" << endl;
    /*
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
    */
    return entry_buf;
}

