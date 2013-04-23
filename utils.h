/*
 * =====================================================================================
 *
 *       Filename:  utils.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/15/2013 11:37:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Benoit Daccache,
 *
 * =====================================================================================
 */

#ifndef __UTILS_H__
#define __UTILS_H__

#include <openssl/sha.h>

unsigned char* sha1p4(unsigned char* input);

void print(unsigned char* buf, int s);

void print_int(unsigned char* buf, int s);

int isOddParity(unsigned char myChar);

unsigned char* reduce(unsigned char* ep, int i);

unsigned char* to_char(int input);

#endif
