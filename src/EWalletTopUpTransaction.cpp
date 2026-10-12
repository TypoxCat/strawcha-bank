#include "EWalletTopUpTransaction.h"
#include "Exceptions.h"
#include "Utils.h"

EWalletTopUpTransaction::EWalletTopUpTransaction(Account* source, const std::string& provider,
                                                 const std::string& phoneNumber, double amount)
    : Transaction(source, amount), provider_(provider), phoneNumber_(phoneNumber) {
    if (provider_.empty() || phoneNumber_.empty()) {
        throw RuleViolationException("Provider and phone number are required.");
    }
}

std::string EWalletTopUpTransaction::describe() const {
    return provider_ + " " + phoneNumber_ + " from " + source_->getAccountNumber();
}

void EWalletTopUpTransaction::execute() {
    if (amount_ < MIN_TOPUP) {
        throw RuleViolationException("Minimum top up is " + formatRupiah(MIN_TOPUP) + ".");
    }
    if (amount_ + fee() > source_->availableFunds()) {
        throw InsufficientFundsException("Insufficient funds for top up plus fee.");
    }
    source_->withdraw(amount_, "Top up " + provider_ + " " + phoneNumber_);
    source_->withdraw(fee(), "Top up fee");
}
