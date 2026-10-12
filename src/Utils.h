#pragma once
#include <string>

// Formats a number like Indonesian Rupiah, e.g. 1500000 -> "Rp 1.500.000"
std::string formatRupiah(double amount);

// Current local time as "YYYY-MM-DD HH:MM:SS"
std::string currentTime();
