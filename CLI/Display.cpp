#include "Display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>
#include <ctime>
#include <stdexcept>

#ifdef _WIN32
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
#endif

namespace Display {

// ── ANSI colour definitions ───────────────────────────────────────────────────
const std::string RESET   = "\033[0m";
const std::string BOLD    = "\033[1m";
const std::string DIM     = "\033[2m";
const std::string RED     = "\033[31m";
const std::string GREEN   = "\033[32m";
const std::string YELLOW  = "\033[33m";
const std::string BLUE    = "\033[34m";
const std::string MAGENTA = "\033[35m";
const std::string CYAN    = "\033[36m";
const std::string WHITE   = "\033[37m";
const std::string BG_BLUE  = "\033[44m";
const std::string BG_GREEN = "\033[42m";
const std::string BG_RED   = "\033[41m";

// ── Terminal setup ────────────────────────────────────────────────────────────
void enableANSI() {
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE) return;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode)) return;
#ifndef ENABLE_VIRTUAL_TERMINAL_PROCESSING
#define ENABLE_VIRTUAL_TERMINAL_PROCESSING 0x0004
#endif
    SetConsoleMode(hOut, dwMode | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    // UTF-8 output
    SetConsoleOutputCP(CP_UTF8);
#endif
}

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

// ── Banner ────────────────────────────────────────────────────────────────────
void printBanner() {
    std::cout << "\n";
    std::cout << BOLD << BLUE;
    std::cout << "  ╔══════════════════════════════════════════════════════════╗\n";
    std::cout << "  ║                                                          ║\n";
    std::cout << "  ║    ██████   MoneyMap - Personal Finance Tracker  ██████  ║\n";
    std::cout << "  ║                                                          ║\n";
    std::cout << "  ║          Track  |  Budget  |  Report  |  Save            ║\n";
    std::cout << "  ║                                                          ║\n";
    std::cout << "  ╚══════════════════════════════════════════════════════════╝\n";
    std::cout << RESET << "\n";
}

// ── Separators ────────────────────────────────────────────────────────────────
void printSeparator(int width, char ch) {
    std::cout << CYAN << DIM << "  ";
    for (int i = 0; i < width; ++i) std::cout << ch;
    std::cout << RESET << "\n";
}

void printDoubleLine(int width) {
    std::cout << BLUE << BOLD << "  ";
    for (int i = 0; i < width; ++i) std::cout << "═";
    std::cout << RESET << "\n";
}

void printBoxed(const std::string& text, int width) {
    int padding = (width - (int)text.size() - 2) / 2;
    std::cout << BOLD << BLUE << "  ╔";
    for (int i = 0; i < width; ++i) std::cout << "═";
    std::cout << "╗\n  ║";
    for (int i = 0; i < padding; ++i) std::cout << " ";
    std::cout << YELLOW << text << BLUE;
    int rpad = width - padding - (int)text.size();
    for (int i = 0; i < rpad; ++i) std::cout << " ";
    std::cout << "║\n  ╚";
    for (int i = 0; i < width; ++i) std::cout << "═";
    std::cout << "╝\n" << RESET << "\n";
}

// ── Messages ──────────────────────────────────────────────────────────────────
void printSuccess(const std::string& msg) {
    std::cout << "\n  " << GREEN << BOLD << "[OK] " << RESET << GREEN << msg << RESET << "\n\n";
}
void printError(const std::string& msg) {
    std::cout << "\n  " << RED << BOLD << "[ERR] " << RESET << RED << msg << RESET << "\n\n";
}
void printWarning(const std::string& msg) {
    std::cout << "\n  " << YELLOW << BOLD << "[WARN] " << RESET << YELLOW << msg << RESET << "\n\n";
}
void printInfo(const std::string& msg) {
    std::cout << "\n  " << CYAN << BOLD << "[INFO] " << RESET << CYAN << msg << RESET << "\n\n";
}

// ── Progress bar ──────────────────────────────────────────────────────────────
void printProgressBar(double pct, int barWidth) {
    int filled = static_cast<int>((pct / 100.0) * barWidth);
    filled = std::min(filled, barWidth);

    std::string colour = (pct >= 100) ? RED : (pct >= 75 ? YELLOW : GREEN);

    std::cout << "  " << colour << "[";
    for (int i = 0; i < barWidth; ++i)
        std::cout << (i < filled ? "\u2588" : "\u2591");   // █ vs ░
    std::cout << "] " << BOLD << std::fixed << std::setprecision(1) << pct << "%" << RESET;
}

