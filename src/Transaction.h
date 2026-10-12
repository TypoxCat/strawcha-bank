#pragma once
#include <string>
#include "Account.h"

enum class TxStatus { Pending, Success, Failed };

// Abstract base class for every banking transaction.
//
// process() is the single public entry point. It runs the subclass's execute()
// and records whether it worked, so the caller never deals with the details
// (abstraction). execute() is protected: nobody outside can run a transaction
// "half way" or skip the status bookkeeping (encapsulation).
class Transaction {
public:
    Transaction(Account* source, double amount);
    virtual ~Transaction() = default;

    bool process();                       // returns true when the transaction succeeded
    std::string receipt() const;          // printable receipt, same code for every type

    int getId() const { return id_; }
    double getAmount() const { return amount_; }
    TxStatus getStatus() const { return status_; }
    const std::string& getFailureReason() const { return failureReason_; }
    const std::string& getTimestamp() const { return timestamp_; }
    Account* getSource() const { return source_; }

    virtual std::string typeName() const = 0;
    virtual std::string describe() const = 0;
    virtual double fee() const = 0;

protected:
    virtual void execute() = 0;           // throws BankException on failure
    Account* source_;
    double amount_;

private:
    static int nextId_;
    int id_;
    TxStatus status_;
    std::string failureReason_;
    std::string timestamp_;
};
