// M-Banking console application
// OOP Pillars assignment: encapsulation, abstraction, inheritance, polymorphism.

#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include "Bank.h"
#include "Exceptions.h"
#include "Utils.h"

using namespace std;

// ---------- small input helpers ----------

static string readLine(const string& prompt) {
    cout << prompt;
    string line;
    if (!getline(cin, line)) {   // input closed (Ctrl+D / end of file)
        cout << "\n";
        exit(0);
    }
    return line;
}

static double readAmount(const string& prompt) {
    while (true) {
        string text = readLine(prompt);
        try {
            size_t pos = 0;
            double value = stod(text, &pos);
            if (pos == text.size()) return value;
        } catch (...) {
        }
        cout << "Please enter a valid number.\n";
    }
}

static int readChoice(const string& prompt) {
    string text = readLine(prompt);
    try {
        return stoi(text);
    } catch (...) {
        return -1;
    }
}

static void printAccounts(const Customer& customer) {
    const auto& accounts = customer.getAccounts();
    if (accounts.empty()) {
        cout << "You do not have any account yet. Open one from the menu.\n";
        return;
    }
    int i = 1;
    for (const auto& acc : accounts) {
        cout << i++ << ". " << acc->summary() << "\n";
    }
    cout << "Total balance: " << formatRupiah(customer.totalBalance()) << "\n";
}

// Lets the user pick one of their own accounts by its list number.
static Account* chooseAccount(const Customer& customer, const string& prompt) {
    const auto& accounts = customer.getAccounts();
    if (accounts.empty()) {
        cout << "You do not have any account yet.\n";
        return nullptr;
    }
    printAccounts(customer);
    int choice = readChoice(prompt);
    if (choice < 1 || choice > static_cast<int>(accounts.size())) {
        cout << "Invalid choice.\n";
        return nullptr;
    }
    return accounts[choice - 1].get();
}

static string chooseFromList(const string& title, const vector<string>& options) {
    cout << title << "\n";
    for (size_t i = 0; i < options.size(); ++i) {
        cout << "  " << i + 1 << ". " << options[i] << "\n";
    }
    int choice = readChoice("Choose: ");
    if (choice < 1 || choice > static_cast<int>(options.size())) return "";
    return options[choice - 1];
}

// ---------- customer menu ----------

