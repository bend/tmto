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

void load_chunck(string s, unsigned char* p, unsigned int i){
    int num = (int) strtoul(s.c_str(), NULL, 16);
    unsigned char* d = to_char(num);
    unsigned int z = 0;
    for(unsigned int k=i ; k < i+4; ++k, ++z){
        p[k] = d[z];
    }
    delete[] d;
}

vector<string> read_pass(const char* path, int nb_pass) {
    fstream fin;
    vector<string> pass;
    fin.open(path, ios::in);
    string word;
    if (fin.is_open()) {
        int i=0;
        while(i< nb_pass){
            getline(fin, word, '\n');
            pass.push_back(word);
            ++i;
        }
    }
    fin.close();
    return pass;
}

void* password_preload(void* args){
    struct load_pass_params *arg = (struct load_pass_params*)args;
    vector<string> pass = read_pass(arg->path, arg->nb_pass);
    if((int)pass.size() != (int)arg->nb_pass) {
        perror("Read pass failed");
        exit(-1);
    }
    int i = 0;
    while(i < arg->nb_pass){
        unsigned char* tPass = new unsigned char[arg->nb_pass]();
        string s = pass[i].substr(0, 8);
        load_chunck(s,tPass, 0);

        s = pass[i].substr(8, 8);
        load_chunck(s,tPass, 4);

        s = pass[i].substr(16, 8);
        load_chunck(s,tPass, 8);

        s = pass[i].substr(24, 8);
        load_chunck(s,tPass, 12);

        s = pass[i].substr(32, 8);
        load_chunck(s,tPass, 16);

        arg->pass.push_back(tPass);
        ++i;
    }
    cout<<"Info : Finished pass load"<<endl;
    return NULL;
}

void* table_preload(void* args) {
    struct load_table_params *arg = (struct load_table_params*) args;
    FILE* f = fopen(arg->path, "r");
    unsigned char* ep = new unsigned char[4]();
    unsigned char* sp = new unsigned char[4]();
    int i = 0;
    while(i < arg->nb_entries) {
        for(int k = 0; k<4; ++k)
            sp[k] = fgetc(f);
        for(int k = 0; k<4; ++k)
            ep[k] = fgetc(f);
        //save the first value
        string s_sp((const char*)(sp), 4);
        string s_ep((const char*)(ep), 4);
        (*arg->map)[s_ep] = s_sp;
        ++i;
    }
    fclose(f);
    delete[] ep;
    delete[] sp;
    cout<<"Info : Finished table load"<<endl;
    return NULL;
}

void* search(void* args){
    struct search_params *arg = (struct search_params*)args;
    int len = arg->chain_length;
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
        for (int index = 1; index < len + 1; ++index) {
            pthread_mutex_unlock(&arg->lock);
            int count = 1;
            unsigned char* red = reduce(pass, len - index );
            while(count < index) {
                unsigned char* sha = sha1p4(red);
                delete[] red;
                red = reduce(sha, len - index + count);
                delete[] sha;
                ++count;
            }
            string s_red((const char*)(red), 4);
            if (map->count(s_red)){
                //Launch the search with the SP
                string sp = (*map)[s_red];
                unsigned char* char_sp = (unsigned char*) sp.c_str();
                unsigned char* f = 0;
                unsigned char* spp = (unsigned char*)malloc(4);
                memcpy(spp, char_sp, 4);
                for(int index = 0; index < len; ++index) {
                    unsigned char* sha = sha1p4(spp);
                    if(strncmp((const char*)sha, (const char*)pass, 20) == 0 ){
                        delete[] sha;
                        f = spp;
                        break;
                    }
                    else {
                        delete[] spp;
                        spp = reduce(sha, index);
                    }
                    delete[] sha;
                }
                if(f) {
                    pthread_mutex_lock(&arg->plock);
                    cout<<"\rInfo : Password found ("<<++arg->nb_found<<"/"<<arg->nb_pass<<")";
                    fflush(stdout);
                    fprint(pass,f, arg->output);
                    pthread_mutex_unlock(&arg->plock);
                    delete[] f;
                    break;
                }
            }    
        }
    }
    return NULL;
}

unsigned char* invert_result(unsigned char* pass){
    unsigned char* n = new unsigned char[4]();
    n[0] = pass[3];
    n[1] = pass[2];
    n[2] = pass[1];
    n[3] = pass[0];
    return n;
}

void fprint(unsigned char* hash, unsigned char* pass, FILE* f){
    int i;
    unsigned char* inv = invert_result(pass);
    for (i = 0; i < 20; i++) {
        fprintf(f, "%02x ", hash[i]);
    }
    fprintf(f, " | ");
    for (i = 0; i < 4; i++) {
        fprintf(f, "%02x ", inv[i]);
    }
    fprintf(f,"\n");
    fflush(f);
    delete[] inv;
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
    sp.chain_length = params->chain_length;
    sp.map = map;
    pthread_mutex_init(&sp.lock, NULL);
    pthread_mutex_init(&sp.plock, NULL);
    sp.pass = &pp.pass;
    sp.next_pass_index = 0;
    sp.nb_pass = params->nb_pass;
    sp.nb_found = 0;
    FILE* f = fopen(params->output_path, "w");
    if(!f){
        perror("ERROR : fopen() : ");
        exit(-1);
    }
    sp.output = f;

    thread_search(&sp, params->nb_threads);
    fclose(f);
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
    rp.output_path = "found.txt";

    start(&rp);

    return 0;
}



