/*
 * =====================================================================================
 *
 *       Filename:  utils.cpp
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

#include "utils.h"
#include <stdio.h>

unsigned char* get_start_point(int i){
    unsigned char* val = new unsigned char[4]();
    int startPoint =  800*i;
    
    val[3] = ( startPoint & (0xFF));
    val[2] = ((startPoint >> 8) & 0xFF);
    val[1] = ((startPoint >> 16) & 0xFF);
    val[0] = ((startPoint >> 24) & 0xFF);
    
    return val;
}

unsigned char* sha1p4(unsigned char* input){
    unsigned char *obuf = new unsigned char[20]();
    SHA1(input, 4,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    return obuf;
}


void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

void print_int(unsigned char* buf, int s){
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

unsigned char* reduce(unsigned char* ep, int i){
    unsigned char *newBuf = new unsigned char[4]();
    if((i % 2) == 0)
        newBuf[0] =((ep[0] + i) % 255);
    else
        newBuf[0] =((ep[0] - i) % 255); 
    
    if((i % 3) == 0)
        newBuf[1] =((ep[1] + i) % 255);
    else
        newBuf[1] =((ep[1] - i) % 255);
    
    if((i % 5) == 0)
        newBuf[2] =((ep[2] + i) % 255);
    else
        newBuf[2] =((ep[2] - i) % 255);

    if((i % 7) == 0)
        newBuf[3] =((ep[3] + i) % 255);
    else
        newBuf[3] =((ep[3] - i) % 255);    

    int parity = (isOddParity(newBuf[0]) + isOddParity(newBuf[1]) + isOddParity(newBuf[2]) + isOddParity(newBuf[3]) ) % 2;
    if( parity == 1){
        int in = i % 4;
        if (in == 0)
            newBuf[0] ^= 1;
        if (in == 1)
            newBuf[1] ^= 1;
        if (in == 2)
            newBuf[2] ^= 1;
        if (in == 3)
            newBuf[3] ^= 1;      
    }
    return newBuf;   
}
