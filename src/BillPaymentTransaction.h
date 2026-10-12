#pragma once
#include "Transaction.h"

// Pay a bill (electricity, water, internet). Money leaves the account.
class BillPaymentTransaction : public Transaction {
public:
    static constexpr double ADMIN_FEE = 2500;

    BillPaymentTransaction(Account* source, const std::string& billType, const std::string& customerNumber, double amount);

    std::string typeName() const override { return "Bill Payment"; }
    std::string describe() const override;
    double fee() const override { return ADMIN_FEE; }

protected:
    void execute() override;

private:
    std::string billType_;
    std::string customerNumber_;
};
