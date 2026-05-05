#include "Budget.h"
#include <QStringList>

Budget::Budget() : m_monthlyLimit(0.0), m_currentSpent(0.0) {}

Budget::Budget(const QString& category, double monthlyLimit)
    : m_category(category), m_monthlyLimit(monthlyLimit), m_currentSpent(0.0) {}

QString Budget::toCSV() const {
    return QString("%1|%2").arg(m_category).arg(m_monthlyLimit, 0, 'f', 2);
}

Budget Budget::fromCSV(const QString& csv) {
    QStringList p = csv.split('|');
    if (p.size() < 2) return Budget();
    return Budget(p[0].trimmed(), p[1].trimmed().toDouble());
}
