#include "ReportWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QFrame>
#include <QDate>
#include <ctime>

// ─── Finance tips ─────────────────────────────────────────────────────────────
static const QStringList TIPS = {
    "💡 Follow the 50/30/20 rule: 50% needs, 30% wants, 20% savings.",
    "💡 Track every small expense – ₹100 daily = ₹3,000/month.",
    "💡 Pay yourself first: transfer savings before spending.",
    "💡 Build an emergency fund covering 3–6 months of expenses.",
    "💡 Avoid lifestyle inflation as your income grows.",
    "💡 Review subscriptions monthly – cancel what you don't use.",
    "💡 Invest early – compounding works best over time.",
    "💡 Set SMART financial goals: Specific, Measurable, Achievable, Relevant, Time-bound.",
};

ReportWidget::ReportWidget(FinanceManager* fm, QWidget* parent)
    : QWidget(parent), m_fm(fm) {
    buildUI();
}

void ReportWidget::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    auto* title = new QLabel("📊  Monthly Report");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    // ── Month/Year selector ───────────────────────────────────────────────────
    auto* selCard = new QWidget();
    selCard->setObjectName("filterBar");
    auto* selRow = new QHBoxLayout(selCard);
    selRow->setContentsMargins(16, 12, 16, 12);
    selRow->setSpacing(12);

    auto* monLbl = new QLabel("Month:");
    monLbl->setObjectName("fieldLabel");
    m_monthBox = new QComboBox();
    m_monthBox->setObjectName("formInput");
    const QStringList months = {"January","February","March","April","May","June",
                                "July","August","September","October","November","December"};
    m_monthBox->addItems(months);
    m_monthBox->setCurrentIndex(QDate::currentDate().month() - 1);

    auto* yrLbl = new QLabel("Year:");
    yrLbl->setObjectName("fieldLabel");
    m_yearSpin = new QSpinBox();
    m_yearSpin->setObjectName("formInput");
    m_yearSpin->setRange(2000, 2100);
    m_yearSpin->setValue(QDate::currentDate().year());

    auto* genBtn = new QPushButton("📊 Generate Report");
    genBtn->setObjectName("primaryBtn");
    connect(genBtn, &QPushButton::clicked, this, &ReportWidget::onGenerateReport);

    selRow->addWidget(monLbl);
    selRow->addWidget(m_monthBox);
    selRow->addWidget(yrLbl);
    selRow->addWidget(m_yearSpin);
    selRow->addWidget(genBtn);
    selRow->addStretch();
    root->addWidget(selCard);

    // ── Summary cards ─────────────────────────────────────────────────────────
    auto* sumCard = new QWidget();
    sumCard->setObjectName("formCard");
    auto* sumGrid = new QHBoxLayout(sumCard);
    sumGrid->setContentsMargins(24, 18, 24, 18);
    sumGrid->setSpacing(32);

    auto makeStat = [&](const QString& lbl, QLabel*& valLbl, const QString& objName) {
        auto* col = new QVBoxLayout();
        auto* l = new QLabel(lbl); l->setObjectName("statLabel");
        valLbl  = new QLabel("₹ 0.00"); valLbl->setObjectName(objName);
        col->addWidget(l);
        col->addWidget(valLbl);
        return col;
    };

    sumGrid->addLayout(makeStat("💵 Total Income",   m_incLbl, "statValueGreen"));
    sumGrid->addLayout(makeStat("💸 Total Expenses", m_expLbl, "statValueRed"));
    sumGrid->addLayout(makeStat("🏦 Savings",        m_savLbl, "statValueBlue"));
    sumGrid->addLayout(makeStat("📈 Savings %",      m_pctLbl, "statValuePurple"));
    sumGrid->addStretch();
    root->addWidget(sumCard);

    // ── Category breakdown ────────────────────────────────────────────────────
    auto* catTitle = new QLabel("📋  Category Budget Usage");
    catTitle->setObjectName("sectionTitle");
    root->addWidget(catTitle);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_catArea   = new QWidget();
    m_catLayout = new QVBoxLayout(m_catArea);
    m_catLayout->setSpacing(8);
    m_catLayout->setContentsMargins(0, 0, 0, 0);
    m_catLayout->addStretch();

    scroll->setWidget(m_catArea);
    root->addWidget(scroll, 1);

    // ── Tip of the month ──────────────────────────────────────────────────────
    auto* tipCard = new QWidget();
    tipCard->setObjectName("tipCard");
    auto* tipLayout = new QHBoxLayout(tipCard);
    tipLayout->setContentsMargins(20, 14, 20, 14);

    m_tipLbl = new QLabel();
    m_tipLbl->setObjectName("tipLabel");
    m_tipLbl->setWordWrap(true);
    tipLayout->addWidget(m_tipLbl);
    root->addWidget(tipCard);
}

