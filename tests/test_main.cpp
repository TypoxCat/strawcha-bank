// Small test program without any framework. Run with: make test
#include <iostream>
#include <string>
#include <cmath>
#include "Bank.h"
#include "CheckingAccount.h"
#include "Exceptions.h"
#include "SavingsAccount.h"

static int passed = 0;
static int failed = 0;

#define CHECK(cond)                                                              \
    do {                                                                         \
        if (cond) { ++passed; }                                                  \
        else { ++failed; std::cout << "FAILED line " << __LINE__ << ": " #cond << "\n"; } \
    } while (0)

#define CHECK_THROWS(type, expr)                                                 \
    do {                                                                         \
        bool thrown = false;                                                     \
        try { expr; } catch (const type&) { thrown = true; }                     \
        CHECK(thrown);                                                           \
    } while (0)

static bool near(double a, double b) { return std::fabs(a - b) < 0.01; }

void testEncapsulation() {
    SavingsAccount acc("1000000001", "Test", 1000000);
    CHECK_THROWS(InvalidAmountException, acc.deposit(0));
    CHECK_THROWS(InvalidAmountException, acc.deposit(-10));
    CHECK_THROWS(InsufficientFundsException, acc.withdraw(2000000));
    CHECK(near(acc.getBalance(), 1000000));      // balance untouched after failed operations
    acc.deposit(500000);
    CHECK(near(acc.getBalance(), 1500000));
}

void testSavings() {
    SavingsAccount acc("1000000001", "Test", 1000000);
    CHECK(near(acc.availableFunds(), 950000));
    CHECK_THROWS(InsufficientFundsException, acc.withdraw(960000));
    acc.withdraw(950000);
    CHECK(near(acc.getBalance(), 50000));
    CHECK_THROWS(RuleViolationException, SavingsAccount("1000000002", "Test", 10000));

    SavingsAccount s2("1000000003", "Test", 1200000);
    s2.monthlyProcess();                         // 1.2M * 3% / 12 = 3000
    CHECK(near(s2.getBalance(), 1203000));
}

void testChecking() {
    CheckingAccount acc("2000000001", "Test", 100000);
    CHECK(near(acc.availableFunds(), 600000));
    acc.withdraw(500000);                        // overdraft
    CHECK(near(acc.getBalance(), -400000));
    CHECK_THROWS(InsufficientFundsException, acc.withdraw(200000));
    acc.monthlyProcess();
    CHECK(near(acc.getBalance(), -410000));
}

void testPolymorphicCollection() {
    Bank bank("Test");
    Customer& c = bank.registerCustomer("Budi", "111111");
    bank.openSavings(c, 1200000);
    bank.openChecking(c, 100000);
    bank.runMonthEnd();
    std::vector<Account*> all = bank.allAccounts();
    CHECK(all.size() == 2);
    CHECK(all[0]->typeName() == "Savings");
    CHECK(all[1]->typeName() == "Checking");
    CHECK(near(all[0]->getBalance(), 1203000));  // interest added
    CHECK(near(all[1]->getBalance(), 90000));    // fee charged
}

void testTransfers() {
    Bank bank("Test");
    Customer& a = bank.registerCustomer("Ani", "111111");
    Customer& b = bank.registerCustomer("Budi", "222222");
    Account* a1 = bank.openSavings(a, 1000000);
    Account* a2 = bank.openChecking(a, 0);
    Account* b1 = bank.openSavings(b, 100000);

    // other owner: fee 2500
    const Transaction& t = bank.transfer(a, a1->getAccountNumber(), b1->getAccountNumber(), 200000);
    CHECK(t.getStatus() == TxStatus::Success);
    CHECK(near(a1->getBalance(), 797500));
    CHECK(near(b1->getBalance(), 300000));

    // same owner: no fee
    bank.transfer(a, a1->getAccountNumber(), a2->getAccountNumber(), 100000);
    CHECK(near(a1->getBalance(), 697500));
    CHECK(near(a2->getBalance(), 100000));

    // too much: failed, nothing moves
    const Transaction& bad = bank.transfer(a, a1->getAccountNumber(), b1->getAccountNumber(), 900000);
    CHECK(bad.getStatus() == TxStatus::Failed);
    CHECK(near(a1->getBalance(), 697500));
    CHECK(near(b1->getBalance(), 300000));

    // cannot use someone else's account as the source
    CHECK_THROWS(RuleViolationException, bank.transfer(a, b1->getAccountNumber(), a1->getAccountNumber(), 1000));
    CHECK_THROWS(AccountNotFoundException, bank.transfer(a, a1->getAccountNumber(), "9999999999", 1000));
    CHECK(bank.transfer(a, a1->getAccountNumber(), a1->getAccountNumber(), 1000).getStatus() == TxStatus::Failed);
}

void testOtherTransactions() {
    Bank bank("Test");
    Customer& a = bank.registerCustomer("Ani", "111111");
    Account* s = bank.openSavings(a, 1000000);

    CHECK(bank.topUpEWallet(a, s->getAccountNumber(), "GoPay", "0812", 100000).getStatus() == TxStatus::Success);
    CHECK(near(s->getBalance(), 899000));
    CHECK(bank.topUpEWallet(a, s->getAccountNumber(), "GoPay", "0812", 5000).getStatus() == TxStatus::Failed);
    CHECK(bank.payBill(a, s->getAccountNumber(), "Water", "777", 100000).getStatus() == TxStatus::Success);
    CHECK(near(s->getBalance(), 796500));
    CHECK(bank.payBill(a, s->getAccountNumber(), "Water", "777", 900000).getStatus() == TxStatus::Failed);
    CHECK_THROWS(InvalidAmountException, bank.payBill(a, s->getAccountNumber(), "Water", "777", -5));
    CHECK(bank.getTransactionLog().size() == 4);
}

void testCustomerAndLogin() {
    Bank bank("Test");
    bank.registerCustomer("Ani", "123456");
    CHECK_THROWS(RuleViolationException, bank.registerCustomer("Bad", "12"));
    CHECK_THROWS(RuleViolationException, bank.registerCustomer("Bad", "12ab56"));
    CHECK_THROWS(AuthenticationException, bank.login("C999", "123456"));
    CHECK(bank.login("C001", "123456") != nullptr);

    for (int i = 0; i < 3; ++i) {
        CHECK_THROWS(AuthenticationException, bank.login("C001", "000000"));
    }
    CHECK_THROWS(AuthenticationException, bank.login("C001", "123456"));   // locked even with right PIN

    Customer* c = nullptr;
    {
        Bank b2("Test2");
        Customer& x = b2.registerCustomer("Citra", "123456");
        c = &x;
        CHECK_THROWS(AuthenticationException, c->changePin("999999", "654321"));
        c->changePin("123456", "654321");
        CHECK(c->verifyPin("654321"));
        CHECK(!c->verifyPin("123456"));
    }
}

int main() {
    testEncapsulation();
    testSavings();
    testChecking();
    testPolymorphicCollection();
    testTransfers();
    testOtherTransactions();
    testCustomerAndLogin();
    std::cout << "Passed: " << passed << ", Failed: " << failed << "\n";
    return failed == 0 ? 0 : 1;
}
