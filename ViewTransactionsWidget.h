#pragma once
#include <QWidget>
#include "FinanceManager.h"

class QTableWidget;
class QComboBox;
class QDateEdit;
class QLabel;

/**
 * @brief Displays all transactions in a sortable table with search/filter controls.
 */
class ViewTransactionsWidget : public QWidget {
    Q_OBJECT
public:
    explicit ViewTransactionsWidget(FinanceManager* fm, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onFilter();
    void onClearFilter();
    void onDeleteSelected();

private:
    void buildUI();
    void populateTable(const std::vector<Transaction>& txns);

    FinanceManager* m_fm;

    QTableWidget* m_table       = nullptr;
    QComboBox*    m_catFilter   = nullptr;
    QDateEdit*    m_fromDate    = nullptr;
    QDateEdit*    m_toDate      = nullptr;
    QLabel*       m_countLabel  = nullptr;
};
