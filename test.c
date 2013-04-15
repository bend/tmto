/*
 * =====================================================================================
 *
 *       Filename:  test.c
 *
 *    Description:  a
 *
 *        Version:  1.0
 *        Created:  04/15/2013 01:47:20 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  YOUR NAME (), 
 *   Organization:  
 *
 * =====================================================================================
 */
#include <stdlib.h>
#include <stdio.h>


int main(){
    char buf = 5;
    char newBuf  = buf;
    newBuf = (newBuf ^= 1 );
    printf("%hhx\n", buf);
    printf("%hhx\n", newBuf);
}
