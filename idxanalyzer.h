#ifndef __idxanalyzer_h__
#define __idxanalyzer_h__

#include <vector>
#include <string>
#include <iostream>
#include <fstream>
using namespace std;


class IdxEntry {
    public:
        int Proc;
#define ID_WRITE 0
#define ID_READ  1
        int ID; //either ID_WRITE or ID_READ
        off_t Logical_offset;
        off_t Length;
        double Begin_timestamp;
        double End_timestamp;
        off_t Logical_tail;
        int ID_2;
        off_t Chunk_offset;
};


class IdxSigUnit {
    off_t init_offset;
    vector <off_t> offset_stride;
    off_t init_length;
    vector <off_t> length_stride;
};

// Each index has its own signature
class IdxSignature {
    public:
        IdxSignature() {}
        bool openTraceFile( const char *fpath );
        bool getNextEntry(IdxEntry &idx_entry);
        bool bufferEntries(); // read some entries from trace file and
                              // put them in sigList
    private:
        ifstream idx_file;
        vector<IdxSigUnit> sig_list;
};


#endif

