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

    ///////////////////////////////////////////////////////////
    // Pattern Unit
    ///////////////////////////////////////////////////////////
    class PatternUnitAbstract: public PatternAbstract
    {
    };

    class PatternUnit: public PatternAbstract
    {
        public:
            vector<off_t> seq;
            int cnt;
    };

    class PatternHeadUnit: public PatternUnit
    {
        public:
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
    };

    class PatternChunk: public PatternAbstract
    {
        public:
            vector<PatternBlock> chunks;
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

    class PatternList: public PatternAbstract
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
