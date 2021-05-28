/*
David Bono

Create a clone of

$ cat /dev/random, in the language of your choice, generating your own entropy

One of the requirements for this prompt is to "[generate] your own entropy". I was 
going to attempt to read certain statistics from the linux kernel, like /proc/interrupts,
along with the current time, network information, etc, However, these debatably wouldn't be
"generating my own entropy". 

My implementation uses multiple(16) psuedo-random number generators, running in concurrent threads, 
constantly writing to the same variable with no locking. Essentially, I'm intentionally
creating a race condition, and abusing the uncertainty of which thread will execute in 
which order to achieve random number generation. In terms of security, this could easily 
be broken if you could predict in which order the threads are processed, however there is a level
of uncertainty in the competing PRNGS on different threads because a higher priority
process could come along and change the order of execution. This, however, would depend
on the CPU scheduler, and a primitive FCFS scheduler would make this make this random
number generator less "random".
 
I could have "generated my own entropy" by listening to a raw audio input stream, 
built up timings between keyboard interrupts, by reading mouse movement data, 
or other things, however none of these things are guaranteed to exist on a 
computer system (i.e. a remote server may not have a mouse and keyboard, not all computers 
necessarily have microphones). My implementation does not rely on this type of
external hardware. 

The other implementation choice I made was after reading the "quantum" value, figure out 
whether it is even or odd, and then bit shift the 1 or 0 into a character to output. I could
have chosen other methods to transfer the 'quantum' value to character output, however I wanted
to ensure that the full range of possible character values (0b00000000 to 0b11111111) could 
be created, and this was the simplest way to do so.

The other decision that had to be made was which numbers to use for the modulus and the 
increment in the PRNG. The decision was just to choose prime numbers, and not to duplicate
the increment number across the threads, otherwise that would defeat the purpose of having
multiple threads performing "uncertain" actions.

Also, the prompt says to implement "$ cat /dev/random", so the raw binary data from
the RNG will be outputted as character data to stdout

This can be built and run by running: $ make run
*/

#include <iostream>
#include <thread>
#include <signal.h>

//quantum value is not written to in a "thread-safe" way (intentionally)
int quantum;
//this is for "safe" closing the program with ctrl+c
bool loop = true;

//multi-threaded PRNG with no locking
//this is the entropy function
void e(int inc, int mod) {
    while(loop) {
        quantum += inc;
        quantum %= mod;
    }
}

//stop handler
void stop(int signum) {
    loop =false;
}

int main() {
    //ctrl+c signal to stop threads (not forcefully)
    signal(SIGINT, stop);

    //create prng threads
    int primes[] = {73,79,83,89,97,101,103,107,109,113, 127, 131,137,139,149,151};
    std::thread* threads[16];

    for(int i=0; i < 16; i++) {
        threads[i] = new std::thread(e, primes[i], 2027);
    }

    //counter for loading randomness into a character
    int i = 0;

    //character to output to the screen
    char c = 0;

    //read the quantum value, and create a character
    //by bitshifting to the left 1, and setting the 
    //last bit equal to 1 or 0 depending on if the 
    //quantum number is even or odd
    while(loop) {
        c << 1;
        c = c + (quantum%2);
        if(i==7)
            std::cout << c;
        i = (i+1)%8;
    }

    //cleanup threads
    for(i = 0; i < 16; i++) {
        threads[i]->join();
        delete threads[i];
    }

    return 0;
}