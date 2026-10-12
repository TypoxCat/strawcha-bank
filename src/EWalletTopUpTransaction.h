#pragma once
#include "Transaction.h"

// Top up an e-wallet (GoPay, OVO, DANA, ...). Money leaves the account and goes outside the bank.
class EWalletTopUpTransaction : public Transaction {
public:
    static constexpr double MIN_TOPUP = 10000;
    static constexpr double TOPUP_FEE = 1000;

    EWalletTopUpTransaction(Account* source, const std::string& provider, const std::string& phoneNumber, double amount);

    std::string typeName() const override { return "E-Wallet Top Up"; }
    std::string describe() const override;
    double fee() const override { return TOPUP_FEE; }

protected:
    void execute() override;

private:
    std::string provider_;
    std::string phoneNumber_;
};
