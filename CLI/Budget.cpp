#include "Budget.h"
#include <sstream>
#include <iomanip>

Budget::Budget() : m_limit(0.0), m_spent(0.0) {}

Budget::Budget(const std::string& category, double limit)
    : m_category(category), m_limit(limit), m_spent(0.0) {}

std::string Budget::toCSV() const {
    std::ostringstream oss;
    oss << m_category << "|" << std::fixed << std::setprecision(2) << m_limit;
    return oss.str();
}

Budget Budget::fromCSV(const std::string& line) {
    std::stringstream ss(line);
    std::string cat, limStr;
    std::getline(ss, cat,    '|');
    std::getline(ss, limStr, '|');
    if (cat.empty() || limStr.empty()) return Budget();
    return Budget(cat, std::stod(limStr));
}
