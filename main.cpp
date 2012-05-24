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
    idxfile::EntryList pb_entrylist;
    IdxSigEntryList sig_entrylist;
    IdxSigEntryList sig_entrylist2;
    IdxSigEntry myentry;

    IdxSigUnit myunit;
    myunit.init = 2;
    myunit.cnt = 3;
    myunit.seq.push_back(4);
    myunit.seq.push_back(5);

    IdxSigUnit myunit2;
    myunit2.deSerialize( myunit.serialize() );
    
    SigStack<IdxSigUnit> stack;
    stack.push(myunit);
    stack.push(myunit);
    
    stack.deSerialize( stack.serialize() );

    myentry.proc = 8848;
    myentry.logical_offset = myunit;
    myentry.length.push(myunit);
    myentry.length.push(myunit);
    myentry.physical_offset.push(myunit);
    myentry.physical_offset.push(myunit);
    myentry.physical_offset.push(myunit);

    vector<IdxSigEntry> alist;
    alist.push_back(myentry);
    alist.push_back(myentry);

    sig_entrylist.append(alist);
    sig_entrylist.append(alist);
    myentry.physical_offset.push(myunit);
    alist.push_back(myentry);
    
    sig_entrylist.append(alist);
    sig_entrylist.show();

    string tmpbuf = sig_entrylist.serialize();
    cout << "just before deSerialize()" << endl;
    sig_entrylist2.deSerialize(tmpbuf);
    cout << "just after deSerialize()" << endl;
    sig_entrylist2.show();

    
    string tmpbuf2 = sig_entrylist2.serialize();
    
    cout << (tmpbuf.compare(tmpbuf2)) ;

    return 0;

/*
    vector<off_t> orig;
    vector<off_t> deltas;
    int i,j;
    for ( i = 0; i < 3; i++ )
    {
        orig.push_back(1);
        orig.push_back(3);
        orig.push_back(8);
    }
    deltas = buildDeltas(orig);
     
    cout <<"orig:"<<orig.size()<<" deltas:"<<deltas.size()<<endl;
    IdxSignature tmp; 
    SigStack<IdxSigUnit> sigstack = tmp.discoverSigPattern(deltas, orig);
    sigstack.show();
    return 0;
*/


    idx_file.open(argv[1]);
    if (idx_file.is_open()) {
        cout << "file is open: " << argv[1]  << endl;
    } else {
        cout << "file is not open." << argv[1] << endl;
    }

    entry_buf = bufferEntries(idx_file, off_deltas);
    cout << "after bufferEntries" << endl;
    cout << "size is: " << entry_buf.size() << endl;;
    //mysig.discoverPattern( off_deltas );
    int proc;
    for ( proc = 0 ; proc < 64 ; proc++ ) {
        sig_entrylist.append(mysig.generateIdxSignature(entry_buf, proc));
    }
    cout << "let the show begin()\n";
    sig_entrylist.show();
    //sig_entrylist.siglistToPblist(sig_entrylist.list, pb_entrylist);
    //cout << pb_entrylist.DebugString();
    //sig_entrylist.saveToFile("hahafile");
    //sig_entrylist.readFromFile("hahafile");
    //cout << sig_entrylist.pb_list.DebugString();
    return 1;
    fstream output("myfile", ios::out | ios::trunc | ios::binary);
    if ( !pb_entrylist.SerializeToOstream(&output) ) {
        cerr<<"failed to write to myfile."<<endl;
        return -1;
    } else {
        cout<<"Write to myfile: OK"<<endl;
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
    int bufsize = 4194305 ;
    int i;
    off_t pre_l_offset, cur_l_offset; //logical offset


    //init the offset deltas
    off_deltas.clear();
    pre_l_offset = 0;
    cur_l_offset = 0;

    //cout << "before for" << endl;
    for ( i = 0 ; i < bufsize && idx_file.good(); i++ ) {
        //cout << i << ".";
        //fflush(stdout);
        /*
           if (!getNextEntry( h_entry, idx_file )) {
        //failed to get next entry
        break;
        }
        */
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
    //cout << "after for" << endl;
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

