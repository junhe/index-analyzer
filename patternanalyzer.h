#ifndef __patternanalyzer_h__
#define __patternanalyzer_h__

namespace MultiLevel {

    class PatternUnit
    {
        off_t init;
        vector<off_t> seq;
        int cnt;
    };

    class PatternBlock 
    {
        vector<PatternUnit> blocks;
    };

    class PatternChunk
    {
        vector<PatternBlock> chunks;
    };
    

}


#endif
