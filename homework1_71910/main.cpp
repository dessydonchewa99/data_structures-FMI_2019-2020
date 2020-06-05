//
//  main.cpp
//  homework1
//
//  Created by Desislava Doncheva on 2.11.19.
//  Copyright © 2019 Desislava Doncheva. All rights reserved.
//

#include <iostream>
#include <string.h>
#include <fstream>

#include "TList.hpp"
#include "Wallet.hpp"
#include "Transaction.hpp"
#include "Order.hpp"
#include "CommandMenu.hpp"

using namespace std;

//const char *OutputDir = "/Users/desislavadoncheva/Downloads/testCoin/";
const char *OutputDir = "";

const char *WalletName = "wallets.dat";
const char *TransactionName = "transactions.dat";
const char *OrderName = "orders.dat";
const char *ComplitedOrdersName = "complited_";

const double TOTAL_BankMoneyAmount = 1000000000;

int main(int argc, const char * argv[])
{
    char WalletFileName[256];
    char TransactionFileName[256];
    char OrderFileName[256];
    char ComplitedFileOrders[256];
    
    char choose[256];
    
    strcpy(WalletFileName, OutputDir);
    strcat(WalletFileName, WalletName);
    
    strcpy(TransactionFileName, OutputDir);
    strcat(TransactionFileName, TransactionName);
    
    strcpy(OrderFileName, OutputDir);
    strcat(OrderFileName, OrderName);
    
    strcpy(ComplitedFileOrders, OutputDir);
    strcat(ComplitedFileOrders, ComplitedOrdersName);
    
    TList<Wallet> walletlist;
    TList<Transaction> transactionlist;
    TList<Order> orderlist;
    
    
    // read files
    ifstream inWallet;
    inWallet.open(WalletFileName, ios::binary | ios::in);
    if(inWallet.is_open())
    {
        //cout << "File "<< WalletName << " is opened(read)" << endl;
        inWallet >> walletlist;
        inWallet.close();
    }
    
    ifstream inTransaction;
    inTransaction.open(TransactionFileName, ios::binary | ios::in);
    if(inTransaction.is_open())
    {
        //cout << "File " << TransactionName <<" is opened(read)" << endl;
        inTransaction >> transactionlist;
        inTransaction.close();
    }
    
    ifstream inOrder;
    inOrder.open(OrderFileName, ios::binary | ios::in);
    if(inOrder.is_open())
    {
        //cout << "File " << OrderName <<" is opened(read)" << endl;
        inOrder >> orderlist;
        inOrder.close();
    }
    
    //if walletslist is empty, create system wallet
    if(walletlist.getSize() == 0)
    {
        Wallet systemWallet;
        strcpy(systemWallet.owner, "BANK");
        systemWallet.id = SYSTEM_WALLET_ID;
        systemWallet.fiatMoney = TOTAL_BankMoneyAmount;
        walletlist.add(systemWallet);
        
        Transaction bankTransaction;
        time(&bankTransaction.time);
        bankTransaction.senderId = SYSTEM_WALLET_ID;
        bankTransaction.receiverId = SYSTEM_WALLET_ID;
        bankTransaction.fmiCoins = (TOTAL_BankMoneyAmount/MONEY_COIN_RATE);
        transactionlist.add(bankTransaction);
    }
    // process commands
    CommandMenu commands(&walletlist, &transactionlist, &orderlist, ComplitedFileOrders);
    bool Exitflag = false;
    while (true)
    {
        cout << endl << "Command:";
        cin.getline(choose, sizeof(choose));
        
        Exitflag = commands.processCommand(choose);
        if(Exitflag)
        {
            break;
        }
    }
    
    // write files
    ofstream outWallet;
    outWallet.open(WalletFileName, ios::binary | ios::out);
    if(outWallet.is_open())
    {
        outWallet << walletlist;
        //cout << "File " << WalletName << " is opened(write)" << endl;
        outWallet.close();
    }
    
    ofstream outTransaction;
    outTransaction.open(TransactionFileName, ios::binary | ios::out);
    if(outTransaction.is_open())
    {
        outTransaction << transactionlist;
        //cout << "File " << TransactionName << " is opened(write)" << endl;
        outTransaction.close();
    }
    
    ofstream outOrder;
    outOrder.open(OrderFileName, ios::binary | ios::out);
    if(outOrder.is_open())
    {
        outOrder << orderlist;
        //cout << "File " << OrderName << " is opened(write)" << endl;
        outOrder.close();
    }
    
    return 0;
}



