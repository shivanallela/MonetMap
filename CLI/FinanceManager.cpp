#include "FinanceManager.h"
#include <fstream>
#include <sstream>
#include <algorithm>

// ─── Constructor ──────────────────────────────────────────────────────────────
FinanceManager::FinanceManager() : m_nextId(1) {
    // Income categories – where money comes FROM
    m_incomeCategories  = {"Salary", "Parents Sent", "Other"};
    // Expense categories – what money is SPENT ON
    m_expenseCategories = {"Food", "Transport", "Entertainment",
                           "Healthcare", "Utilities", "Shopping", "Other"};
}

// ─── Transactions ─────────────────────────────────────────────────────────────
void FinanceManager::addTransaction(const Transaction& t) {
    m_transactions.push_back(t);
    m_nextId = std::max(m_nextId, t.getId() + 1);
}

bool FinanceManager::removeTransaction(int id) {
    auto before = m_transactions.size();
    m_transactions.erase(
        std::remove_if(m_transactions.begin(), m_transactions.end(),
                       [id](const Transaction& t){ return t.getId() == id; }),
        m_transactions.end());
    return m_transactions.size() < before;
}

std::vector<Transaction> FinanceManager::getAllTransactions() const {
    return m_transactions;
}

std::vector<Transaction> FinanceManager::getByCategory(const std::string& cat) const {
    std::vector<Transaction> res;
    for (const auto& t : m_transactions)
        if (t.getCategory() == cat) res.push_back(t);
    return res;
}

std::vector<Transaction> FinanceManager::getByMonth(int year, int month) const {
    char buf[8];
    snprintf(buf, sizeof(buf), "%04d-%02d", year, month);
    std::string prefix(buf);

    std::vector<Transaction> res;
    for (const auto& t : m_transactions)
        if (t.getDate().substr(0, 7) == prefix) res.push_back(t);
    return res;
}

std::vector<Transaction> FinanceManager::getByDateRange(const std::string& from,
                                                         const std::string& to) const {
    std::vector<Transaction> res;
    for (const auto& t : m_transactions)
        if (t.getDate() >= from && t.getDate() <= to) res.push_back(t);
    return res;
}

// ─── Income categories ────────────────────────────────────────────────────────
void FinanceManager::addIncomeCategory(const std::string& cat) {
    if (!incomeCategoryExists(cat)) m_incomeCategories.push_back(cat);
}
bool FinanceManager::incomeCategoryExists(const std::string& cat) const {
    return std::find(m_incomeCategories.begin(), m_incomeCategories.end(), cat)
           != m_incomeCategories.end();
}
std::vector<std::string> FinanceManager::getIncomeCategories() const {
    return m_incomeCategories;
}

// ─── Expense categories ───────────────────────────────────────────────────────
void FinanceManager::addExpenseCategory(const std::string& cat) {
    if (!expenseCategoryExists(cat)) m_expenseCategories.push_back(cat);
}
bool FinanceManager::expenseCategoryExists(const std::string& cat) const {
    return std::find(m_expenseCategories.begin(), m_expenseCategories.end(), cat)
           != m_expenseCategories.end();
}
std::vector<std::string> FinanceManager::getExpenseCategories() const {
    return m_expenseCategories;
}

// ─── Legacy combined category helpers ────────────────────────────────────────
void FinanceManager::addCategory(const std::string& cat) {
    addIncomeCategory(cat);
    addExpenseCategory(cat);
}
bool FinanceManager::categoryExists(const std::string& cat) const {
    return incomeCategoryExists(cat) || expenseCategoryExists(cat);
}
std::vector<std::string> FinanceManager::getCategories() const {
    // Merge income + expense (deduplicated)
    std::vector<std::string> all = m_incomeCategories;
    for (const auto& c : m_expenseCategories)
        if (std::find(all.begin(), all.end(), c) == all.end())
            all.push_back(c);
    return all;
}

// ─── Budgets ──────────────────────────────────────────────────────────────────
void FinanceManager::setBudget(const std::string& cat, double limit) {
    if (m_budgets.count(cat))
        m_budgets[cat].setLimit(limit);
    else
        m_budgets[cat] = Budget(cat, limit);
}

