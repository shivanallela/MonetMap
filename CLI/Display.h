#pragma once
#include <string>
#include <vector>
#include <iostream>

/**
 * @brief Console display utilities – ANSI colors, boxes, tables, progress bars.
 *        Works on Windows 10+ (ANSI VT enabled) and any Linux/macOS terminal.
 */
namespace Display {

    // ── ANSI colour codes ────────────────────────────────────────────────────
    extern const std::string RESET;
    extern const std::string BOLD;
    extern const std::string DIM;
    extern const std::string RED;
    extern const std::string GREEN;
    extern const std::string YELLOW;
    extern const std::string BLUE;
    extern const std::string MAGENTA;
    extern const std::string CYAN;
    extern const std::string WHITE;
    extern const std::string BG_BLUE;
    extern const std::string BG_GREEN;
    extern const std::string BG_RED;

    // ── Terminal setup ───────────────────────────────────────────────────────
    void enableANSI();          // Enable ANSI on Windows 10+
    void clearScreen();

    // ── Decorative elements ──────────────────────────────────────────────────
    void printBanner();         // MoneyMap ASCII art banner
    void printSeparator(int width = 62, char ch = '-');
    void printDoubleLine(int width = 62);
    void printBoxed(const std::string& text, int width = 62);

    // ── Coloured messages ────────────────────────────────────────────────────
    void printSuccess(const std::string& msg);
    void printError(const std::string& msg);
    void printWarning(const std::string& msg);
    void printInfo(const std::string& msg);

    // ── Progress bar ─────────────────────────────────────────────────────────
    void printProgressBar(double pct, int barWidth = 25);

    // ── Table helpers ────────────────────────────────────────────────────────
    void printTableHeader();
    void printTableRow(const std::string& id,    const std::string& date,
                       const std::string& cat,   const std::string& type,
                       const std::string& amt,   const std::string& desc,
                       bool isIncome);
    void printTableFooter();

    // ── Input helpers ────────────────────────────────────────────────────────
    void        pressEnter();
    std::string getInput(const std::string& prompt);
    int         getInt(const std::string& prompt);
    double      getDouble(const std::string& prompt);
    int         getMenuChoice(const std::string& prompt, int min, int max);

    // ── Date helpers ─────────────────────────────────────────────────────────
    std::string todayString();          // returns "YYYY-MM-DD"
    bool        isValidDate(const std::string& d);
    int         dateYear(const std::string& d);
    int         dateMonth(const std::string& d);
    std::string monthName(int m);
}
