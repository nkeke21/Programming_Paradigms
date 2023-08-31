#ifndef _ACCOUNT_H
#define _ACCOUNT_H

#include <semaphore.h>
#include <stdint.h>

typedef uint64_t AccountNumber;
typedef int64_t AccountAmount;

typedef struct Account {
  AccountNumber accountNumber; //unique number for account
  AccountAmount balance;  //balance 
  sem_t accLock;  //to protect synchronization
  int64_t branchID; //savind branchID
} Account;


Account *Account_LookupByNumber(struct Bank *bank, AccountNumber accountNum);

void Account_Adjust(struct Bank *bank, Account *account,
                    AccountAmount amount,
                    int updateBranch);

AccountAmount Account_Balance(Account *account);

AccountNumber Account_MakeAccountNum(int branch, int subaccount);

int Account_IsSameBranch(AccountNumber accountNum1, AccountNumber accountNum2);

void Account_Init(Bank *bank, Account *account, int id, int branch,
                  AccountAmount initialAmount);

#endif /* _ACCOUNT_H */
