#pragma once
#include <string>
#include <unordered_map>
#include <mutex>
#include <memory>
#include "third_party/httplib.h"
#include "../src/Bank.h"

struct Session {
    std::string customerId;
    std::time_t expiresAt;
};

class Api {
public:
    Api();
    ~Api();

    // Bind routes to server
    void registerRoutes(httplib::Server& svr);

private:
    Bank bank_;
    mutable std::mutex mutex_;
    std::unordered_map<std::string, Session> sessions_; // token -> session

    // Token helpers
    std::string generateToken();
    bool verifyBearer(const httplib::Request& req, std::string& customerId) const;
    bool verifyStaff(const httplib::Request& req) const;

    // Auth
    void handleRegister(const httplib::Request& req, httplib::Response& res);
    void handleLogin(const httplib::Request& req, httplib::Response& res);
    void handleLogout(const httplib::Request& req, httplib::Response& res);
    void handleChangePin(const httplib::Request& req, httplib::Response& res);

    // Me
    void handleMe(const httplib::Request& req, httplib::Response& res);

    // Accounts
    void handleListAccounts(const httplib::Request& req, httplib::Response& res);
    void handleCreateAccount(const httplib::Request& req, httplib::Response& res);
    void handleAccountDetail(const httplib::Request& req, httplib::Response& res);
    void handleDeposit(const httplib::Request& req, httplib::Response& res);
    void handleWithdraw(const httplib::Request& req, httplib::Response& res);

    // Transactions
    void handleTransfer(const httplib::Request& req, httplib::Response& res);
    void handleTopUp(const httplib::Request& req, httplib::Response& res);
    void handlePayBill(const httplib::Request& req, httplib::Response& res);
    void handleListMyTransactions(const httplib::Request& req, httplib::Response& res);
    void handleGetTransaction(const httplib::Request& req, httplib::Response& res);

    // Staff
    void handleStaffMonthEnd(const httplib::Request& req, httplib::Response& res);
    void handleStaffLog(const httplib::Request& req, httplib::Response& res);
};