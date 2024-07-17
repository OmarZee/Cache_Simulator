#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>

using namespace std;

#define DBG 1
#define DRAM_SIZE (64*1024*1024)
#define CACHE_SIZE (64*1024)
#define NO_OF_Iterations 1000000 // Changed to 1,000,000
#define CACHE_LINE_SIZES 4
#define NUM_GENERATORS 6

enum cacheResType {MISS=0, HIT=1};

/* The following implements a random number generator */
unsigned int m_w = 0xABABAB55;    /* must not be zero, nor 0x464fffff */
unsigned int m_z = 0x05080902;    /* must not be zero, nor 0x9068ffff */
unsigned int rand_()
{
    m_z = 36969 * (m_z & 65535) + (m_z >> 16);
    m_w = 18000 * (m_w & 65535) + (m_w >> 16);
    return (m_z << 16) + m_w;  /* 32-bit result */
}

unsigned int memGen1()
{
    static unsigned int addr=0;
    return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
    static unsigned int addr=0;
    return rand_()%(24*1024);
}

unsigned int memGen3()
{
    return rand_()%(DRAM_SIZE);
}

unsigned int memGen4()
{
    static unsigned int addr=0;
    return (addr++)%(4*1024);
}

unsigned int memGen5()
{
    static unsigned int addr=0;
    return (addr++)%(1024*64);
}

unsigned int memGen6()
{
    static unsigned int addr=0;
    return (addr+=32)%(64*4*1024);
}

// Direct Mapped Cache Simulator
struct CacheLineDM {
    bool valid;
    unsigned int tag;
};

cacheResType cacheSimDM(unsigned int addr, unsigned int line_size, vector<CacheLineDM>& cache)
{    
    unsigned int index = (addr / line_size) % cache.size();
    unsigned int tag = addr / (line_size * cache.size());
    
    if (cache[index].valid && cache[index].tag == tag) {
        return HIT;
    } else {
        cache[index].valid = true;
        cache[index].tag = tag;
        return MISS;
    }
}

// Fully Associative Cache Simulator
struct CacheLineFA {
    bool valid;
    unsigned int tag;
};

cacheResType cacheSimFA(unsigned int addr, unsigned int line_size, vector<CacheLineFA>& cache)
{    
    unsigned int tag = addr / line_size;

    for (auto& line : cache) {
        if (line.valid && line.tag == tag) {
            return HIT;
        }
    }

    // Random replacement policy
    int replace_index = rand() % cache.size();
    cache[replace_index].valid = true;
    cache[replace_index].tag = tag;
    
    return MISS;
}

char *msg[2] = {"Miss","Hit"};

int main()
{
    unsigned int hit;
    cacheResType r;
    unsigned int addr;
    unsigned int line_sizes[CACHE_LINE_SIZES] = {16, 32, 64, 128};
    unsigned int (*memGens[NUM_GENERATORS])() = {memGen1, memGen2, memGen3, memGen4, memGen5, memGen6};

    // Run experiments
    for (int i = 0; i < CACHE_LINE_SIZES; ++i) {
        unsigned int line_size = line_sizes[i];

        // Direct Mapped Cache
        cout << "Direct Mapped Cache Simulator with Line Size " << line_size << " bytes\n";
        vector<CacheLineDM> cacheDM(CACHE_SIZE / line_size);
        hit = 0;

        for (int gen = 0; gen < NUM_GENERATORS; ++gen) {
            for (int inst = 0; inst < NO_OF_Iterations; ++inst) {
                addr = memGens[gen]();
                r = cacheSimDM(addr, line_size, cacheDM);
                if (r == HIT) hit++;
            }
            cout << "Memory Generator " << gen + 1 << ": Hit ratio = " << (100.0 * hit / NO_OF_Iterations) << "%\n";
        }

        // Fully Associative Cache
        cout << "\nFully Associative Cache Simulator with Line Size " << line_size << " bytes\n";
        vector<CacheLineFA> cacheFA(CACHE_SIZE / line_size);
        hit = 0;

        for (int gen = 0; gen < NUM_GENERATORS; ++gen) {
            for (int inst = 0; inst < NO_OF_Iterations; ++inst) {
                addr = memGens[gen]();
                r = cacheSimFA(addr, line_size, cacheFA);
                if (r == HIT) hit++;
            }
            cout << "Memory Generator " << gen + 1 << ": Hit ratio = " << (100.0 * hit / NO_OF_Iterations) << "%\n";
        }

        cout << endl;
    }

    return 0;
}
