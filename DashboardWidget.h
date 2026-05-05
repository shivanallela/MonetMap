#pragma once
#include <QWidget>
#include <QVBoxLayout>
#include "FinanceManager.h"

class QLabel;

/**
 * @brief Dashboard screen – shows summary stat cards + recent transactions.
 */
class DashboardWidget : public QWidget {
    Q_OBJECT
public:
    explicit DashboardWidget(FinanceManager* fm, QWidget* parent = nullptr);
    void refresh();

private:
    void buildUI();
    QWidget* makeStatCard(const QString& title, const QString& value,
                          const QString& color, const QString& icon);

    FinanceManager* m_fm;

    QLabel* m_incomeVal   = nullptr;
    QLabel* m_expenseVal  = nullptr;
    QLabel* m_savingsVal  = nullptr;
    QLabel* m_savingsPct  = nullptr;
    QWidget* m_recentList = nullptr;
    QVBoxLayout* m_recentLayout = nullptr;
};
