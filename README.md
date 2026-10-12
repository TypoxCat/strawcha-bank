# M-Banking System (C++17)

OOP Pillars assignment: a small console m-banking system.

## Build and run (console)
```
make          # builds ./mbanking
./mbanking    # sample logins: C001 / 123456 (Dina), C002 / 654321 (Nawa)
make test     # builds and runs tests/test_main.cpp (49 checks)
make clean
```
Needs g++ (or any compiler) with C++17.

## Web GUI (React + Vite) + REST API

The C++ `Bank` facade is exposed as a small JSON REST API (`server/`),
and a React app under `web/` provides the UI.

```
make mbanking-server     # builds ./mbanking-server (C++ API)
./mbanking-server        # serves on http://localhost:8080

cd web
npm install
npm run dev              # http://localhost:5173 (proxies /api -> :8080)
npm run build            # production build into web/dist
```
Demo logins: `C001 / 123456` (Dina), `C002 / 654321` (Nawa).
Staff console passcode: `staff123`.

State is kept in memory (same as the console app), so restarting the
server resets all customers and transactions.

### API endpoints
| Method | Path | Notes |
|---|---|---|
| POST | /api/auth/register | `{name, pin}` |
| POST | `/api/auth/login` | `{customerId, pin}` -> `{token, customer}` |
| POST | `/api/auth/logout` | Bearer token |
| POST | `/api/auth/pin` | `{oldPin, newPin}` |
| GET  | `/api/me` | profile + accounts + total balance |
| GET/POST | `/api/accounts` | list / open new account |
| GET | `/api/accounts/:no` | detail + mutation history |
| POST | `/api/accounts/:number/deposit` / `/withdraw` | `{amount, note}` |
| POST | `/api/transfer` `/api/topup` / `/api/paybill` | returns transaction + receipt |
| GET | `/api/transactions`, `/api/transactions/:id` | own transactions |
| POST | `/api/staff/month-end` | header `X-Staff-Key` |
| GET | `/api/staff/log` | header `X-Staff-Key` |
| GET | `/api/health` | health check |

## Files (src/)
| File | Role |
|---|---|
| Account.h/.cpp | Abstract base class for accounts (encapsulates balance and history) |
| SavingsAccount.h/.cpp | Account with minimum balance and monthly interest |
| CheckingAccount.h/.cpp | Account with overdraft and monthly fee |
| Transaction.h/.cpp | Abstract base class for transactions |
| TransferTransaction.h/.cpp | Transfer between accounts |
| EWalletTopUpTransaction.h/.cpp | E-wallet top up |
| BillPaymentTransaction.h/.cpp | Bill payment |
| Customer.h/.cpp | Customer, PIN protection and owned accounts |
| Bank.h/.cpp | Facade: customers, accounts, transactions, month end |
| Exceptions.h | BankException hierarchy |
| Utils.h/.cpp | Rupiah formatting, timestamp |
| main.cpp | Console menu and demo mode |
