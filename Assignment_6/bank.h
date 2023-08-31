#ifndef _BANK_H
#define _BANK_H

#include <semaphore.h>

typedef struct Bank {
  unsigned int numberBranches;  
  unsigned int numToFinish; //helps us to know the last worker who will do the report_Dereport
  struct       Branch  *branches;
  struct       Report  *report;
  sem_t check;   //help us to know who is the last
  sem_t nextDay; //how many workers wants to start the next day
  sem_t transfer; 
  int workers; //amount of workers
  sem_t block; //wait until all transactions are carried and then able the bank to know ballance
  sem_t lock;
  sem_t work;
} Bank;

#include "account.h"

int Bank_Balance(Bank *bank, AccountAmount *balance);

Bank *Bank_Init(int numBranches, int numAccounts, AccountAmount initAmount,
                AccountAmount reportingAmount,
                int numWorkers);

int Bank_Validate(Bank *bank);
int Bank_Compare(Bank *bank1, Bank *bank2);



#endif /* _BANK_H */
