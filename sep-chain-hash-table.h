#ifndef SEP_CHAIN_HASH_TABLE_H
#define SEP_CHAIN_HASH_TABLE_H

#include <vector>
#include <set>
#include "hashFunctions.h"
using namespace std;

const int TABLE_SIZE = 11;
typedef set<int> collision_register;
typedef vector<collision_register> hash_table_vector;

// Separate Chaining Hash Table - Set
template <typename H, typename=typename enable_if<is_base_of<Hasher, H>::value, H>::type>
struct SeparateChainingHashTableSet {
  
public:
  
private:
  
  int counter;      // numero de elementos de la tabla
  hash_table_vector table;
  H hasher;
  
  // Complejidad: O(n)
  void resize() {
    hash_table_vector table_tmp(2*table.size());
    for (uint i = 0; i < table.size(); ++i) {
      for (auto it = table[i].begin(); it!=table[i].end(); ++it) {
        int key = *it;
        uint position = hasher.get_hash(key)%table_tmp.size();
        table_tmp[position].insert(key);
      }
    }
    table=table_tmp;
  }
  
  
public:
  SeparateChainingHashTableSet() {
    table = hash_table_vector(TABLE_SIZE);
    counter = 0;    
  }
  
  /*Inserta un elemento 'key' en la tabla
  Coste: O(1)
  El peor de los casos, todas las entradas irian al mismo set: O(n)*/
  void insert(int key) {
    uint position = hasher.get_hash(key)%table.size();
    auto it = table[position].find(key);
    if (it! = table[position].end()) return;
    
    table[position].insert(key);
    ++counter;
    if (counter >= 2*(int)table.size()) resize();
  }
  
  /*Eliminar 'key' de la hash table
  Coste: O(1)
  El peor de los casos, todas las entradas irian al mismo set: O(n)*/
  void erase(int key) {
    uint position = hasher.get_hash(key)%table.size();
    auto it = table[position].find(key);
    if (it! = table[position].end()) table[position].erase(it);
  }
  
  /*Retorna si 'key' existe en la hash table
  Coste: O(1)
  El peor de los casos, se recorre el numero de entradas: O(n)*/
  bool contains(int key) {
    uint position = hasher.get_hash(key)%table.size();
    auto it = table[position].find(key);
    if (it! = table[position].end()) return true;
    return false;
  }
  
};


// Separate Chaining Hash Table - List
template <typename H, typename=typename enable_if<is_base_of<Hasher, H>::value, H>::type>
struct SeparateChainingHashTableList {
  
public:
//     HashTableEntry **ht, **top;
  
private:
    struct HashTableEntry {
        int value, key;
        HashTableEntry *next;
        HashTableEntry *prev;
        HashTableEntry(int key, int value) {
            this->key = key;
            this->value = value;
            this->next = NULL;
        }
    };
    
    HashTableEntry **hash_table, **top;
    H hasher;     // hashers
  
  
public:  
    SeparateChainingHashTableList() { 
        hash_table = new HashTableEntry*[TABLE_SIZE];
        for (int i = 0; i < TABLE_SIZE; i++) {
            hash_table[i] = NULL;
        }
    } 
        
    void insert(int key, int value) {
        //uint
        int hash_v = hasher.get_hash(key)%TABLE_SIZE;
        HashTableEntry* prev = NULL;
        HashTableEntry* en = hash_table[hash_v];
        while (en!= NULL) {
            prev = en;
            en = en->next;
        }
        if (en == NULL) {
            en = new HashTableEntry(key, value);
            if (prev == NULL) {
                hash_table[hash_v] = en;
            } else {
                prev->next = en;
            }
        } else {
            en->value = value;
        }
    }
    
    void erase(int key) {
        int hash_v = hasher.get_hash(key)%TABLE_SIZE;
        HashTableEntry* en = hash_table[hash_v];
        HashTableEntry* prev = NULL;
        if (en == NULL || en->key != key) return;                
        while (en->next != NULL) {
            prev = en;
            en = en->next;
        }
        
        if (prev != NULL) {
            prev->next = en->next;
        }        
        delete en;        
    }
    
    bool contains(int key) {
        int hash_v = hasher.get_hash(key)%TABLE_SIZE;
        HashTableEntry* en = hash_table[hash_v];
        if (en != NULL) {
            while (en != NULL) {
                if (en->key == key) {
                    return true;
                }                
                en = en->next;
            }
        }
       return false;
    }  
};


#endif
