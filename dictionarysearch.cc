#include <iostream>
#include <fstream>
#include <cstring>
#include <dirent.h>
#include <algorithm>
#include <vector>
#include <set>
#include <sys/time.h>
#include "sep-chain-hash-table.h"
#include "open-hash-table.h"
#include "bloom-filter.h"
using namespace std;

const char* EXP1_FOLDER="./words/size/";
const char* EXP2_FOLDER="./words/probs/";
const char* DIC_EXT="dict";
const char* TXT_EXT=".txt";

int exper;
int algo;
int hash_func;

set<string> dicts;
set<string> texts;

double segs_start, segs_contains;
static double total_secs=0;
static double total_start=0;
static double total_contained=0;

long long total_hash_cnt, start_hash_count, contains_hash_count;

void read_console(int argc, char** argv);
inline void read_console_error();
void get_file_names();
void run_experiment1();
void run_experiment2();
void read_dic(const char* path, const string &d, vector<int> &dic);
void read_txt(const char* path, const string &d, vector<int> &txt);
inline void print_experiment_info(const string &d, const vector<int> &dic, const vector<int> &txt);
int search_in_dict(vector<int> &dic, vector<int> &txt);
int ht_open_addressing_search(vector<int> &dic, vector<int> &txt);
int ht_separate_chaining_search(vector<int> &dic, vector<int> &txt);
int bloom_filter_search(vector<int> &dic, vector<int> &txt);
template<typename T> 
int perform_contains(T &data_structure, vector<int> &dic, vector<int> &txt);
inline void update_time(const struct timeval &time1, const struct timeval &time2, const struct timeval &time3);
inline bool check_filetype(const string &str, const string &suffix);
inline void print_experiment_results(int words_found, int total);


int main(int argc, char** argv) {
    ios_base::sync_with_stdio(0);
    cin.tie(0);  
    
    read_console(argc, argv);  
    get_file_names(); 
    if(exper == 1) {
        run_experiment1();      
    } else if (exper == 2) {
        run_experiment2();     
    } else {
        cout<<"Error en numero de experimento" << endl;      
    }
}

inline void read_console_error() {
  cout << "Usage: searchtexts [-exp1|exp2] (oa-linear|oa-double|sep-set|sep-linked|bloom) (h3|fnv1|mur)"<< endl;
  exit(0);
}

void read_console(int argc, char** argv) {
    int index=1;
    if (argc > index and !strcmp(argv[index],"-exp1")) exper=1;      
    else if (argc > index and !strcmp(argv[index],"-exp2")) exper=2;
    else read_console_error();    
    ++index;
    if (argc > index and !strcmp(argv[index],"oa-linear")) algo = 1;
    else if (argc > index and !strcmp(argv[index],"oa-double")) algo = 2;
    else if (argc > index and !strcmp(argv[index],"sepset")) algo = 3;
    else if (argc > index and !strcmp(argv[index],"sep-linked")) algo = 4;
    else if (argc > index and !strcmp(argv[index],"bloom")) algo = 5;
    else read_console_error();    
    ++index;
    if (argc>index and !strcmp(argv[index],"h3")) hash_func=1;
    else if (argc>index and !strcmp(argv[index],"fnv1")) hash_func=2;
    else if (argc>index and !strcmp(argv[index],"mur")) hash_func=3;
    else read_console_error();
}

inline bool check_filetype(const string &str, const string &suffix) {
  return str.size()>=suffix.size() and str.compare(str.size()-suffix.size(), suffix.size(), suffix)==0;
}

void get_file_names() {
    dicts = set<string>();
    texts = set<string>();    
    DIR *pDIR;
    struct dirent *entry;
    const char* path = EXP1_FOLDER;
    if (exper == 2) {
        path = EXP2_FOLDER;
    } 
    if ((pDIR=opendir(path))) {
        while ((entry=readdir(pDIR))) {
        const char* file_name = entry->d_name;
            if (strcmp(file_name, ".")!=0 and strcmp(file_name, "..")!=0) {
                const string name(file_name);
                if (check_filetype(name, DIC_EXT)) dicts.insert(name.substr(0, name.size()-4));
                if (check_filetype(name, TXT_EXT)) texts.insert(name.substr(0, name.size()-4));
            }
        }
        closedir(pDIR);
    }
}

void read_txt(const char* path, const string &name, vector<int> &txt) {   
  ifstream txt_file(path + name + TXT_EXT);
  if (txt_file.is_open()) {
    int num;
    while (txt_file>>num) txt.push_back(num);
    txt_file.close();
  } else {
    cout<<"No se puede leer el archivo '" << path + name + TXT_EXT + "'" << endl;
    exit(0);
  }
}

void read_dic(const char* path, const string &name, vector<int> &dic) {
    ifstream dic_file(path + name + DIC_EXT);
    if (dic_file.is_open()) {
        int num;
        while (dic_file>>num) dic.push_back(num);
        dic_file.close();
    } else {
        cout<<"No se puede leer el archivo '" << path + name + DIC_EXT +"'" << endl;
        exit(0);
    } 
}

void run_experiment1() {
    vector<int> dic;
    for (const string &name : dicts) {
        read_dic(EXP1_FOLDER, name, dic);
    }    
    for (const string &name : texts) { 
        total_secs=total_start=total_contained=0;
        vector<int> txt;        
        read_txt(EXP1_FOLDER, name, txt);
        
        print_experiment_info(name, dic, txt);
        int words_found = search_in_dict(dic, txt);  //found words unumeber
        
        print_experiment_results(words_found, txt.size()); 
        cout<<"Total time (s): "<<total_secs<<"; init (s): "<<total_start<<"; check (s): "<<total_contained<<"\n\n";
    }
}

