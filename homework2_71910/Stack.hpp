//
//  Stack.hpp
//  homework2_test
//
//  Created by Desislava Doncheva on 17.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef Stack_hpp
#define Stack_hpp

#include <stdio.h>

struct RepeatInfo
{
    unsigned count;
    unsigned offset;
    
    };

class Stack
{
private:
    unsigned capacity;
    int top;
    RepeatInfo* buffer;
    
    void increaseCapacity();
    
public:
    Stack(unsigned initialCapacity);
    
    void push(RepeatInfo rpi);
    RepeatInfo pop();
    RepeatInfo peek();
    
    unsigned getSize();
    bool isFull();
    bool isEmpty();
    
    void print();
    
    ~Stack();
    
};



#endif /* Stack_hpp */
