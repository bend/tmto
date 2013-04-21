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
 *
 * =====================================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
using namespace std;

#include "utils.h"

unsigned char* chain(unsigned char* value, int chainSize){
    unsigned char* sha;
    unsigned char* red = new unsigned char[4]();
    memcpy(red, value, 4);
            
    int i = 0;
    while(i < chainSize) {   
        sha = sha1p4(red);
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
        sp = get_start_point(i);
        ep = chain(sp, chainLength);
        delete[] oldep;
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

