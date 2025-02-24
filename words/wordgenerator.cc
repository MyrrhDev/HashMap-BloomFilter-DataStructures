#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <cstdlib>
#include <string>
#include <vector>
#include <set>
using namespace std;

const uint MAX_WORD_LEN = 1e9;

const char* EXPER_PREFIX="exper-";
const char* DICT_EXT=".dict";
const char* TXT_EXT=".txt";

set<uint> dict;
vector<uint> text;

bool isFloat( string myString ) {
    istringstream iss(myString);
    float f;
    iss >> noskipws >> f;    
    return iss.eof() && !iss.fail(); 
}

vector<float> cambiar_probabilidades() {
    cout << "Escribe que probabilidades (float) quieres probar, escribe 'done' para terminar" << endl;
    vector<float> test;
    string probs;
    while(cin >> probs and isFloat(probs)) {
            float floatie = stof(probs);
            test.push_back(floatie);
    }    
    return test;
}

void save_dict_file(int type, uint n, uint dict_arr[]) {
    string dic_file_name(EXPER_PREFIX+to_string(type)+'-'+to_string(n)+DICT_EXT);
    cout << "Escribiendo archivo de diccionario (" << dic_file_name << ")" << endl;
    random_shuffle(dict_arr, dict_arr+n);
    ofstream dic_file(dic_file_name);
    if (dic_file.is_open()) {
        for (uint i = 0; i < n; ++i) {
            dic_file << dict_arr[i] <<'\n';
        }
        dic_file.close();
    } else {
        cout << "Error al escribir el diccionario '" << dic_file_name << "'" << endl;
        exit(0);
    }
}

void save_text_file(int type, uint n, uint m,float prob) {
    string txt_file_name(EXPER_PREFIX+to_string(type)+'-'+to_string(n)+'-'+to_string(m)+'-'+to_string(prob)+TXT_EXT);
    cout << "Escribiendo archivo de texto (" << txt_file_name << ")" << endl;
    random_shuffle(text.begin(), text.end());
    ofstream txt_file(txt_file_name);
    if (txt_file.is_open()) {
        for (uint x = 0; x < text.size(); x++) {
//         for (int x : text) {
            txt_file << text[x] <<'\n';
        }
        txt_file.close();
    } else {
        cout<<"Error al escribir el texto '" << txt_file_name << "'" << endl;
        exit(0);
    } 
}


void crear_diccionario(uint n, int experiment, uint dict_arr[]) {
    cout<<"Creando diccionario ("<<n<<" palabras)"<<endl;
    while (dict.size() < n) {
        uint x = rand()%MAX_WORD_LEN;
        if (!dict.count(x)) dict.insert(x);
    }
    
    auto it = dict.begin();
    //   uint dict_arr[n];
    for (uint i=0; i<n; ++i, ++it) {
        dict_arr[i] = *it;
    }
    sort(dict_arr, dict_arr+n);    
    save_dict_file(experiment,n,dict_arr);
}

void crear_texto(int mfactor, uint n, float prob, int experiment, uint dict_arr[]) {
    text.clear();
    uint m = mfactor*n;
    cout << "Creando texto (" << m << " palabras)" << endl;
    
    text.resize(m);
    for (uint i = 0; i < m; ++i) {
        float flnum = (rand()%100)/100.0;
        if (flnum <= prob) {
            text[i] = dict_arr[rand()%n];
        } else {
            uint x = rand()%MAX_WORD_LEN;
            while (dict.count(x)) {
                x = rand()%MAX_WORD_LEN;
            }
            text[i] = x;
        }
    }
    save_text_file(experiment,n,m,prob);
}


void test_different_sizes(uint n, float prob, uint number_texts, int experiment, uint dict_arr[]) {
    for(uint i = 1; i <= number_texts; ++i) {
        crear_texto(i+1,n,prob,experiment,dict_arr);
    }
}


void test_different_prob(uint n, int exp, vector<float> probs, uint dict_arr[]) {
   for(uint i = 0; i < probs.size(); ++i) {
       crear_texto(2,n,probs[i],exp,dict_arr);
   }
}


int main() {
  // esta funcion accelera el cin/cout
  ios_base::sync_with_stdio(0);
  cin.tie(0);
  
  srand(5);
  
  uint n;
  uint number_texts;
  vector<float> probs = {0,0.1,0.2,0.3,0.4,0.5,0.6,0.7,0.8,0.9,1.0};
  
  cout<<"Modalidad de creacion de textos:"<<endl;
  cout<<"1 - Crea textos con diferente tamaño"<<endl;
  cout<<"2 - Crea textos con diferentes probabilidades"<<endl;
  int expression;
  cin >> expression;
  
  switch(expression) {
    case 1:
    {
        cout << "1 - Crea textos con diferente tamaño" << endl;
        cout << "Inserta las palabras en el diccionario:" << endl;
        cin >> n;
        uint dict_arr1[n];
        cout << "Inserta la probabilidad que las palabras del texto esten en el diccionario" << endl;
        float prob;
        cin >> prob;
        while (prob < 0.0 or prob > 1.0) {
            cout<<"La proporción de palabras [0% .50%.. 100%] va de 0.. 0.5 1.00"<<endl;
            cin>>prob;
        } 
        
        crear_diccionario(n,expression,dict_arr1);
        cout << "Inserta cuantos textos quieres crear: (2n, 3n, 4n, etc)" << endl;
        cin >> number_texts;        
        test_different_sizes(n,prob,number_texts,expression,dict_arr1);        
        break;
    }
    case 2:
    {
        cout << "2 - Crea textos con diferentes probabilidades" << endl;
        cout << "Inserta las palabras en el diccionario:" << endl;
        cin >> n;
        uint dict_arr2[n];
        crear_diccionario(n,expression,dict_arr2);
        cout << "Por default se creara un texto para cada probabilidad desde [0% ... 100%]" << endl;
        cout<<"1 - Default probabilidades"<<endl;
        cout<<"2 - Crear nuevas probabilidades"<<endl;
        int option;
        cin >> option;        
        if(option == 2) {
            vector<float> new_probs = cambiar_probabilidades();
            test_different_prob(n, expression, new_probs, dict_arr2);
        } else if (option == 1) {
            test_different_prob(n, expression, probs, dict_arr2);
        }
        break;
    }
    }    
    cout << "The end." << endl;
}
