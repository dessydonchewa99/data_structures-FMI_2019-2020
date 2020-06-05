//
//  TList.cpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include "TList.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "Order.hpp"
//T *list;
//unsigned size, capacity;

template<class T>
TList<T>::TList()
{
    //TList(10);
    size = 0;
    capacity = 0;
    try
    {
        list = new T[capacity];
        capacity = CAPACITY_INCREMENT;
    }
    catch(...)
    {
        list = nullptr;
        capacity = 0;
    }

}
template<class T>
TList<T>::TList(unsigned initial_capacity)
{
    if(initial_capacity <= 0)
    {
        initial_capacity = 1;
    }
    size = 0;
    capacity = initial_capacity;
    try
    {
        list = new T[initial_capacity];
        capacity = initial_capacity;
    }
    catch(...)
    {
        list = nullptr;
        capacity = 0;
    }
    
}

template<class T>
TList<T>::TList(const TList<T>& t)
{
    size = t.size;
    capacity = t.capacity;
    list = new(nothrow) T[capacity];
    for (unsigned i = 0; i < size; i++)
    {
        list[i] = t.list[i];
    }
}

template<class T>
bool TList<T>::setSize(unsigned newSize)
{
    clearAll();
    size = newSize;
    capacity = newSize;
    list = new(nothrow) T[capacity];
    return true;
}

template<class T>
bool TList<T>::increaseCapacity()
{
    T *tmp = nullptr;
    try
    {
        tmp = new T[capacity+CAPACITY_INCREMENT];
    }
    catch (...)
    {
        tmp = nullptr;
    }
    
    if(tmp == nullptr)
    {
        return false;
    }
    for(int i = 0; i < size; i++)
    {
        tmp[i] = list[i];
    }
    
    delete [] list;
    list = tmp;
    capacity = capacity + CAPACITY_INCREMENT;
    return true;
}

template<class T>
bool TList<T>::isFull()
{
    if(size == capacity)
    {
        return true;
    }
    return false;
}

template<class T>
unsigned TList<T>::getSize()
{
    return size;
}

template<class T>
bool TList<T>::add(T newItem)
{
    bool canAdd = false;
    if(isFull())
    {
        if(increaseCapacity())
        {
            canAdd = true;
        }
    }
    else
    {
        canAdd = true;
    }
    if(canAdd)
    {
        list[size++] = newItem;
        return true;
    }
    return false;
}

template<class T>
T TList<T>::get(unsigned ind)
{
    T t;
    if(ind >= 0 and ind < size)
    {
        if(list != nullptr)
        {
            t = list[ind];
        }
    }
    return t;
}

template<class T>
bool TList<T>::update(unsigned ind, T newItem)
{
    if(ind >= 0 and ind < size)
    {
        if(list != nullptr)
        {
            list[ind] = newItem;
            return true;
        }
        else
        {
            return false;
        }
    }
    return false;
}

template<class T>
bool TList<T>::insert(unsigned ind, T newItem)
{
    if(ind >= 0 and ind < size)
    {
        bool canAdd = false;
        if(isFull())
        {
            if(increaseCapacity())
            {
                canAdd = true;
            }
        }
        else
        {
            canAdd = true;
        }
        if(canAdd)
        {
            if(list != nullptr)
            {
                for(unsigned i = size ; i > ind ; i--)
                {
                    list[i] = list[i-1];
                }
                size++;
                list[ind] = newItem;
                return true;
            }
            else
            {
                return false;
            }
        }
    }
    return false;
}

template<class T>
bool TList<T>::del(unsigned ind)
{
    if(list == nullptr)
    {
        return false;
    }
    if(ind >= 0 and ind < size)
    {
        for(unsigned i = ind ; i < size-1 ; i++)
        {
            list[i] = list[i+1];
        }
        size--;
        return true;
    }
    return false;
}

