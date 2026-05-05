#include "AddTransactionWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDateEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QMessageBox>
#include <QDate>

AddTransactionWidget::AddTransactionWidget(FinanceManager* fm, QWidget* parent)
    : QWidget(parent), m_fm(fm) {
    buildUI();
}

void AddTransactionWidget::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(48, 32, 48, 32);
    root->setSpacing(20);

    auto* title = new QLabel("➕  Add Transaction");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    // ── Main card ─────────────────────────────────────────────────────────────
    auto* card = new QWidget();
    card->setObjectName("formCard");
    auto* cardLayout = new QVBoxLayout(card);
    cardLayout->setContentsMargins(32, 28, 32, 28);
    cardLayout->setSpacing(16);

    // Type selection
    auto* typeBox = new QGroupBox("Transaction Type");
    typeBox->setObjectName("typeGroup");
    auto* typeRow = new QHBoxLayout(typeBox);
    m_incomeRad  = new QRadioButton("💵  Income");
    m_expenseRad = new QRadioButton("💸  Expense");
    m_incomeRad->setChecked(true);
    auto* typeBtnGroup = new QButtonGroup(this);
    typeBtnGroup->addButton(m_incomeRad);
    typeBtnGroup->addButton(m_expenseRad);
    typeRow->addWidget(m_incomeRad);
    typeRow->addWidget(m_expenseRad);
    typeRow->addStretch();
    cardLayout->addWidget(typeBox);

    // Form fields
    auto* form = new QFormLayout();
    form->setHorizontalSpacing(20);
    form->setVerticalSpacing(12);

    m_amountEdit = new QLineEdit();
    m_amountEdit->setPlaceholderText("e.g.  1500.00");
    m_amountEdit->setObjectName("formInput");

    m_categoryBox = new QComboBox();
    m_categoryBox->setObjectName("formInput");
    populateCategoryCombo();

    m_descEdit = new QLineEdit();
    m_descEdit->setPlaceholderText("Short description…");
    m_descEdit->setObjectName("formInput");

    m_dateEdit = new QDateEdit(QDate::currentDate());
    m_dateEdit->setCalendarPopup(true);
    m_dateEdit->setObjectName("formInput");
    m_dateEdit->setDisplayFormat("dd / MM / yyyy");

    form->addRow("Amount (₹) *", m_amountEdit);
    form->addRow("Category *",   m_categoryBox);
    form->addRow("Description",  m_descEdit);
    form->addRow("Date *",       m_dateEdit);
    cardLayout->addLayout(form);

    // Submit button
    auto* submitBtn = new QPushButton("✔  Add Transaction");
    submitBtn->setObjectName("primaryBtn");
    submitBtn->setFixedHeight(44);
    connect(submitBtn, &QPushButton::clicked, this, &AddTransactionWidget::onSubmit);
    cardLayout->addWidget(submitBtn);

    // Status label
    m_statusLabel = new QLabel();
    m_statusLabel->setObjectName("statusLabel");
    m_statusLabel->setAlignment(Qt::AlignCenter);
    cardLayout->addWidget(m_statusLabel);

    root->addWidget(card);

    // ── Add new category card ─────────────────────────────────────────────────
    auto* catCard = new QWidget();
    catCard->setObjectName("formCard");
    auto* catLayout = new QHBoxLayout(catCard);
    catLayout->setContentsMargins(24, 18, 24, 18);
    catLayout->setSpacing(12);

    auto* catLbl = new QLabel("➕  New Category:");
    catLbl->setObjectName("fieldLabel");
    m_newCatEdit = new QLineEdit();
    m_newCatEdit->setPlaceholderText("Category name…");
    m_newCatEdit->setObjectName("formInput");
    auto* addCatBtn = new QPushButton("Add Category");
    addCatBtn->setObjectName("secondaryBtn");
    connect(addCatBtn, &QPushButton::clicked, this, &AddTransactionWidget::onAddCategory);

    catLayout->addWidget(catLbl);
    catLayout->addWidget(m_newCatEdit, 1);
    catLayout->addWidget(addCatBtn);
    root->addWidget(catCard);
    root->addStretch();
}

void AddTransactionWidget::populateCategoryCombo() {
    m_categoryBox->clear();
    for (const auto& cat : m_fm->getCategories())
        m_categoryBox->addItem(cat);
}

void AddTransactionWidget::resetForm() {
    m_amountEdit->clear();
    m_descEdit->clear();
    m_dateEdit->setDate(QDate::currentDate());
    m_incomeRad->setChecked(true);
    m_statusLabel->clear();
    populateCategoryCombo();
}

void AddTransactionWidget::onSubmit() {
    // Validate
    bool ok;
    double amount = m_amountEdit->text().toDouble(&ok);
    if (!ok || amount <= 0) {
        m_statusLabel->setProperty("error", true);
        m_statusLabel->setStyleSheet("color:#D32F2F; font-weight:bold;");
        m_statusLabel->setText("⚠  Please enter a valid positive amount.");
        return;
    }
    if (m_categoryBox->currentText().isEmpty()) {
        m_statusLabel->setStyleSheet("color:#D32F2F; font-weight:bold;");
        m_statusLabel->setText("⚠  Please select a category.");
        return;
    }

    // Build transaction
    Transaction::Type type = m_incomeRad->isChecked() ? Transaction::INCOME : Transaction::EXPENSE;
    Transaction t(m_fm->getNextId(), amount,
                  m_categoryBox->currentText(),
                  m_descEdit->text().isEmpty() ? "-" : m_descEdit->text(),
                  m_dateEdit->date(), type);
    m_fm->addTransaction(t);

    // Check budget
    if (type == Transaction::EXPENSE) {
        QDate d = m_dateEdit->date();
        m_fm->recalculateBudgets(d.year(), d.month());
        if (m_fm->isBudgetExceeded(m_categoryBox->currentText())) {
            QMessageBox warn(this);
            warn.setWindowTitle("⚠ Budget Exceeded!");
            warn.setText(QString("<b>Warning:</b> You have exceeded your budget for <b>%1</b>!")
                             .arg(m_categoryBox->currentText()));
            warn.setIcon(QMessageBox::Warning);
            warn.setStyleSheet("background:#FFF3E0; color:#E65100;");
            warn.exec();
        }
    }

    m_statusLabel->setStyleSheet("color:#2E7D32; font-weight:bold;");
    m_statusLabel->setText("✅  Transaction added successfully!");

    emit transactionAdded();
    resetForm();
}

void AddTransactionWidget::onAddCategory() {
    QString cat = m_newCatEdit->text().trimmed();
    if (cat.isEmpty()) return;
    m_fm->addCategory(cat);
    m_newCatEdit->clear();
    populateCategoryCombo();
    m_categoryBox->setCurrentText(cat);
}
