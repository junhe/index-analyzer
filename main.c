
#include <iostream>
using namespace std;

#include "idxanalyzer.h"

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    
    mysig.openTraceFile("trace.txt");
    mysig.bufferEntries();
    cout<<"hello"<<endl;
    return 0;
}

