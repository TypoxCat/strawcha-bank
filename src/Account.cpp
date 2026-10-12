#include "Account.h"
#include "Exceptions.h"
#include "Utils.h"

namespace {
const double EPSILON = 1e-9;
}

Account::Account(const std::string& accountNumber, const std::string& ownerName, double initialDeposit)
    : accountNumber_(accountNumber), ownerName_(ownerName), balance_(0) {
    if (initialDeposit < 0) {
        throw InvalidAmountException("Initial deposit cannot be negative.");
    }
    balance_ = initialDeposit;
    addHistory("Account opened with " + formatRupiah(initialDeposit));
}

void Account::validateAmount(double amount) {
    // written as !(amount > 0) so NaN is rejected too
    if (!(amount > 0)) {
        throw InvalidAmountException("Amount must be greater than zero.");
    }
}

void Account::deposit(double amount) {
    deposit(amount, "Deposit");
}

void Account::deposit(double amount, const std::string& note) {
    validateAmount(amount);
    credit(amount, note);
}

void Account::withdraw(double amount) {
    withdraw(amount, "Withdrawal");
}

void Account::withdraw(double amount, const std::string& note) {
    validateAmount(amount);
    // availableFunds() is virtual, so savings and checking apply different limits
    if (amount > availableFunds() + EPSILON) {
        throw InsufficientFundsException("Insufficient funds. Available: " + formatRupiah(availableFunds()));
    }
    debit(amount, note);
}

void Account::credit(double amount, const std::string& note) {
    balance_ += amount;
    addHistory(note + " +" + formatRupiah(amount) + " | Balance: " + formatRupiah(balance_));
}

void Account::debit(double amount, const std::string& note) {
    balance_ -= amount;
    addHistory(note + " -" + formatRupiah(amount) + " | Balance: " + formatRupiah(balance_));
}

void Account::addHistory(const std::string& text) {
    history_.push_back(currentTime() + " | " + text);
}

std::string Account::summary() const {
    return typeName() + " " + accountNumber_ + " (" + ownerName_ + ") | Balance: " + formatRupiah(balance_);
}
