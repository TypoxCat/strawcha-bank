#pragma once
#include "Account.h"

// A checking account IS-A account. No interest, a monthly admin fee,
// but the balance may go below zero up to the overdraft limit.
class CheckingAccount : public Account {
public:
    static constexpr double OVERDRAFT_LIMIT = 500000;
    static constexpr double MONTHLY_FEE = 10000;

    CheckingAccount(const std::string& accountNumber, const std::string& ownerName, double initialDeposit);

    double availableFunds() const override;
    void monthlyProcess() override;
    std::string typeName() const override { return "Checking"; }
    std::string summary() const override;
};
