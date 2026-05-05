#pragma once
#include <vector>
#include <map>
#include <QString>
#include <QDate>
#include "Transaction.h"
#include "Budget.h"

/**
 * @brief Core business-logic class – manages transactions, budgets, categories
 *        and handles all file I/O via fstream-style QFile / QTextStream.
 */
class FinanceManager {
public:
    FinanceManager();

    /* ---------- Transaction CRUD ---------- */
    void addTransaction(const Transaction& t);
    void removeTransaction(int id);
    std::vector<Transaction> getAllTransactions()                                       const;
    std::vector<Transaction> getTransactionsByCategory(const QString& category)        const;
    std::vector<Transaction> getTransactionsByDateRange(const QDate& from,
                                                        const QDate& to)               const;
    std::vector<Transaction> getTransactionsByMonth(int year, int month)               const;

    /* ---------- Category management ---------- */
    void addCategory(const QString& category);
    std::vector<QString> getCategories() const { return m_categories; }

    /* ---------- Budget management ---------- */
    void setBudget(const QString& category, double limit);
    std::map<QString, Budget> getBudgets() const { return m_budgets; }
    Budget getBudget(const QString& category) const;
    bool   isBudgetExceeded(const QString& category) const;

    /* Recalculate "spent" totals for a given month */
    void recalculateBudgets(int year, int month);

    /* ---------- Report helpers ---------- */
    double getTotalIncome(int year, int month)      const;
    double getTotalExpenses(int year, int month)    const;
    double getSavings(int year, int month)          const;
    double getSavingsPct(int year, int month)       const;

    /* ---------- Persistence (file I/O) ---------- */
    bool saveTransactions(const QString& filepath);
    bool loadTransactions(const QString& filepath);
    bool saveBudgets(const QString& filepath);
    bool loadBudgets(const QString& filepath);
    bool saveCategories(const QString& filepath);
    bool loadCategories(const QString& filepath);

    int  getNextId() const { return m_nextId; }

private:
    std::vector<Transaction>  m_transactions;
    std::vector<QString>      m_categories;
    std::map<QString, Budget> m_budgets;
    int                       m_nextId;
};
