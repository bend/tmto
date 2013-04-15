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
    int i;
    for (i = 0; i < 20; i++) {
        printf("%02x ", obuf[i]);
    }
    printf("\n");
    return obuf;
}

int isEvenParity(unsigned char myChar) {
    int parity = 0;

    for (myChar &= ~0x80;  myChar != 0;  myChar >>= 1) {
        parity ^= (myChar & 1);   // Toggle parity on each '1' bit.
    }
    return parity ;
}

unsigned char* reduce(unsigned char *hash, int i){
    unsigned char *newBuf = new unsigned char[4]();
    newBuf[0] =((hash[0] + i) % 255); 
    newBuf[1] =((hash[1] + i) % 255);
    newBuf[2] =((hash[2] + i) % 255);
    newBuf[3] =((hash[3] + i) % 255);
    int parity = (isEvenParity(newBuf[0]) + isEvenParity(newBuf[1]) + isEvenParity(newBuf[2]) + isEvenParity(newBuf[3]) ) % 2;
    if( parity == 0){
        cout<<"not parity\n";
        printf("%hhx\n", newBuf[0]);
        newBuf[0] = (newBuf[0] ^= 1);
        printf("%hhx\n", newBuf[0]);
    }
    return newBuf;
}

int main()
{
    unsigned int x;
    stringstream ss;
    ss<<hex << "29885187";
    ss >> x;
    cout<<x<<endl;
    sha4(x);

    return 0;
}

