/*
 * =====================================================================================
 *
 *       Filename:  tester.cpp
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/16/2013 11:49:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Benoit Daccache (BD), ben.daccache@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

#include <boost/unordered_map.hpp>

struct load_pass_params {
    vector<unsigned char*> pass;
    char* path;
    int nbPass;
};

struct load_table_params {
    char* path;
    int nbEntries;
    boost::unordered_map<string, string> map;
};

struct search_params {
    int chainLength;
    boost::unordered_map<string, string> map;
};

unsigned int nextI;
vector<unsigned char*> vectPass;
pthread_mutex_t lock;
pthread_mutex_t plock;
        

void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

void printToInt(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%u ", buf[i]);
    }
    printf("\n");
}

int isOddParity(unsigned char myChar) {
    myChar ^= myChar >> 4;
    myChar &= 0xf;
    return (0x6996 >> myChar) & 1;
}

unsigned char* reduce(unsigned char *hash, int i){
    unsigned char *newBuf = new unsigned char[4]();
    newBuf[0] =((hash[0] + i) % 255); 
    newBuf[1] =((hash[1] + i) % 255);
    newBuf[2] =((hash[2] + i) % 255);
    newBuf[3] =((hash[3] + i) % 255);
    int parity = (isOddParity(newBuf[0]) + isOddParity(newBuf[1]) + isOddParity(newBuf[2]) + isOddParity(newBuf[3]) ) % 2;
    if( parity == 1){
        newBuf[0] ^= 1;
    }
    return newBuf;
}

unsigned char* intToChar(int input){
    unsigned char *value = new unsigned char[4]();

    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);
    return value;

}


unsigned char* sha4(unsigned char* input){
    unsigned char *obuf = new unsigned char[20]();
    SHA1(input, 4,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    return obuf;
}

/*
 * Load the table in the hashmap with Key = EP and Value = SP
 */
void* loadTable(void* args) {
    struct load_table_params *arg = (struct load_table_params*)args;
    const char* path = arg->path;
    int nbEntries = arg->nbEntries;
    boost::unordered_map<string, string> map = arg->map;
    FILE* f = fopen(path, "r");
    int i = 0;
    unsigned char* sp = new unsigned char[4]();
    unsigned char* ep = new unsigned char[4]();
    
    //load the 2 first values
    sp[i] = fgetc(f);
    sp[i+1] = fgetc(f);
    sp[i+2] = fgetc(f);
    sp[i+3] = fgetc(f);

    ep[i] = fgetc(f);
    ep[i+1] = fgetc(f);
    ep[i+2] = fgetc(f);
    ep[i+3] = fgetc(f);
    
    //save the first value
    string s_sp((const char*)(sp), 4);
    string s_ep((const char*)(ep), 4);
    map[s_ep] = s_sp;

    while(i < nbEntries-1){
        //New StartPoint = old EndPoint
        s_sp = s_ep;
        
        unsigned char* char_sp = (unsigned char*) s_sp.c_str();
        
        //get the new EndPoint
        ep[0] = fgetc(f);
        ep[1] = fgetc(f);
        ep[2] = fgetc(f);
        ep[3] = fgetc(f);
        cout << "STARTING POINT : ";
        print(char_sp,4);
        cout << "END POINT : ";
        print(ep,4);
        s_ep = string((const char*)(ep), 4);
        //save the values
        map[s_ep] = s_sp;
        ++i;
    }
    delete[] ep;
    delete[] sp;
    fclose(f);
    return NULL;
}

/*
 * Load the hash in a table. save in char*
 */
void* loadPass(void* args){
    struct load_pass_params *arg = (struct load_pass_params*)args;
    const char* path = arg->path;
    int nbEntries = arg->nbPass;
    vector<unsigned char*> vPass = arg->pass;
    fstream fin;
    vector<string> pass;
    fin.open(path, ios::in);
    string word;
    if (fin.is_open()) {
        int i=0;
        while(i<nbEntries){
            getline(fin, word, '\n');
            pass.push_back(word);
            ++i;
        }
        stringstream ss;
        i = 0;
        while(i<nbEntries){
            unsigned char* tPass = new unsigned char[20]();
            string s = pass[i].substr(0, 8);
            int num1 = (int) strtoul(s.c_str(), NULL, 16);
            unsigned char* d1 = intToChar(num1);
            tPass[0] = d1[0];
            tPass[1] = d1[1];
            tPass[2] = d1[2];
            tPass[3] = d1[3];
            delete[] d1;
            
            s = pass[i].substr(8, 8);
            int num2 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d2 = intToChar(num2);
            tPass[4] = d2[0];
            tPass[5] = d2[1];
            tPass[6] = d2[2];
            tPass[7] = d2[3];
            delete[] d2;
            
            s = pass[i].substr(16, 8);
            int num3 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d3 = intToChar(num3);
            tPass[8] = d3[0];
            tPass[9] = d3[1];
            tPass[10] = d3[2];
            tPass[11] = d3[3];
            delete[] d3;
            
            s = pass[i].substr(24, 8);
            int num4 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d4 = intToChar(num4);
            tPass[12] = d4[0];
            tPass[13] = d4[1];
            tPass[14] = d4[2];
            tPass[15] = d4[3];
            delete[] d4;
            
            s = pass[i].substr(32, 8);
            int num5 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d5 = intToChar(num5);
            tPass[16] = d5[0];
            tPass[17] = d5[1];
            tPass[18] = d5[2];
            tPass[19] = d5[3];
            delete[] d5;
            vPass.push_back(tPass);
            ++i;
        }
    }
    fin.close();
    arg->pass = vPass;
    cout<<"Finished pass load"<<endl;
    return NULL;
}
bool isEqual(unsigned char* a, unsigned char* b, unsigned int size) {
    unsigned int i = 0;
    while(i<size) {
        if(a[i] != b[i] )
            return false;
        ++i;
    }
    return true;
}

