#pragma once
#include "Account.h"

// A savings account IS-A account. It earns interest but must keep a minimum balance.
class SavingsAccount : public Account {
public:
    static constexpr double MIN_BALANCE = 50000;
    static constexpr double ANNUAL_INTEREST_RATE = 0.03;

    SavingsAccount(const std::string& accountNumber, const std::string& ownerName, double initialDeposit);

    double availableFunds() const override;
    void monthlyProcess() override;
    std::string typeName() const override { return "Savings"; }
    std::string summary() const override;
};
