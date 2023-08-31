#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <assert.h>
#include <inttypes.h>

#include "teller.h"
#include "account.h"
#include "error.h"
#include "debug.h"
#include "branch.h"

/*
 * deposit money into an account
 */
int
Teller_DoDeposit(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoDeposit(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);

  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }
  //lock branch and account
  int64_t id = account->branchID;
  sem_wait(&(bank->branches[id].branchL));
  sem_wait(&account->accLock);

  Account_Adjust(bank,account, amount, 1);

  //release
  sem_post(&account->accLock);
  sem_post(&(bank->branches[id].branchL));

  return ERROR_SUCCESS;
}

/*
 * withdraw money from an account
 */
int
Teller_DoWithdraw(Bank *bank, AccountNumber accountNum, AccountAmount amount)
{
   // printf("withdraw\n");

  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoWithdraw(account 0x%"PRIx64" amount %"PRId64")\n",
                accountNum, amount));

  Account *account = Account_LookupByNumber(bank, accountNum);
  if (account == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  //locking branch and account
  int64_t id = account->branchID;
  sem_wait(&(bank->branches[id].branchL));
  sem_wait(&account->accLock);

  if (amount > Account_Balance(account)) {
    //release
    sem_post(&account->accLock);
    sem_post(&(bank->branches[id].branchL));
    return ERROR_INSUFFICIENT_FUNDS;
  }
  Account_Adjust(bank,account, -amount, 1); //transaction

  //release
  sem_post(&account->accLock);
  sem_post(&(bank->branches[id].branchL));
  return ERROR_SUCCESS;
}

/*
 * do a tranfer from one account to another account
 */
int
Teller_DoTransfer(Bank *bank, AccountNumber srcAccountNum,
                  AccountNumber dstAccountNum,
                  AccountAmount amount)
{
		
  assert(amount >= 0);

  DPRINTF('t', ("Teller_DoTransfer(src 0x%"PRIx64", dst 0x%"PRIx64
                ", amount %"PRId64")\n",
                srcAccountNum, dstAccountNum, amount));

  Account *srcAccount = Account_LookupByNumber(bank, srcAccountNum);
  if (srcAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  Account *dstAccount = Account_LookupByNumber(bank, dstAccountNum);
  if (dstAccount == NULL) {
    return ERROR_ACCOUNT_NOT_FOUND;
  }

  //to check if two accounts are in the same branch
  int same = Account_IsSameBranch(srcAccountNum, dstAccountNum);
  int64_t srcBranchID;
  int64_t dstBranchID;

  if(!same){ 
      //get known their branch ids
      srcBranchID = srcAccount->branchID;
      dstBranchID = dstAccount->branchID;

      if(srcBranchID < dstBranchID){ //prevent deadlocking
        sem_wait(&(bank->branches[srcBranchID].branchL));
        sem_wait(&(bank->branches[dstBranchID].branchL));
      } else {
        sem_wait(&(bank->branches[dstBranchID].branchL));
        sem_wait(&(bank->branches[srcBranchID].branchL));
      
      }
  }

  if(srcAccount->accountNumber < dstAccount->accountNumber){ //prevent deadlocking
    //diff accounts
    sem_wait(&(srcAccount->accLock));
    sem_wait(&(dstAccount->accLock));
    } else if(srcAccount->accountNumber > dstAccount->accountNumber){
    //diff accounts
    sem_wait(&(dstAccount->accLock));
    sem_wait(&(srcAccount->accLock));
    } else {
    sem_wait(&srcAccount->accLock); //same accounts
  }

      if(amount > Account_Balance(srcAccount)){
        if(!same){
          //release branches and accounts
          sem_post(&(srcAccount->accLock));
          sem_post(&(dstAccount->accLock));
          sem_post(&(bank->branches[srcBranchID].branchL));
          sem_post(&(bank->branches[dstBranchID].branchL));
        } else {
          if(srcAccountNum != dstAccountNum){
            //release accounts
            sem_post(&(srcAccount->accLock));
            sem_post(&(dstAccount->accLock));
          } else {
            //release one account. (if accounts were same)
            sem_post(&srcAccount->accLock);
          }
        }
        return ERROR_INSUFFICIENT_FUNDS;
      }
      int updateBranch = !Account_IsSameBranch(srcAccountNum, dstAccountNum);
      
      //pefrom all transactions and then able bank to know the ballance
      sem_wait(&bank->block);
      Account_Adjust(bank, srcAccount, -amount, updateBranch);
      Account_Adjust(bank, dstAccount, amount, updateBranch);
      sem_post(&bank->block);

      //releasing
      if(!same){
          sem_post(&(srcAccount->accLock));
          sem_post(&(dstAccount->accLock));
          sem_post(&(bank->branches[srcBranchID].branchL));
          sem_post(&(bank->branches[dstBranchID].branchL));
        } else {
          if(srcAccountNum != dstAccountNum){
            sem_post(&(srcAccount->accLock));
            sem_post(&(dstAccount->accLock));
          } else {
            sem_post(&srcAccount->accLock);
          }
        }
  return ERROR_SUCCESS;

}
