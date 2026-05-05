/**
 * MoneyMap – Personal Finance Tracker
 * C++17 Command-Line Application
 * Author: Student Project | Compiled with g++ -std=c++17
 */

#include "FinanceManager.h"
#include "Display.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <ctime>
#include <algorithm>

using namespace Display;
using namespace std;

// ─────────────────────────────────────────────────────────────────────────────
// App class – wraps all menu logic
// ─────────────────────────────────────────────────────────────────────────────
class App {
public:
    App() {
        enableANSI();
        loadData();
    }

    ~App() { saveData(); }

    void run() {
        clearScreen();
        printBanner();
        printInfo("Data loaded. Welcome to MoneyMap!");
        pressEnter();
        mainMenu();
    }

private:
    FinanceManager fm;

    static const char* F_TX;
    static const char* F_BU;
    static const char* F_CA;

    // ── Persistence ──────────────────────────────────────────────────────────
    void loadData() {
        fm.loadCategories(F_CA);
        fm.loadTransactions(F_TX);
        fm.loadBudgets(F_BU);
    }

    void saveData() {
        fm.saveCategories(F_CA);
        fm.saveTransactions(F_TX);
        fm.saveBudgets(F_BU);
    }

    // ── Main Menu ─────────────────────────────────────────────────────────────
    void mainMenu() {
        while (true) {
            clearScreen();
            printBanner();

            // Quick today stats
            time_t now = time(nullptr);
            tm* lt = localtime(&now);
            int y = lt->tm_year + 1900, m = lt->tm_mon + 1;
            fm.recalculateBudgets(y, m);

            double inc = fm.getTotalIncome(y, m);
            double exp = fm.getTotalExpenses(y, m);
            double sav = fm.getSavings(y, m);

            cout << BOLD << "  Today: " << todayString()
                 << "    |   " << monthName(m) << " " << y << "\n" << RESET;
            cout << "  " << GREEN << "Income: Rs " << fixed << setprecision(2) << inc << RESET
                 << "   " << RED << "Expense: Rs " << exp << RESET
                 << "   " << CYAN << "Savings: Rs " << sav << RESET << "\n\n";

            printDoubleLine();
            cout << BOLD << YELLOW << "  MAIN MENU\n" << RESET;
            printSeparator();

            cout << BOLD << "\n"
                 << "  [1]  Transaction Management\n"
                 << "  [2]  Category Management\n"
                 << "  [3]  Budget Management\n"
                 << "  [4]  Monthly Report\n"
                 << "  [5]  Search & Filter\n"
                 << "  [6]  Save Data\n"
                 << "  [0]  Exit\n\n" << RESET;

            printSeparator();
            int ch = getMenuChoice("  > Enter choice: ", 0, 6);

            switch (ch) {
                case 1: transactionMenu();   break;
                case 2: categoryMenu();      break;
                case 3: budgetMenu();        break;
                case 4: reportMenu();        break;
                case 5: searchMenu();        break;
                case 6:
                    saveData();
                    printSuccess("Data saved successfully!");
                    pressEnter();
                    break;
                case 0:
                    saveData();
                    clearScreen();
                    cout << BOLD << GREEN
                         << "\n  Thank you for using MoneyMap! Goodbye.\n\n" << RESET;
                    return;
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // TRANSACTION MENU
    // ─────────────────────────────────────────────────────────────────────────
    void transactionMenu() {
        while (true) {
            clearScreen();
            printBoxed("  TRANSACTION MANAGEMENT");

            cout << BOLD
                 << "  [1]  Add Income\n"
                 << "  [2]  Add Expense\n"
                 << "  [3]  View All Transactions\n"
                 << "  [4]  Delete a Transaction\n"
                 << "  [0]  Back to Main Menu\n\n" << RESET;

            printSeparator();
            int ch = getMenuChoice("  > Enter choice: ", 0, 4);

            switch (ch) {
                case 1: addTransaction(Transaction::Type::INCOME);  break;
                case 2: addTransaction(Transaction::Type::EXPENSE); break;
                case 3: viewTransactions(fm.getAllTransactions(), "All Transactions"); break;
                case 4: deleteTransaction(); break;
                case 0: return;
            }
        }
    }

    void addTransaction(Transaction::Type type) {
        clearScreen();
        bool isIncome = (type == Transaction::Type::INCOME);
        string typeStr = isIncome ? "INCOME" : "EXPENSE";
        printBoxed("  ADD " + typeStr);

        // Amount
        double amount = getDouble("  Amount (Rs): ");

        // Show correct category list based on type
        auto cats = isIncome ? fm.getIncomeCategories() : fm.getExpenseCategories();

        cout << "\n  " << BOLD;
        if (isIncome)
            cout << GREEN << "Income Categories (source of money):" << RESET << "\n";
        else
            cout << RED << "Expense Categories (spending):" << RESET << "\n";

        for (size_t i = 0; i < cats.size(); ++i)
            cout << "  [" << (i+1) << "] " << cats[i] << "\n";
        cout << "\n";

        int catIdx = getMenuChoice("  > Select category (1-" + to_string(cats.size()) + "): ",
                                   1, (int)cats.size());
        string category = cats[catIdx - 1];

        // Description
        string desc = getInput("  Description (optional, press Enter to skip): ");
        if (desc.empty()) desc = "-";

        // Date – quick choice: today or another day
        cout << "\n  " << BOLD << "Date of Transaction:\n" << RESET;
        cout << "  [1]  Today (" << todayString() << ")\n";
        cout << "  [2]  Another date\n";
        int dateCh = getMenuChoice("  > ", 1, 2);

        string date;
        if (dateCh == 1) {
            date = todayString();
            cout << "  " << GREEN << "Date set to: " << date << RESET << "\n";
        } else {
            cout << "  Enter date [YYYY-MM-DD]: ";
            getline(cin, date);
            while (!isValidDate(date)) {
                printError("Invalid date. Use format YYYY-MM-DD (e.g. 2025-05-01).");
                cout << "  Enter date [YYYY-MM-DD]: ";
                getline(cin, date);
            }
        }

        // Create and add
        Transaction t(fm.getNextId(), amount, category, desc, date, type);
        fm.addTransaction(t);
        saveData();

        // Budget check for expenses
        if (!isIncome) {
            fm.recalculateBudgets(dateYear(date), dateMonth(date));
            Budget b = fm.getBudget(category);
            if (b.getLimit() > 0 && b.isExceeded()) {
                cout << "\n";
                cout << BG_RED << BOLD << WHITE
                     << "  !!!  BUDGET EXCEEDED for " << category
                     << "  !!!" << RESET << "\n";
                ostringstream spentStr, limStr;
                spentStr << fixed << setprecision(2) << b.getSpent();
                limStr   << fixed << setprecision(2) << b.getLimit();
                printWarning("Spent: Rs " + spentStr.str() +
                             " / Limit: Rs " + limStr.str());
            }
        }

        ostringstream amtStr;
        amtStr << fixed << setprecision(2) << amount;
        printSuccess(typeStr + " of Rs " + amtStr.str() +
                     " added under '" + category + "'.");
        pressEnter();
    }

    void viewTransactions(const vector<Transaction>& txns, const string& title) {
        clearScreen();
        printBoxed("  " + title + " (" + to_string(txns.size()) + " records)");

        if (txns.empty()) {
            printInfo("No transactions found.");
            pressEnter();
            return;
        }

        printTableHeader();
        for (const auto& t : txns) {
            ostringstream amtStr;
            amtStr << fixed << setprecision(2) << t.getAmount();
            printTableRow(to_string(t.getId()), t.getDate(),
                          t.getCategory(), t.getTypeStr(),
                          amtStr.str(), t.getDescription(),
                          t.getType() == Transaction::Type::INCOME);
        }
        printTableFooter();

        // Summary below
        double totInc = 0, totExp = 0;
        for (const auto& t : txns) {
            if (t.getType() == Transaction::Type::INCOME) totInc += t.getAmount();
            else                                          totExp += t.getAmount();
        }
        cout << "\n  " << GREEN << "Total Income : Rs " << fixed << setprecision(2) << totInc << RESET << "\n";
        cout <<   "  " << RED   << "Total Expense: Rs " << totExp << RESET << "\n";
        cout <<   "  " << CYAN  << "Net Balance  : Rs " << (totInc - totExp) << RESET << "\n\n";
        pressEnter();
    }

    void deleteTransaction() {
        clearScreen();
        printBoxed("  DELETE TRANSACTION");
        viewTransactions(fm.getAllTransactions(), "All Transactions");

        if (fm.getAllTransactions().empty()) return;

        int id = getInt("  Enter Transaction ID to delete (0 = cancel): ");
        if (id == 0) return;

        if (fm.removeTransaction(id)) {
            saveData();
            printSuccess("Transaction #" + to_string(id) + " deleted.");
        } else {
            printError("Transaction ID " + to_string(id) + " not found.");
        }
        pressEnter();
    }

    // ─────────────────────────────────────────────────────────────────────────
    // CATEGORY MENU
    // ─────────────────────────────────────────────────────────────────────────
    void categoryMenu() {
        while (true) {
            clearScreen();
            printBoxed("  CATEGORY MANAGEMENT");

            // Show income categories
            auto incCats = fm.getIncomeCategories();
            cout << BOLD << GREEN << "  Income Categories:" << RESET << "\n";
            printSeparator(40);
            for (size_t i = 0; i < incCats.size(); ++i)
                cout << "  " << GREEN << "[" << (i+1) << "]" << RESET
                     << " " << incCats[i] << "\n";
            cout << "\n";

            // Show expense categories
            auto expCats = fm.getExpenseCategories();
            cout << BOLD << RED << "  Expense Categories:" << RESET << "\n";
            printSeparator(40);
            for (size_t i = 0; i < expCats.size(); ++i)
                cout << "  " << RED << "[" << (i+1) << "]" << RESET
                     << " " << expCats[i] << "\n";
            cout << "\n";

            printSeparator(40);
            cout << BOLD
                 << "  [1]  Add Income Category\n"
                 << "  [2]  Add Expense Category\n"
                 << "  [0]  Back\n\n" << RESET;

            int ch = getMenuChoice("  > Enter choice: ", 0, 2);
            if (ch == 0) return;

            string name = getInput(ch == 1 ? "  New income category name: "
                                           : "  New expense category name: ");
            if (name.empty()) { printError("Name cannot be empty."); pressEnter(); continue; }

            if (ch == 1) {
                if (fm.incomeCategoryExists(name)) {
                    printWarning("Income category '" + name + "' already exists.");
                } else {
                    fm.addIncomeCategory(name);
                    saveData();
                    printSuccess("Income category '" + name + "' added!");
                }
            } else {
                if (fm.expenseCategoryExists(name)) {
                    printWarning("Expense category '" + name + "' already exists.");
                } else {
                    fm.addExpenseCategory(name);
                    saveData();
                    printSuccess("Expense category '" + name + "' added!");
                }
            }
            pressEnter();
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // BUDGET MENU
    // ─────────────────────────────────────────────────────────────────────────
    void budgetMenu() {
        while (true) {
            clearScreen();
            printBoxed("  BUDGET MANAGEMENT");

            // Recalculate for current month
            time_t now = time(nullptr);
            tm* lt = localtime(&now);
            int y = lt->tm_year + 1900, m = lt->tm_mon + 1;
            fm.recalculateBudgets(y, m);

            cout << BOLD << "  Budgets for " << monthName(m) << " " << y << ":\n" << RESET;
            printSeparator(55);

            auto budgets = fm.getBudgets();
            if (budgets.empty()) {
                printInfo("No budgets set yet.");
            } else {
                for (auto& p : budgets) {
                    const Budget& b = p.second;
                    ostringstream info;
                    info << fixed << setprecision(2)
                         << "  Rs " << b.getSpent() << " / Rs " << b.getLimit();

                    cout << "\n  " << BOLD << YELLOW << b.getCategory() << RESET
                         << "  " << (b.isExceeded() ? RED + "EXCEEDED!" : "") << RESET << "\n";
                    cout << info.str() << "  (Remaining: Rs " << b.getRemaining() << ")\n";
                    printProgressBar(b.getUsagePct());
                    cout << "\n";
                }
            }

            printSeparator(55);
            cout << "\n" << BOLD
                 << "  [1]  Set / Update Budget for a Category\n"
                 << "  [0]  Back\n\n" << RESET;

            int ch = getMenuChoice("  > Enter choice: ", 0, 1);
            if (ch == 0) return;

            // Select category
            auto cats = fm.getCategories();
            cout << "\n  " << BOLD << "Select Category:\n" << RESET;
            for (size_t i = 0; i < cats.size(); ++i)
                cout << "  [" << (i+1) << "] " << cats[i] << "\n";
            int idx = getMenuChoice("  > Category: ", 1, (int)cats.size());

            double limit = getDouble("  Monthly limit (Rs): ");
            fm.setBudget(cats[idx - 1], limit);
            saveData();
            printSuccess("Budget set for '" + cats[idx - 1] +
                         "': Rs " + [&](){ ostringstream os;
                             os << fixed << setprecision(2) << limit; return os.str(); }());
            pressEnter();
        }
    }

    // ─────────────────────────────────────────────────────────────────────────
    // MONTHLY REPORT
    // ─────────────────────────────────────────────────────────────────────────
    void reportMenu() {
        clearScreen();
        printBoxed("  MONTHLY REPORT");

        // Ask month / year
        cout << "  Enter year  (e.g. 2025, press Enter for current): ";
        string yStr; getline(cin, yStr);
        cout << "  Enter month (1-12, press Enter for current):      ";
        string mStr; getline(cin, mStr);

        time_t now = time(nullptr);
        tm* lt = localtime(&now);
        int y = yStr.empty() ? lt->tm_year + 1900 : stoi(yStr);
        int m = mStr.empty() ? lt->tm_mon + 1      : stoi(mStr);

        if (m < 1 || m > 12) { printError("Invalid month."); pressEnter(); return; }

        fm.recalculateBudgets(y, m);
        double inc = fm.getTotalIncome(y, m);
        double exp = fm.getTotalExpenses(y, m);
        double sav = fm.getSavings(y, m);
        double pct = fm.getSavingsPct(y, m);

        clearScreen();
        printBoxed("  REPORT: " + monthName(m) + " " + to_string(y));

        // ── Summary section ───────────────────────────────────────────────
        printDoubleLine(55);
        cout << BOLD << "  SUMMARY\n" << RESET;
        printSeparator(55);

        auto printStat = [&](const string& label, double val, const string& color){
            cout << "  " << BOLD << setw(22) << left << label << RESET
                 << color << "Rs " << fixed << setprecision(2) << val << RESET << "\n";
        };

        printStat("Total Income:",    inc, GREEN);
        printStat("Total Expenses:",  exp, RED);
        printStat("Savings:",         sav, sav >= 0 ? CYAN : RED);
        cout << "  " << BOLD << setw(22) << left << "Savings %:" << RESET
             << MAGENTA << fixed << setprecision(1) << pct << " %" << RESET << "\n";

        // Savings progress bar
        cout << "\n  Savings Rate: ";
        printProgressBar(std::max(0.0, pct), 30);
        cout << "\n\n";

        // ── Category breakdown ─────────────────────────────────────────────
        printDoubleLine(55);
        cout << BOLD << "  CATEGORY-WISE BUDGET USAGE\n" << RESET;
        printSeparator(55);

        auto budgets = fm.getBudgets();
        if (budgets.empty()) {
            printInfo("No budgets configured.");
        } else {
            cout << "\n";
            for (auto& p : budgets) {
                const Budget& b = p.second;
                if (b.getLimit() <= 0) continue;

                cout << "  " << BOLD << YELLOW << setw(16) << left
                     << b.getCategory() << RESET;
                printProgressBar(b.getUsagePct(), 20);
                cout << "  Rs " << fixed << setprecision(0)
                     << b.getSpent() << "/" << b.getLimit();
                if (b.isExceeded())
                    cout << "  " << RED << BOLD << "OVER BUDGET!" << RESET;
                cout << "\n";
            }
        }

        // ── Category expense breakdown ─────────────────────────────────────
        cout << "\n";
        printDoubleLine(55);
        cout << BOLD << "  EXPENSE BY CATEGORY\n" << RESET;
        printSeparator(55);

        auto txns = fm.getByMonth(y, m);
        map<string, double> catTotals;
        for (const auto& t : txns)
            if (t.getType() == Transaction::Type::EXPENSE)
                catTotals[t.getCategory()] += t.getAmount();

        if (catTotals.empty()) {
            printInfo("No expenses this month.");
        } else {
            cout << "\n";
            for (auto& p : catTotals) {
                double pctOfExp = (exp > 0) ? (p.second / exp) * 100.0 : 0;
                cout << "  " << BOLD << setw(16) << left << p.first << RESET
                     << RED << "Rs " << fixed << setprecision(2) << p.second << RESET
                     << "  (" << setprecision(1) << pctOfExp << "% of expenses)\n";
            }
        }

        // ── Tip of the month ───────────────────────────────────────────────
        static const vector<string> TIPS = {
            "Follow the 50/30/20 rule: 50% needs, 30% wants, 20% savings.",
            "Track every small expense – Rs100 daily = Rs3,000/month.",
            "Pay yourself first: transfer savings before spending.",
            "Build an emergency fund covering 3-6 months of expenses.",
            "Avoid lifestyle inflation as your income grows.",
            "Review subscriptions monthly – cancel what you don't use.",
            "Invest early – compounding works best over long term.",
            "Set SMART goals: Specific, Measurable, Achievable, Relevant, Time-bound.",
        };

        cout << "\n";
        printDoubleLine(55);
        cout << "\n  " << BOLD << YELLOW << "TIP OF THE MONTH: " << RESET << GREEN
             << TIPS[(m - 1) % TIPS.size()] << "\n" << RESET;

        cout << "\n";
        pressEnter();
    }

    // ─────────────────────────────────────────────────────────────────────────
    // SEARCH & FILTER
    // ─────────────────────────────────────────────────────────────────────────
    void searchMenu() {
        while (true) {
            clearScreen();
            printBoxed("  SEARCH & FILTER");

            cout << BOLD
                 << "  [1]  Filter by Category\n"
                 << "  [2]  Filter by Date Range\n"
                 << "  [3]  Filter by Month\n"
                 << "  [4]  Filter by Type (Income/Expense)\n"
                 << "  [0]  Back\n\n" << RESET;

            printSeparator();
            int ch = getMenuChoice("  > Enter choice: ", 0, 4);

            switch (ch) {
                case 0: return;
                case 1: filterByCategory();  break;
                case 2: filterByDateRange(); break;
                case 3: filterByMonth();     break;
                case 4: filterByType();      break;
            }
        }
    }

    void filterByCategory() {
        clearScreen();
        printBoxed("  FILTER BY CATEGORY");

        auto cats = fm.getCategories();
        cout << BOLD << "  Select Category:\n" << RESET;
        for (size_t i = 0; i < cats.size(); ++i)
            cout << "  [" << (i+1) << "] " << cats[i] << "\n";
        cout << "\n";

        int idx = getMenuChoice("  > Category: ", 1, (int)cats.size());
        string cat = cats[idx - 1];
        auto txns = fm.getByCategory(cat);
        viewTransactions(txns, "Category: " + cat);
    }

    void filterByDateRange() {
        clearScreen();
        printBoxed("  FILTER BY DATE RANGE");

        string from, to;
        do {
            from = getInput("  From date [YYYY-MM-DD]: ");
        } while (!isValidDate(from) && (printError("Invalid date."), true));

        do {
            to = getInput("  To date   [YYYY-MM-DD]: ");
        } while (!isValidDate(to) && (printError("Invalid date."), true));

        auto txns = fm.getByDateRange(from, to);
        viewTransactions(txns, "Date Range: " + from + " to " + to);
    }

    void filterByMonth() {
        clearScreen();
        printBoxed("  FILTER BY MONTH");

        int y = getInt("  Year  (e.g. 2025): ");
        int m = getMenuChoice("  Month (1-12): ", 1, 12);

        auto txns = fm.getByMonth(y, m);
        viewTransactions(txns, monthName(m) + " " + to_string(y));
    }

    void filterByType() {
        clearScreen();
        printBoxed("  FILTER BY TYPE");

        cout << BOLD
             << "  [1]  Income\n"
             << "  [2]  Expense\n\n" << RESET;
        int ch = getMenuChoice("  > ", 1, 2);

        Transaction::Type filterType = (ch == 1) ? Transaction::Type::INCOME
                                                  : Transaction::Type::EXPENSE;
        vector<Transaction> result;
        for (const auto& t : fm.getAllTransactions())
            if (t.getType() == filterType) result.push_back(t);

        viewTransactions(result, ch == 1 ? "All Income" : "All Expenses");
    }
};

// ─────────────────────────────────────────────────────────────────────────────
const char* App::F_TX = "transactions.csv";
const char* App::F_BU = "budgets.csv";
const char* App::F_CA = "categories.csv";

// ─────────────────────────────────────────────────────────────────────────────
int main() {
    App app;
    app.run();
    return 0;
}
