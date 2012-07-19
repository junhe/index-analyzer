/* 
 * Usage: mpirun -np num-of-proc-in-map-file ./xmain mapfile outputfile
 */

#include <iostream>
#include <fstream>
#include <sstream>
#include <iterator>
#include <stdlib.h>
#include <mpi.h>
#include <vector>
#include <assert.h>

class HostEntry
{
    public:
        off_t  logical_offset;
        off_t  physical_offset;  // I tried so hard to not put this in here
        // to save some bytes in the index entries
        // on disk.  But truncate breaks it all.
        // we assume that each write makes one entry
        // in the data file and one entry in the index
        // file.  But when we remove entries and
        // rewrite the index, then we break this
        // assumption.  blech.
        size_t length;
        double begin_timestamp;
        double end_timestamp;
        pid_t  id;      // needs to be last so no padding
};

using namespace std;

int rank, size;
MPI_Status stat;
double openTime = 0, rwTime = 0, closeTime = 0;
double start, end;
off_t totalBytes = 0;
char mode;

void bufferEntries(ifstream &idx_file, MPI_File fh);

int main(int argc, char ** argv)
{
    int rc;
    ifstream idx_file;
    MPI_File fh;

    MPI_Init (&argc, &argv);/* starts MPI */
    MPI_Comm_rank (MPI_COMM_WORLD, &rank);/* get current process id */
    MPI_Comm_size (MPI_COMM_WORLD, &size);/* get number of processes */

    if ( argc != 4 ) {
        if ( rank == 0 ) {
            printf("Usage: mpirun -np num-of-proc-in-map-file %s " 
                   "mapfile output-file r/w\n", argv[0] );
        }
        MPI_Finalize();
        return 0;
    }

    mode = argv[3][0];
    if ( rank == 0 ) {
        if ( mode == 'w' ) {
            cout << "To Write File" << endl;
        } else {
            cout << "To Read File" << endl;
        }  
    }
    printf( "Hello from process %d of %d\n", rank, size );

    //all ranks open a file for writing together
    start = MPI_Wtime();
    if ( mode == 'w' ) {
        rc = MPI_File_open( MPI_COMM_WORLD, argv[2], 
                      MPI_MODE_CREATE | MPI_MODE_WRONLY, MPI_INFO_NULL, &fh );
    } else {
        rc = MPI_File_open( MPI_COMM_WORLD, argv[2], 
                       MPI_MODE_RDONLY, MPI_INFO_NULL, &fh );
    }
    end = MPI_Wtime();
    openTime = end - start;
    
    assert(rc == MPI_SUCCESS);


    if ( rank == 0 ) {
        // Rank 0 opens map file
        idx_file.open(argv[1]);
        if (idx_file.is_open()) {
            cout << "map file is open: " << argv[1]  << endl;
        } else {
            cout << "file is not open." << argv[1] << endl;
            exit(-1);
        }
        bufferEntries(idx_file, fh);
        idx_file.close();
    } else {
        // other ranks just receive offset and length from rank 0
        //static int mywrites = 0;
        while (1) {
            int flag = 0; //1: has entry comming, 0:no entry comming
            int ret;
            rc = MPI_Recv( &flag, 1, MPI_INT, 0, 1, MPI_COMM_WORLD, &stat );
            if ( flag == 1 ) {
                // entry is comming
                HostEntry entry;
                rc = MPI_Recv( &entry, sizeof(HostEntry), MPI_CHAR, 0, 1, 
                               MPI_COMM_WORLD, &stat );
                
                string buf(entry.length, 'a'+rank);
                assert(buf.size()==entry.length);
                
                start = MPI_Wtime();
                if ( mode == 'w' ) {
                    ret = MPI_File_write_at(fh, entry.logical_offset,
                            (void *) buf.c_str(),  entry.length, MPI_CHAR, &stat);
                } else {
                    ret = MPI_File_read_at(fh, entry.logical_offset,
                            &buf[0],  entry.length, MPI_CHAR, &stat);
                }
                end = MPI_Wtime();
                rwTime += end - start;

                assert(ret == MPI_SUCCESS);
                
                // Get bytes written
                int cnt;
                MPI_Get_count( &stat, MPI_CHAR, &cnt );
                totalBytes += cnt;
                
                /*  
                mywrites++;
                if ( mywrites % 1024 == 0 ) {
                    cout <<".";
                    fflush(stdout);
                }
                */

                /*
                cout << "[" << rank << "] [" << entry.id << "]: " 
                     << entry.logical_offset << ", " 
                     << entry.physical_offset << ", "
                     << entry.length << endl;
                */

            } else {
                // no entry is coming. This rank is done.
                break;
            }
        }
    }
   
    cout<<"End of the program"<<endl;
    
    start = MPI_Wtime();
    MPI_File_close(&fh);
    end = MPI_Wtime();
    closeTime = end - start;

    cout << "Open Time: " << openTime << endl
         << "rw Time: " << rwTime << endl
         << "Close Time: " << closeTime << endl
         << "total bytes: " << totalBytes << endl;

    double totaltime = openTime + rwTime + closeTime;
    double aggTotalTime = 0;

    MPI_Reduce( &totaltime, &aggTotalTime, 1, 
                MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
    off_t aggTotalBytes = 0;
    MPI_Reduce( &totalBytes, &aggTotalBytes, 1,
                MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD );
    if ( rank == 0 ) {
        cout << "----------- Final Performance ----------" << endl;
        cout << "Total Time: " << aggTotalTime << endl
             << "Total Bytes: " << aggTotalBytes << endl;
        double bandwidth =aggTotalBytes/aggTotalTime;
        cout << "Bandwidth: " //<< bandwidth << "bytes/sec " 
             << bandwidth/(1024*1024) << " MB/sec" << endl;
    }


    MPI_Finalize();
    return 0;
}


void deleteSubStr( string del, string &line ) 
{
    size_t found;
    
    found = line.find(del);
    while (found != string::npos) 
    {
        line.erase(found, del.size());  
        found = line.find(del);
    }
}

void replaceSubStr( string del, string newstr, string &line, int startpos = 0 ) 
{
    size_t found;
    
    found = line.find(del, startpos);
    while (found != string::npos) 
    {
        line.replace(found, del.size(), newstr);  
        found = line.find(del, startpos);
    }
}


void bufferEntries(ifstream &idx_file, MPI_File fh)
{
    //cout << "i am bufferEntries()" << endl;
    HostEntry h_entry;
    HostEntry &idx_entry = h_entry;
    vector<HostEntry> entry_buf;
    int i,flag;

    int maxprocnum = 0;
    for ( i = 0 ; idx_file.good(); i++ ) {
        string line;
        if (  !idx_file.good()
              || !getline(idx_file, line).good() 
              || line.size() < 1)
        {
            break;
        }

        if ( line[0] == '#' ) {
            cout << "skiping---" << line << endl;
            continue;
        }
        
        replaceSubStr( "[", " ", line );
        replaceSubStr( "]", " ", line );
        replaceSubStr( ".", " ", line, 107 ); //107 is the byte # where chunk info starts
        //cout << line << endl;

        vector<string> tokens;
        vector<string>::iterator iter;
        istringstream iss(line);
        copy(istream_iterator<string>(iss),
                istream_iterator<string>(),
                back_inserter<vector<string> >(tokens));

        idx_entry.id = atoi( tokens[7].c_str() );
        if ( idx_entry.id > maxprocnum ) {
            maxprocnum = idx_entry.id;
        }
        if ( idx_entry.id >= size ) {
            fprintf(stderr, "num of proc is too small for this map. mapid:%d", idx_entry.id);
        }
        assert( idx_entry.id < size );

        stringstream convert(tokens[2]);
        if ( !(convert >> idx_entry.logical_offset) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[3]);
        if ( !(convert >> idx_entry.length) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }

        convert.clear();
        convert.str(tokens[8]);
        if ( !(convert >> idx_entry.physical_offset) ) {
            cout << "error on converting" << endl;
            exit(-1);
        }


        if ( h_entry.id == 0 ) {
            //if it is rank0's job, just do it
            //static int mywrites = 0;
            int ret;

            string buf(h_entry.length, 'a'+rank);
            //cout << buf << endl;
            assert(buf.size()==h_entry.length);
            
            start = MPI_Wtime();
            if ( mode == 'w' ) {
                ret = MPI_File_write_at(fh, h_entry.logical_offset, (void *)buf.c_str(), 
                                  h_entry.length, MPI_CHAR, &stat);
            } else {
                ret = MPI_File_read_at(fh, h_entry.logical_offset,
                        &buf[0],  h_entry.length, MPI_CHAR, &stat);
            }
            end = MPI_Wtime();
            rwTime += end - start;
           
            assert(ret == MPI_SUCCESS);
            
            // Get bytes written
            int cnt;
            MPI_Get_count( &stat, MPI_CHAR, &cnt );
            totalBytes += cnt;

            
            
            //mywrites++;
            
            /*
            cout << "[" << rank << "] [" << h_entry.id << "]: " 
                 << h_entry.logical_offset << ", " 
                 << h_entry.physical_offset << ", "
                 << h_entry.length << endl;
            */
            /* 
            if (mywrites % 1024 == 0) {
                cout <<".";
                fflush(stdout);
            }
            */
        } else {
            flag = 1;
            MPI_Send(&flag, 1, MPI_INT, h_entry.id, 1, MPI_COMM_WORLD);
            MPI_Send(&h_entry, sizeof(HostEntry), MPI_CHAR, h_entry.id, 1, 
                    MPI_COMM_WORLD);
        }
    }

    cout << "all entries in map are handled" << endl;
    flag = 0;
    int rankid;
    for ( rankid = 0 ; rankid <= maxprocnum ; rankid++ ) {
        MPI_Send(&flag, 1, MPI_INT, rankid, 1, MPI_COMM_WORLD);
    }
    return ;
}


