#pragma once
#include <string>

/**
 * @brief Represents a single financial transaction.
 *        Uses pipe '|' as CSV delimiter to avoid comma conflicts.
 */
class Transaction {
public:
    enum class Type { INCOME, EXPENSE };

    Transaction();
    Transaction(int id, double amount, const std::string& category,
                const std::string& description, const std::string& date, Type type);

    // ── Getters ──────────────────────────────────────────────────────────────
    int         getId()          const { return m_id; }
    double      getAmount()      const { return m_amount; }
    std::string getCategory()    const { return m_category; }
    std::string getDescription() const { return m_description; }
    std::string getDate()        const { return m_date; }
    Type        getType()        const { return m_type; }
    std::string getTypeStr()     const { return m_type == Type::INCOME ? "Income" : "Expense"; }

    void setId(int id) { m_id = id; }

    // ── CSV serialization ─────────────────────────────────────────────────────
    std::string          toCSV()                         const;
    static Transaction   fromCSV(const std::string& line);

private:
    int         m_id;
    double      m_amount;
    std::string m_category;
    std::string m_description;
    std::string m_date;   // "YYYY-MM-DD"
    Type        m_type;
};
