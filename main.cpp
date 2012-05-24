#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>

#include "idxanalyzer.h"

using namespace std;



vector<HostEntry> bufferEntries(ifstream &idx_file, int &maxproc);

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

    int maxproc;
    entry_buf = bufferEntries(idx_file, maxproc);
    
    int proc;
    for ( proc = 0 ; proc <= maxproc ; proc++ ) {
        sig_entrylist.append(mysig.generateIdxSignature(entry_buf, proc));
    }
    
    sig_entrylist.show();

    cout<<"End of the program"<<endl;
    return 0;
}


void deleteSubStr( string del, string &line ) 
{
    size_t found;
    
    found = line.find(del);
    while (found != string::npos) 
    {
        line.erase(found, del.size());  
        found = line.find(del);
    }
}

vector<HostEntry> bufferEntries(ifstream &idx_file, int &numofproc)
{
    //cout << "i am bufferEntries()" << endl;
    HostEntry h_entry;
    HostEntry &idx_entry = h_entry;
    vector<HostEntry> entry_buf;
    int bufsize = 4194305 ;
    int i;

    numofproc = 0;
    for ( i = 0 ; i < bufsize && idx_file.good(); i++ ) {
        string line;
        if (  !idx_file.good()
              || !getline(idx_file, line).good() 
              || line.size() < 1)
        {
            break;
        }

        if ( line[0] == '#' ) {
            cout << "skiping---" << line << endl;
            continue;
        }
        
        deleteSubStr( "[", line );
        deleteSubStr( "]", line );
        deleteSubStr( ". ", line );
        //cout << line << endl;

        vector<string> tokens;
        vector<string>::iterator iter;
        istringstream iss(line);
        copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter<vector<string> >(tokens));

        idx_entry.id = atoi( tokens[0].c_str() );
        if ( idx_entry.id > numofproc ) {
            numofproc = idx_entry.id;
        }


        stringstream convert(tokens[2]);
        if ( !(convert >> idx_entry.logical_offset) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[3]);
        if ( !(convert >> idx_entry.length) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[8]);
        if ( !(convert >> idx_entry.physical_offset) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }


        //sscanf( tokens[2].c_str(), "%lld", &idx_entry.logical_offset);
        //sscanf( tokens[3].c_str(), "%lld", &idx_entry.length);
        //idx_entry.begin_timestamp = atof( tokens[4].c_str() );
        //idx_entry.end_timestamp = atof( tokens[5].c_str() );
        //sscanf( tokens[6].c_str(), "%lld", &(idx_entry.logical_tail));
        //sscanf( tokens[8].c_str(), "%lld", &(idx_entry.physical_offset));


        entry_buf.push_back(h_entry);
    }
return entry_buf;
}

