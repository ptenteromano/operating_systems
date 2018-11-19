/*
* Phil Tenteromano
* 11/15/2018
* Bounder Buffer lab
* Operating Systems
* CISC 3595
*
* Using two threads, semaphores, mutex
* Syncronize threads to share a circular buffer
* Simulates a multithread 'cat' linux command

* Need to link with pthread on compilation:
* g++ lab3.cpp -lpthread
*
* Example execution call: `./a.out 10 test.txt`
*
* Only tested on linux systems - other OS's may fail 
* 
*/

#include <iostream>
#include <string>
#include <fstream>
#include <pthread.h>
#include <semaphore.h>

using namespace std;

class SharedBuffer {
	// variables shared by the threads
    private:
        int size;
        string file;
        string *buffer;

        // circular buffer indices
        int write;
        int read;
        
        // semaphores and mutex
        sem_t mtx_lock;
        sem_t item;
        sem_t blank;
        
        // termination condtions
        bool eof;
        int linesWritten;

    public:
        // constructor takes in cmd line arguments and inits globals
        SharedBuffer(int size, string fileName) {
            if (size > 0)
                this->size = size;
            else
                this->size = 5;
            buffer = new string[this->size];
            write = 0;
            read = 0;
            file = fileName;
            eof = false;
            linesWritten = 0;
            
            // init the semaphores and binary mutex
            sem_init(&mtx_lock, 0 , 1);
            sem_init(&item, 0, 0);
            sem_init(&blank, 0, this->size);
        }

        // producer thread
        void* producer(void*) {
            
            ifstream ifs(file, ifstream::in);
            string line;

            // open and process the file
            if (ifs.is_open()) {
                while( getline(ifs,line)) {
                    // wait on a 'blank' in the buffer
                    sem_wait(&blank);
                        sem_wait(&mtx_lock);
                            // acquire mutex and enter Critical Section
                            // load line into the buffer, shift write index, tally a write
                            buffer[write] = line;
                            write = (write + 1) % size;
                            linesWritten++;
                        sem_post(&mtx_lock);
                    // signal consumer there is something in the buffer
                    sem_post(&item);
                }
                // test for end of file, producer then terminates
                if (ifs.eof())
                    eof = true;
            }
            // error check and exit completely
            else {
                cout << "Could not open file: " << '"' << file << '"' << endl;
                delete []buffer;
                exit(1);
			}

			ifs.close();

            return 0;
        }
        // consumer thread
        void* consumer(void*) {
            
            int linesRead = 0;
            cout<< "File Contents: " << endl << endl;

            while(true) {
                // wait for an item in the buffer
                sem_wait(&item);
                    sem_wait(&mtx_lock);
                        // acquire mutex, output data, shift read index, tally a read
                        cout << buffer[read] << endl;
                        read = (read + 1) % size;
                        linesRead++;
                    sem_post(&mtx_lock);
                // signal an empty space in the buffer
                sem_post(&blank);
                
                // check if consumer should terminate
                // since producer will end first and signal eof,
                // also check if indices are equal and tallies are equal
                if (eof && (read == write) && (linesRead == linesWritten)) {
                    cout<<"Total lines read: " << linesRead << endl;
                    delete []buffer;
                    break;
				}
            };

            return 0;
        }

        // needed to properly cast class methods to thread
        static void *prod_helper(void *context) {
            return ((SharedBuffer*)context)->producer(NULL);
        }

        static void *cons_helper(void *context) {
            return ((SharedBuffer*)context)->consumer(NULL);
        }
};

int main(int argc, char *argv[]) {

    int size;
    string getFile;
    
    // exit if program not used properly
    if (argc != 3) {
        cout << "Need the buffer size and a filename!" << endl;
        exit(1);
    }
    // if args are correct, parse for an integer
    try {
        size = stoi(argv[1], nullptr, 0);
        getFile = argv[2];
        cout << "Buffer Size: " << size << endl
            << "File name: \"" << getFile << '"' << endl;
    }
    catch (...) {
        // force exit if unable to get buffer size
        cout << "First argument needs to be an integer!" << endl;
        exit(1);
    }

    // program starts - initalize object
    SharedBuffer prod_consum(size, getFile);
    
    // create and start threads using object methods
    pthread_t prod, cons;

    pthread_create(&prod, NULL, &SharedBuffer::prod_helper, &prod_consum);
    pthread_create(&cons, NULL, &SharedBuffer::cons_helper, &prod_consum);
    
    // bring producer and consumer back into main() thread and exit properly
    void* result;
    pthread_join(prod, &result);
    pthread_join(cons, &result);

    return 0;
}
