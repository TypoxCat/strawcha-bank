#include "Utils.h"
#include <cmath>
#include <ctime>

std::string formatRupiah(double amount) {
    long long value = std::llround(amount);
    bool negative = value < 0;
    if (negative) value = -value;

    std::string digits = std::to_string(value);
    std::string result;
    int count = 0;
    for (int i = static_cast<int>(digits.size()) - 1; i >= 0; --i) {
        result.insert(result.begin(), digits[i]);
        ++count;
        if (count % 3 == 0 && i != 0) {
            result.insert(result.begin(), '.');
        }
    }
    return std::string(negative ? "-" : "") + "Rp " + result;
}

std::string currentTime() {
    std::time_t now = std::time(nullptr);
    char buffer[32];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));
    return std::string(buffer);
}
