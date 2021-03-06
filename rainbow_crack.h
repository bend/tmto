/*
 * =====================================================================================
 *
 *       Filename:  rainbow_crack.h
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
#ifndef __CRACK_H__
#define __CRACK_H__

#include "utils.h"

#include <vector>
#include <iostream>
#include <boost/unordered_map.hpp>

struct load_pass_params {
    std::vector<unsigned char*> pass;
    char* path;
};

struct load_table_params {
    char* path;
    boost::unordered_map<std::string, std::string> *map;
};

struct search_params {
    unsigned int chain_length;
    unsigned int nb_found;
    boost::unordered_map<std::string, std::string> *map;
    int next_pass_index;
    std::vector<unsigned char*> *pass;
    pthread_mutex_t lock;
    pthread_mutex_t plock;
    FILE* output;
};

struct run_params {
    char* pass_path;
    char* table_path;
    unsigned int chain_length;
    unsigned int nb_threads;
    char* output_path;
};

void* table_preload(void* args);

void* password_preload(void* args);

std::vector<std::string> read_pass(const char* path);

void load_chunk(std::string s, unsigned char* p, unsigned int i);

unsigned char* to_char(int input);

bool equal(unsigned char* a, unsigned char* b, unsigned int size) ;

void* search(void* args);

void start(struct run_params *params);

void thread_search(search_params *params, unsigned int nb_threads);

void fprint(unsigned char* hash, unsigned char* pass, FILE* f);

#endif
