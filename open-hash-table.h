#ifndef OPEN_HASH_TABLE_H
#define OPEN_HASH_TABLE_H

#include <vector>
#include "hash-functions.h"
using namespace std;

// Open Addressing Hash Table
template <typename H, typename=typename enable_if<is_base_of<Hasher, H>::value, H>::type>
class OpenAddressingHashTableLinear {
  
public:
    long long hash_cnt=0;
  
private:
    // numero de elementos
    int counter;
    // tabla de entradas
    vector<int*> hash_table;
    // hash function
    H hasher;

    inline uint slot(int key, const vector<int*> &table) {
        ++hash_cnt;
        uint pos = hasher.get_hash(key)%table.size();
        while (table[pos] != NULL) {
            if (*table[pos] == key) return pos;
            ++pos%=table.size();
        }    
        return pos;
    }  
  
    void resize() {
        vector<int*> hash_table_tmp(2*hash_table.size(), NULL);
        for (uint i=0; i<hash_table.size(); ++i) {
            if (hash_table[i] != NULL) {
                uint pos = slot(*hash_table[i], hash_table_tmp);
                hash_table_tmp[pos] = hash_table[i];
            }
        }
        hash_table=hash_table_tmp;
    }
  
public:  
  // Crea la hash table, con capacidad inicial de 11
  // Factor de carga: 0.8
  OpenAddressingHashTableLinear() {
    counter = 0;
    hash_table = vector<int*>(11, NULL);
  }
  
  void insert(int key) {    
    uint pos = slot(key, hash_table);
    if (hash_table[pos] == NULL) {      
      ++counter;
      hash_table[pos] = new int(key);
      if (counter >= (int)(0.8*hash_table.size())) resize();
    }
  }
    
  void erase(int key) {    
    uint pos = slot(key, hash_table);
    if (hash_table[pos] != NULL) delete hash_table[pos];    
    uint nex_pos = pos;
    while (hash_table[pos] != NULL) {      
      hash_table[pos] = NULL;
      bool loop = true;
      while (loop) {       
        ++nex_pos%=hash_table.size();
        if (hash_table[nex_pos] == NULL) return;
        ++hash_cnt;
        uint p = hasher.get_hash(*hash_table[nex_pos])%hash_table.size();
        loop = pos<=nex_pos?pos<p&&p<=nex_pos:pos<p||p<=nex_pos;
      }     
      hash_table[pos] = hash_table[nex_pos];
      pos = nex_pos;
    }
  }  
 
  bool contains(int key) {
    uint pos = slot(key, hash_table);
    if (hash_table[pos] != NULL) return true;
    return false;
  }  
};

template <typename H, typename=typename enable_if<is_base_of<Hasher, H>::value, H>::type>
class OpenAddressingHashTableDouble {
public:
    long long hash_cnt=0;
    #define PRIME 7 
  
private:
    // numero de elementos
    int counter;
    // tabla de entradas
    vector<int*> hash_table;
    // hash function
    H hasher;
    
    //Double hash
    inline uint slot(int key, const vector<int*> &table) {
        ++hash_cnt;
        uint pos = hasher.get_hash(key)%table.size();
        if (table[pos] != NULL) { //colision
            if (*table[pos] == key) return pos;
            uint pos2 = hash2(key)%table.size();
            uint i = 1;
            uint new_pos = (pos + (i*pos2))%table.size();
            while (table[new_pos] != NULL) {
//                 cout << "new_pos wow" << endl;
                if (*table[new_pos] == key) return new_pos;
                ++i;
                new_pos = (pos + i*pos2)%table.size();                
            }
            return new_pos;
        }
        return pos;
    }
  
    void resize() {
        vector<int*> hash_table_tmp(2*hash_table.size(), NULL);
        for (uint i=0; i<hash_table.size(); ++i) {
            if (hash_table[i] != NULL) {
                uint pos = slot(*hash_table[i], hash_table_tmp);
                hash_table_tmp[pos] = hash_table[i];
            }
        }
        hash_table=hash_table_tmp;
    }
  
public:  
  OpenAddressingHashTableDouble() {
    counter = 0;
    hash_table = vector<int*>(11, NULL);
  }
  
  uint hash2(int key) { 
    return (PRIME - (key % PRIME)); 
  } 
    
  void insert(int key) {
    uint pos = slot(key, hash_table);
    if (hash_table[pos] == NULL) {      
      ++counter;
      hash_table[pos] = new int(key);
      if (counter >= (int)(0.8*hash_table.size())) resize();
    } else { //collision
        uint pos2 = hash2(key)%hash_table.size();
        int i = 1; 
        while (1) {
            int new_pos = (pos + i*pos2)%hash_table.size();
            if (hash_table[new_pos] == NULL) { 
                hash_table[new_pos] = new int(key); 
                break;
            } 
            ++i;            
        }
    }
  }
    
  void erase(int key) {    
    uint pos = slot(key, hash_table);
    if (hash_table[pos] != NULL) delete hash_table[pos];    
    uint nex_pos = pos;
    while (hash_table[pos] != NULL) {      
      hash_table[pos] = NULL;
      bool loop = true;
      while (loop) {       
        ++nex_pos%=hash_table.size();
        if (hash_table[nex_pos] == NULL) return;
        ++hash_cnt;
        uint p = hasher.get_hash(*hash_table[nex_pos])%hash_table.size();
        loop = pos<=nex_pos?pos<p&&p<=nex_pos:pos<p||p<=nex_pos;
      }     
      hash_table[pos] = hash_table[nex_pos];
      pos = nex_pos;
    }
  }  
 
  bool contains(int key) {
    uint pos = slot(key, hash_table);
    if (hash_table[pos] != NULL) return true;
    return false;
  } 
    
};


#endif