void run_experiment2() {
    vector<int> dic;
    for (const string &name : dicts) {
        read_dic(EXP1_FOLDER, name, dic);
    }    
    for (const string &name : texts) {
        total_secs=total_start=total_contained=0;
        vector<int> txt;        
        read_txt(EXP2_FOLDER, name, txt);        
        print_experiment_info(name, dic, txt);
        int words_found = search_in_dict(dic, txt);        
        print_experiment_results(words_found, txt.size()); 
        cout<<"Total time (s): "<<total_secs<<"; init (s): "<<total_start<<"; check (s): "<<total_contained<<"\n\n";
    }
}

int ht_open_addressing_search(vector<int> &dic, vector<int> &txt) {
    switch(algo) {
        case 1:
        {
            if (hash_func==1) {
                OpenAddressingHashTable<H3> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==2) {
                OpenAddressingHashTable<FNV1> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==3) {
                OpenAddressingHashTable<Murmur3> ht;
                return perform_contains(ht, dic, txt);
            }
        }
        case 2:
        {
            if (hash_func==1) {
                OpenAddressingHashTable<H3> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==2) {
                OpenAddressingHashTable<FNV1> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==3) {
                OpenAddressingHashTable<Murmur3> ht;
                return perform_contains(ht, dic, txt);
            }
        }
    }
    return -1;
}

int ht_separate_chaining_search(vector<int> &dic, vector<int> &txt) {
    switch(algo) {
        case 3:
        {
            if (hash_func==1) {
                SeparateChainingHashTableSet<H3> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==2) {
                SeparateChainingHashTableSet<FNV1> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==3) {
                SeparateChainingHashTableSet<Murmur3> ht;
                return perform_contains(ht, dic, txt);
            }
            break;
        }
        case 4:
        {
            if (hash_func==1) {
                SeparateChainingHashTableList<H3> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==2) {
                SeparateChainingHashTableList<FNV1> ht;
                return perform_contains(ht, dic, txt);
            } else if (hash_func==3) {
                SeparateChainingHashTableList<Murmur3> ht;
                return perform_contains(ht, dic, txt);
            }
            break;
        }
    }
    return -1;
}

int bloom_filter_search(vector<int> &dic, vector<int> &txt) {
    int n = dic.size();
    if (hash_func==1) {
        BloomFilter<Murmur3> bf(n?1.0/n:0, n);
        return perform_contains(bf, dic, txt);
    } else if (hash_func==2) {
        BloomFilter<FNV1> bf(n?1.0/n:0, n);
        return perform_contains(bf, dic, txt);
    } else if (hash_func==3) {
        BloomFilter<H3> bf(n?1.0/n:0, n);
        return perform_contains(bf, dic, txt);
    }
    return -1;
}

inline void update_time(const struct timeval &time1, const struct timeval &time2, const struct timeval &time3) {
    double s1 = time1.tv_sec+(time1.tv_usec/1000000.0);
    double s2 = time2.tv_sec+(time2.tv_usec/1000000.0);
    double s3 = time3.tv_sec+(time3.tv_usec/1000000.0);
    segs_start = s2-s1;
    segs_contains = s3-s2;
    total_start+=segs_start;
    total_contained+=segs_contains;
    total_secs+=segs_start+segs_contains;
}

template<typename T> 
int perform_contains(T &data_structure, vector<int> &dic, vector<int> &txt) {
    struct timeval time1, time2, time3;
    int words_found = 0;
    gettimeofday(&time1, NULL);
    for (int x : dic) {
        data_structure.insert(x);
    }
    start_hash_count=data_structure.hash_cnt;
    gettimeofday(&time2, NULL); 
    for (int x : txt) {
        if (data_structure.contains(x)) ++words_found;
    }
    gettimeofday(&time3, NULL);
    update_time(time1, time2, time3);
    contains_hash_count = data_structure.hash_cnt-start_hash_count;
    total_hash_cnt = data_structure.hash_cnt;
    cout<<"\n  Hashes al insertar() "<<start_hash_count<<"\n";
    cout<<"  Hashes en search() "<<contains_hash_count<<"\n";
    cout<<"  Total hashes "<<total_hash_cnt<<"\n\n";    
    return words_found;
}

int search_in_dict(vector<int> &dic, vector<int> &txt) {
  switch (algo) {
    case 1: return ht_open_addressing_search(dic, txt);
    case 2: return ht_open_addressing_search(dic, txt);
    case 3: return ht_separate_chaining_search(dic, txt);
    case 4: return ht_separate_chaining_search(dic, txt);
    case 5: return bloom_filter_search(dic, txt);   
  }
  return -1;
}

inline void algo_hash_str(string &str) {  
    if (algo == 1) str="open addressing-linear";
    else if (algo == 2) str= "open addressing-double hash";
    else if (algo == 3) str="separate chaining-set";
    else if (algo == 4) str="separate chaining-single linked list";
    else if (algo == 5) str="Bloom filter";
    str+=" y la funcion de hash: ";
    if (hash_func == 1) str+="H3";
    else if (hash_func == 2) str+="FNV1";
    else if (hash_func == 3) str+="Murmur3";
}

inline void print_experiment_info(const string &name, const vector<int> &dic, const vector<int> &txt) {
    cout<<"Experimento " << exper << ": " << name <<'\n';
    string hs;
    algo_hash_str(hs);
    cout<<" Con "<<hs<<'\n';
    cout<<" Diccionario de "<<dic.size()<<" palabras\n";
    cout<<" Texto de "<<txt.size()<<" palabras"<<endl;
}

inline void print_experiment_results(int words_found, int total) {
    if (words_found == -1) cout << "Error en estructura de datos\n"; 
    else cout << "  Se han encontrado: " << words_found << " palabras (" << 100.0*words_found/total<<"%)\n"<<endl;
}