std::map<std::string, Budget> FinanceManager::getBudgets() const {
    return m_budgets;
}

Budget FinanceManager::getBudget(const std::string& cat) const {
    auto it = m_budgets.find(cat);
    return (it != m_budgets.end()) ? it->second : Budget(cat, 0.0);
}

void FinanceManager::recalculateBudgets(int year, int month) {
    for (auto& p : m_budgets) p.second.setSpent(0.0);

    char buf[8];
    snprintf(buf, sizeof(buf), "%04d-%02d", year, month);
    std::string prefix(buf);

    for (const auto& t : m_transactions) {
        if (t.getType() == Transaction::Type::EXPENSE &&
            t.getDate().substr(0, 7) == prefix) {
            if (m_budgets.count(t.getCategory()))
                m_budgets[t.getCategory()].addSpent(t.getAmount());
        }
    }
}

// ─── Report helpers ───────────────────────────────────────────────────────────
double FinanceManager::getTotalIncome(int year, int month) const {
    double total = 0;
    for (const auto& t : getByMonth(year, month))
        if (t.getType() == Transaction::Type::INCOME) total += t.getAmount();
    return total;
}

double FinanceManager::getTotalExpenses(int year, int month) const {
    double total = 0;
    for (const auto& t : getByMonth(year, month))
        if (t.getType() == Transaction::Type::EXPENSE) total += t.getAmount();
    return total;
}

double FinanceManager::getSavings(int year, int month) const {
    return getTotalIncome(year, month) - getTotalExpenses(year, month);
}

double FinanceManager::getSavingsPct(int year, int month) const {
    double inc = getTotalIncome(year, month);
    return (inc > 0) ? (getSavings(year, month) / inc) * 100.0 : 0.0;
}

// ─── File I/O (fstream) ───────────────────────────────────────────────────────
bool FinanceManager::saveTransactions(const std::string& file) {
    std::ofstream out(file);
    if (!out.is_open()) return false;
    for (const auto& t : m_transactions) out << t.toCSV() << "\n";
    return true;
}

bool FinanceManager::loadTransactions(const std::string& file) {
    std::ifstream in(file);
    if (!in.is_open()) return false;
    m_transactions.clear();
    m_nextId = 1;
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Transaction t = Transaction::fromCSV(line);
        m_transactions.push_back(t);
        m_nextId = std::max(m_nextId, t.getId() + 1);
    }
    return true;
}

bool FinanceManager::saveBudgets(const std::string& file) {
    std::ofstream out(file);
    if (!out.is_open()) return false;
    for (const auto& p : m_budgets) out << p.second.toCSV() << "\n";
    return true;
}

bool FinanceManager::loadBudgets(const std::string& file) {
    std::ifstream in(file);
    if (!in.is_open()) return false;
    m_budgets.clear();
    std::string line;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        Budget b = Budget::fromCSV(line);
        m_budgets[b.getCategory()] = b;
    }
    return true;
}

bool FinanceManager::saveCategories(const std::string& file) {
    // Prefix: 'I:' for income, 'E:' for expense
    std::ofstream out(file);
    if (!out.is_open()) return false;
    for (const auto& c : m_incomeCategories)  out << "I:" << c << "\n";
    for (const auto& c : m_expenseCategories) out << "E:" << c << "\n";
    return true;
}

bool FinanceManager::loadCategories(const std::string& file) {
    std::ifstream in(file);
    if (!in.is_open()) return false;   // file missing → keep constructor defaults

    // Load into temp vectors first
    std::vector<std::string> tempIncome, tempExpense;
    std::string line;
    while (std::getline(in, line)) {
        if (line.size() < 3) continue;
        if      (line.substr(0, 2) == "I:") tempIncome.push_back(line.substr(2));
        else if (line.substr(0, 2) == "E:") tempExpense.push_back(line.substr(2));
        else    tempExpense.push_back(line);   // legacy untagged → expense
    }

    // Only replace defaults if we actually found valid entries
    if (!tempIncome.empty())  m_incomeCategories  = tempIncome;
    if (!tempExpense.empty()) m_expenseCategories = tempExpense;
    return true;
}
