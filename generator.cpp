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

unsigned char* get_sp(int iteration){
    unsigned char* startPoint = new unsigned char[4]();
    int startPointInt =  55*iteration;
    
    startPoint[3] = ( startPointInt & (0xFF));
    startPoint[2] = ((startPointInt >> 8) & 0xFF);
    startPoint[1] = ((startPointInt >> 16) & 0xFF);
    startPoint[0] = ((startPointInt >> 24) & 0xFF);
    return startPoint;
       
}

void generate(struct run_params* params){
    unsigned char* val;
    unsigned char* sp = new unsigned char[4]();
    unsigned char* ep = new unsigned char[4]();
    unsigned char* temp;
    
    val = to_char(params->init);
    memcpy(sp, val, 4);
    FILE* tf = fopen(params->path, "w");
    boost::unordered_map<string,string> map;

    int i = 0;
    int j = 0;
    while(i<params->nb_entries) {
         ep = create_chain(sp, params->chain_length);
         string s_ep((const char*)(ep), 4); 
         if (map.count(s_ep)){
            --i;
        } else {
            fwrite(tf, sp);   
            fwrite(tf, ep);
            map[s_ep] = "";
        }
        sp = get_sp(j); 
        temp = ep;
        delete[] temp;
        if(i%1000 == 0){
            cout<<"\r"<<(double)(i/(double)params->nb_entries)*100.0<<"%";
        }
        ++i;
        ++j;
    
    }
    fclose(tf);
}

int main()
{
    struct run_params sp;
    generate(&sp);
    cout << endl;
    cout << "-------------------" <<endl;
    cout << "Generation finished " <<endl;
    cout << "-------------------"<<endl;
    cout << endl;
    //Write last EP
    return 0;
}



