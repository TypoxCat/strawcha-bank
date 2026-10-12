#pragma once
#include <stdexcept>
#include <string>

// All errors thrown by the banking system derive from BankException,
// so the UI layer can catch one type and show a clean message.
class BankException : public std::runtime_error {
public:
    explicit BankException(const std::string& msg) : std::runtime_error(msg) {}
};

class InvalidAmountException : public BankException {
public:
    explicit InvalidAmountException(const std::string& msg) : BankException(msg) {}
};

class InsufficientFundsException : public BankException {
public:
    explicit InsufficientFundsException(const std::string& msg) : BankException(msg) {}
};

class AccountNotFoundException : public BankException {
public:
    explicit AccountNotFoundException(const std::string& msg) : BankException(msg) {}
};

class AuthenticationException : public BankException {
public:
    explicit AuthenticationException(const std::string& msg) : BankException(msg) {}
};

// Used when an operation breaks a business rule (minimum top up, not the owner, etc.)
class RuleViolationException : public BankException {
public:
    explicit RuleViolationException(const std::string& msg) : BankException(msg) {}
};
