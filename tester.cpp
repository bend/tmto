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
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <openssl/sha.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
using namespace std;

void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
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

unsigned char* intToChar(int input){
    unsigned char *value = new unsigned char[4]();

    value[3] = ( input & (0xFF));
    value[2] = ((input >> 8) & 0xFF);
    value[1] = ((input >> 16) & 0xFF);
    value[0] = ((input >> 24) & 0xFF);
    return value;

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

int loadTable(const char* path, int nbEntries, unsigned char* sp, unsigned char* ep) {
    FILE* f = fopen(path, "r");
    int i = 0;
    while(i<nbEntries){
        sp[i] = fgetc(f);
        sp[i+1] = fgetc(f);
        sp[i+2] = fgetc(f);
        sp[i+3] = fgetc(f);

        ep[i] = fgetc(f);
        ep[i+1] = fgetc(f);
        ep[i+2] = fgetc(f);
        ep[i+3] = fgetc(f);
        ++i;
    }
    fclose(f);
    return 0;
}

unsigned char* loadPass(const char* path, int nbEntries){
    fstream fin;
    vector<string> pass;
    fin.open(path, ios::in);
    string word;
    unsigned char* decoded = new unsigned char[nbEntries*20]();
    if (fin.is_open()) {
        int i=0;
        while(i<nbEntries){
            getline(fin, word, '\n');
            pass.push_back(word);
            ++i;
        }
        stringstream ss;
        i = 0;
        while(i<nbEntries){
            string s = pass[i].substr(0, 8);
            int num1 = (int)strtol(s.c_str(), NULL, 16);
            unsigned char* d1 = intToChar(num1);
            decoded[i*20] = d1[0];
            decoded[i*20 + 1] = d1[1];
            decoded[i*20 + 2] = d1[2];
            decoded[i*20 + 3] = d1[3];
            delete[] d1;
            
            s = pass[i].substr(8, 8);
            int num2 = (int)strtol(s.c_str(), NULL, 16);
            unsigned char* d2 = intToChar(num2);
            decoded[i*20+4] = d2[0];
            decoded[i*20 + 5] = d2[1];
            decoded[i*20 + 6] = d2[2];
            decoded[i*20 + 7] = d2[3];
            delete[] d2;
            
            s = pass[i].substr(16, 8);
            int num3 = (int)strtol(s.c_str(), NULL, 16);
            unsigned char* d3 = intToChar(num3);
            decoded[i*20+8] = d3[0];
            decoded[i*20 + 9] = d3[1];
            decoded[i*20 + 10] = d3[2];
            decoded[i*20 + 11] = d3[3];
            delete[] d3;
            
            s = pass[i].substr(24, 8);
            int num4 = (int)strtol(s.c_str(), NULL, 16);
            unsigned char* d4 = intToChar(num4);
            decoded[i*20+12] = d4[0];
            decoded[i*20 + 13] = d4[1];
            decoded[i*20 + 14] = d4[2];
            decoded[i*20 + 15] = d4[3];
            delete[] d4;
            
            s = pass[i].substr(32, 8);
            int num5 = (int)strtol(s.c_str(), NULL, 16);
            unsigned char* d5 = intToChar(num5);
            decoded[i*20+16] = d5[0];
            decoded[i*20 + 17] = d5[1];
            decoded[i*20 + 18] = d5[2];
            decoded[i*20 + 19] = d5[3];
            delete[] d5;
            ++i;
        }
    }
    fin.close();
    
    return decoded;
}
bool isEqual(unsigned char* a, unsigned char* b, unsigned int size) {
    unsigned int i = 0;
    while(i<size) {
        if(a[i] != b[i] )
            return false;
        ++i;
    }
    return true;
}

unsigned char* found(unsigned char* tsp, int nbEntries, unsigned char* pass) {
    unsigned char* red = tsp;
    for(int index = 0; index<nbEntries; ++index) {
        unsigned char* sha = sha4(red);
        delete[] red;
        if(isEqual(sha, pass, 20) )
            return tsp;
        else {
            red = reduce(sha, index);
        }
        delete[] sha;
    }
    return NULL;
}


void search(unsigned char* sp, unsigned char* ep, int nbEntries, unsigned char* pass, int nbPass, int chainLength)
{
    for (int index = 1; index<nbEntries; ++index) {
        for(int j =0; j< nbPass; ++j) {
            int count = 1;
            unsigned char* red = reduce(&pass[j], nbEntries - index);
            while(count < index) {
                unsigned char* sha = sha4(red);
                delete[] red;
                red = reduce(sha, nbEntries - index + count);
                delete[] sha;
                ++count;
            }
            int z = 0;
            while(z<nbEntries){
                if(isEqual(&ep[z*4], &pass[j*4], 4)) {
                    unsigned char* f = found(&sp[index*4], chainLength, &pass[j*4]);
                    if(f) {
                        print(f,4);
                        print(&pass[j], 20);
                        delete[] f;
                    }
                }
                ++z;

            }
        }
        
    }
}

int main()
{
    /* 
    unsigned int x;
    stringstream ss;
    ss<<hex << "e92b21b0";
    ss >> x;
    unsigned char* obuf = sha4(x);
    print(obuf, 4);
    */
    int nbEntries = 1000;
    unsigned char* sp = new unsigned char[4*nbEntries]();
    unsigned char* ep = new unsigned char[4*nbEntries]();
    loadTable("table.dat", nbEntries, sp, ep);
    cout<<"Load table ok"<<endl;
    unsigned char* pass = loadPass("pass.txt", 20);
    cout<<"Load pass ok"<<endl;
    search(sp, ep,  nbEntries, pass, 20, 2000);

    delete[] sp;
    delete[] ep;
    delete[] pass;






    return 0;
}

