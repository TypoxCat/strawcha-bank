#include "Api.h"
#include "third_party/json.hpp"
#include "../src/Exceptions.h"
#include "../src/Utils.h"
#include "../src/Account.h"
#include "../src/SavingsAccount.h"
#include "../src/CheckingAccount.h"
#include "../src/Transaction.h"
#include "../src/TransferTransaction.h"
#include "../src/EWalletTopUpTransaction.h"
#include "../src/BillPaymentTransaction.h"
#include <random>
#include <chrono>
#include <sstream>
#include <iomanip>

using json = nlohmann::json;
using namespace std::chrono;

namespace {
    const int SESSION_TTL_SECONDS = 3600 * 24; // 24 hours
}

Api::Api() : bank_("Strawcha Bank") {
    // Seed data to match console app
    try {
        Customer& dina = bank_.registerCustomer("Dina", "123456");
        bank_.openSavings(dina, 5000000);
        bank_.openChecking(dina, 1000000);
        Customer& nawa = bank_.registerCustomer("Nawa", "654321");
        bank_.openSavings(nawa, 2000000);
    } catch (...) {
        // If already seeded somehow, ignore
    }
}

Api::~Api() = default;

std::string Api::generateToken() {
    static std::mt19937 rng(std::random_device{}());
    static std::uniform_int_distribution<unsigned int> dist(0, 0xFFFFFFFF);
    std::ostringstream oss;
    oss << std::hex << std::setfill('0');
    for (int i = 0; i < 8; ++i) {
        oss << std::setw(8) << dist(rng);
    }
    return oss.str();
}

bool Api::verifyBearer(const httplib::Request& req, std::string& customerId) const {
    auto it = req.headers.find("Authorization");
    if (it == req.headers.end()) return false;
    const std::string& val = it->second;
    const std::string prefix = "Bearer ";
    if (val.rfind(prefix, 0) != 0) return false;
    std::string token = val.substr(prefix.size());
    if (token.empty()) return false;
    std::lock_guard<std::mutex> lock(mutex_);
    auto sit = sessions_.find(token);
    if (sit == sessions_.end()) return false;
    // Check expiry
    time_t now = std::time(nullptr);
    if (now > sit->second.expiresAt) {
        // expired - caller can treat as unauth; but we don't mutate here (const) - simpler: allow re-check not mutating
        return false;
    }
    customerId = sit->second.customerId;
    return true;
}

bool Api::verifyStaff(const httplib::Request& req) const {
    auto it = req.headers.find("X-Staff-Key");
    if (it == req.headers.end()) return false;
    // simple passcode as agreed
    if (it->second == "staff123") return true;
    return false;
}

static json accountToJson(const Account& acc) {
    json j;
    j["accountNumber"] = acc.getAccountNumber();
    j["ownerName"] = acc.getOwnerName();
    j["balance"] = acc.getBalance();
    j["typeName"] = acc.typeName();
    j["availableFunds"] = acc.availableFunds();
    j["summary"] = acc.summary();
    // history
    const auto& h = acc.getHistory();
    j["history"] = json::array();
    for (const auto& line : h) j["history"].push_back(line);
    return j;
}

static json customerToJson(const Customer& c) {
    json j;
    j["id"] = c.getId();
    j["name"] = c.getName();
    j["locked"] = c.isLocked();
    j["totalBalance"] = c.totalBalance();
    const auto& accs = c.getAccounts();
    j["accounts"] = json::array();
    for (const auto& a : accs) j["accounts"].push_back(accountToJson(*a));
    return j;
}

static json transactionToJson(const Transaction& t) {
    json j;
    j["id"] = t.getId();
    j["typeName"] = t.typeName();
    j["amount"] = t.getAmount();
    j["fee"] = t.fee();
    j["total"] = t.getAmount() + t.fee();
    j["status"] = (t.getStatus() == TxStatus::Success ? "SUCCESS" : (t.getStatus() == TxStatus::Failed ? "FAILED" : "PENDING"));
    j["failureReason"] = t.getFailureReason();
    j["timestamp"] = t.getTimestamp();
    j["describe"] = t.describe();
    j["receipt"] = t.receipt();
    if (t.getSource()) j["sourceAccount"] = t.getSource()->getAccountNumber();
    return j;
}

