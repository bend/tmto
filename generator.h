/*
 * =====================================================================================
 *
 *       Filename:  generator.h
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
#ifndef __GENERATOR_HH__
#define __GENERATOR_HH__

#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>

unsigned char* create_chain(unsigned char* val, size_t size);

void fwrite(FILE* file, unsigned char* c);

#endif
