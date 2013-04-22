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
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sstream>
#include <fstream>
#include "tester.h"
using namespace std;

using namespace boost;

        
unsigned char* to_char(int input){
    unsigned char *value = new unsigned char[4]();

    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);
    return value;

}

void* table_preload(void* args) {
    struct load_table_params *arg = (struct load_table_params*) args;
    FILE* f = fopen(arg->path, "r");
    unsigned char* endPoint = new unsigned char[4]();
    unsigned char* startPoint = new unsigned char[4]();
    int i = 0;
    while(i < arg->nb_entries) {
        startPoint[0] = fgetc(f);
        startPoint[1] = fgetc(f);
        startPoint[2] = fgetc(f);
        startPoint[3] = fgetc(f);
        endPoint[0] = fgetc(f);
        endPoint[1] = fgetc(f);
        endPoint[2] = fgetc(f);
        endPoint[3] = fgetc(f);
        //save the first value
        string s_sp((const char*)(startPoint), 4);
        string s_ep((const char*)(endPoint), 4);
        (*arg->map)[s_ep] = s_sp;
        ++i;
    }
    fclose(f);
    delete[] endPoint;
    delete[] startPoint;
    
    cout<<"Info :: table_preload end"<<endl;

    return NULL;
}

void* password_preload(void* args){
    struct load_pass_params *arg = (struct load_pass_params*)args;
    fstream fin;
    vector<string> pass;
    fin.open(arg->path, ios::in);
    string word;
    if (fin.is_open()) {
        int i=0;
        while(i< arg->nb_pass){
            getline(fin, word, '\n');
            pass.push_back(word);
            ++i;
        }
        stringstream ss;
        i = 0;
        while(i < arg->nb_pass){
            unsigned char* tPass = new unsigned char[20]();
            string s = pass[i].substr(0, 8);
            int num1 = (int) strtoul(s.c_str(), NULL, 16);
            unsigned char* d1 = to_char(num1);
            tPass[0] = d1[0];
            tPass[1] = d1[1];
            tPass[2] = d1[2];
            tPass[3] = d1[3];
            delete[] d1;
            
            s = pass[i].substr(8, 8);
            int num2 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d2 = to_char(num2);
            tPass[4] = d2[0];
            tPass[5] = d2[1];
            tPass[6] = d2[2];
            tPass[7] = d2[3];
            delete[] d2;
            
            s = pass[i].substr(16, 8);
            int num3 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d3 = to_char(num3);
            tPass[8] = d3[0];
            tPass[9] = d3[1];
            tPass[10] = d3[2];
            tPass[11] = d3[3];
            delete[] d3;
            
            s = pass[i].substr(24, 8);
            int num4 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d4 = to_char(num4);
            tPass[12] = d4[0];
            tPass[13] = d4[1];
            tPass[14] = d4[2];
            tPass[15] = d4[3];
            delete[] d4;
            
            s = pass[i].substr(32, 8);
            int num5 = (int)strtoul(s.c_str(), NULL, 16);
            unsigned char* d5 = to_char(num5);
            tPass[16] = d5[0];
            tPass[17] = d5[1];
            tPass[18] = d5[2];
            tPass[19] = d5[3];
            delete[] d5;
            arg->pass.push_back(tPass);
            ++i;
        }
    }
    fin.close();
    cout<<"Finished pass load"<<endl;
    return NULL;
}

unsigned char* find(unsigned char* s, int len, unsigned char* pass) {
    unsigned char* red = (unsigned char*)malloc(4);
    memcpy(red, s, 4);
    for(int index = 0; index < len; ++index) {
        unsigned char* sha = sha1p4(red);
        if(strncmp((const char*)sha, (const char*)pass, 20) == 0 ){
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
    unordered_map<string, string> *map = arg->map;
    unsigned char* pass;
    /*  Get Next pass */
    while(1){
        pthread_mutex_lock(&arg->lock);
        if(arg->next_pass_index < (int)arg->pass->size()) {
            pass = (*arg->pass)[arg->next_pass_index];
            arg->next_pass_index++;
        }else {
            pthread_mutex_unlock(&arg->lock);
            return NULL;
        }
        cout<<"Pass is : ";
        print(pass, 20);

        for (int index = 1; index < chainLength + 1; ++index) {
            pthread_mutex_unlock(&arg->lock);
            int count = 1;
            unsigned char* red = reduce(pass, chainLength - index );
            while(count < index) {
                unsigned char* sha = sha1p4(red);
                delete[] red;
                red = reduce(sha, chainLength - index + count);
                delete[] sha;
                ++count;
            }
            //Check if reduce is on the hashmap
            //convert into string
            string s_red((const char*)(red), 4);
            //delete[] red;
            //Found in the hashmap ?
            if (map->count(s_red)){
                //Launch the search with the SP
                string sp = (*map)[s_red];

                unsigned char* char_sp = (unsigned char*) sp.c_str();
                
                unsigned char* f = find(char_sp, chainLength, pass);
                if(f) {
                    pthread_mutex_lock(&arg->plock);
                    cout << "== Hash FOUND ! ==" << endl;
                    cout << "INT : ";
                    print(f,4);
                    cout << "HASH : ";
                    print(pass, 20);
                    pthread_mutex_unlock(&arg->plock);
                    delete[] f;
                    break;
                }
            }    
        }
    }
    return NULL;
}

void thread_search(struct search_params *sp, unsigned int nb_threads){
    
    pthread_t threads[nb_threads];

    for(unsigned int i = 0; i< nb_threads; ++i){
        pthread_create(&threads[i], NULL, &search, (void*)sp);
    }
    
    for(unsigned int i = 0; i< nb_threads; ++i){
        pthread_join(threads[i], NULL);
    }

}

void start(struct run_params* params){
    /* Load table args  */
    unordered_map<string,string> *map = new unordered_map<string, string>();
    struct load_table_params tp;
    tp.map = map;
    tp.path = params->table_path;
    tp.nb_entries = params->nb_entries;

    /*  Load pass args */
    struct load_pass_params pp;
    pp.path = params->pass_path;
    pp.nb_pass = params->nb_pass;
    
    /*  Load table and args in threads */
    pthread_t load_pass_thread;
    pthread_t load_table_thread;
    pthread_create(&load_table_thread, NULL, &table_preload, (void*)&tp);
    pthread_create(&load_pass_thread, NULL, &password_preload, (void*)&pp);
    pthread_join(load_table_thread, NULL);
    pthread_join(load_pass_thread, NULL);
    /* Load completed */

    /* Search params*/
    struct search_params sp;
    sp.chainLength = params->chain_length;
    sp.map = map;
    pthread_mutex_init(&sp.lock, NULL);
    pthread_mutex_init(&sp.plock, NULL);
    sp.pass = &pp.pass;
    sp.next_pass_index = 0;

    thread_search(&sp, params->nb_threads);
}

int main(char* argc, char** argv)
{
    //char path[20] = "table10000.dat";
    //char path2[20] = "pass.txt";
    //
    struct run_params rp;
    rp.chain_length = 2000;
    rp.nb_entries = 2000000;
    rp.nb_pass = 20;
    rp.nb_threads = 4;
    rp.table_path = "table2.dat";
    rp.pass_path =  "pass.txt";

    start(&rp);

    return 0;
}



