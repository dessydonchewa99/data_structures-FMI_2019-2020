//
//  Tree.cpp
//  hw3
//
//  Created by Desislava Doncheva on 16.12.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include "Tree.hpp"
//destructor
Tree::~Tree()
{
    clear(root);
}

//insert value, path(way), level
void Tree::insert(int val, int path[], int l)
{
    insert(root, val, path, l);
}

//dfs
void Tree::dfs(Node* root, int arr[], int &count)
{
    if(!root->children.empty())
    {
        
        for(list<Node*>::iterator it = root->children.begin(); it!=root->children.end(); ++it)
        {
            dfs((*it), arr, count);
        }
    }
    arr[count] = root->val;
    //cout<<root->val<<endl;
    count++;
    
}

//dfs2
void Tree::dfs2(Node* root, int arr[], int &count, int& prev)
{
    if(!root->children.empty())
    {
        
        for(list<Node*>::iterator it = root->children.begin(); it!=root->children.end(); ++it)
        {
            dfs2((*it), arr, count,(root)->val);
        }
    }
    if(prev != -1)
    {
        arr[count] = abs(root->val-prev);
        //cout<<root->val<<endl;
        count++;
    }
    
}

//Is the tree Odd?
bool Tree::isOdd(int n) // n = vuzli
{
    int arr[n];
    int c =0;
    dfs(root, arr, c);
    int arr_temp[n];
    for(int i = 0; i < n; i++)
    {
        arr_temp[i] = 0;
    }
    for(int i = 0; i < n ; i ++)
    {
        if(arr[i]%2 == 0)
        {
            cout<<arr[i];
            //cout<<"k"; test
            return false;
        }
        if(arr[i]/2>n-1)
        {
            //cout<<"r"; test
            return false;
        }
        arr_temp[arr[i]/2] ++;
    }
    
    for(int i = 0; i < n; i++)
    {
        if(arr_temp[i]!=1)
        {
            //cout<<"t"; test
            return false;
        }
    }
    return true;
}

//Is the tree Gracious?
bool Tree::isGracious(int m,int n)
{
    if(isOdd(n))
    {
        int arr[m];
        int c = 0;
        int d = -1;
        dfs2(root, arr,c ,d);
        sort(arr,arr+m);
        for(int i=1;i<m;i++)
        {
            if(arr[i]==arr[i-1])
            {
                cout<<arr[i]<<" "<<arr[i-1];
                return false;
            }
        }
        return true;
    }
    else
    {
        return false;
    }
    return false;
}

//help functions, used in destructor r=root
void Tree::clear(Node* r)
{
    if(r == nullptr) return;
    for(list<Node*>::iterator it = r->children.begin(); it!=r->children.end(); ++it)
    {
        clear (*it);
    }
}

//help function for insertion, contains root
bool Tree::insert(Node*& r, int _val, int path[], int level)
{
    if(!r && level) return false;
    if(!r && !level)
    {
        r = new Node(_val);
        return true;
    }
    if(!level)
    {
        r->children.push_back(new Node(_val));
        return true;
    }
    else
    {
        for(list<Node*> :: iterator it = r->children.begin(); it!=r->children.end(); ++it)
        {
            if((*it)->val == *path)
            {
                return insert((*it), _val, path+1, level-1);
            }
        }
    }
    return false;
}
