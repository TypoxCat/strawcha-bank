#pragma once
#include <memory>
#include <string>
#include <vector>
#include "Customer.h"
#include "Transaction.h"

// Facade of the system. The UI only talks to Bank; Bank handles customers,
// account numbers, ownership checks and runs the transactions.
class Bank {
public:
    explicit Bank(const std::string& name);

    const std::string& getName() const { return name_; }

    Customer& registerCustomer(const std::string& name, const std::string& pin);
    Customer* login(const std::string& customerId, const std::string& pin);

    Account* openSavings(Customer& customer, double initialDeposit);
    Account* openChecking(Customer& customer, double initialDeposit);
    Account* findAccount(const std::string& accountNumber) const;
    std::vector<Account*> allAccounts() const;

    // Every operation below returns the recorded transaction so the caller can print the receipt.
    const Transaction& transfer(Customer& actor, const std::string& fromNumber,
                                const std::string& toNumber, double amount);
    const Transaction& topUpEWallet(Customer& actor, const std::string& fromNumber,
                                    const std::string& provider, const std::string& phone, double amount);
    const Transaction& payBill(Customer& actor, const std::string& fromNumber,
                               const std::string& billType, const std::string& customerNo, double amount);

    void runMonthEnd();
    const std::vector<std::unique_ptr<Transaction>>& getTransactionLog() const { return log_; }
    Customer* findCustomerById(const std::string& id) const { return findCustomer(id); }

private:
    Customer* findCustomer(const std::string& id) const;
    std::string name_;
    std::vector<std::unique_ptr<Customer>> customers_;
    std::vector<std::unique_ptr<Transaction>> log_;
    int nextCustomerNo_;
    long long nextAccountNo_;

    Account* requireOwnedAccount(Customer& actor, const std::string& accountNumber) const;
    std::string newAccountNumber(int typePrefix);
    const Transaction& run(std::unique_ptr<Transaction> tx);
};
