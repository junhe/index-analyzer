#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>

#include "idxanalyzer.h"

using namespace std;

#define off_t long long int


vector<HostEntry> bufferEntries(ifstream &idx_file);

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    ifstream idx_file;
    vector<HostEntry> entry_buf;
    IdxSigEntryList sig_entrylist;
    IdxSigEntryList sig_entrylist2;
    IdxSigEntry myentry;

    
    idx_file.open(argv[1]);
    if (idx_file.is_open()) {
        cout << "map file is open: " << argv[1]  << endl;
    } else {
        cout << "file is not open." << argv[1] << endl;
        exit(-1);
    }

    entry_buf = bufferEntries(idx_file);
    cout << "after bufferEntries" << endl;
    cout << "size is: " << entry_buf.size() << endl;;
    
    int proc;
    for ( proc = 0 ; proc < 64 ; proc++ ) {
        sig_entrylist.append(mysig.generateIdxSignature(entry_buf, proc));
    }
    
    sig_entrylist.show();

    cout<<"End of the program"<<endl;
    return 0;
}

vector<HostEntry> bufferEntries(ifstream &idx_file)
{
    //cout << "i am bufferEntries()" << endl;
    HostEntry h_entry;
    HostEntry &idx_entry = h_entry;
    vector<HostEntry> entry_buf;
    int bufsize = 4194305 ;
    int i;


    //cout << "before for" << endl;
    for ( i = 0 ; i < bufsize && idx_file.good(); i++ ) {
        string line;
        if ( !getline(idx_file, line).good() || line.size() < 8 ) {
            break;
        }

        vector<string> tokens;
        vector<string>::iterator iter;
        istringstream iss(line);
        copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter<vector<string> >(tokens));

        idx_entry.id = atoi( tokens[0].c_str() );
        //idx_entry.ID = (tokens[1] == string("w")) ? ID_WRITE:ID_READ;
        sscanf( tokens[2].c_str(), "%lld", &idx_entry.logical_offset);
        sscanf( tokens[3].c_str(), "%lld", &idx_entry.length);
        //idx_entry.begin_timestamp = atof( tokens[4].c_str() );
        //idx_entry.end_timestamp = atof( tokens[5].c_str() );
        //sscanf( tokens[6].c_str(), "%lld", &(idx_entry.logical_tail));
        //sscanf( tokens[8].c_str(), "%lld", &(idx_entry.physical_offset));


        entry_buf.push_back(h_entry);
    }
return entry_buf;
}

