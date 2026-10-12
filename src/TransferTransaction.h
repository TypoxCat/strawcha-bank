#pragma once
#include "Transaction.h"

// Move money from one bank account to another.
// Transfers between two accounts of the same owner are free, otherwise a flat fee applies.
class TransferTransaction : public Transaction {
public:
    static constexpr double OTHER_OWNER_FEE = 2500;

    TransferTransaction(Account* source, Account* destination, double amount);

    std::string typeName() const override { return "Transfer"; }
    std::string describe() const override;
    double fee() const override;

protected:
    void execute() override;

private:
    Account* destination_;
};
