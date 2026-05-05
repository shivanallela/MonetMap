#include "MainWindow.h"
#include "DashboardWidget.h"
#include "AddTransactionWidget.h"
#include "ViewTransactionsWidget.h"
#include "BudgetWidget.h"
#include "ReportWidget.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QWidget>
#include <QLabel>
#include <QStyle>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>
#include <QApplication>

// ─── Helper: create a sidebar nav button ─────────────────────────────────────
static QPushButton* makeSideBtn(const QString& icon, const QString& label) {
    auto* btn = new QPushButton(icon + "  " + label);
    btn->setObjectName("navBtn");
    btn->setCheckable(false);
    btn->setCursor(Qt::PointingHandCursor);
    btn->setFixedHeight(48);
    return btn;
}

// ─── Constructor ─────────────────────────────────────────────────────────────
MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    m_fm = new FinanceManager();
    loadAllData();
    setupUI();
    navigateToDashboard();
}

MainWindow::~MainWindow() {
    saveAllData();
    delete m_fm;
}

// ─── UI setup ─────────────────────────────────────────────────────────────────
void MainWindow::setupUI() {
    setWindowTitle("MoneyMap – Personal Finance Tracker");
    resize(1150, 720);

    auto* central = new QWidget(this);
    setCentralWidget(central);

    auto* root = new QHBoxLayout(central);
    root->setContentsMargins(0, 0, 0, 0);
    root->setSpacing(0);

    // ── Sidebar ──────────────────────────────────────────────────────────────
    auto* sidebar = new QWidget();
    sidebar->setObjectName("sidebar");
    sidebar->setFixedWidth(210);

    auto* sideLayout = new QVBoxLayout(sidebar);
    sideLayout->setContentsMargins(12, 24, 12, 24);
    sideLayout->setSpacing(4);

    // Logo / app name
    auto* logo = new QLabel("💰 MoneyMap");
    logo->setObjectName("sidebarLogo");
    sideLayout->addWidget(logo);
    sideLayout->addSpacing(24);

    // Nav buttons
    m_btnDash   = makeSideBtn("🏠", "Dashboard");
    m_btnAdd    = makeSideBtn("➕", "Add Transaction");
    m_btnView   = makeSideBtn("📋", "Transactions");
    m_btnBudget = makeSideBtn("🎯", "Budget");
    m_btnReport = makeSideBtn("📊", "Monthly Report");

    sideLayout->addWidget(m_btnDash);
    sideLayout->addWidget(m_btnAdd);
    sideLayout->addWidget(m_btnView);
    sideLayout->addWidget(m_btnBudget);
    sideLayout->addWidget(m_btnReport);
    sideLayout->addStretch();

    // Version label at bottom
    auto* ver = new QLabel("v1.0  |  C++ & Qt");
    ver->setObjectName("sidebarVer");
    sideLayout->addWidget(ver);

    // ── Stacked content area ─────────────────────────────────────────────────
    m_stack = new QStackedWidget();

    m_dash   = new DashboardWidget(m_fm);
    m_addTx  = new AddTransactionWidget(m_fm);
    m_viewTx = new ViewTransactionsWidget(m_fm);
    m_budget = new BudgetWidget(m_fm);
    m_report = new ReportWidget(m_fm);

    m_stack->addWidget(m_dash);    // index 0
    m_stack->addWidget(m_addTx);   // index 1
    m_stack->addWidget(m_viewTx);  // index 2
    m_stack->addWidget(m_budget);  // index 3
    m_stack->addWidget(m_report);  // index 4

    root->addWidget(sidebar);
    root->addWidget(m_stack, 1);

    // ── Wire signals ─────────────────────────────────────────────────────────
    connect(m_btnDash,   &QPushButton::clicked, this, &MainWindow::navigateToDashboard);
    connect(m_btnAdd,    &QPushButton::clicked, this, &MainWindow::navigateToAddTransaction);
    connect(m_btnView,   &QPushButton::clicked, this, &MainWindow::navigateToViewTransactions);
    connect(m_btnBudget, &QPushButton::clicked, this, &MainWindow::navigateToBudget);
    connect(m_btnReport, &QPushButton::clicked, this, &MainWindow::navigateToReport);

    connect(m_addTx,  &AddTransactionWidget::transactionAdded, this, &MainWindow::onDataChanged);
    connect(m_budget, &BudgetWidget::budgetChanged,            this, &MainWindow::onDataChanged);
}

// ─── Navigation helpers ───────────────────────────────────────────────────────
void MainWindow::setActiveNav(QPushButton* btn) {
    const QList<QPushButton*> btns = {m_btnDash, m_btnAdd, m_btnView, m_btnBudget, m_btnReport};
    for (auto* b : btns) b->setProperty("active", b == btn);
    for (auto* b : btns) b->style()->unpolish(b), b->style()->polish(b);
    m_activBtn = btn;
}

static void fadeIn(QWidget* w) {
    auto* eff = new QGraphicsOpacityEffect(w);
    w->setGraphicsEffect(eff);
    auto* anim = new QPropertyAnimation(eff, "opacity", w);
    anim->setDuration(220);
    anim->setStartValue(0.0);
    anim->setEndValue(1.0);
    anim->start(QAbstractAnimation::DeleteWhenStopped);
}

void MainWindow::navigateToDashboard() {
    setActiveNav(m_btnDash);
    m_dash->refresh();
    m_stack->setCurrentIndex(0);
    fadeIn(m_dash);
}

void MainWindow::navigateToAddTransaction() {
    setActiveNav(m_btnAdd);
    m_addTx->resetForm();
    m_stack->setCurrentIndex(1);
    fadeIn(m_addTx);
}

void MainWindow::navigateToViewTransactions() {
    setActiveNav(m_btnView);
    m_viewTx->refresh();
    m_stack->setCurrentIndex(2);
    fadeIn(m_viewTx);
}

void MainWindow::navigateToBudget() {
    setActiveNav(m_btnBudget);
    m_budget->refresh();
    m_stack->setCurrentIndex(3);
    fadeIn(m_budget);
}

void MainWindow::navigateToReport() {
    setActiveNav(m_btnReport);
    m_report->refresh();
    m_stack->setCurrentIndex(4);
    fadeIn(m_report);
}

void MainWindow::onDataChanged() {
    saveAllData();
    // Refresh whichever screen is currently visible
    int idx = m_stack->currentIndex();
    if (idx == 0) m_dash->refresh();
    else if (idx == 2) m_viewTx->refresh();
    else if (idx == 3) m_budget->refresh();
    else if (idx == 4) m_report->refresh();
}

// ─── Persistence ─────────────────────────────────────────────────────────────
void MainWindow::loadAllData() {
    m_fm->loadCategories(F_CATEGORIES);
    m_fm->loadTransactions(F_TRANSACTIONS);
    m_fm->loadBudgets(F_BUDGETS);
}

void MainWindow::saveAllData() {
    m_fm->saveCategories(F_CATEGORIES);
    m_fm->saveTransactions(F_TRANSACTIONS);
    m_fm->saveBudgets(F_BUDGETS);
}
