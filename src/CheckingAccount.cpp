#include "CheckingAccount.h"
#include "Utils.h"

CheckingAccount::CheckingAccount(const std::string& accountNumber, const std::string& ownerName, double initialDeposit)
    : Account(accountNumber, ownerName, initialDeposit) {}

double CheckingAccount::availableFunds() const {
    double funds = getBalance() + OVERDRAFT_LIMIT;
    return funds > 0 ? funds : 0;
}

void CheckingAccount::monthlyProcess() {
    debit(MONTHLY_FEE, "Monthly admin fee");
}

std::string CheckingAccount::summary() const {
    return Account::summary() + " | Overdraft limit: " + formatRupiah(OVERDRAFT_LIMIT);
}
