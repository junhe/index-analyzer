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
    off_t Init_offset;
    vector <off_t> Offset_stride;
    off_t Init_length;
    vector <off_t> Length_stride;
    int Req_count;
};

// Each index has its own signature
class IdxSignature {
    public:
        IdxSignature() {}
        bool openTraceFile( const char *fpath );
        bool getNextEntry(IdxEntry &idx_entry);
    private:
        ifstream idx_file;
};


#endif