template<class T>
void TList<T>::clearAll()
{
    delete [] list;
    list = nullptr;
    size = 0;
    capacity = 0;
}

template<class T>
TList<T>& TList<T>::operator=(const TList<T>& t)
{
    if(this!=&t)
    {
        size = t.size;
        capacity = t.capacity;
        list = new(nothrow) T[capacity];
        for (unsigned i = 0; i < size; i++)
        {
            list[i] = t.list[i];
        }
    }
    return (*this);
}

template<class T>
TList<T>::~TList()
{
    delete [] list;
    list = nullptr;
}

ostream& operator<<(ostream& obj, TList<Wallet>& wl)
{
    obj << "WalletList" << endl << wl.getSize() << endl;
    for(unsigned i = 0; i < wl.getSize(); i++)
    {
        Wallet w = wl.get(i);
        obj << "Wallet_" << i << endl << w.owner << endl << w.id << endl << fixed << w.fiatMoney << endl;
    }
    return obj;
}
ostream& operator<<(ostream& obj, TList<Transaction>& tl)
{
    obj << "TransactionList" << endl << tl.getSize() << endl;
    for(unsigned i = 0; i < tl.getSize(); i++)
    {
        Transaction t = tl.get(i);
        obj << "Transaction_" << i << endl << t.time << endl << t.senderId << endl << t.receiverId << endl << fixed << t.fmiCoins << endl;
    }
    return obj;
}
ostream& operator<<(ostream& obj, TList<Order>& ol)
{
    obj << "OrderList" << endl << ol.getSize() << endl;
    for(unsigned i = 0; i < ol.getSize(); i++)
    {
        Order o = ol.get(i);
        obj << "Order_" << i << endl << o.type << endl << o.walletId << endl << fixed << o.fmiCoins << endl;
    }
    return obj;
}

istream& operator>>(istream& obj, TList<Wallet>& wl)
{
    char strline[256];
    unsigned listcount;
    Wallet walletread;
    
    obj >> strline; // read unnecessery text
    obj >> listcount;
    
    wl.clearAll();
    if(listcount > 0)
    {
        wl.setSize(listcount);
        
        for (unsigned i = 0; i < wl.getSize(); i++)
        {
            obj >> strline;
            obj >> walletread.owner;
            obj >> walletread.id;
            obj >> walletread.fiatMoney;
            wl.update(i, walletread);
        }
    }
    return obj;
}
istream& operator>>(istream& obj, TList<Transaction>& tl)
{
    char strline[256];
    unsigned listcount;
    Transaction transactionread;
    
    obj >> strline; // read unnecessery text
    obj >> listcount;
    
    tl.clearAll();
    if(listcount > 0)
    {
        tl.setSize(listcount);
        
        for (unsigned i = 0; i < tl.getSize(); i++)
        {
            obj >> strline; // read unnecessery text
            obj >> transactionread.time;
            obj >> transactionread.senderId;
            obj >> transactionread.receiverId;
            obj >> transactionread.fmiCoins;
            tl.update(i, transactionread);
        }
    }
    return obj;
}
istream& operator>>(istream& obj, TList<Order>& ol)
{
    char strline[256];
    unsigned listcount;
    Order orderread;
    int typetemp;
    
    obj >> strline; // read unnecessery text
    obj >> listcount;
    
    ol.clearAll();
    if(listcount > 0)
    {
        ol.setSize(listcount);
        
        for (unsigned i = 0; i < ol.getSize(); i++)
        {
            obj >> strline; // read unnecessery text
            obj >> typetemp;
            if(typetemp == 0)
            {
                orderread.type = Order::SELL;
            }
            else
            {
                orderread.type = Order::BUY;
            }
            obj >> orderread.walletId;
            obj >> orderread.fmiCoins;
            ol.update(i, orderread);
        }
    }
    return obj;
}

template class TList<Wallet>;
template class TList<Transaction>;
template class TList<Order>;
