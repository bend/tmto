/*
 * =====================================================================================
 *
 *       Filename:  rainbow_gen.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/2013 11:37:01 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Benoit Daccache, 
 *   Organization:  
 *
 * =====================================================================================
 */
#ifndef __GEN_H__
#define __GEN_H__

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

struct run_params{
    char* path;
    int nb_entries;
    int chain_length;
    int init;
};

unsigned char* get_sp(int iteration);

unsigned char* create_chain(unsigned char* val, size_t size);

void fwrite(FILE* file, unsigned char* c);

void generate(struct run_params* params); 


#endif
