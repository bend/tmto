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

    chainage(0, 2000);



    return 0;
}

