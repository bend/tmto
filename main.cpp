/*
 * =====================================================================================
 *
 *       Filename:  main.cpp
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

int isOddParity(unsigned char myChar) {
    /* 
    int parity = 0;

    for (myChar &= ~0x80;  myChar != 0;  myChar >>= 1) {
        parity ^= (myChar & 1);   // Toggle parity on each '1' bit.
    }
    */
    myChar ^= myChar >> 4;
    myChar &= 0xf;
    int i = (0x6996 >> myChar) & 1;
    cout<< i<<endl;
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
        cout<<"not parity\n";
        printf("%hhx\n", newBuf[0]);
        newBuf[0] ^= 1;
        printf("%hhx\n", newBuf[0]);
    }
    return newBuf;
}

void print(unsigned char* buf, int s){
    int i;
    for (i = 0; i < s; i++) {
        printf("%02x ", buf[i]);
    }
    printf("\n");
}

int main()
{
    unsigned int x;
    stringstream ss;
    ss<<hex << "e92b21b0";
    ss >> x;
    unsigned char* obuf = sha4(x);
    print(obuf, 20);
    unsigned char* ret = reduce(obuf, 1);
    print(ret, 4);



    return 0;
}