// ── Transaction table ─────────────────────────────────────────────────────────
void printTableHeader() {
    std::cout << BOLD << BLUE;
    std::cout << "  ┌──────┬────────────┬──────────────┬──────────┬────────────┬──────────────────────┐\n";
    std::cout << "  │  ID  │    Date    │   Category   │   Type   │  Amount(Rs)│     Description      │\n";
    std::cout << "  ├──────┼────────────┼──────────────┼──────────┼────────────┼──────────────────────┤\n";
    std::cout << RESET;
}

void printTableRow(const std::string& id,   const std::string& date,
                   const std::string& cat,  const std::string& type,
                   const std::string& amt,  const std::string& desc,
                   bool isIncome) {
    const std::string& typeColor = isIncome ? GREEN : RED;
    std::cout << "  │"
              << std::setw(6)  << std::right << id                              << "│"
              << std::setw(12) << date                                           << "│"
              << std::setw(14) << cat.substr(0, 13)                              << "│"
              << typeColor << std::setw(10) << type << RESET                     << "│"
              << (isIncome ? GREEN : RED)
              << std::setw(12) << amt.substr(0, 11) << RESET                    << "│"
              << std::setw(22) << desc.substr(0, 21)                             << "│\n";
}

void printTableFooter() {
    std::cout << BOLD << BLUE;
    std::cout << "  └──────┴────────────┴──────────────┴──────────┴────────────┴──────────────────────┘\n";
    std::cout << RESET;
}

// ── Input helpers ─────────────────────────────────────────────────────────────
void pressEnter() {
    std::cout << "\n  " << DIM << "Press Enter to continue..." << RESET;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get();
}

std::string getInput(const std::string& prompt) {
    std::cout << "  " << CYAN << prompt << RESET;
    std::string val;
    std::getline(std::cin, val);
    return val;
}

int getInt(const std::string& prompt) {
    while (true) {
        std::string s = getInput(prompt);
        try {
            size_t pos;
            int v = std::stoi(s, &pos);
            if (pos == s.size()) return v;
        } catch (...) {}
        printError("Please enter a valid integer.");
    }
}

double getDouble(const std::string& prompt) {
    while (true) {
        std::string s = getInput(prompt);
        try {
            size_t pos;
            double v = std::stod(s, &pos);
            if (pos == s.size() && v > 0) return v;
        } catch (...) {}
        printError("Please enter a valid positive number.");
    }
}

int getMenuChoice(const std::string& prompt, int mn, int mx) {
    while (true) {
        std::string s = getInput(prompt);
        try {
            int v = std::stoi(s);
            if (v >= mn && v <= mx) return v;
        } catch (...) {}
        printError("Invalid choice. Enter a number between " +
                   std::to_string(mn) + " and " + std::to_string(mx) + ".");
    }
}

// ── Date helpers ──────────────────────────────────────────────────────────────
std::string todayString() {
    std::time_t now = std::time(nullptr);
    std::tm*    t   = std::localtime(&now);
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday);
    return std::string(buf);
}

bool isValidDate(const std::string& d) {
    if (d.size() != 10) return false;
    if (d[4] != '-' || d[7] != '-') return false;
    try {
        int y = std::stoi(d.substr(0, 4));
        int m = std::stoi(d.substr(5, 2));
        int day = std::stoi(d.substr(8, 2));
        return (y >= 2000 && y <= 2100 && m >= 1 && m <= 12 && day >= 1 && day <= 31);
    } catch (...) { return false; }
}

int dateYear(const std::string& d) {
    return std::stoi(d.substr(0, 4));
}

int dateMonth(const std::string& d) {
    return std::stoi(d.substr(5, 2));
}

const std::string MONTHS[] = {"", "January","February","March","April","May","June",
                                   "July","August","September","October","November","December"};
std::string monthName(int m) {
    return (m >= 1 && m <= 12) ? MONTHS[m] : "Unknown";
}

} // namespace Display