void ReportWidget::refresh() {
    onGenerateReport();
}

void ReportWidget::onGenerateReport() {
    int month = m_monthBox->currentIndex() + 1;
    int year  = m_yearSpin->value();
    showReport(year, month);
}

void ReportWidget::showReport(int year, int month) {
    m_fm->recalculateBudgets(year, month);

    double inc = m_fm->getTotalIncome(year, month);
    double exp = m_fm->getTotalExpenses(year, month);
    double sav = m_fm->getSavings(year, month);
    double pct = m_fm->getSavingsPct(year, month);

    m_incLbl->setText(QString("₹ %1").arg(inc, 0, 'f', 2));
    m_expLbl->setText(QString("₹ %1").arg(exp, 0, 'f', 2));
    m_savLbl->setText(QString("₹ %1").arg(sav, 0, 'f', 2));
    m_pctLbl->setText(QString("%1 %").arg(pct, 0, 'f', 1));

    // Rebuild category breakdown
    QLayoutItem* item;
    while ((item = m_catLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto budgets = m_fm->getBudgets();
    for (auto& pair : budgets) {
        const Budget& b = pair.second;
        if (b.getMonthlyLimit() <= 0) continue;
        bool exceeded = b.isExceeded();
        double pctUsed = std::min(b.getUsagePct(), 100.0);

        auto* row = new QWidget();
        row->setObjectName(exceeded ? "budgetRowExceeded" : "budgetRow");
        auto* rl = new QVBoxLayout(row);
        rl->setContentsMargins(16, 10, 16, 10);
        rl->setSpacing(4);

        auto* header = new QHBoxLayout();
        auto* catLbl = new QLabel((exceeded ? "🔴 " : "✅ ") + b.getCategory());
        catLbl->setObjectName("budgetCat");
        auto* amtLbl = new QLabel(
            QString("₹%1 spent  /  ₹%2 limit")
                .arg(b.getCurrentSpent(), 0, 'f', 2)
                .arg(b.getMonthlyLimit(), 0, 'f', 2));
        amtLbl->setObjectName(exceeded ? "budgetAmtRed" : "budgetAmtBlue");
        header->addWidget(catLbl);
        header->addStretch();
        header->addWidget(amtLbl);
        rl->addLayout(header);

        auto* bar = new QProgressBar();
        bar->setRange(0, 100);
        bar->setValue((int)pctUsed);
        bar->setFormat(QString("%1%").arg((int)pctUsed));
        bar->setObjectName(exceeded ? "barExceeded" : (pctUsed > 75 ? "barWarning" : "barOk"));
        bar->setFixedHeight(16);
        rl->addWidget(bar);

        m_catLayout->addWidget(row);
    }

    if (budgets.empty()) {
        auto* lbl = new QLabel("No budget data. Set budgets in the Budget screen.");
        lbl->setObjectName("emptyLabel");
        lbl->setAlignment(Qt::AlignCenter);
        m_catLayout->addWidget(lbl);
    }
    m_catLayout->addStretch();

    // Tip of the month (rotate by month number)
    std::srand(month + year * 12);
    m_tipLbl->setText("🌟  Tip of the Month:  " + TIPS[(month - 1) % TIPS.size()]);
}