// Routes registration
void Api::registerRoutes(httplib::Server& svr) {
    svr.Post("/api/auth/register", [this](const auto& req, auto& res){ handleRegister(req,res); });
    svr.Post("/api/auth/login", [this](const auto& req, auto& res){ handleLogin(req,res); });
    svr.Post("/api/auth/logout", [this](const auto& req, auto& res){ handleLogout(req,res); });
    svr.Post("/api/auth/pin", [this](const auto& req, auto& res){ handleChangePin(req,res); });

    svr.Get("/api/me", [this](const auto& req, auto& res){ handleMe(req,res); });

    svr.Get("/api/accounts", [this](const auto& req, auto& res){ handleListAccounts(req,res); });
    svr.Post("/api/accounts", [this](const auto& req, auto& res){ handleCreateAccount(req,res); });
    svr.Get("/api/accounts/:number", [this](const auto& req, auto& res){ handleAccountDetail(req,res); });
    svr.Post("/api/accounts/:number/deposit", [this](const auto& req, auto& res){ handleDeposit(req,res); });
    svr.Post("/api/accounts/:number/withdraw", [this](const auto& req, auto& res){ handleWithdraw(req,res); });

    svr.Post("/api/transfer", [this](const auto& req, auto& res){ handleTransfer(req,res); });
    svr.Post("/api/topup", [this](const auto& req, auto& res){ handleTopUp(req,res); });
    svr.Post("/api/paybill", [this](const auto& req, auto& res){ handlePayBill(req,res); });
    svr.Get("/api/transactions", [this](const auto& req, auto& res){ handleListMyTransactions(req,res); });
    svr.Get("/api/transactions/:id", [this](const auto& req, auto& res){ handleGetTransaction(req,res); });

    svr.Post("/api/staff/month-end", [this](const auto& req, auto& res){ handleStaffMonthEnd(req,res); });
    svr.Get("/api/staff/log", [this](const auto& req, auto& res){ handleStaffLog(req,res); });
}

// Helpers to send errors
static void sendError(httplib::Response& res, int code, const std::string& msg) {
    res.status = code;
    res.set_content(json{{"error", msg}}.dump(), "application/json");
}

void Api::handleRegister(const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid json");
        std::string name = body.value("name", "");
        std::string pin = body.value("pin", "");
        if (name.empty() || pin.size() != 6) throw std::runtime_error("invalid");
        std::lock_guard<std::mutex> lock(mutex_);
        Customer& c = bank_.registerCustomer(name, pin);
        res.set_content(json{{"id", c.getId()}, {"name", c.getName()}}.dump(), "application/json");
    } catch (const std::exception& e) {
        sendError(res, 400, e.what());
    }
}

void Api::handleLogin(const httplib::Request& req, httplib::Response& res) {
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid json");
        std::string id = body.value("customerId", "");
        std::string pin = body.value("pin", "");
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.login(id, pin);
        // create token
        std::string token = generateToken();
        time_t now = std::time(nullptr);
        sessions_[token] = {c->getId(), now + SESSION_TTL_SECONDS};
        json out;
        out["token"] = token;
        out["customer"] = customerToJson(*c);
        res.set_content(out.dump(), "application/json");
    } catch (const AuthenticationException& e) {
        sendError(res, 401, e.what());
    } catch (const std::exception& e) {
        sendError(res, 400, e.what());
    }
}

void Api::handleLogout(const httplib::Request& req, httplib::Response& res) {
    auto it = req.headers.find("Authorization");
    if (it != req.headers.end()) {
        const std::string& val = it->second;
        const std::string prefix = "Bearer ";
        if (val.rfind(prefix,0)==0) {
            std::string token = val.substr(prefix.size());
            if (!token.empty()) {
                std::lock_guard<std::mutex> lock(mutex_);
                sessions_.erase(token);
            }
        }
    }
    res.set_content("{}", "application/json");
}

void Api::handleChangePin(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        std::string oldp = body.value("oldPin","");
        std::string newp = body.value("newPin","");
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        c->changePin(oldp, newp);
        res.set_content("{}", "application/json");
    } catch (const BankException& e) {
        // map generically
        sendError(res, 400, e.what());
    } catch (...) {
        sendError(res, 400, "invalid request");
    }
}

