#pragma once
#include <vector>
#include <map>
#include <string>
#include "Transaction.h"
#include "Budget.h"

/**
 * @brief Core business logic class.
 *        Manages all transactions, categories, budgets and file I/O.
 */
class FinanceManager {
public:
    FinanceManager();

    // ── Transactions ──────────────────────────────────────────────────────────
    void                     addTransaction(const Transaction& t);
    bool                     removeTransaction(int id);
    std::vector<Transaction> getAllTransactions()                                    const;
    std::vector<Transaction> getByCategory(const std::string& cat)                  const;
    std::vector<Transaction> getByMonth(int year, int month)                        const;
    std::vector<Transaction> getByDateRange(const std::string& from,
                                            const std::string& to)                  const;

    // ── Categories (separated by type) ──────────────────────────────────────
    // Income categories: Salary, Parents Sent, Other
    void                     addIncomeCategory(const std::string& cat);
    bool                     incomeCategoryExists(const std::string& cat)           const;
    std::vector<std::string> getIncomeCategories()                                  const;

    // Expense categories: Food, Transport, Entertainment, etc.
    void                     addExpenseCategory(const std::string& cat);
    bool                     expenseCategoryExists(const std::string& cat)          const;
    std::vector<std::string> getExpenseCategories()                                 const;

    // Legacy – returns ALL categories (income + expense combined)
    void                     addCategory(const std::string& cat);
    bool                     categoryExists(const std::string& cat)                 const;
    std::vector<std::string> getCategories()                                        const;

    // ── Budgets ───────────────────────────────────────────────────────────────
    void                          setBudget(const std::string& cat, double limit);
    std::map<std::string, Budget> getBudgets()                                      const;
    Budget                        getBudget(const std::string& cat)                 const;
    void                          recalculateBudgets(int year, int month);

    // ── Report helpers ────────────────────────────────────────────────────────
    double getTotalIncome(int year, int month)   const;
    double getTotalExpenses(int year, int month) const;
    double getSavings(int year, int month)       const;
    double getSavingsPct(int year, int month)    const;

    // ── File I/O (fstream) ────────────────────────────────────────────────────
    bool saveTransactions(const std::string& file);
    bool loadTransactions(const std::string& file);
    bool saveBudgets(const std::string& file);
    bool loadBudgets(const std::string& file);
    bool saveCategories(const std::string& file);
    bool loadCategories(const std::string& file);

    int  getNextId() const { return m_nextId; }

private:
    std::vector<Transaction>       m_transactions;
    std::vector<std::string>       m_incomeCategories;   // Salary, Parents Sent, Other
    std::vector<std::string>       m_expenseCategories;  // Food, Transport, etc.
    std::map<std::string, Budget>  m_budgets;
    int                            m_nextId;
};
