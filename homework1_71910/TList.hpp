//
//  TList.hpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#ifndef TList_hpp
#define TList_hpp

#include <stdio.h>
#include <iostream>
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "Order.hpp"

using namespace std;

template<class T> class TList
{
private:
    T *list;
    unsigned size, capacity;
    const unsigned CAPACITY_INCREMENT = 1000;
    bool increaseCapacity();
    bool isFull();
public:
    TList();
    TList(unsigned initial_capacity);
    TList(const TList<T>& t);
    bool setSize(unsigned newSize);
    unsigned getSize();
    bool add(T newItem);
    T get(unsigned ind);
    bool update(unsigned ind, T newItem);
    bool insert(unsigned ind, T newItem);
    bool del(unsigned ind);
    void clearAll();
    TList<T>& operator=(const TList<T>& t);
    ~TList();
    friend ostream& operator<<(ostream& obj, TList<Wallet>& wl);
    friend ostream& operator<<(ostream& obj, TList<Transaction>& tl);
    friend ostream& operator<<(ostream& obj, TList<Order>& ol);
    
    friend istream& operator>>(istream& obj, TList<Wallet>& wl);
    friend istream& operator>>(istream& obj, TList<Transaction>& tl);
    friend istream& operator>>(istream& obj, TList<Order>& ol);
    
};

#endif /* TList_hpp */