void Api::handleMe(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    std::lock_guard<std::mutex> lock(mutex_);
    Customer* c = bank_.findCustomerById(cid);
    if (!c) { sendError(res, 404, "not found"); return; }
    res.set_content(customerToJson(*c).dump(), "application/json");
}

void Api::handleListAccounts(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    std::lock_guard<std::mutex> lock(mutex_);
    Customer* c = bank_.findCustomerById(cid);
    if (!c) { sendError(res, 404, "not found"); return; }
    json arr = json::array();
    for (const auto& a : c->getAccounts()) arr.push_back(accountToJson(*a));
    res.set_content(arr.dump(), "application/json");
}

void Api::handleCreateAccount(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        std::string type = body.value("type","savings");
        double initial = body.value("initialDeposit", 0.0);
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        Account* acc = nullptr;
        if (type == "checking") acc = bank_.openChecking(*c, initial);
        else acc = bank_.openSavings(*c, initial);
        res.set_content(accountToJson(*acc).dump(), "application/json");
    } catch (const BankException& e) {
        sendError(res, 400, e.what());
    } catch (...) {
        sendError(res, 400, "invalid request");
    }
}

void Api::handleAccountDetail(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    std::string num = req.path_params.at("number");
    std::lock_guard<std::mutex> lock(mutex_);
    Customer* c = bank_.findCustomerById(cid);
    if (!c) { sendError(res, 404, "not found"); return; }
    Account* a = c->findOwnAccount(num);
    if (!a) { sendError(res, 403, "not your account"); return; }
    res.set_content(accountToJson(*a).dump(), "application/json");
}

void Api::handleDeposit(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        std::string num = req.path_params.at("number");
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        double amount = body.value("amount", 0.0);
        std::string note = body.value("note", "Deposit");
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        Account* a = c->findOwnAccount(num);
        if (!a) { sendError(res, 403, "not your account"); return; }
        a->deposit(amount, note);
        res.set_content(accountToJson(*a).dump(), "application/json");
    } catch (const InvalidAmountException& e) {
        sendError(res, 400, e.what());
    } catch (const BankException& e) {
        sendError(res, 400, e.what());
    } catch (...) {
        sendError(res, 400, "invalid request");
    }
}

void Api::handleWithdraw(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        std::string num = req.path_params.at("number");
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        double amount = body.value("amount", 0.0);
        std::string note = body.value("note", "Withdrawal");
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        Account* a = c->findOwnAccount(num);
        if (!a) { sendError(res, 403, "not your account"); return; }
        a->withdraw(amount, note);
        res.set_content(accountToJson(*a).dump(), "application/json");
    } catch (const InsufficientFundsException& e) {
        sendError(res, 409, e.what());
    } catch (const InvalidAmountException& e) {
        sendError(res, 400, e.what());
    } catch (const BankException& e) {
        sendError(res, 400, e.what());
    } catch (...) {
        sendError(res, 400, "invalid request");
    }
}

void Api::handleTransfer(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        std::string from = body.value("from", "");
        std::string to = body.value("to", "");
        double amount = body.value("amount", 0.0);
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        const Transaction& t = bank_.transfer(*c, from, to, amount);
        res.set_content(transactionToJson(t).dump(), "application/json");
    } catch (const AuthenticationException& e) { sendError(res, 401, e.what()); }
    catch (const AccountNotFoundException& e) { sendError(res, 404, e.what()); }
    catch (const RuleViolationException& e) { sendError(res, 403, e.what()); }
    catch (const InsufficientFundsException& e) { sendError(res, 409, e.what()); }
    catch (const InvalidAmountException& e) { sendError(res, 400, e.what()); }
    catch (const BankException& e) { sendError(res, 400, e.what()); }
    catch (...) { sendError(res, 400, "invalid request"); }
}

