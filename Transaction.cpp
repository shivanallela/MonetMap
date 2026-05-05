#include "Transaction.h"
#include <QStringList>

Transaction::Transaction()
    : m_id(0), m_amount(0.0), m_type(EXPENSE) {}

Transaction::Transaction(int id, double amount, const QString& category,
                         const QString& description, const QDate& date, Type type)
    : m_id(id), m_amount(amount), m_category(category),
      m_description(description), m_date(date), m_type(type) {}

QString Transaction::toCSV() const {
    // Use '|' delimiter
    return QString("%1|%2|%3|%4|%5|%6")
        .arg(m_id)
        .arg(m_amount, 0, 'f', 2)
        .arg(m_category)
        .arg(m_description)
        .arg(m_date.toString("yyyy-MM-dd"))
        .arg(m_type == INCOME ? "income" : "expense");
}

Transaction Transaction::fromCSV(const QString& csv) {
    QStringList p = csv.split('|');
    if (p.size() < 6) return Transaction();
    Transaction t;
    t.setId(p[0].trimmed().toInt());
    t.setAmount(p[1].trimmed().toDouble());
    t.setCategory(p[2].trimmed());
    t.setDescription(p[3].trimmed());
    t.setDate(QDate::fromString(p[4].trimmed(), "yyyy-MM-dd"));
    t.setType(p[5].trimmed() == "income" ? INCOME : EXPENSE);
    return t;
}
