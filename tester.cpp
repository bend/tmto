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
#include <getopt.h>
#include "tester.h"
using namespace std;

using namespace boost;

void load_chunk(string str, unsigned char* p, unsigned int i){
    int toi= (int) strtoul(str.c_str(), NULL, 16);
    unsigned char* d = to_char(toi);
    unsigned int z = 0;
    for(unsigned int k=i ; k < i+4; ++k, ++z){
        p[k] = d[z];
    }
    delete[] d;
}

vector<string> read_pass(const char* path) {
    fstream stream;
    vector<string> pass;
    stream.open(path, ios::in);
    string chunk;
    if (stream.is_open()) {
        int i=0;
        while(stream.good()){
            getline(stream, chunk, '\n');
            if(chunk.size()  != 0) {
                pass.push_back(chunk);
                ++i;
            }
        }
    }
    stream.close();
    return pass;
}

void* password_preload(void* args){
    struct load_pass_params *arg = (struct load_pass_params*)args;
    vector<string> pass = read_pass(arg->path);
    if((int)pass.size() == 0) {
        perror("Error : Read pass failed");
        exit(-1);
    }
    size_t i = 0;
    while(i < pass.size()){
        unsigned char* tPass = new unsigned char[pass.size()]();
        string s = pass[i].substr(0, 8);
        load_chunk(s,tPass, 0);

        s = pass[i].substr(8, 8);
        load_chunk(s,tPass, 4);

        s = pass[i].substr(16, 8);
        load_chunk(s,tPass, 8);

        s = pass[i].substr(24, 8);
        load_chunk(s,tPass, 12);

        s = pass[i].substr(32, 8);
        load_chunk(s,tPass, 16);

        arg->pass.push_back(tPass);
        ++i;
    }
    cout<<"Info : Finished pass load ("<<i<<" pass)"<<endl;
    return NULL;
}

void* table_preload(void* args) {
    struct load_table_params *arg = (struct load_table_params*) args;
    FILE* f = fopen(arg->path, "r");
    unsigned char* ep = new unsigned char[4]();
    unsigned char* sp = new unsigned char[4]();
    int i = 0;
    int car = fgetc(f);
    while(car != EOF) {
        for(int k = 0; k<4; ++k){
            sp[k] = car;
            car = fgetc(f);
        }
        for(int k = 0; k<4; ++k){
            ep[k] = car;
            car = fgetc(f);
        }
        string s_sp((const char*)(sp), 4);
        string s_ep((const char*)(ep), 4);
        (*arg->map)[s_ep] = s_sp;
        ++i;
    }
    fclose(f);
    delete[] ep;
    delete[] sp;
    cout<<"Info : Finished table load ("<<i<<" entries)"<<endl;
    return NULL;
}

void* search(void* args){
    struct search_params *arg = (struct search_params*)args;
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
        unsigned int index = 1;
        while(index < arg->chain_length + 1){
            pthread_mutex_unlock(&arg->lock);
            unsigned int count = 1;
            unsigned char* red = reduce(pass, arg->chain_length - index );
            while(count < index) {
                unsigned char* sha = sha1p4(red);
                delete[] red;
                red = reduce(sha, arg->chain_length - index + count);
                delete[] sha;
                ++count;
            }
            string s_red((const char*)(red), 4);
            if (arg->map->count(s_red)){
                string sp = (*arg->map)[s_red];
                unsigned char* char_sp = (unsigned char*) sp.c_str();
                unsigned char* f = 0;
                unsigned char* spp = new unsigned char[4]();
                memcpy(spp, char_sp, 4);
                unsigned int ind =0;
                while(ind<arg->chain_length){
                    unsigned char* sha = sha1p4(spp);
                    if(strncmp((const char*)sha, (const char*)pass, 20) == 0 ){
                        delete[] sha;
                        f = spp;
                        break;
                    }
                    else {
                        delete[] spp;
                        spp = reduce(sha, ind);
                    }
                    delete[] sha;
                    ++ind;
                }
                if(f) {
                    pthread_mutex_lock(&arg->plock);
                    cout<<"\rInfo : Password found ("<<++arg->nb_found<<"/"<<arg->pass->size()<<")";
                    fflush(stdout);
                    fprint(pass,f, arg->output);
                    pthread_mutex_unlock(&arg->plock);
                    delete[] f;
                    break;
                }
            }    
            ++index;
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
        fprintf(f, "%02x", hash[i]);
    }
    fprintf(f, " | ");
    for (i = 0; i < 4; i++) {
        fprintf(f, "%02x", inv[i]);
    }
    fprintf(f,"\n");
    fflush(f);
    delete[] inv;
}

void thread_search(struct search_params *sp, unsigned int nb_threads){
    
    cout<<"Info : Starting search with up to "<<nb_threads<<" threads "<<endl;
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

    /*  Load pass args */
    struct load_pass_params pp;
    pp.path = params->pass_path;
    
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

int main(int argc, char** argv)
{
    int c;
    static struct option long_options[] =
    {
        {"table", required_argument, NULL, 't'},
        {"chainlength", required_argument, NULL, 'c'},
        {"pass", required_argument, NULL, 'p'},
        {"threads", required_argument, NULL, 's'},
        {"out", required_argument, NULL, 'o'},
        {NULL, 0, NULL, 0}
    };
    char found[] = "found.txt";
    int option_index = 0;
    struct run_params rp;
    rp.chain_length = 0;
    rp.nb_threads = 1;
    rp.output_path = found;
    rp.pass_path = NULL;
    rp.table_path = NULL;
    while ((c = getopt_long (argc, argv, "t:c:p:s:o:", long_options, &option_index)) != -1){
        switch (c)
        {
            case 't':
                rp.table_path = ((char*)optarg);
                break;
            case 'c':
                rp.chain_length = strtol((char*)optarg, NULL, 10);
                break;
            case 'p':
                rp.pass_path = ((char*)optarg);
                break;
            case 's':
                rp.nb_threads = strtol((char*)optarg, NULL, 10);
                break;
            case 'o':
                rp.output_path = ((char*)optarg);
                break;

        }
    }

    if(rp.chain_length == 0 || rp.pass_path == NULL || rp.table_path == NULL){
        cout<<"Error : Missing parameter(s)"<<endl;
        exit(-1);
    }

    start(&rp);

    return 0;
}