void Api::handleTopUp(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        std::string from = body.value("from", "");
        std::string provider = body.value("provider", "");
        std::string phone = body.value("phone", "");
        double amount = body.value("amount", 0.0);
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        const Transaction& t = bank_.topUpEWallet(*c, from, provider, phone, amount);
        res.set_content(transactionToJson(t).dump(), "application/json");
    } catch (const RuleViolationException& e) { sendError(res, 403, e.what()); }
    catch (const InsufficientFundsException& e) { sendError(res, 409, e.what()); }
    catch (const InvalidAmountException& e) { sendError(res, 400, e.what()); }
    catch (const BankException& e) { sendError(res, 400, e.what()); }
    catch (...) { sendError(res, 400, "invalid request"); }
}

void Api::handlePayBill(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    try {
        auto body = json::parse(req.body, nullptr, false);
        if (body.is_discarded()) throw std::runtime_error("invalid");
        std::string from = body.value("from", "");
        std::string billType = body.value("billType", "");
        std::string customerNo = body.value("customerNo", "");
        double amount = body.value("amount", 0.0);
        std::lock_guard<std::mutex> lock(mutex_);
        Customer* c = bank_.findCustomerById(cid);
        if (!c) { sendError(res, 404, "not found"); return; }
        const Transaction& t = bank_.payBill(*c, from, billType, customerNo, amount);
        res.set_content(transactionToJson(t).dump(), "application/json");
    } catch (const RuleViolationException& e) { sendError(res, 403, e.what()); }
    catch (const InsufficientFundsException& e) { sendError(res, 409, e.what()); }
    catch (const InvalidAmountException& e) { sendError(res, 400, e.what()); }
    catch (const BankException& e) { sendError(res, 400, e.what()); }
    catch (...) { sendError(res, 400, "invalid request"); }
}

void Api::handleListMyTransactions(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    std::lock_guard<std::mutex> lock(mutex_);
    Customer* c = bank_.findCustomerById(cid);
    if (!c) { sendError(res, 404, "not found"); return; }
    // build set of owned account numbers
    std::unordered_map<std::string,bool> own;
    for (const auto& a : c->getAccounts()) own[a->getAccountNumber()] = true;
    json arr = json::array();
    const auto& log = bank_.getTransactionLog();
    // return in order (most recent last is natural; but keep as is; frontend can reverse)
    for (const auto& tx : log) {
        const Account* src = tx->getSource();
        if (src && own.count(src->getAccountNumber())) {
            arr.push_back(transactionToJson(*tx));
        }
    }
    res.set_content(arr.dump(), "application/json");
}

void Api::handleGetTransaction(const httplib::Request& req, httplib::Response& res) {
    std::string cid;
    if (!verifyBearer(req, cid)) { sendError(res, 401, "unauthorized"); return; }
    int tid = std::stoi(req.path_params.at("id"));
    std::lock_guard<std::mutex> lock(mutex_);
    Customer* c = bank_.findCustomerById(cid);
    if (!c) { sendError(res, 404, "not found"); return; }
    std::unordered_map<std::string,bool> own;
    for (const auto& a : c->getAccounts()) own[a->getAccountNumber()] = true;
    const auto& log = bank_.getTransactionLog();
    for (const auto& tx : log) {
        if (tx->getId() == tid) {
            const Account* src = tx->getSource();
            if (src && own.count(src->getAccountNumber())) {
                res.set_content(transactionToJson(*tx).dump(), "application/json");
                return;
            } else {
                sendError(res, 403, "not yours"); return;
            }
        }
    }
    sendError(res, 404, "not found");
}

void Api::handleStaffMonthEnd(const httplib::Request& req, httplib::Response& res) {
    if (!verifyStaff(req)) { sendError(res, 401, "staff key required"); return; }
    std::lock_guard<std::mutex> lock(mutex_);
    bank_.runMonthEnd();
    res.set_content(json{{"ok", true}}.dump(), "application/json");
}

void Api::handleStaffLog(const httplib::Request& req, httplib::Response& res) {
    if (!verifyStaff(req)) { sendError(res, 401, "staff key required"); return; }
    std::lock_guard<std::mutex> lock(mutex_);
    json arr = json::array();
    for (const auto& tx : bank_.getTransactionLog()) {
        arr.push_back(transactionToJson(*tx));
    }
    // also include accounts overview
    json accounts = json::array();
    for (Account* a : bank_.allAccounts()) accounts.push_back(accountToJson(*a));
    json out;
    out["transactions"] = arr;
    out["accounts"] = accounts;
    res.set_content(out.dump(), "application/json");
}