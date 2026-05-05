#include "FinanceManager.h"
#include <algorithm>
#include <QFile>
#include <QTextStream>

// ─── Constructor ─────────────────────────────────────────────────────────────
FinanceManager::FinanceManager() : m_nextId(1) {
    // Default categories available at startup
    m_categories = {"Salary", "Food", "Transport", "Entertainment",
                    "Healthcare", "Utilities", "Shopping", "Other"};
}

// ─── Transaction CRUD ─────────────────────────────────────────────────────────
void FinanceManager::addTransaction(const Transaction& t) {
    m_transactions.push_back(t);
    m_nextId = std::max(m_nextId, t.getId() + 1);
}

void FinanceManager::removeTransaction(int id) {
    m_transactions.erase(
        std::remove_if(m_transactions.begin(), m_transactions.end(),
                       [id](const Transaction& t){ return t.getId() == id; }),
        m_transactions.end());
}

std::vector<Transaction> FinanceManager::getAllTransactions() const {
    return m_transactions;
}

std::vector<Transaction> FinanceManager::getTransactionsByCategory(const QString& cat) const {
    std::vector<Transaction> result;
    for (const auto& t : m_transactions)
        if (t.getCategory() == cat) result.push_back(t);
    return result;
}

std::vector<Transaction> FinanceManager::getTransactionsByDateRange(
        const QDate& from, const QDate& to) const {
    std::vector<Transaction> result;
    for (const auto& t : m_transactions)
        if (t.getDate() >= from && t.getDate() <= to) result.push_back(t);
    return result;
}

std::vector<Transaction> FinanceManager::getTransactionsByMonth(int year, int month) const {
    std::vector<Transaction> result;
    for (const auto& t : m_transactions)
        if (t.getDate().year() == year && t.getDate().month() == month)
            result.push_back(t);
    return result;
}

// ─── Category management ──────────────────────────────────────────────────────
void FinanceManager::addCategory(const QString& category) {
    if (std::find(m_categories.begin(), m_categories.end(), category) == m_categories.end())
        m_categories.push_back(category);
}

// ─── Budget management ────────────────────────────────────────────────────────
void FinanceManager::setBudget(const QString& category, double limit) {
    if (m_budgets.count(category))
        m_budgets[category].setMonthlyLimit(limit);
    else
        m_budgets[category] = Budget(category, limit);
}

Budget FinanceManager::getBudget(const QString& category) const {
    auto it = m_budgets.find(category);
    return (it != m_budgets.end()) ? it->second : Budget(category, 0.0);
}

bool FinanceManager::isBudgetExceeded(const QString& category) const {
    auto it = m_budgets.find(category);
    return (it != m_budgets.end()) && it->second.isExceeded();
}

void FinanceManager::recalculateBudgets(int year, int month) {
    for (auto& pair : m_budgets) pair.second.setCurrentSpent(0.0);
    for (const auto& t : m_transactions) {
        if (t.getType() == Transaction::EXPENSE &&
            t.getDate().year() == year && t.getDate().month() == month) {
            if (m_budgets.count(t.getCategory()))
                m_budgets[t.getCategory()].addSpent(t.getAmount());
        }
    }
}

// ─── Report helpers ───────────────────────────────────────────────────────────
double FinanceManager::getTotalIncome(int year, int month) const {
    double total = 0;
    for (const auto& t : m_transactions)
        if (t.getType() == Transaction::INCOME &&
            t.getDate().year() == year && t.getDate().month() == month)
            total += t.getAmount();
    return total;
}

double FinanceManager::getTotalExpenses(int year, int month) const {
    double total = 0;
    for (const auto& t : m_transactions)
        if (t.getType() == Transaction::EXPENSE &&
            t.getDate().year() == year && t.getDate().month() == month)
            total += t.getAmount();
    return total;
}

double FinanceManager::getSavings(int year, int month) const {
    return getTotalIncome(year, month) - getTotalExpenses(year, month);
}

double FinanceManager::getSavingsPct(int year, int month) const {
    double inc = getTotalIncome(year, month);
    return inc > 0 ? (getSavings(year, month) / inc) * 100.0 : 0.0;
}

// ─── Persistence ──────────────────────────────────────────────────────────────
bool FinanceManager::saveTransactions(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    for (const auto& t : m_transactions) out << t.toCSV() << "\n";
    return true;
}

bool FinanceManager::loadTransactions(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);
    m_transactions.clear();
    m_nextId = 1;
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Transaction t = Transaction::fromCSV(line);
            m_transactions.push_back(t);
            m_nextId = std::max(m_nextId, t.getId() + 1);
        }
    }
    return true;
}

bool FinanceManager::saveBudgets(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    for (const auto& pair : m_budgets) out << pair.second.toCSV() << "\n";
    return true;
}

bool FinanceManager::loadBudgets(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);
    m_budgets.clear();
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) {
            Budget b = Budget::fromCSV(line);
            m_budgets[b.getCategory()] = b;
        }
    }
    return true;
}

bool FinanceManager::saveCategories(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) return false;
    QTextStream out(&file);
    for (const auto& cat : m_categories) out << cat << "\n";
    return true;
}

bool FinanceManager::loadCategories(const QString& filepath) {
    QFile file(filepath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return false;
    QTextStream in(&file);
    m_categories.clear();
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        if (!line.isEmpty()) m_categories.push_back(line);
    }
    return true;
}
