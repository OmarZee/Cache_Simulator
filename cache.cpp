#include <iostream>
#include <iomanip>
#include <string>
#include <cmath>
#include <vector>

using namespace std;

#define DBG 1
#define DRAM_SIZE (64*1024*1024)
#define CACHE_SIZE (64*1024)


vector<vector<int>> cache;
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


unsigned int log2(unsigned int x) {
    return log(x) / log(2);
}

void calculateAndPrintBits(unsigned int cacheLineSize) {
    numberOfLines = CACHE_SIZE / cacheLineSize;
    offsetSize = log2(cacheLineSize);
    indexSize = log2(numberOfLines);
    tagSize = 32 - indexSize - offsetSize;

    // cout << "Cache Line Size: " << cacheLineSize << " bytes" << endl;
    // cout << "Offset bits: " << offsetSize << endl;
    // cout << "Index bits: " << indexSize << endl;
    // cout << "Tag bits: " << tagSize << endl;
    // cout << "---------------------------------" << endl;
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
	calculateAndPrintBits(lineSize);
	// This function accepts the memory address for the memory transaction and 
	// returns whether it caused a cache miss or a cache hit
	// Dividing the address into three parts: offset, index, and tag
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

	// Print the address, offset, index, tag, and valid bit
	// cout << "Address = " << address << endl;
	// cout << "Offset = " << unsignedIntToBinaryString(offset) << endl;
	// cout << "Index = " << unsignedIntToBinaryString(index) << endl;
	// cout << "Tag = " << unsignedIntToBinaryString(tag) << endl;
	// cout << "Valid = " << valid << endl;

	cache.resize(numberOfLines, vector<int>(lineSize / 4, -1)); // Initialize the cache with -1


	if(cache[index][offset/4] == -1) { // If the cache is empty
		cache[index][offset/4] = tag; // Store the tag
		return MISS; // Return miss(cold start miss)
	}
	else if(cache[index][offset/4] == tag) { // If the cache is not empty and the tag matches
		return HIT; // Return hit
	}
	else { // If the cache is not empty and the tag does not match
		cache[index][offset/4] = tag; // Update the tag
		return MISS; // Return miss(conflict miss)
	}

	// The current implementation assumes there is no cache; so, every transaction is a miss
	// return MISS;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr, unsigned int lineSize)
{	
	// This function accepts the memory address for the read and 
	// returns whether it caused a cache miss or a cache hit

	calculateAndPrintBits(lineSize);
	// This function accepts the memory address for the memory transaction and 
	// returns whether it caused a cache miss or a cache hit
	// Dividing the address into three parts: offset, index, and tag

	tag = addr;

	cache.resize(numberOfLines, vector<int>(lineSize / 4, -1)); // Initialize the cache with -1

	int counter = 0;

	for(int i = 0; i < numberOfLines; i++) {
		for(int j = 0; j < lineSize/4 + 1; j++) {
			if(cache[i][j] == tag) {
				return HIT;
			}
			else if(cache[i][j] == -1) {
				cache[i][j] = tag;
				return MISS;
			}
			if(counter == numberOfLines - 1){
				int x = rand() % numberOfLines; // Randomly select an index
				cache[x][j] = tag; // Update the tag
				return MISS; // Capacity miss
			}
		}
		counter++;
	}
	// if(cache[index][offset/4] == -1) { // If the cache is empty
	// 	cache[index][offset/4] = tag; // Store the tag
	// 	return MISS; // Return miss(cold start miss)
	// }
	// else if(cache[index][offset/4] == tag) { // If the cache is not empty and the tag matches
	// 	return HIT; // Return hit
	// }
	// else { // If the cache is at full capacity and the tag does not match
	// 	index = rand() % numberOfLines; // Randomly select an index
	// 	cache[index][offset/4] = tag; // Update the tag
	// 	return MISS; // Return miss(capacity miss)
	// }

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}

char *msg[2] = {"Miss","Hit"};

#define NO_OF_Iterations 10000 // Change to 1,000,000

int main()
{
	unsigned int hit = 0;
	cacheResType r;
	unsigned int addr;
	int lineSize = 16;

	// Cache initialization
	//cache[64000/lineSize][lineSize/4]; // Size of cache rows and columns according to lineSize

	cout << "Direct Mapped Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen2();
		r = cacheSimDM(addr, lineSize);
		if(r == HIT) hit++;
		cout <<"0x" << setfill('0') << setw(8)  << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "Hit ratio = " << (100*hit/NO_OF_Iterations)<< "%" << endl;

	cout << "-----------------------------------------------------------------------------------------------" << endl;

	cout << "Fully Associative Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen2();
		r = cacheSimFA(addr, lineSize);
		if(r == HIT) hit++;
		cout <<"0x" << setfill('0') << setw(8) << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "FA Hit ratio = " << (100*hit/NO_OF_Iterations)<< "%" << endl;

	return 0;
}
