#include "BillPaymentTransaction.h"
#include "Exceptions.h"

BillPaymentTransaction::BillPaymentTransaction(Account* source, const std::string& billType,
                                               const std::string& customerNumber, double amount)
    : Transaction(source, amount), billType_(billType), customerNumber_(customerNumber) {
    if (billType_.empty() || customerNumber_.empty()) {
        throw RuleViolationException("Bill type and customer number are required.");
    }
}

std::string BillPaymentTransaction::describe() const {
    return billType_ + " bill, customer no. " + customerNumber_;
}

void BillPaymentTransaction::execute() {
    if (amount_ + fee() > source_->availableFunds()) {
        throw InsufficientFundsException("Insufficient funds for bill plus admin fee.");
    }
    source_->withdraw(amount_, "Pay " + billType_ + " " + customerNumber_);
    source_->withdraw(fee(), "Bill admin fee");
}
