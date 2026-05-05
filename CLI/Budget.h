#pragma once
#include <string>

/**
 * @brief Tracks monthly budget limit and current spending for a category.
 */
class Budget {
public:
    Budget();
    Budget(const std::string& category, double limit);

    std::string getCategory()  const { return m_category; }
    double      getLimit()     const { return m_limit; }
    double      getSpent()     const { return m_spent; }
    double      getRemaining() const { return m_limit - m_spent; }
    double      getUsagePct()  const {
        return (m_limit > 0) ? (m_spent / m_limit) * 100.0 : 0.0;
    }
    bool isExceeded() const { return (m_limit > 0) && (m_spent > m_limit); }

    void setLimit(double limit)  { m_limit = limit; }
    void setSpent(double spent)  { m_spent = spent; }
    void addSpent(double amount) { m_spent += amount; }

    // CSV serialization
    std::string      toCSV()                      const;
    static Budget    fromCSV(const std::string& line);

private:
    std::string m_category;
    double      m_limit;
    double      m_spent;
};
