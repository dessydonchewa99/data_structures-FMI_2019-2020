//
//  Stack.cpp
//  homework2_test
//
//  Created by Desislava Doncheva on 17.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <stdio.h>
#include "Stack.hpp"

const int INCREASE_CAPACITY = 20;

using namespace std;

Stack::Stack(unsigned initialCapacity)
{
    if(initialCapacity <= 0)
    {
        initialCapacity = 1;
    }
    top = -1;
    capacity = initialCapacity;
    buffer = new(nothrow) RepeatInfo[capacity];
}

void Stack::push(RepeatInfo rpi)
{
    if(isFull() == true)
    {
        increaseCapacity();
    }
    top++;
    buffer[top] = rpi;
}
RepeatInfo Stack::pop()
{
    RepeatInfo retItem;
    retItem.count = 0;
    retItem.offset = 0;
    if(isEmpty() == true)
    {
        cout << "Couldn't pop an element. The stack is empty!" << endl;
        return retItem;
    }
    retItem = buffer[top];
    top--;
    return retItem;
}

RepeatInfo Stack::peek()
{
    RepeatInfo retItem;
    retItem.count = 0;
    retItem.offset = 0;
    if(isEmpty() == true)
    {
        return retItem;
    }
    return buffer[top];
}
bool Stack::isFull()
{
    if(getSize() == capacity)
    {
        return true;
    }
    return false;
}
bool Stack::isEmpty()
{
    if(getSize() == 0)
    {
        return true;
    }
    return false;
}
void Stack::increaseCapacity()
{
    if(isFull() == true)
    {
        unsigned i = 0;
        unsigned oldCapacity = capacity;
        capacity = capacity + INCREASE_CAPACITY;
        RepeatInfo* newBuffer = new RepeatInfo[capacity];
        for(i = 0 ; i < oldCapacity; i++)
        {
            newBuffer[i] = buffer[i];
        }
        delete [] buffer;
        buffer = newBuffer;
    }
}

unsigned Stack::getSize()
{
    return top+1;
}

void Stack::print()
{
    unsigned i = 0;
    if(isEmpty()==true)
    {
        return;
    }
    for(i = 0; i < getSize(); i++)
    {
        cout << "count: " << buffer[i].count << endl;
        cout << "offset: " << buffer[i].offset << endl;
    }
    cout << endl;
}

Stack::~Stack()
{
    delete [] buffer;
}
