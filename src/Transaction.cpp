#include "Transaction.h"
#include "Exceptions.h"
#include "Utils.h"
#include <sstream>

int Transaction::nextId_ = 1;

Transaction::Transaction(Account* source, double amount)
    : source_(source), amount_(amount), id_(nextId_++), status_(TxStatus::Pending), timestamp_(currentTime()) {
    if (source_ == nullptr) {
        throw AccountNotFoundException("Source account does not exist.");
    }
    if (!(amount_ > 0)) {
        throw InvalidAmountException("Amount must be greater than zero.");
    }
}

bool Transaction::process() {
    try {
        execute();
        status_ = TxStatus::Success;
    } catch (const BankException& e) {
        status_ = TxStatus::Failed;
        failureReason_ = e.what();
    }
    return status_ == TxStatus::Success;
}

std::string Transaction::receipt() const {
    std::ostringstream out;
    out << "============ RECEIPT ============\n";
    out << "Transaction ID : " << id_ << "\n";
    out << "Type           : " << typeName() << "\n";
    out << "Time           : " << timestamp_ << "\n";
    out << "Details        : " << describe() << "\n";
    out << "Amount         : " << formatRupiah(amount_) << "\n";
    out << "Fee            : " << formatRupiah(fee()) << "\n";
    out << "Total          : " << formatRupiah(amount_ + fee()) << "\n";
    if (status_ == TxStatus::Success) {
        out << "Status         : SUCCESS\n";
    } else if (status_ == TxStatus::Failed) {
        out << "Status         : FAILED (" << failureReason_ << ")\n";
    } else {
        out << "Status         : PENDING\n";
    }
    out << "=================================";
    return out.str();
}
