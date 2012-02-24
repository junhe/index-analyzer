
#include <iostream>
using namespace std;

#include "idxanalyzer.h"

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    IdxEntry ientry;
    
    mysig.openTraceFile("trace.txt");
    mysig.getNextEntry(ientry);

    cout<<"hello"<<endl;
    return 0;
}

