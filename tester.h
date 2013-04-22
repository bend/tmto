/*
 * =====================================================================================
 *
 *       Filename:  tester.h
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
#ifndef __TESTER_H__
#define __TESTER_H__

#include "utils.h"

#include <vector>
#include <iostream>
#include <boost/unordered_map.hpp>

struct load_pass_params {
    std::vector<unsigned char*> pass;
    char* path;
    int nb_pass;
};

struct load_table_params {
    char* path;
    int nb_entries;
    boost::unordered_map<std::string, std::string> *map;
};

struct search_params {
    int chain_length;
    boost::unordered_map<std::string, std::string> *map;
    int next_pass_index;
    std::vector<unsigned char*> *pass;
    pthread_mutex_t lock;
    pthread_mutex_t plock;
    FILE* output;
};

struct run_params {
    char* pass_path;
    unsigned int nb_pass;
    char* table_path;
    unsigned int nb_entries;
    unsigned int chain_length;
    unsigned int nb_threads;
    char* output_path;
};

void* table_preload(void* args);

void* password_preload(void* args);

std::vector<std::string> read_pass(const char* path);

void load_chunck(std::string s, unsigned char* p, unsigned int i);

unsigned char* to_char(int input);

bool equal(unsigned char* a, unsigned char* b, unsigned int size) ;

void* search(void* args);

void start(struct run_params *params);

void thread_search(search_params *params, unsigned int nb_threads);

void fprint(unsigned char* hash, unsigned char* pass, FILE* f);

#endif
