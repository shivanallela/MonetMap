#include "DashboardWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include <QDate>
#include <QFrame>

DashboardWidget::DashboardWidget(FinanceManager* fm, QWidget* parent)
    : QWidget(parent), m_fm(fm) {
    buildUI();
}

// ─── Build static structure ───────────────────────────────────────────────────
void DashboardWidget::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(24);

    // Page title
    auto* title = new QLabel("📊  Dashboard");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    auto* subtitle = new QLabel("Overview for " + QDate::currentDate().toString("MMMM yyyy"));
    subtitle->setObjectName("pageSubtitle");
    root->addWidget(subtitle);

    // ── Stat cards row ────────────────────────────────────────────────────────
    auto* cardRow = new QHBoxLayout();
    cardRow->setSpacing(16);

    // We create placeholder card widgets; refresh() fills in values
    auto* incCard = new QWidget();
    incCard->setObjectName("statCard");
    auto* incL = new QVBoxLayout(incCard);
    auto* incIcon  = new QLabel("💵");  incIcon->setObjectName("statIcon");
    auto* incTitle = new QLabel("Total Income"); incTitle->setObjectName("statLabel");
    m_incomeVal    = new QLabel("₹ 0.00");        m_incomeVal->setObjectName("statValueGreen");
    incL->addWidget(incIcon); incL->addWidget(incTitle); incL->addWidget(m_incomeVal);

    auto* expCard = new QWidget();
    expCard->setObjectName("statCard");
    auto* expL = new QVBoxLayout(expCard);
    auto* expIcon  = new QLabel("💸");  expIcon->setObjectName("statIcon");
    auto* expTitle = new QLabel("Total Expenses"); expTitle->setObjectName("statLabel");
    m_expenseVal   = new QLabel("₹ 0.00");          m_expenseVal->setObjectName("statValueRed");
    expL->addWidget(expIcon); expL->addWidget(expTitle); expL->addWidget(m_expenseVal);

    auto* savCard = new QWidget();
    savCard->setObjectName("statCard");
    auto* savL = new QVBoxLayout(savCard);
    auto* savIcon  = new QLabel("🏦");  savIcon->setObjectName("statIcon");
    auto* savTitle = new QLabel("Savings");        savTitle->setObjectName("statLabel");
    m_savingsVal   = new QLabel("₹ 0.00");          m_savingsVal->setObjectName("statValueBlue");
    savL->addWidget(savIcon); savL->addWidget(savTitle); savL->addWidget(m_savingsVal);

    auto* pctCard = new QWidget();
    pctCard->setObjectName("statCard");
    auto* pctL = new QVBoxLayout(pctCard);
    auto* pctIcon  = new QLabel("📈");  pctIcon->setObjectName("statIcon");
    auto* pctTitle = new QLabel("Savings %");      pctTitle->setObjectName("statLabel");
    m_savingsPct   = new QLabel("0.00 %");           m_savingsPct->setObjectName("statValuePurple");
    pctL->addWidget(pctIcon); pctL->addWidget(pctTitle); pctL->addWidget(m_savingsPct);

    cardRow->addWidget(incCard);
    cardRow->addWidget(expCard);
    cardRow->addWidget(savCard);
    cardRow->addWidget(pctCard);
    root->addLayout(cardRow);

    // ── Recent transactions ───────────────────────────────────────────────────
    auto* recTitle = new QLabel("🕐  Recent Transactions");
    recTitle->setObjectName("sectionTitle");
    root->addWidget(recTitle);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_recentList   = new QWidget();
    m_recentLayout = new QVBoxLayout(m_recentList);
    m_recentLayout->setSpacing(8);
    m_recentLayout->setContentsMargins(0, 0, 0, 0);
    m_recentLayout->addStretch();

    scroll->setWidget(m_recentList);
    root->addWidget(scroll, 1);
}

// ─── Refresh (called every time this screen becomes active) ───────────────────
void DashboardWidget::refresh() {
    QDate today = QDate::currentDate();
    int y = today.year(), m = today.month();

    m_fm->recalculateBudgets(y, m);

    double inc  = m_fm->getTotalIncome(y, m);
    double exp  = m_fm->getTotalExpenses(y, m);
    double sav  = m_fm->getSavings(y, m);
    double pct  = m_fm->getSavingsPct(y, m);

    m_incomeVal->setText(QString("₹ %1").arg(inc, 0, 'f', 2));
    m_expenseVal->setText(QString("₹ %1").arg(exp, 0, 'f', 2));
    m_savingsVal->setText(QString("₹ %1").arg(sav, 0, 'f', 2));
    m_savingsPct->setText(QString("%1 %").arg(pct, 0, 'f', 1));

    // Clear and rebuild recent list (up to 8 most recent)
    QLayoutItem* item;
    while ((item = m_recentLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto txns = m_fm->getAllTransactions();
    // Show last 8 in reverse order
    int start = std::max(0, (int)txns.size() - 8);
    for (int i = (int)txns.size() - 1; i >= start; --i) {
        const auto& t = txns[i];
        auto* row = new QWidget();
        row->setObjectName("recentRow");
        auto* rl = new QHBoxLayout(row);
        rl->setContentsMargins(12, 8, 12, 8);

        bool isInc = (t.getType() == Transaction::INCOME);
        auto* typeLabel = new QLabel(isInc ? "▲" : "▼");
        typeLabel->setObjectName(isInc ? "arrowUp" : "arrowDown");

        auto* catLabel  = new QLabel(t.getCategory());
        catLabel->setObjectName("recentCat");
        auto* descLabel = new QLabel(t.getDescription());
        descLabel->setObjectName("recentDesc");
        descLabel->setAlignment(Qt::AlignRight);

        auto* amtLabel  = new QLabel(QString("%1₹ %2")
                                         .arg(isInc ? "+" : "-")
                                         .arg(t.getAmount(), 0, 'f', 2));
        amtLabel->setObjectName(isInc ? "amtGreen" : "amtRed");
        amtLabel->setAlignment(Qt::AlignRight);

        auto* dateLabel = new QLabel(t.getDate().toString("dd MMM"));
        dateLabel->setObjectName("recentDate");

        rl->addWidget(typeLabel);
        rl->addWidget(catLabel);
        rl->addWidget(descLabel, 1);
        rl->addWidget(amtLabel);
        rl->addWidget(dateLabel);

        m_recentLayout->addWidget(row);
    }

    if (txns.empty()) {
        auto* empty = new QLabel("No transactions yet. Add one to get started!");
        empty->setObjectName("emptyLabel");
        empty->setAlignment(Qt::AlignCenter);
        m_recentLayout->addWidget(empty);
    }

    m_recentLayout->addStretch();
}