void reorderPass(unsigned char* pass, int index, int nbPass){
    while(index < nbPass){
        for(int i = 0; i < 20; i++){
                pass[index*20+i] = pass[(index+1)*20+i];
        }
        ++index;
    }      

     
}

unsigned char* found(unsigned char* tsp, int lengthChain, unsigned char* pass) {
    unsigned char* red = (unsigned char*)malloc(4);
    memcpy(red, tsp, 4);
    for(int index = 0; index < lengthChain; ++index) {
        unsigned char* sha = sha4(red);
        if(isEqual(sha, pass, 20) ){
            delete[] sha;
            return red;
        }
        else {
            delete[] red;
            red = reduce(sha, index);
        }
        delete[] sha;
    }
    delete[] red;
    return NULL;
}


void* search(void* args){
    struct search_params *arg = (struct search_params*)args;
    int chainLength = arg->chainLength;
    boost::unordered_map<string, string> map = arg->map;

    for (int index = 1; index < chainLength + 1; ++index) {
        while(1) {
            unsigned char* pass;
            /*  Get Next pass */
            pthread_mutex_lock(&lock);
            if(nextI < vectPass.size()) {
                pass = vectPass[nextI];
                ++nextI;
            }else {
                pthread_mutex_unlock(&lock);
                return NULL;
            }
            pthread_mutex_unlock(&lock);
            //cout<<"Processing pass : ";
            print(pass, 20);

            int count = 1;
            unsigned char* red = reduce(pass, chainLength - index );
            while(count < index) {
                unsigned char* sha = sha4(red);
                delete[] red;
                red = reduce(sha, chainLength - index + count);
                delete[] sha;
                ++count;
            }
            //Check if reduce is on the hashmap
            //convert into string
            string s_red((const char*)(red), 4);
            //Found in the hashmap ?
            if (map.count(s_red)){
                cout << " ATTENTION : Valeur en HASH MAP"<< endl;

                //Launch the search with the SP
                string sp = map[s_red];

                unsigned char* char_sp = (unsigned char*) sp.c_str();
                cout << "StartPoint : ";
                print(char_sp,4);
                cout << "EndPoint : ";
                print(red,4);
                unsigned char* f = found(char_sp, chainLength, pass);
                if(f) {
                    pthread_mutex_lock(&plock);
                    cout << "== Hash FOUND ! ==" << endl;
                    cout << "INT : ";
                    print(f,4);
                    cout << "HASH : ";
                    print(pass, 20);
                    pthread_mutex_unlock(&plock);
                    delete[] f;
                }
            }    
            delete[] red;
        }
    }
    return NULL;
}


int main()
{
    int nbEntries = 10;
    int chainLength = 1;
    int nbPass = 20;
    int nbThreads = 2;
    cout << "== INIT ==" << endl;
    cout << "NbEntries : " << nbEntries << endl;
    cout << "chainLength : " << chainLength << endl;
    cout << "nbPass : " << nbPass << endl;
    boost::unordered_map<string,string> map;
    char path[20] = "table2.dat";
    char path2[20] = "pass.txt";

    /* Load table args  */
    struct load_table_params tp;
    tp.map = map;
    tp.path = path;
    tp.nbEntries = nbEntries;

    /*  Load pass args */
    struct load_pass_params pp;
    pp.path = path2;
    pp.nbPass = nbPass;

    /*  Load table and args in threads */
    pthread_t load_pass_thread;
    pthread_t load_table_thread;
    pthread_create(&load_table_thread, NULL, &loadTable, (void*)&tp);
    pthread_create(&load_pass_thread, NULL, &loadPass, (void*)&pp);
    pthread_join(load_table_thread, NULL);
    pthread_join(load_pass_thread, NULL);

    /* Init mutex */
    pthread_mutex_init(&lock, NULL);
    pthread_mutex_init(&plock, NULL);
    vectPass = pp.pass;
    nextI = 0;
    
    /* Search params*/
    struct search_params sp;
    sp.chainLength = chainLength;
    sp.map = map;
  
    pthread_t threads[nbThreads];

    for(int i = 0; i< nbThreads; ++i){
        pthread_create(&threads[i], NULL, &search, (void*)&sp);
    }
    
    for(int i = 0; i< nbThreads; ++i){
        pthread_join(threads[i], NULL);
    }


    return 0;
}



