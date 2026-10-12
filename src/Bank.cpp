#include "Bank.h"
#include "BillPaymentTransaction.h"
#include "CheckingAccount.h"
#include "EWalletTopUpTransaction.h"
#include "Exceptions.h"
#include "SavingsAccount.h"
#include "TransferTransaction.h"
#include <iomanip>
#include <sstream>

Bank::Bank(const std::string& name) : name_(name), nextCustomerNo_(1), nextAccountNo_(1) {}

Customer& Bank::registerCustomer(const std::string& name, const std::string& pin) {
    std::ostringstream id;
    id << "C" << std::setw(3) << std::setfill('0') << nextCustomerNo_;
    customers_.push_back(std::make_unique<Customer>(id.str(), name, pin));
    ++nextCustomerNo_;   // only used if the constructor above did not throw
    return *customers_.back();
}

Customer* Bank::findCustomer(const std::string& id) const {
    for (const auto& c : customers_) {
        if (c->getId() == id) return c.get();
    }
    return nullptr;
}

Customer* Bank::login(const std::string& customerId, const std::string& pin) {
    Customer* customer = findCustomer(customerId);
    // same message for "no such customer" and "wrong PIN" so nobody can probe which IDs exist
    if (customer == nullptr) {
        throw AuthenticationException("Invalid customer ID or PIN.");
    }
    if (!customer->verifyPin(pin)) {
        if (customer->isLocked()) {
            throw AuthenticationException("Too many wrong attempts. The customer is now locked.");
        }
        throw AuthenticationException("Invalid customer ID or PIN.");
    }
    return customer;
}

std::string Bank::newAccountNumber(int typePrefix) {
    // 10 digits: first digit is the account type (1 savings, 2 checking)
    long long number = typePrefix * 1000000000LL + nextAccountNo_;
    ++nextAccountNo_;
    return std::to_string(number);
}

Account* Bank::openSavings(Customer& customer, double initialDeposit) {
    auto account = std::make_unique<SavingsAccount>(newAccountNumber(1), customer.getName(), initialDeposit);
    return customer.addAccount(std::move(account));
}

Account* Bank::openChecking(Customer& customer, double initialDeposit) {
    auto account = std::make_unique<CheckingAccount>(newAccountNumber(2), customer.getName(), initialDeposit);
    return customer.addAccount(std::move(account));
}

Account* Bank::findAccount(const std::string& accountNumber) const {
    for (const auto& c : customers_) {
        Account* acc = c->findOwnAccount(accountNumber);
        if (acc != nullptr) return acc;
    }
    return nullptr;
}

std::vector<Account*> Bank::allAccounts() const {
    std::vector<Account*> result;
    for (const auto& c : customers_) {
        for (const auto& acc : c->getAccounts()) {
            result.push_back(acc.get());
        }
    }
    return result;
}

Account* Bank::requireOwnedAccount(Customer& actor, const std::string& accountNumber) const {
    Account* acc = actor.findOwnAccount(accountNumber);
    if (acc == nullptr) {
        throw RuleViolationException("Account " + accountNumber + " does not belong to you.");
    }
    return acc;
}

const Transaction& Bank::run(std::unique_ptr<Transaction> tx) {
    Transaction* raw = tx.get();
    log_.push_back(std::move(tx));
    raw->process();
    return *raw;
}

const Transaction& Bank::transfer(Customer& actor, const std::string& fromNumber,
                                  const std::string& toNumber, double amount) {
    Account* from = requireOwnedAccount(actor, fromNumber);
    Account* to = findAccount(toNumber);
    if (to == nullptr) {
        throw AccountNotFoundException("Destination account " + toNumber + " not found.");
    }
    return run(std::make_unique<TransferTransaction>(from, to, amount));
}

const Transaction& Bank::topUpEWallet(Customer& actor, const std::string& fromNumber,
                                      const std::string& provider, const std::string& phone, double amount) {
    Account* from = requireOwnedAccount(actor, fromNumber);
    return run(std::make_unique<EWalletTopUpTransaction>(from, provider, phone, amount));
}

const Transaction& Bank::payBill(Customer& actor, const std::string& fromNumber,
                                 const std::string& billType, const std::string& customerNo, double amount) {
    Account* from = requireOwnedAccount(actor, fromNumber);
    return run(std::make_unique<BillPaymentTransaction>(from, billType, customerNo, amount));
}

void Bank::runMonthEnd() {
    // We do not know (or care) which kind of account each one is.
    for (Account* acc : allAccounts()) {
        acc->monthlyProcess();
    }
}
