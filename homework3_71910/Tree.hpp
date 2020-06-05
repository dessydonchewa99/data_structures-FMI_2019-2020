//
//  Tree.hpp
//  hw3
//
//  Created by Desislava Doncheva on 16.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Tree_hpp
#define Tree_hpp

#include<iostream>
#include<stdio.h>
//#include<new>
#include<list>

using namespace std;

struct Node
{
    int val;
    list<Node*> children;
    Node(int _val)
    {
        val = _val;
    }
};

class Tree
{
private:
    Node* root;
    
public:
    Tree():root(nullptr) {}
    ~Tree();
    
    void insert(int val, int path[], int l);
   
    void dfs2(Node* root, int arr[], int &count, int& prev);
    
    bool isOdd(int n); // n = vuzli
    bool isGracious(int m, int n);
    
private:
    void clear(Node* r);
    bool insert(Node*& r, int _val, int path[], int level);
    void dfs(Node* root, int arr[], int &count);
};

#endif /* Tree_hpp */
