#include <iostream>
#include <sstream>
#include <iterator>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>

#include "idxanalyzer.h"
#include "patternanalyzer.h"

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
        //cerr << "map file is open: " << argv[1]  << endl;
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
            //cerr << "index file is open" << endl;
        }
    }


    int maxproc;
    entry_buf = bufferEntries(idx_file, maxproc);
  

    //cout << "entry buffered" << endl;
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

    //cout << "old size: " << entry_buf.size() << endl;
    //cout << "compressed size " << compressed.size() << endl;
    entry_buf.clear();

    MultiLevel::PatternCombo globalcombo;
    int totalsize = 0;
    for ( proc = 0 ; proc <= maxproc ; proc++ ) {
        //sig_entrylist.append(mysig.generateIdxSignature(entry_buf, proc));
        MultiLevel::PatternCombo combo, combo2;
        combo.buildFromHostEntries(compressed, proc);
        string tmpbuf = combo.serialize();
        //cout << "serid size: " << tmpbuf.size() << endl;
        totalsize += tmpbuf.size();
        globalcombo.append( combo );
        //cout << "APPENDED globalcombo:\n" << globalcombo.show() << endl;
    }
  
    //cout << "expanded?:" << globalcombo.expandBadCompression() << endl;
    
    globalcombo.logical_offset.compressMyInit(6);
    globalcombo.logical_offset.compressDeltaChildren(10);
    globalcombo.length.compressMyInit(6);
    globalcombo.length.compressDeltaChildren(10);
    globalcombo.physical_offset.compressMyInit(6);
    globalcombo.physical_offset.compressDeltaChildren(10);
    
    string combuf = globalcombo.serialize();
    //cout << "FINAL: " << globalcombo.show() << endl;
    cout << compressed.size()*48 << " " << combuf.size() << endl;


    /*
    globalcombo.logical_offset.compressMyInit(3);
    globalcombo.logical_offset.compressDeltaChildren(10);
    cout << "totalsize: " << globalcombo.serialize().size() << endl;
    cout << globalcombo.show() << endl;
    globalcombo.length.compressMyInit(6);
    globalcombo.length.compressDeltaChildren(10);
    globalcombo.physical_offset.compressMyInit(6);
    globalcombo.physical_offset.compressDeltaChildren(10);

    cout << "COMPRESSED:" << globalcombo.show() << endl;
    cout << "compressed totalsize: " << globalcombo.serialize().size() << endl;
    
    globalcombo.logical_offset.compressMyInit(6);
    globalcombo.logical_offset.compressDeltaChildren( 20 );
   
    cout << "COMPRESSED2:" << globalcombo.show() << endl;
    cout << "compressed2 totalsize: " << globalcombo.serialize().size() << endl;

    globalcombo.logical_offset.compressMyInit(6);
    globalcombo.logical_offset.compressDeltaChildren( 40 );
    
    cout << "COMPRESSED3:" << globalcombo.show() << endl;
    cout << "compressed3 totalsize: " << globalcombo.serialize().size() << endl;

    globalcombo.logical_offset.compressMyInit(10);
    globalcombo.logical_offset.compressDeltaChildren( 20 );
    
    cout << "COMPRESSED3:" << globalcombo.show() << endl;
    cout << "compressed3 totalsize: " << globalcombo.serialize().size() << endl;

    globalcombo.logical_offset.compressMyInit(10);
    globalcombo.logical_offset.compressDeltaChildren( 20 );
    
    cout << "COMPRESSED3:" << globalcombo.show() << endl;
    cout << "compressed3 totalsize: " << globalcombo.serialize().size() << endl;

    globalcombo.logical_offset.compressMyInit(20);
    globalcombo.logical_offset.compressDeltaChildren( 20 );
    
    cout << "COMPRESSED3:" << globalcombo.show() << endl;
    cout << "compressed3 totalsize: " << globalcombo.serialize().size() << endl;

    int i;
    int sizeofcombo = globalcombo.getNumOfVal();
    cout << "size of compressed:" << compressed.size() << endl;
    cout << "sizeofcombo:" << sizeofcombo << endl;
    for ( i = 0 ; i < sizeofcombo ; i++ ) {
        cout << globalcombo.logical_offset.recoverPos(i) << " : "
             << compressed[i].logical_offset << " | "
             << globalcombo.length.recoverPos(i) << " : "
             << compressed[i].length << " | "
             << globalcombo.physical_offset.recoverPos(i) << " : "
             << compressed[i].physical_offset 
             << "   " << (globalcombo.logical_offset.recoverPos(i) == compressed[i].logical_offset)
             << endl; 
    }

    */

    //cout << sig_entrylist.show();

    if ( argc == 3 ) {
        sig_entrylist.saveToFile(fd);
        cerr << "index saved to " << argv[2] << endl;
        close(fd);
    }

    //cerr<<"End of the program"<<endl;
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

void replaceSubStr( string del, string newstr, string &line ) 
{
    size_t found;
    
    found = line.find(del);
    while (found != string::npos) 
    {
        line.replace(found, del.size(), newstr);  
        found = line.find(del);
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
            //cerr << "skipping---" << line << endl;
            continue;
        }
        
        replaceSubStr( "[", " ", line );
        replaceSubStr( "]", " ", line );
        replaceSubStr( ". ", " ", line );
        //cerr << line << endl;

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

