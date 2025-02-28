#ifndef HASH_FUNCTIONS_H
#define HASH_FUNCTIONS_H

#include <random>
#include <limits>
#include <vector>
using namespace std;

typedef vector<uint> unsigned_vector;
typedef vector<unsigned_vector> index_mapping;

class Hasher {
public:  
    virtual uint get_hash(int data) const=0;
};


class H3 : public Hasher {
private:
  static size_t const bits_per_byte=numeric_limits<unsigned char>::digits;
  constexpr static size_t byte_range=numeric_limits<unsigned char>::max()+1U;
  
  index_mapping bytes;
  size_t siz;  
  
public:  
  H3(uint seed = rand()) {
    siz = sizeof(int);
    minstd_rand0 prng(seed);
    uint bits[siz*bits_per_byte];
    for (uint bit = 0; bit < siz*bits_per_byte; ++bit) {
      bits[bit] = 0;
      for (uint i = 0; i<sizeof(int)/2; ++i)
        bits[bit] = (bits[bit]<<16)|(prng()&0xFFFF);
    }
    bytes = index_mapping(siz, vector<uint>(byte_range));
    for (uint byte = 0; byte < siz; ++byte) for (uint val=0; val<byte_range; ++val) {
      bytes[byte][val] = 0;
      for (uint bit = 0; bit < bits_per_byte; ++bit) if (val&(1U<<bit))
        bytes[byte][val]^=bits[byte*bits_per_byte+bit];
    }
  }
  
  uint get_hash(int data) const {
    int offset = 0;
    auto *p = static_cast<unsigned char const*>((void const*)&data);
    uint result = 0;
    uint n=(siz+3)/4;
    switch (siz%4) {
      case 0: do { result^=bytes[offset++][*p++];
      case 3:      result^=bytes[offset++][*p++];
      case 2:      result^=bytes[offset++][*p++];
      case 1:      result^=bytes[offset++][*p++];
              } while (--n>0);
    }
    return result;
  }
  
};

#define ROTL(x, n) (((x)<<(n))|((x)>>(32-(n))))
class Murmur3 : public Hasher {  
private:
  uint mseed;
    
public:
  Murmur3(uint seed=rand()) {
    mseed=seed;
  }
  
  uint get_hash(int x) const {
    uint hash = mseed;
    
    const uint c1 = 0xcc9e2d51;
    const uint c2 = 0x1b873593;
    
    uint k = x;
    k *= c1;
    k = ROTL(k, 15);
    k *= c2;
    
    hash ^= k;
    hash = ROTL(hash, 13); 
    hash = hash*5+0xe6546b64;    
    hash ^= 1U;
    hash ^= hash>>16;
    hash *= 0x85ebca6b;
    hash ^= hash>>13;
    hash *= 0xc2b2ae35;
    hash ^= hash>>16;    
    return hash;
  }
};


// Fowler–Noll–Vo hash function
class FNV1 : public Hasher {  
private:  
  uint fnv_prime;
  uint fnv_offset_basis;
  
public:
  
  FNV1() {
    static uint index=0;
    static const uint prim[]={104395069, 103660133,
      102919889, 102183511, 101447123, 100710977,
      99972791, 99236399, 98498003, 97761161,
      97025821, 96291841, 95554541, 94822691,
      94087991, 93354307, 92619853, 91885789,
      91152661, 90419179, 89687243, 88952401, 
      88221223, 87491093, 86758523, 86028157};
    fnv_prime=prim[index];
    ++index%=26;
    fnv_offset_basis=2166136261U;
  }
  
  uint get_hash(int x) const {
    uint hash = fnv_offset_basis; 
    auto *p = static_cast<unsigned char const*>((void const*)&x);
    for (uint i = 0; i < 4; ++i) {
      hash *= fnv_prime;
      hash ^= *p++;
    }
    return hash;
  }
  
};

#endif
