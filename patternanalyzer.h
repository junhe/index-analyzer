#ifndef __patternanalyzer_h__
#define __patternanalyzer_h__

#include <string>

namespace MultiLevel {

    class PatternAbstract {
        virtual string show() = 0;
    }

    class PatternUnit: public PatternAbstract
    {
        off_t init;
        vector<off_t> seq;
        int cnt;
    };

    class PatternBlock: public PatternAbstract 
    {
        vector<PatternUnit> blocks;
    };

    class PatternChunk: public PatternAbstract
    {
        vector<PatternBlock> chunks;
    };
    
    class PatternEntry: public PatternAbstract
    {
        pid_t original_chunk_id;
        pid_t new_chunk_id;
        PatternChunk logical_offset;
        PatternChunk length;
        PatternChunk physical_offset;
    };

    class PatternList: public PatternAbstract
    {
        vector<PatternEntry> list;
    }
}


#endif
