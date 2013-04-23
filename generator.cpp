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
#include <getopt.h>

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
            cout<<"\rInfo: Generating table ("<<(double)(i/(double)params->nb_entries)*100.0<<"%)";
        }
        ++i;
        ++j;
    
    }
    fclose(tf);
}

int main(int argc, char** argv)
{
    int c;
    static struct option long_options[] =
    {
        {"chainlength", required_argument, NULL, 'c'},
        {"output", required_argument, NULL, 'o'},
        {"entries", required_argument, NULL, 'n'},
        {"start", required_argument, NULL, 's'},
        {NULL, 0, NULL, 0}
    };
    char out[] = "rainbow.db";
    int option_index = 0;

    struct run_params sp;
    sp.chain_length = 1000;
    sp.init = 2342342;
    sp.nb_entries = 1250000;
    sp.path = out;
    
    while ((c = getopt_long (argc, argv, "c:o:n:s:", long_options, &option_index)) != -1){
        switch (c)
        {
            case 'c':
                sp.chain_length = strtol((char*)optarg, NULL, 10);
                break;
            case 'o':
                sp.path = ((char*)optarg);
                break;
            case 'n':
                sp.nb_entries = strtol((char*)optarg, NULL, 10);
                break;
            case 's': 
                sp.init = strtol((char*)optarg, NULL, 10);
                break;
        }
    }

    cout<<"Info : Generating table " << sp.path << " of "<< sp.nb_entries <<" entries"<<endl;
    cout<<"Info : Chain length is "<<sp.chain_length<<endl;
    generate(&sp);
    return 0;
}



