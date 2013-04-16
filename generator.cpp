/*
 * =====================================================================================
 *
 *       Filename:  generator.cpp
 *
 *    Description:  a
 *    
 *
 *        Version:  1.0
 *        Created:  04/15/2013 11:37:01 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
using namespace std;

void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

unsigned char* sha4(int input){
    unsigned char value [4];
    unsigned char *obuf = new unsigned char[20]();

    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);

    SHA1(value, 4,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    return obuf;
}

unsigned char* sha4(unsigned char* input){
    unsigned char *obuf = new unsigned char[20]();
    SHA1(input, 4,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    SHA1(obuf, 20,obuf);
    return obuf;
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

unsigned char* chainage(unsigned char* value, int chainSize){
    unsigned char* sha;
    unsigned char* red = (unsigned char*)malloc(4);
    memcpy(red, value, 4);
    int i = 0;
    while(i < chainSize) {
        sha = sha4(red);
        free(red);
        red = reduce(sha, i);
        free(sha);
        ++i;
    }
    return red;
}

void writeChar(FILE* f, unsigned char* c){
    fputc(c[0], f);
    fputc(c[1], f);
    fputc(c[2], f);
    fputc(c[3], f);
}


int main()
{
    int input = 0;

    int nbEntries = 1250000;
    int chainLength = 2000;

    unsigned char value [4];
    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);
    int i = 0;
    unsigned char* ep = value;
    FILE* tf = fopen("table.dat", "w");
    while(i<nbEntries){
        writeChar(tf, ep);
        ep = chainage(ep, chainLength);
        writeChar(tf,ep);
        if(i%100 == 0)
            printf("%f \n",(i/(double)nbEntries)*100.0); 
        ++i;
    }
    fclose(tf);

    return 0;
}

