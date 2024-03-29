#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "idxanalyzer.h"

using namespace std;



vector<HostEntry> bufferEntries(ifstream &idx_file, int &maxproc);

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    ifstream idx_file;
    int fd;
    vector<HostEntry> entry_buf;
    IdxSigEntryList sig_entrylist;

    if ( ! ( argc == 2 || argc == 3 ) ) {
        cerr << "Usage:" << endl
             << argv[0] << " map-file" << endl
             << "  Just print the complex pattern to stdout" << endl
             << argv[0] << " map-file index-output" << endl
             << "  Print out the pattern to stdout and write it to index-output file" << endl;
        exit(-1);
    }


    idx_file.open(argv[1]);
    if (idx_file.is_open()) {
        cerr << "map file is open: " << argv[1]  << endl;
    } else {
        cerr << "map file is not open." << argv[1] << endl;
        exit(-1);
    }

    if ( argc == 3 ) {
        fd = open( argv[2], O_WRONLY | O_CREAT, 
                   S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH );
        if ( fd == -1 ) {
            cerr << "index file is not open" << endl;
            exit(-1);
        } else {
            cerr << "index file is open" << endl;
        }
    }


    int maxproc;
    entry_buf = bufferEntries(idx_file, maxproc);
    
    int proc;
    // compress contiguous
    vector<HostEntry> compressed;
    for ( proc = 0 ; proc <= maxproc ; proc++ ) {
        vector<HostEntry>::iterator it;
        for ( it = entry_buf.begin() ;
              it != entry_buf.end() ;
              it++ ) 
        {
            if ( it->id != proc )
                continue;

            if ( !compressed.empty() &&
                 compressed.back().id == it->id &&
                 compressed.back().logical_offset + 
                    compressed.back().length == it->logical_offset ) 
            {
                compressed.back().length += it->length;
            } else {
                compressed.push_back( *it );
            }
        }
    }

    cout << "old size: " << entry_buf.size() << endl;
    cout << "compressed size " << compressed.size() << endl;
    entry_buf.clear();

    for ( proc = 0 ; proc <= maxproc ; proc++ ) {
        sig_entrylist.append(mysig.generateIdxSignature(compressed, proc));
    }
    
    cout << sig_entrylist.show();

    if ( argc == 3 ) {
        sig_entrylist.saveToFile(fd);
        cerr << "index saved to " << argv[2] << endl;
        close(fd);
    }

    cerr<<"End of the program"<<endl;
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

void replaceSubStr( string del, string newstr, string &line, int startpos = 0 ) 
{
    size_t found;
    
    found = line.find(del, startpos);
    while (found != string::npos) 
    {
        line.replace(found, del.size(), newstr);  
        found = line.find(del, startpos);
    }
}

vector<HostEntry> bufferEntries(ifstream &idx_file, int &numofproc)
{
    //cerr << "i am bufferEntries()" << endl;
    HostEntry h_entry;
    HostEntry &idx_entry = h_entry;
    vector<HostEntry> entry_buf;
    int i;

    numofproc = 0;
    for ( i = 0 ; idx_file.good(); i++ ) {
        string line;
        if (  !idx_file.good()
              || !getline(idx_file, line).good() 
              || line.size() < 1)
        {
            break;
        }

        if ( line[0] == '#' ) {
            cerr << "skiping---" << line << endl;
            continue;
        }
        
        replaceSubStr( "[", " ", line );
        replaceSubStr( "]", " ", line );
        replaceSubStr( ".", " ", line, 107 ); //107 is the byte # where chunk info starts
        //cerr << line << endl;

        vector<string> tokens;
        vector<string>::iterator iter;
        istringstream iss(line);
        copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter<vector<string> >(tokens));
        assert( tokens.size() == 9 );
        idx_entry.id = atoi( tokens[0].c_str() );

        if ( idx_entry.id > numofproc ) {
            numofproc = idx_entry.id;
        }


        stringstream convert(tokens[2]);
        if ( !(convert >> idx_entry.logical_offset) ) {
            cerr << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[3]);
        if ( !(convert >> idx_entry.length) ) {
            cerr << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[8]);
        if ( !(convert >> idx_entry.physical_offset) ) {
            cerr << "error on converting" << endl;
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