static void customerMenu(Bank& bank, Customer& customer) {
    cout << "\nWelcome, " << customer.getName() << "!\n";
    bool running = true;
    while (running) {
        cout << "\n--- " << bank.getName() << " | " << customer.getName() << " ---\n"
             << "1. My accounts\n"
             << "2. Deposit\n"
             << "3. Withdraw\n"
             << "4. Transfer\n"
             << "5. E-wallet top up\n"
             << "6. Pay bill\n"
             << "7. Account statement\n"
             << "8. Open new account\n"
             << "9. Change PIN\n"
             << "0. Logout\n";
        int choice = readChoice("Choose: ");

        try {
            switch (choice) {
            case 1:
                printAccounts(customer);
                break;
            case 2: {
                Account* acc = chooseAccount(customer, "Deposit to account no.: ");
                if (!acc) break;
                acc->deposit(readAmount("Amount: "));
                cout << "Deposit successful. New balance: " << formatRupiah(acc->getBalance()) << "\n";
                break;
            }
            case 3: {
                Account* acc = chooseAccount(customer, "Withdraw from account no.: ");
                if (!acc) break;
                acc->withdraw(readAmount("Amount: "));
                cout << "Withdrawal successful. New balance: " << formatRupiah(acc->getBalance()) << "\n";
                break;
            }
            case 4: {
                Account* from = chooseAccount(customer, "Transfer from account no.: ");
                if (!from) break;
                string to = readLine("Destination account number: ");
                double amount = readAmount("Amount: ");
                cout << bank.transfer(customer, from->getAccountNumber(), to, amount).receipt() << "\n";
                break;
            }
            case 5: {
                Account* from = chooseAccount(customer, "Pay from account no.: ");
                if (!from) break;
                string provider = chooseFromList("E-wallet:", {"GoPay", "OVO", "DANA", "ShopeePay"});
                if (provider.empty()) { cout << "Invalid choice.\n"; break; }
                string phone = readLine("Phone number: ");
                double amount = readAmount("Amount: ");
                cout << bank.topUpEWallet(customer, from->getAccountNumber(), provider, phone, amount).receipt() << "\n";
                break;
            }
            case 6: {
                Account* from = chooseAccount(customer, "Pay from account no.: ");
                if (!from) break;
                string type = chooseFromList("Bill type:", {"Electricity", "Water", "Internet"});
                if (type.empty()) { cout << "Invalid choice.\n"; break; }
                string customerNo = readLine("Customer number on the bill: ");
                double amount = readAmount("Bill amount: ");
                cout << bank.payBill(customer, from->getAccountNumber(), type, customerNo, amount).receipt() << "\n";
                break;
            }
            case 7: {
                Account* acc = chooseAccount(customer, "Statement for account no.: ");
                if (!acc) break;
                cout << "\nStatement " << acc->getAccountNumber() << "\n";
                for (const auto& line : acc->getHistory()) cout << "  " << line << "\n";
                break;
            }
            case 8: {
                string type = chooseFromList("Account type:", {"Savings", "Checking"});
                if (type.empty()) { cout << "Invalid choice.\n"; break; }
                double initial = readAmount("Opening deposit: ");
                Account* acc = (type == "Savings") ? bank.openSavings(customer, initial)
                                                   : bank.openChecking(customer, initial);
                cout << "Account opened: " << acc->summary() << "\n";
                break;
            }
            case 9: {
                string oldPin = readLine("Old PIN: ");
                string newPin = readLine("New PIN (6 digits): ");
                customer.changePin(oldPin, newPin);
                cout << "PIN changed.\n";
                break;
            }
            case 0:
                running = false;
                break;
            default:
                cout << "Unknown menu choice.\n";
            }
        } catch (const BankException& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
}

// ---------- demo (no typing needed) ----------

static void section(const string& title) {
    cout << "\n##### " << title << " #####\n";
}

static void runDemo() {
    Bank bank("Demo Bank");
    Customer& dina = bank.registerCustomer("Dina", "123456");
    Customer& nawa = bank.registerCustomer("Nawa", "654321");
    Account* dinaSavings  = bank.openSavings(dina, 5000000);
    Account* dinaChecking = bank.openChecking(dina, 1000000);
    Account* nawaSavings  = bank.openSavings(nawa, 2000000);

    section("1. Encapsulation: rules are enforced by the class itself");
    cout << "Balance can only be changed through deposit/withdraw.\n";
    try { dinaSavings->withdraw(10000000); }
    catch (const BankException& e) { cout << "Withdraw 10.000.000 -> " << e.what() << "\n"; }
    try { dinaSavings->deposit(-500); }
    catch (const BankException& e) { cout << "Deposit -500 -> " << e.what() << "\n"; }
    try { dinaSavings->withdraw(4980000); }
    catch (const BankException& e) { cout << "Withdraw 4.980.000 (below min balance) -> " << e.what() << "\n"; }
    cout << "Balance is still " << formatRupiah(dinaSavings->getBalance()) << "\n";

    cout << "\nPIN is protected too. Three wrong logins:\n";
    for (int i = 1; i <= 3; ++i) {
        try { bank.login("C001", "000000"); }
        catch (const BankException& e) { cout << "  try " << i << " -> " << e.what() << "\n"; }
    }
    try { bank.login("C001", "123456"); }
    catch (const BankException& e) { cout << "  correct PIN now -> " << e.what() << "\n"; }
    dina.unlock();   // done by bank staff in real life

    section("2. Abstraction: one call hides validation, fees and bookkeeping");
    const Transaction& t1 = bank.transfer(dina, dinaSavings->getAccountNumber(), nawaSavings->getAccountNumber(), 750000);
    cout << t1.receipt() << "\n";

    section("3. Inheritance: Savings and Checking are both Accounts");
    cout << dinaSavings->summary() << "\n" << dinaChecking->summary() << "\n";
    cout << "Overdraft on checking: withdraw 1.400.000 from a 1.000.000 balance\n";
    dinaChecking->withdraw(1400000);
    cout << dinaChecking->summary() << "\n";

    section("4. Polymorphism (dynamic): same call, different behavior");
    vector<Account*> accounts = bank.allAccounts();
    for (Account* acc : accounts) {
        cout << acc->typeName() << " " << acc->getAccountNumber()
             << " can use " << formatRupiah(acc->availableFunds()) << "\n";
    }
    cout << "\nMonth-end processing:\n";
    bank.runMonthEnd();
    for (Account* acc : accounts) cout << "  " << acc->summary() << "\n";

    cout << "\nTransactions of different types, handled through Transaction:\n";
    bank.topUpEWallet(nawa, nawaSavings->getAccountNumber(), "GoPay", "081234567890", 100000);
    bank.payBill(nawa, nawaSavings->getAccountNumber(), "Electricity", "5123456789", 350000);
    bank.topUpEWallet(nawa, nawaSavings->getAccountNumber(), "OVO", "081234567890", 5000);   // below minimum
    for (const auto& tx : bank.getTransactionLog()) {
        cout << "  #" << tx->getId() << " " << tx->typeName() << " | " << tx->describe()
             << " | " << (tx->getStatus() == TxStatus::Success ? "SUCCESS" : "FAILED: " + tx->getFailureReason()) << "\n";
    }

    section("5. Polymorphism (static): overloaded deposit()");
    nawaSavings->deposit(100000);
    nawaSavings->deposit(250000, "Salary");
    for (const auto& line : nawaSavings->getHistory()) cout << "  " << line << "\n";
}

// ---------- main ----------

int main() {
    Bank bank("Strawcha Bank");

    // sample data so the program can be tried right away
    Customer& dina = bank.registerCustomer("Dina", "123456");
    bank.openSavings(dina, 5000000);
    bank.openChecking(dina, 1000000);
    Customer& nawa = bank.registerCustomer("Nawa", "654321");
    bank.openSavings(nawa, 2000000);

    cout << "=== Strawcha Bank (STC) ===\n"
         << "Sample logins: C001 / 123456 (Dina), C002 / 654321 (Nawa)\n";

    bool running = true;
    while (running) {
        cout << "\n1. Login\n2. Register\n3. Month-end processing (bank staff)\n4. Run demo\n0. Exit\n";
        int choice = readChoice("Choose: ");
        try {
            switch (choice) {
            case 1: {
                string id = readLine("Customer ID: ");
                string pin = readLine("PIN: ");
                Customer* customer = bank.login(id, pin);
                customerMenu(bank, *customer);
                break;
            }
            case 2: {
                string name = readLine("Full name: ");
                string pin = readLine("Choose a 6 digit PIN: ");
                Customer& c = bank.registerCustomer(name, pin);
                cout << "Registered. Your customer ID is " << c.getId() << ". Log in to open an account.\n";
                break;
            }
            case 3:
                bank.runMonthEnd();
                cout << "Month-end done (interest added, fees charged).\n";
                break;
            case 4:
                runDemo();
                break;
            case 0:
                running = false;
                break;
            default:
                cout << "Unknown menu choice.\n";
            }
        } catch (const BankException& e) {
            cout << "Error: " << e.what() << "\n";
        }
    }
    cout << "Thank you for using Strawcha Bank.\n";
    return 0;
}
