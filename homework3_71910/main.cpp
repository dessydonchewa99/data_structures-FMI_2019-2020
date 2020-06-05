//
//  main.cpp
//  hw3
//
//  Created by Desislava Doncheva on 16.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//
#define MAX_SIZE 256

#include <iostream>
#include <string.h>
#include "Tree.hpp"
using namespace std;


int main(int argc, const char * argv[])
{
    // tree examples for tests
    Tree t;
    Tree t2;
    int arr[1] = {7};
    int arr2[2] = {7,3};
    int arr3[3] = {7,3,9};
    t.insert(7, {}, 0);
    t.insert(3, {}, 0);
    t.insert(9,arr2+1, 1);
    t.insert(5, arr3+1, 2);
    t.insert(1,arr3+1, 2);
    
    int arr1[1] = {1};
    t2.insert(1, {}, 0);
    t2.insert(3, {}, 0);
    t2.insert(5,{}, 0);
    t2.insert(7, {}, 0);
    t2.insert(9,{}, 0);
    cout<<t2.isGracious(4,5);
    
    return 0;
}
