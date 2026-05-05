#pragma once
#include <QWidget>
#include "FinanceManager.h"

class QComboBox;
class QLineEdit;
class QVBoxLayout;

/**
 * @brief Screen for setting and viewing monthly category budgets with progress bars.
 */
class BudgetWidget : public QWidget {
    Q_OBJECT
public:
    explicit BudgetWidget(FinanceManager* fm, QWidget* parent = nullptr);
    void refresh();

signals:
    void budgetChanged();

private slots:
    void onSetBudget();

private:
    void buildUI();
    void rebuildBudgetList();

    FinanceManager* m_fm;

    QComboBox*   m_catBox    = nullptr;
    QLineEdit*   m_limitEdit = nullptr;
    QVBoxLayout* m_listLayout = nullptr;
    QWidget*     m_listArea  = nullptr;
};
