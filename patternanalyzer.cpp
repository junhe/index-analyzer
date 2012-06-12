
#include "patternanalyzer.h"

#include <sstream>


namespace MultiLevel {
   
   PatternBlock buildDeltaBlock( PatternBlock &pblock )
   {
       vector<PatternHeadUnit>::iterator it;
       PatternBlock deltas;
       for ( it = pblock.block.begin();
             it != pblock.block.end();
             it++ ) 
       {
           if ( it != pblock.block.begin() ) {
               PatternHeadUnit phu;
               phu.init = it->init - (it-1)->init;
               deltas.block.push_back( phu );
           }
       }
       return deltas;
   }



   // input: many PatternUnit (just offset, or PatternHeadUnit)
   // output: a PatternChunk describing the pattern:
   //            with two blocks if pattern found
   //            with only one if not found
   PatternChunk PatternAnalyzer::generatePatterns( PatternBlock &pblock )
   {

   }


   PatternHeadUnit::PatternHeadUnit (off_t x)
       :init(x)
   {
   }


   string PatternHeadUnit::show()
   {
       ostringstream oss;
       oss << init << ",";
       oss << "(";
       vector<off_t>::iterator it;
       for ( it = seq.begin();
             it != seq.end();
             it++ )
       {
           oss << *it ;
           if ( it + 1 != seq.end() ) {
               // not the last one
               oss << ",";
           }
       }
       oss << ")";
       return oss.str();
   }





}


