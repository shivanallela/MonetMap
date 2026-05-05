#pragma once
#include <QString>
#include <QDate>

/**
 * @brief Represents a single financial transaction (income or expense)
 * Uses '|' as CSV delimiter to safely handle commas in descriptions
 */
class Transaction {
public:
    enum Type { INCOME, EXPENSE };

    Transaction();
    Transaction(int id, double amount, const QString& category,
                const QString& description, const QDate& date, Type type);

    // Getters
    int     getId()          const { return m_id; }
    double  getAmount()      const { return m_amount; }
    QString getCategory()    const { return m_category; }
    QString getDescription() const { return m_description; }
    QDate   getDate()        const { return m_date; }
    Type    getType()        const { return m_type; }
    QString getTypeString()  const { return m_type == INCOME ? "Income" : "Expense"; }

    // Setters
    void setId(int id)                       { m_id = id; }
    void setAmount(double amount)            { m_amount = amount; }
    void setCategory(const QString& c)       { m_category = c; }
    void setDescription(const QString& d)   { m_description = d; }
    void setDate(const QDate& date)          { m_date = date; }
    void setType(Type type)                  { m_type = type; }

    // CSV serialization (delimiter: '|')
    QString toCSV() const;
    static Transaction fromCSV(const QString& csv);

private:
    int     m_id;
    double  m_amount;
    QString m_category;
    QString m_description;
    QDate   m_date;
    Type    m_type;
};
