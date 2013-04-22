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


using namespace std;

#include "generator.h"
#include <boost/unordered_map.hpp>

unsigned char* create_chain(unsigned char* val, size_t size){
    unsigned char* sha;
    unsigned char* red = new unsigned char[4]();
    memcpy(red, val, 4);
    size_t i = 0;
    while(i < size) {   
        sha = sha1p4(red);
        delete[] red;
        red = reduce(sha, i);
        delete[] sha;
        ++i;
    }
    return red;
}

void fwrite(FILE* f, unsigned char* c){
    fputc(c[0], f);
    fputc(c[1], f);
    fputc(c[2], f);
    fputc(c[3], f);
}

unsigned char* getStartPoint(int iteration){
    unsigned char* startPoint = new unsigned char[4]();
    int startPointInt =  55*iteration;
    
    startPoint[3] = ( startPointInt & (0xFF));
    startPoint[2] = ((startPointInt >> 8) & 0xFF);
    startPoint[1] = ((startPointInt >> 16) & 0xFF);
    startPoint[0] = ((startPointInt >> 24) & 0xFF);
    return startPoint;
       
}

int main()
{
    unsigned char* value;
    unsigned char* startPoint = new unsigned char[4]();
    unsigned char* endPoint = new unsigned char[4]();
    unsigned char* oldep;
    int startValue = 0;
    
    value = to_char(startValue);
    memcpy(startPoint, value, 4);

    int numberEntries = 1250000;
    int chainLength = 2;

    cout << "== Starting Generation ==" << endl;
    cout << "[Entries : " << numberEntries << "]" << endl;
    cout << "[Chain Length : " << chainLength << "]" << endl;
    FILE* tf = fopen("tabletemp.dat", "w");
    boost::unordered_map<string,string> map;

    int countDoublon = 0;
    
    //Creation de la table
    for(int i = 0,j=0;i<numberEntries;i++,j++){
         endPoint = create_chain(startPoint, chainLength);
         string s_ep((const char*)(endPoint), 4); 
         if (map.count(s_ep)){
            countDoublon++;
            --i;
        } else {
             fwrite(tf, startPoint);   
             fwrite(tf, endPoint);
             map[s_ep] = "";
        }
        startPoint = getStartPoint(j); 
        oldep = endPoint;
        delete[] oldep;
    }
    cout << endl;
    cout << "-------------------" <<endl;
    cout << "Generation finished " <<endl;
    cout << "-------------------"<<endl;
    cout << endl;
    cout << "== Generation Statistics == " << endl;
    cout << "Copies during generation : " << countDoublon <<endl;
    //Write last EP
    fclose(tf);
    return 0;
}

