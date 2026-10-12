#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Account.h"

// A bank customer. Owns their accounts and protects their PIN.
//
// Encapsulation: the PIN is never stored as plain text and there is no getter for it.
// Outside code can only ask verifyPin(). After 3 wrong tries the customer is locked.
class Customer {
public:
    static constexpr int MAX_FAILED_ATTEMPTS = 3;

    Customer(const std::string& id, const std::string& name, const std::string& pin);

    const std::string& getId() const { return id_; }
    const std::string& getName() const { return name_; }

    bool verifyPin(const std::string& pin);
    void changePin(const std::string& oldPin, const std::string& newPin);
    bool isLocked() const { return locked_; }
    void unlock();

    Account* addAccount(std::unique_ptr<Account> account);
    Account* findOwnAccount(const std::string& accountNumber) const;
    const std::vector<std::unique_ptr<Account>>& getAccounts() const { return accounts_; }
    double totalBalance() const;

private:
    std::string id_;
    std::string name_;
    std::size_t pinHash_;
    int failedAttempts_;
    bool locked_;
    std::vector<std::unique_ptr<Account>> accounts_;

    static void checkPinFormat(const std::string& pin);
    static std::size_t hashPin(const std::string& pin);
};
