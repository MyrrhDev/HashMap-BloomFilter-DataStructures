#ifndef BLOOM_FILTER_H
#define BLOOM_FILTER_H

#include <random>
#include <vector>
#include <cmath>
#include "hash-functions.h"
using namespace std;

template <typename H, typename=typename enable_if<is_base_of<Hasher, H>::value, H>::type>
class BloomFilter {

private:  
  double falsepos;
  // m is number of bits in the aray
  // n is the number of elements (max capacity)
  // k is number of hash functions
  int n, m, k;
  vector<bool> bits;
  vector<H> hashers;  
  
  
public:
  long long hash_cnt=0;
  
  BloomFilter(double falsepos, int n) : falsepos(falsepos), n(n) {
    const double ln2=log(2);
    m=ceil(-(n*log(falsepos)/(ln2*ln2)));
    k=ceil(ln2*m/n);
    bits=vector<bool>(m, 0);
    hashers=vector<H>(k);
  }
  
  void insert(int value) {
    for (int i=0; i<k; ++i) {
      ++hash_cnt;      
      uint h=hashers[i].get_hash(value);
      bits[h%m]=1;
    }
  }
  
  bool contains(int value) {
    for (int i=0; i<k; ++i) {
      ++hash_cnt;      
      uint h=hashers[i].get_hash(value);
      if (!bits[h%m]) return 0;
    }
    return 1;
  }
};

#endif
