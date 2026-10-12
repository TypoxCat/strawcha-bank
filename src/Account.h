#pragma once
#include <string>
#include <vector>

// Abstract base class for every kind of bank account.
//
// Encapsulation: balance_ and history_ are private. The only way to change
// them is through deposit/withdraw (public, with validation) or the protected
// credit/debit helpers that subclasses use for interest and fees.
//
// Abstraction: users of Account only call deposit(), withdraw(), monthlyProcess()
// etc. They do not need to know how interest or overdraft is calculated.
class Account {
public:
    Account(const std::string& accountNumber, const std::string& ownerName, double initialDeposit);
    virtual ~Account() = default;

    // Overloaded functions (static polymorphism): same name, different parameters.
    void deposit(double amount);
    void deposit(double amount, const std::string& note);
    void withdraw(double amount);
    void withdraw(double amount, const std::string& note);

    double getBalance() const { return balance_; }
    const std::string& getAccountNumber() const { return accountNumber_; }
    const std::string& getOwnerName() const { return ownerName_; }
    const std::vector<std::string>& getHistory() const { return history_; }

    // Each account type has its own rule for these (dynamic polymorphism).
    virtual double availableFunds() const = 0;   // how much can be taken out right now
    virtual void monthlyProcess() = 0;           // interest or fees at month end
    virtual std::string typeName() const = 0;
    virtual std::string summary() const;

protected:
    // Only subclasses can change the balance without the withdraw limit check
    // (needed for monthly interest and fees).
    void credit(double amount, const std::string& note);
    void debit(double amount, const std::string& note);

private:
    std::string accountNumber_;
    std::string ownerName_;
    double balance_;
    std::vector<std::string> history_;

    void addHistory(const std::string& text);
    static void validateAmount(double amount);
};
