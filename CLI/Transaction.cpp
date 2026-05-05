#include "Transaction.h"
#include <sstream>
#include <iomanip>
#include <vector>
#include <string>

Transaction::Transaction()
    : m_id(0), m_amount(0.0), m_type(Type::EXPENSE) {}

Transaction::Transaction(int id, double amount, const std::string& category,
                         const std::string& description, const std::string& date, Type type)
    : m_id(id), m_amount(amount), m_category(category),
      m_description(description), m_date(date), m_type(type) {}

std::string Transaction::toCSV() const {
    std::ostringstream oss;
    oss << m_id << "|"
        << std::fixed << std::setprecision(2) << m_amount << "|"
        << m_category << "|"
        << m_description << "|"
        << m_date << "|"
        << (m_type == Type::INCOME ? "income" : "expense");
    return oss.str();
}

Transaction Transaction::fromCSV(const std::string& line) {
    // Split by '|'
    std::vector<std::string> parts;
    std::stringstream ss(line);
    std::string token;
    while (std::getline(ss, token, '|')) parts.push_back(token);

    if (parts.size() < 6) return Transaction();

    Transaction t;
    t.m_id          = std::stoi(parts[0]);
    t.m_amount      = std::stod(parts[1]);
    t.m_category    = parts[2];
    t.m_description = parts[3];
    t.m_date        = parts[4];
    t.m_type        = (parts[5] == "income") ? Type::INCOME : Type::EXPENSE;
    return t;
}
