#include "Customer.h"
#include "Exceptions.h"
#include <cctype>
#include <functional>

Customer::Customer(const std::string& id, const std::string& name, const std::string& pin)
    : id_(id), name_(name), failedAttempts_(0), locked_(false) {
    if (name.empty()) {
        throw RuleViolationException("Customer name cannot be empty.");
    }
    checkPinFormat(pin);
    pinHash_ = hashPin(pin);
}

void Customer::checkPinFormat(const std::string& pin) {
    if (pin.size() != 6) {
        throw RuleViolationException("PIN must be exactly 6 digits.");
    }
    for (char c : pin) {
        if (!std::isdigit(static_cast<unsigned char>(c))) {
            throw RuleViolationException("PIN must contain digits only.");
        }
    }
}

std::size_t Customer::hashPin(const std::string& pin) {
    // simple hash for this assignment, a real system would use bcrypt/argon2 with a salt
    return std::hash<std::string>{}("mbank-salt:" + pin);
}

bool Customer::verifyPin(const std::string& pin) {
    if (locked_) {
        throw AuthenticationException("This customer is locked. Please contact the bank.");
    }
    if (hashPin(pin) == pinHash_) {
        failedAttempts_ = 0;
        return true;
    }
    ++failedAttempts_;
    if (failedAttempts_ >= MAX_FAILED_ATTEMPTS) {
        locked_ = true;
    }
    return false;
}

void Customer::changePin(const std::string& oldPin, const std::string& newPin) {
    if (!verifyPin(oldPin)) {
        throw AuthenticationException("Old PIN is incorrect.");
    }
    checkPinFormat(newPin);
    pinHash_ = hashPin(newPin);
}

void Customer::unlock() {
    locked_ = false;
    failedAttempts_ = 0;
}

Account* Customer::addAccount(std::unique_ptr<Account> account) {
    accounts_.push_back(std::move(account));
    return accounts_.back().get();
}

Account* Customer::findOwnAccount(const std::string& accountNumber) const {
    for (const auto& acc : accounts_) {
        if (acc->getAccountNumber() == accountNumber) {
            return acc.get();
        }
    }
    return nullptr;
}

double Customer::totalBalance() const {
    double total = 0;
    for (const auto& acc : accounts_) {
        total += acc->getBalance();
    }
    return total;
}
