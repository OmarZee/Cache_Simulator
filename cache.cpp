#include <iostream>
#include <iomanip>
#include <string>

using namespace std;

#define		DBG				1
#define		DRAM_SIZE		(64*1024*1024)
#define		CACHE_SIZE		(64*1024)

string offset, index, tag, valid;

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

void addressBreakdown(unsigned int addr, unsigned int lineSize)
{
    string address = unsignedIntToBinaryString(addr);

	if (lineSize == 16)
	{
		// 4000 lines in the cache
		// getting offset 
		for (int i = 124; i < 128; i++)
		{
			offset = address[i];
		}
		// getting index
		for (int i = 113; i < 124; i++)
		{
			index = address[i];
		}
		// getting tag
		for (int i = 1; i < 113; i++)
		{
			tag = address[i];
		}
		// getting valid 
		valid = address[0];

        cout << "offset = " << offset << endl;
        cout << "index = " << index << endl;
        cout << "tag = " << tag << endl;
        cout << "valid = " << valid << endl;
	}
	else if (lineSize == 32)
	{
		// 2000 lines in the cache
		for (int i = 251; i < 256; i++)
		{
			offset = address[i];
		}
		// getting index
		for (int i = 240; i < 251; i++)
		{
			index = address[i];
		}
		// getting tag
		for (int i = 1; i < 240; i++)
		{
			tag = address[i];
		}
		// getting valid 
		valid = address[0];
	}
	else if (lineSize == 64)
	{
		// 1000 lines in the cache
		for (int i = 506; i < 512; i++)
		{
			offset = address[i];
		}
		// getting index
		for (int i = 495; i < 506; i++)
		{
			index = address[i];
		}
		// getting tag
		for (int i = 1; i < 495; i++)
		{
			tag = address[i];
		}
		// getting valid 
		valid = address[0];
	}
	else if (lineSize == 128)
	{
		// 500 lines in the cache
		for (int i = 1014; i < 1024; i++)
		{
			offset = address[i];
		}
		// getting index
		for (int i = 1005; i < 1014; i++)
		{
			index = address[i];
		}
		// getting tag
		for (int i = 1; i < 1005; i++)
		{
			tag = address[i];
		}
		// getting valid 
		valid = address[0];
	}
}

// Direct Mapped Cache Simulator
cacheResType cacheSimDM(unsigned int addr, unsigned int lineSize)
{	
	// This function accepts the memory address for the memory transaction and 
	// returns whether it caused a cache miss or a cache hit

    addressBreakdown(addr, lineSize);

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}

// Fully Associative Cache Simulator
cacheResType cacheSimFA(unsigned int addr, unsigned int lineSize)
{	
	// This function accepts the memory address for the read and 
	// returns whether it caused a cache miss or a cache hit

    addressBreakdown(addr, lineSize);

	// The current implementation assumes there is no cache; so, every transaction is a miss
	return MISS;
}
char *msg[2] = {"Miss","Hit"};

#define		NO_OF_Iterations	100		// Change to 1,000,000
int main()
{
	unsigned int hit = 0;
	cacheResType r;
	
	unsigned int addr;
	//unsigned int lineSize;

	cout << "Direct Mapped Cache Simulator\n";

	for(int inst=0;inst<NO_OF_Iterations;inst++)
	{
		addr = memGen2();
		r = cacheSimDM(addr, 16);
		if(r == HIT) hit++;
		cout <<"0x" << setfill('0') << setw(8) << hex << addr <<" ("<< msg[r] <<")\n";
	}
	cout << "Hit ratio = " << (100*hit/NO_OF_Iterations)<< endl;
}