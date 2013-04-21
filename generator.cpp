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
 *         Author:  Benoit Daccache
 *   Organization:  
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/sha.h>
#include <iostream>
using namespace std;

void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
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


unsigned char* chain(unsigned char* value, int chainSize){
    unsigned char* sha;
    unsigned char* red = new unsigned char[4]();
    memcpy(red, value, 4);
            
    int i = 0;
    while(i < chainSize) {   
        sha = sha4(red);
        //if (i == 50){
         //   cout << "HASH : ";
        //    print(sha,20);
        //}
        delete[] red;
        red = reduce(sha, i);
        delete[] sha;
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

unsigned char* calculStartPoint(int i,int nbEntries){
    unsigned char* val = new unsigned char[4]();
    int startPoint =  800*i;
    
    val[3] = ( startPoint & (0xFF));
    val[2] = ((startPoint >> 8) & 0xFF);
    val[1] = ((startPoint >> 16) & 0xFF);
    val[0] = ((startPoint >> 24) & 0xFF);
    
    return val;
       
}


int main()
{
    int input = 0;

    int nbEntries = 2500000;
    int chainLength = 3000;

    unsigned char value [4];
    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);
    int i = 0;
    
    unsigned char* ep = new unsigned char[4]();
    unsigned char* sp = new unsigned char[4]();

    
    cout << "NbEntries : " << nbEntries << endl;
    cout << "chainLength : " << chainLength << endl;
    memcpy(ep, value, 4);
    FILE* tf = fopen("table2.dat", "w");
    
    while(i<nbEntries){
        writeChar(tf, ep);
        unsigned char* oldep = ep;
        sp = calculStartPoint(i,nbEntries);
        //cout << "SP : ";
        //print(sp,4);
        ep = chain(sp, chainLength);
        //cout << "EP : ";
        //print(ep,4);
        delete[] oldep;
        //writeChar(tf,ep);
        if(i%100 == 0) {
            printf("%f\n",(i/(double)nbEntries)*100.0); 
        }
        ++i;
    }
    //write last EP
     writeChar(tf, ep);
    cout << "End Point : ";
    print(ep,4);
    delete[] ep;
    fclose(tf);
    return 0;
}

