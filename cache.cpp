#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

#define DBG 1
#define DRAM_SIZE (64*1024*1024)
#define CACHE_SIZE (64*1024)


vector<int> cache;
char valid;

int offset, index, tag;

unsigned int numberOfLines;
unsigned int offsetSize;
unsigned int indexSize;
unsigned int tagSize;

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

// Convert an unsigned int to a binary string
string unsignedIntToBinaryString(unsigned int n) 
{
    if (n == 0) return "0";
    
    string binaryString;
    while (n > 0) {
        binaryString = (n % 2 == 0 ? "0" : "1") + binaryString;
        n /= 2;
    }
    return binaryString;
}

// Memory address generators
unsigned int memGen1()
{
	static unsigned int addr=0;
	return (addr++)%(DRAM_SIZE);
}

unsigned int memGen2()
{
	static unsigned int addr=0;
	return  rand_()%(24*1024);
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

// Calculate the log base 2 of a number
unsigned int log2(unsigned int x) 
{
    return log(x) / log(2);
}

// Calculate the number of lines, offset, index, and tag bits
void calculateBits(unsigned int cacheLineSize) 
{
    numberOfLines = CACHE_SIZE / cacheLineSize;
    offsetSize = log2(cacheLineSize);
    indexSize = log2(numberOfLines);
    tagSize = 32 - indexSize - offsetSize;
}

unsigned int getOffset(unsigned int addr) 
{
    return addr & ((1 << offsetSize) - 1); // Mask for offset
}

unsigned int getIndex(unsigned int addr) 
{
    return (addr >> offsetSize) & ((1 << indexSize) - 1); // Shift and mask for index
}

unsigned int getTag(unsigned int addr) 
{
    return addr >> (offsetSize + indexSize); // Shift for tag
}

// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr, unsigned int lineSize)
{	
	calculateBits(lineSize);

	offset = getOffset(addr);
	index = getIndex(addr);
	tag = getTag(addr);
	string address = unsignedIntToBinaryString(addr);
	if (address.size() < 32) {
		valid = '0';
	}
	else{
		valid = '1';
	}

	cache.resize(numberOfLines, -1); // Initialize the cache with -1

	if(cache[index] == -1) { // If the cache is empty
		cache[index] = tag; // Store the tag
		return MISS; // Return miss(cold start miss)
	}
	else if(cache[index] == tag) { // If the cache is not empty and the tag matches
		return HIT; // Return hit
	}
	else { // If the cache is not empty and the tag does not match
		cache[index] = tag; // Update the tag
		return MISS; // Return miss(conflict miss)
	}

}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr, unsigned int lineSize)
{	

	calculateBits(lineSize);

	tag = addr >> offsetSize; // Use tag as higher bits of address

	if (cache.empty()) {
        cache.resize(numberOfLines, -1); // Initialize cache on first use
    }

	// Check if tag is already in cache
    for(int i = 0; i < numberOfLines; i++) 
	{
        if(cache[i] == tag) // If tags are equal then it is a HIT
		{
            return HIT; 
        }
    }

    // Look for an empty slot
    for(int i = 0; i < numberOfLines; i++) 
	{
        if(cache[i] == -1) 
		{
            cache[i] = tag; // Replace with new tag
            return MISS; //Return miss (cold start miss)
        }
    }

	// If cache is full and no match, replace a random line
    int randomIndex = rand() % numberOfLines;
    cache[randomIndex] = tag; // Replace with new tag
    return MISS; // Return miss (capacity miss)
}

char *msg[2] = {"Miss","Hit"};

#define NO_OF_Iterations 1000000 // Change to 1,000,000

int main()
{
	unsigned int hit = 0;
	cacheResType r;
	unsigned int addr;
	int lineSize = 16;

	cout << "Direct Mapped Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen6();
		r = cacheSimDM(addr, lineSize);
		if(r == HIT) hit++;
		//cout <<"0x" << setfill('0') << setw(8)  << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "DM Hit ratio = " << (100*hit/NO_OF_Iterations)<< "%" << endl;

	cout << "-----------------------------------------------------------------------------------------------" << endl;

	cout << "Fully Associative Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen1();
		r = cacheSimFA(addr, lineSize);
		if(r == HIT) hit++;
		//cout <<"0x" << setfill('0') << setw(8) << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "FA Hit ratio = " << (100*hit/NO_OF_Iterations)<< "%" << endl;
	cout << "-----------------------------------------------------------------------------------------------" << endl;

	return 0;
}