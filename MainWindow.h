#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include "FinanceManager.h"

class DashboardWidget;
class AddTransactionWidget;
class ViewTransactionsWidget;
class BudgetWidget;
class ReportWidget;

/**
 * @brief Main application window with a dark-blue sidebar and stacked content area.
 */
class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void navigateToDashboard();
    void navigateToAddTransaction();
    void navigateToViewTransactions();
    void navigateToBudget();
    void navigateToReport();
    void onDataChanged();   // save + refresh

private:
    void setupUI();
    void setupSidebar(QVBoxLayout* sideLayout);
    void loadAllData();
    void saveAllData();
    void setActiveNav(QPushButton* btn);

    // Sidebar nav buttons
    QPushButton* m_btnDash   = nullptr;
    QPushButton* m_btnAdd    = nullptr;
    QPushButton* m_btnView   = nullptr;
    QPushButton* m_btnBudget = nullptr;
    QPushButton* m_btnReport = nullptr;
    QPushButton* m_activBtn  = nullptr;

    QStackedWidget*         m_stack   = nullptr;
    DashboardWidget*        m_dash    = nullptr;
    AddTransactionWidget*   m_addTx   = nullptr;
    ViewTransactionsWidget* m_viewTx  = nullptr;
    BudgetWidget*           m_budget  = nullptr;
    ReportWidget*           m_report  = nullptr;

    FinanceManager* m_fm = nullptr;

    // Data file paths (stored next to the executable)
    static constexpr const char* F_TRANSACTIONS = "transactions.csv";
    static constexpr const char* F_BUDGETS      = "budgets.csv";
    static constexpr const char* F_CATEGORIES   = "categories.csv";
};
