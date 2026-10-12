#include "TransferTransaction.h"
#include "Exceptions.h"

TransferTransaction::TransferTransaction(Account* source, Account* destination, double amount)
    : Transaction(source, amount), destination_(destination) {
    if (destination_ == nullptr) {
        throw AccountNotFoundException("Destination account does not exist.");
    }
}

double TransferTransaction::fee() const {
    if (source_->getOwnerName() == destination_->getOwnerName()) {
        return 0;
    }
    return OTHER_OWNER_FEE;
}

std::string TransferTransaction::describe() const {
    return source_->getAccountNumber() + " -> " + destination_->getAccountNumber() +
           " (" + destination_->getOwnerName() + ")";
}

void TransferTransaction::execute() {
    if (source_ == destination_) {
        throw RuleViolationException("Cannot transfer to the same account.");
    }
    double total = amount_ + fee();
    // check first so we never end up with money taken but not delivered
    if (total > source_->availableFunds()) {
        throw InsufficientFundsException("Insufficient funds for transfer plus fee.");
    }
    source_->withdraw(amount_, "Transfer to " + destination_->getAccountNumber());
    if (fee() > 0) {
        source_->withdraw(fee(), "Transfer fee");
    }
    destination_->deposit(amount_, "Transfer from " + source_->getAccountNumber());
}
