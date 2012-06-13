#ifndef __patternanalyzer_h__
#define __patternanalyzer_h__

#include <string>
#include <vector>
#include <iostream>

using namespace std;

namespace MultiLevel {

    class PatternAbstract {
        /*
        virtual string show() = 0;
        virtual bool follows( PatternAbstract &other ) = 0;
        virtual string serialize() = 0;
        virtual string deSerialize() = 0;
        */
    };


    class DeltaNode: public PatternAbstract {
        public:
            DeltaNode()
                :cnt(1)
            {}

            int cnt; // how many times the node repeats
            vector<DeltaNode *> children;
            vector<off_t> elements;       // one of sizes of 
                                          // children or elements must be 0
            bool isLeaf() const;
            string show() const;
    };


    ///////////////////////////////////////////////////////////
    // Pattern Unit
    ///////////////////////////////////////////////////////////
    class PatternUnitAbstract: public PatternAbstract
    {
    };


    class PatternUnit: public PatternAbstract
    {
        public:
            PatternUnit()
                :cnt(0)
            {}
            vector<off_t> seq;
            int cnt;
            string show();
    };

    class PatternHeadUnit: public PatternUnit
    {
        public:
            PatternHeadUnit ()
                :init(0)
            {}
            PatternHeadUnit (off_t x);
            off_t init;
            string show();
    };

    class PatternInitUnit: public PatternUnitAbstract
    {
        public:
            off_t init;
    };

    ///////////////////////////////////////////////////////////

    class PatternBlock: public PatternAbstract 
    {
        public:
            vector<PatternHeadUnit> block;
            string show();
    };

    // A pattern chunk is a pattern with multi levels
    class PatternChunk: public PatternAbstract
    {
        public:
            vector<PatternBlock> chunk;
    };

    class PatternChunkList: public PatternAbstract
    {
        public:
            vector<PatternChunk> list; 
    };
    
    class PatternEntry: public PatternAbstract
    {
        public:
            pid_t original_chunk_id;
            pid_t new_chunk_id;
            PatternChunk logical_offset;
            PatternChunk length;
            PatternChunk physical_offset;
    };

    class PatternEntryList: public PatternAbstract
    {
        public:
            vector<PatternEntry> list;
    };

    class PatternAnalyzer 
    {
        public:
            PatternChunk generatePatterns( PatternBlock &pblock );
    };

}


#endif
