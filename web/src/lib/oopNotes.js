// Catatan pilar OOP yang tampil sebagai "chat bubble" di sisi kiri/kanan
// frame mobile pada layar lebar. Satu entry per halaman.
//
// Semua potongan kode diambil apa adanya dari folder src/ (+ file asalnya)
// supaya pembaca bisa membayangkan bagaimana tiap pilar bekerja.
//
// 4 pilar:
//   - Encapsulation : data disembunyikan, diubah hanya lewat method
//   - Abstraction   : pemakai cukup panggil method, detailnya disembunyikan
//   - Inheritance   : class turunan mewarisi class dasar (IS-A)
//   - Polymorphism  : satu perintah, banyak perilaku (virtual / override / overload)

export const PILLARS = {
  '/login': {
    left: [
      {
        pillar: 'Encapsulation', tone: 'berry',
        text: 'PIN tidak disimpan sebagai teks asli, hanya hash-nya. Tidak ada getter PIN — dunia luar hanya boleh memanggil verifyPin(). Salah 3x akun terkunci.',
        file: 'src/Customer.cpp',
        code: `private:
    std::size_t pinHash_;   // bukan teks asli
    int failedAttempts_;
    bool locked_;

bool Customer::verifyPin(const std::string& pin) {
    if (hashPin(pin) == pinHash_) { failedAttempts_ = 0; return true; }
    if (++failedAttempts_ >= MAX_FAILED_ATTEMPTS) locked_ = true;
    return false;
}`,
      },
    ],
    right: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Halaman login cuma butuh jawaban ya/tidak. Mencari customer dan memverifikasi PIN disembunyikan di dalam Bank.',
        file: 'src/Bank.h',
        code: `// UI cukup memanggil:
Customer* Bank::login(const std::string& id,
                      const std::string& pin);
// Bank yang mengurus cari customer + verifikasi PIN.`,
      },
    ],
  },

  '/register': {
    left: [
      {
        pillar: 'Encapsulation', tone: 'berry',
        text: 'PIN divalidasi formatnya lalu di-hash sebelum disimpan. Yang tersimpan hanya hash, bukan PIN-nya.',
        file: 'src/Customer.cpp',
        code: `checkPinFormat(pin);          // wajib 6 digit angka
pinHash_ = hashPin(pin);      // simpan hash-nya saja`,
      },
    ],
    right: [
      {
        pillar: 'Inheritance', tone: 'matcha',
        text: 'Nasabah "memiliki" rekening. Account adalah class dasar yang diturunkan menjadi Savings/Checking, disimpan lewat pointer agar bisa polimorfik.',
        file: 'src/Customer.h',
        code: `std::vector<std::unique_ptr<Account>> accounts_;
Account* addAccount(std::unique_ptr<Account> a);`,
      },
    ],
  },
  '/dashboard': {
    left: [
      {
        pillar: 'Encapsulation', tone: 'berry',
        text: 'Saldo dan riwayat bersifat private. Beranda hanya bisa MEMBACA lewat getter, tidak bisa mengubah langsung.',
        file: 'src/Account.h',
        code: `private:
    double balance_;                     // tidak bisa diubah dari luar
    std::vector<std::string> history_;
public:
    double getBalance() const { return balance_; }`,
      },
    ],
    right: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Beranda cuma menampilkan satu angka total. Kerumitan menjumlahkan semua rekening disembunyikan di dalam Customer.',
        file: 'src/Customer.cpp',
        code: `double Customer::totalBalance() const {
    double total = 0;
    for (const auto& acc : accounts_) total += acc->getBalance();
    return total;   // halaman tinggal menampilkan angkanya
}`,
      },
    ],
  },
  '/accounts': {
    left: [
      {
        pillar: 'Inheritance', tone: 'matcha',
        text: 'Savings dan Checking adalah turunan Account. Keduanya mewarisi deposit/withdraw, tapi punya aturan sendiri untuk saldo & bunga/biaya.',
        file: 'src/SavingsAccount.h',
        code: `class SavingsAccount : public Account {
    double availableFunds() const override;   // saldo - MIN_BALANCE
    void   monthlyProcess()  override;        // tambah bunga 3%/tahun
    std::string typeName()   const override;  // "Savings"
};
class CheckingAccount : public Account {
    // limit overdraft 500rb, biaya admin bulanan
};`,
      },
    ],
    right: [
      {
        pillar: 'Static Polymorphism', tone: 'butter',
        text: 'Nama fungsi sama, parameter beda (overloading). withdraw() memanggil availableFunds() yang virtual, jadi batas saldo otomatis beda per tipe akun.',
        file: 'src/Account.cpp',
        code: `void Account::deposit(double amount);                            // + "Deposit"
void Account::deposit(double amount, const std::string& note);   // overload

void Account::withdraw(double amount) {
    if (amount > availableFunds() + EPSILON)   // virtual -> beda tiap tipe
        throw InsufficientFundsException(...);
    debit(amount, note);
}`,
      },
    ],
  },
  '/transfer': {
    left: [
      {
        pillar: 'Polymorphism', tone: 'berry',
        text: 'Method yang punya BANYAK implementasi: virtual di class dasar, di-override di setiap turunan. Inilah bukti polymorphism di proyek ini.',
        file: 'src/Transaction.h (base)',
        code: `virtual std::string typeName() const = 0;   // Transfer / E-Wallet Top Up / Bill Payment
virtual std::string describe() const = 0;   // kalimat detail beda tiap tipe
virtual double      fee()      const = 0;   // 0 / 1.000 / 2.500
virtual void        execute()  = 0;         // langkah eksekusi beda

// Account juga polimorfik:
virtual double availableFunds() const = 0;  // rumus limit berbeda
virtual void   monthlyProcess() = 0;        // bunga vs biaya admin
virtual std::string summary() const;`,
      },
      {
        pillar: 'Polymorphism', tone: 'berry',
        text: 'Objek dipanggil lewat pointer Transaction*, tetapi method yang jalan adalah versi Transfer — dipilih saat runtime.',
        file: 'src/TransferTransaction.cpp',
        code: `std::string typeName() const override { return "Transfer"; }

double fee() const override {
    // gratis jika sesama pemilik, kalau beda pemilik kena biaya
    return source_->getOwnerName() == destination_->getOwnerName()
         ? 0 : OTHER_OWNER_FEE;
}

void execute() override {
    if (amount_ + fee() > source_->availableFunds())
        throw InsufficientFundsException(...);
    source_->withdraw(amount_, "Transfer to " + destination_->getAccountNumber());
    destination_->deposit(amount_, "Transfer from " + source_->getAccountNumber());
}`,
      },
    ],
    right: [
      {
        pillar: 'Inheritance', tone: 'matcha',
        text: 'TransferTransaction IS-A Transaction, jadi dapat gratis: id, status, timestamp, dan receipt(). Ia hanya menambahkan tujuan + aturan sendiri.',
        file: 'src/TransferTransaction.h',
        code: `class TransferTransaction : public Transaction {
    std::string typeName() const override;
    std::string describe() const override;
    double      fee()      const override;
protected:
    void execute() override;   // detail langkah transfer
};`,
      },
    ],
  },
  '/payments': {
    left: [
      {
        pillar: 'Polymorphism', tone: 'berry',
        text: 'Setiap jenis transaksi punya describe() dan fee() sendiri. Satu perintah, banyak perilaku — inti polymorphism.',
        file: 'src/EWalletTopUpTransaction.cpp / BillPaymentTransaction.cpp',
        code: `// Top up e-wallet
std::string describe() const override { return provider_ + " " + phoneNumber_; }
double fee() const override { return TOPUP_FEE; }     // 1.000

// Bayar tagihan
std::string describe() const override {
    return billType_ + " tagihan, no. " + customerNumber_;
}
double fee() const override { return ADMIN_FEE; }     // 2.500`,
      },
    ],
    right: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Bank tidak peduli jenis transaksinya. Cukup panggil process() satu kali, versi execute() yang tepat dipilih otomatis.',
        file: 'src/Bank.cpp',
        code: `const Transaction& Bank::run(std::unique_ptr<Transaction> tx) {
    Transaction* raw = tx.get();
    log_.push_back(std::move(tx));
    raw->process();     // -> execute() versi subclass-nya
    return *raw;
}`,
      },
    ],
  },
  '/history': {
    left: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Riwayat diperlakukan sebagai Transaction biasa. receipt() memanggil typeName(), describe(), fee() yang virtual — pemanggil tak perlu tahu tipe aslinya.',
        file: 'src/Transaction.cpp',
        code: `std::string Transaction::receipt() const {
    out << "Type    : " << typeName();   // virtual -> sesuai tipe
    out << "Details : " << describe();   // virtual -> sesuai tipe
    out << "Fee     : " << fee();        // virtual -> sesuai tipe
}`,
      },
    ],
    right: [
      {
        pillar: 'Polymorphism', tone: 'berry',
        text: 'Satu loop mencetak semua tipe transaksi. Perilaku beda muncul dari override, bukan dari if/switch tipe.',
        file: 'src/Bank.cpp',
        code: `for (auto& tx : log_)
    print(tx->typeName(), tx->describe(), tx->fee());
    // satu loop, hasil berbeda untuk tiap jenis transaksi`,
      },
    ],
  },
  '/akun': {
    left: [
      {
        pillar: 'Encapsulation', tone: 'berry',
        text: 'Ganti PIN wajib menyertakan PIN lama. Hanya lewat method ini PIN bisa berubah dari luar.',
        file: 'src/Customer.cpp',
        code: `void Customer::changePin(const std::string& oldPin,
                        const std::string& newPin) {
    if (!verifyPin(oldPin)) throw AuthenticationException(...);
    checkPinFormat(newPin);
    pinHash_ = hashPin(newPin);
}`,
      },
    ],
    right: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Profil dibangun dari getter publik. Tidak ada cara mengintip field mentah seperti pinHash_ dari luar class.',
        file: 'src/Customer.h',
        code: `const std::string& getId() const;
const std::string& getName() const;
// tidak ada getter untuk pinHash_`,
      },
    ],
  },
  '/staff': {
    left: [
      {
        pillar: 'Abstraction', tone: 'matcha',
        text: 'Staff cukup memanggil satu fungsi month-end. Bank yang mengurus semuanya tanpa perlu tahu tipe tiap akun.',
        file: 'src/Bank.cpp',
        code: `void Bank::runMonthEnd() {
    for (Account* acc : allAccounts())
        acc->monthlyProcess();   // Bank tak peduli Savings / Checking
}`,
      },
    ],
    right: [
      {
        pillar: 'Polymorphism', tone: 'berry',
        text: 'monthlyProcess() yang sama menghasilkan aksi berbeda: tabungan dapat bunga, giro kena biaya admin.',
        file: 'src/SavingsAccount.cpp / CheckingAccount.cpp',
        code: `// Savings
void SavingsAccount::monthlyProcess() {
    credit(getBalance() * ANNUAL_INTEREST_RATE / 12.0, "Monthly interest");
}

// Checking
void CheckingAccount::monthlyProcess() {
    debit(MONTHLY_FEE, "Monthly admin fee");
}`,
      },
    ],
  },
}

export default PILLARS
