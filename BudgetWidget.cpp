#include "BudgetWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QLineEdit>
#include <QPushButton>
#include <QProgressBar>
#include <QScrollArea>
#include <QFrame>
#include <QDate>

BudgetWidget::BudgetWidget(FinanceManager* fm, QWidget* parent)
    : QWidget(parent), m_fm(fm) {
    buildUI();
}

void BudgetWidget::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(20);

    auto* title = new QLabel("🎯  Budget Management");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    auto* subtitle = new QLabel("Set monthly spending limits per category.");
    subtitle->setObjectName("pageSubtitle");
    root->addWidget(subtitle);

    // ── Set budget form ───────────────────────────────────────────────────────
    auto* formCard = new QWidget();
    formCard->setObjectName("formCard");
    auto* formRow = new QHBoxLayout(formCard);
    formRow->setContentsMargins(24, 18, 24, 18);
    formRow->setSpacing(12);

    auto* catLbl = new QLabel("Category:");
    catLbl->setObjectName("fieldLabel");
    m_catBox = new QComboBox();
    m_catBox->setObjectName("formInput");
    m_catBox->setFixedWidth(160);

    auto* limLbl = new QLabel("Monthly Limit (₹):");
    limLbl->setObjectName("fieldLabel");
    m_limitEdit = new QLineEdit();
    m_limitEdit->setPlaceholderText("e.g.  5000");
    m_limitEdit->setObjectName("formInput");
    m_limitEdit->setFixedWidth(140);

    auto* setBtn = new QPushButton("✔  Set Budget");
    setBtn->setObjectName("primaryBtn");
    connect(setBtn, &QPushButton::clicked, this, &BudgetWidget::onSetBudget);

    formRow->addWidget(catLbl);
    formRow->addWidget(m_catBox);
    formRow->addWidget(limLbl);
    formRow->addWidget(m_limitEdit);
    formRow->addWidget(setBtn);
    formRow->addStretch();
    root->addWidget(formCard);

    // ── Budget list ───────────────────────────────────────────────────────────
    auto* listTitle = new QLabel("📊  Current Budgets (This Month)");
    listTitle->setObjectName("sectionTitle");
    root->addWidget(listTitle);

    auto* scroll = new QScrollArea();
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    m_listArea = new QWidget();
    m_listLayout = new QVBoxLayout(m_listArea);
    m_listLayout->setSpacing(10);
    m_listLayout->setContentsMargins(0, 0, 0, 0);
    m_listLayout->addStretch();

    scroll->setWidget(m_listArea);
    root->addWidget(scroll, 1);
}

void BudgetWidget::refresh() {
    // Repopulate category combo
    m_catBox->clear();
    for (const auto& c : m_fm->getCategories())
        m_catBox->addItem(c);

    // Recalculate budgets for current month
    QDate today = QDate::currentDate();
    m_fm->recalculateBudgets(today.year(), today.month());
    rebuildBudgetList();
}

void BudgetWidget::rebuildBudgetList() {
    // Clear existing rows
    QLayoutItem* item;
    while ((item = m_listLayout->takeAt(0)) != nullptr) {
        if (item->widget()) item->widget()->deleteLater();
        delete item;
    }

    auto budgets = m_fm->getBudgets();
    if (budgets.empty()) {
        auto* lbl = new QLabel("No budgets set yet. Use the form above to add one.");
        lbl->setObjectName("emptyLabel");
        lbl->setAlignment(Qt::AlignCenter);
        m_listLayout->addWidget(lbl);
    } else {
        for (auto& pair : budgets) {
            const Budget& b = pair.second;
            bool exceeded   = b.isExceeded();
            double pct      = std::min(b.getUsagePct(), 100.0);

            auto* row = new QWidget();
            row->setObjectName(exceeded ? "budgetRowExceeded" : "budgetRow");
            auto* rl = new QVBoxLayout(row);
            rl->setContentsMargins(16, 12, 16, 12);
            rl->setSpacing(6);

            // Header: category name + amounts
            auto* header = new QHBoxLayout();
            auto* catLbl = new QLabel((exceeded ? "🔴 " : "✅ ") + b.getCategory());
            catLbl->setObjectName("budgetCat");
            auto* amtLbl = new QLabel(
                QString("₹ %1 / ₹ %2")
                    .arg(b.getCurrentSpent(), 0, 'f', 2)
                    .arg(b.getMonthlyLimit(), 0, 'f', 2));
            amtLbl->setObjectName(exceeded ? "budgetAmtRed" : "budgetAmtBlue");
            amtLbl->setAlignment(Qt::AlignRight);
            header->addWidget(catLbl);
            header->addStretch();
            header->addWidget(amtLbl);
            rl->addLayout(header);

            // Progress bar
            auto* bar = new QProgressBar();
            bar->setRange(0, 100);
            bar->setValue((int)pct);
            bar->setTextVisible(true);
            bar->setFormat(QString("%1%").arg((int)pct));
            bar->setObjectName(exceeded ? "barExceeded" : (pct > 75 ? "barWarning" : "barOk"));
            bar->setFixedHeight(18);
            rl->addWidget(bar);

            // Remaining
            auto* remLbl = new QLabel(exceeded
                ? QString("⚠  Over budget by ₹ %1").arg(-b.getRemaining(), 0, 'f', 2)
                : QString("Remaining: ₹ %1").arg(b.getRemaining(), 0, 'f', 2));
            remLbl->setObjectName(exceeded ? "remRed" : "remGreen");
            rl->addWidget(remLbl);

            m_listLayout->addWidget(row);
        }
    }
    m_listLayout->addStretch();
}

void BudgetWidget::onSetBudget() {
    bool ok;
    double limit = m_limitEdit->text().toDouble(&ok);
    if (!ok || limit <= 0 || m_catBox->currentText().isEmpty()) return;

    m_fm->setBudget(m_catBox->currentText(), limit);
    m_limitEdit->clear();
    emit budgetChanged();
    refresh();
}
