#include "SavingsAccount.h"
#include "Exceptions.h"
#include "Utils.h"

SavingsAccount::SavingsAccount(const std::string& accountNumber, const std::string& ownerName, double initialDeposit)
    : Account(accountNumber, ownerName, initialDeposit) {
    if (initialDeposit < MIN_BALANCE) {
        throw RuleViolationException("Savings account needs an opening deposit of at least " + formatRupiah(MIN_BALANCE) + ".");
    }
}

double SavingsAccount::availableFunds() const {
    double free = getBalance() - MIN_BALANCE;
    return free > 0 ? free : 0;
}

void SavingsAccount::monthlyProcess() {
    double interest = getBalance() * ANNUAL_INTEREST_RATE / 12.0;
    if (interest > 0) {
        credit(interest, "Monthly interest");
    }
}

std::string SavingsAccount::summary() const {
    return Account::summary() + " | Min balance: " + formatRupiah(MIN_BALANCE);
}
