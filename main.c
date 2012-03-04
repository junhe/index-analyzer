
#include <iostream>
using namespace std;

#include "idxanalyzer.h"

int main(int argc, char ** argv)
{
    IdxSignature mysig;
    
    mysig.openTraceFile("trace.txt");
    mysig.bufferEntries();

    /*
    Tuple a(3,5,6);
    Tuple b(3,5,4);
    cout << "a==b" << (a == b) << endl;
    */
    
    cout<<"hello"<<endl;
    return 0;
}

