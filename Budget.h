#pragma once
#include <QString>

/**
 * @brief Stores monthly budget limit and spending for a category
 */
class Budget {
public:
    Budget();
    Budget(const QString& category, double monthlyLimit);

    QString getCategory()     const { return m_category; }
    double  getMonthlyLimit() const { return m_monthlyLimit; }
    double  getCurrentSpent() const { return m_currentSpent; }
    double  getRemaining()    const { return m_monthlyLimit - m_currentSpent; }
    double  getUsagePct()     const {
        return m_monthlyLimit > 0 ? (m_currentSpent / m_monthlyLimit) * 100.0 : 0.0;
    }
    bool isExceeded() const { return m_currentSpent > m_monthlyLimit && m_monthlyLimit > 0; }

    void setMonthlyLimit(double limit)   { m_monthlyLimit = limit; }
    void setCurrentSpent(double spent)   { m_currentSpent = spent; }
    void addSpent(double amount)         { m_currentSpent += amount; }

    // CSV serialization
    QString toCSV() const;
    static Budget fromCSV(const QString& csv);

private:
    QString m_category;
    double  m_monthlyLimit;
    double  m_currentSpent;
};
