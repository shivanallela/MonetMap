#include "ViewTransactionsWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QHeaderView>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QDate>

ViewTransactionsWidget::ViewTransactionsWidget(FinanceManager* fm, QWidget* parent)
    : QWidget(parent), m_fm(fm) {
    buildUI();
}

void ViewTransactionsWidget::buildUI() {
    auto* root = new QVBoxLayout(this);
    root->setContentsMargins(32, 28, 32, 28);
    root->setSpacing(16);

    auto* title = new QLabel("📋  Transactions");
    title->setObjectName("pageTitle");
    root->addWidget(title);

    // ── Filter bar ────────────────────────────────────────────────────────────
    auto* filterCard = new QWidget();
    filterCard->setObjectName("filterBar");
    auto* filterRow = new QHBoxLayout(filterCard);
    filterRow->setContentsMargins(16, 12, 16, 12);
    filterRow->setSpacing(12);

    auto* catLbl = new QLabel("Category:");
    catLbl->setObjectName("fieldLabel");
    m_catFilter = new QComboBox();
    m_catFilter->setObjectName("formInput");
    m_catFilter->setFixedWidth(160);

    auto* fromLbl = new QLabel("From:");
    fromLbl->setObjectName("fieldLabel");
    m_fromDate = new QDateEdit(QDate::currentDate().addMonths(-1));
    m_fromDate->setCalendarPopup(true);
    m_fromDate->setObjectName("formInput");
    m_fromDate->setDisplayFormat("dd/MM/yyyy");

    auto* toLbl = new QLabel("To:");
    toLbl->setObjectName("fieldLabel");
    m_toDate = new QDateEdit(QDate::currentDate());
    m_toDate->setCalendarPopup(true);
    m_toDate->setObjectName("formInput");
    m_toDate->setDisplayFormat("dd/MM/yyyy");

    auto* filterBtn = new QPushButton("🔍 Filter");
    filterBtn->setObjectName("primaryBtn");
    auto* clearBtn  = new QPushButton("✖ Clear");
    clearBtn->setObjectName("secondaryBtn");
    auto* deleteBtn = new QPushButton("🗑 Delete");
    deleteBtn->setObjectName("dangerBtn");

    connect(filterBtn, &QPushButton::clicked, this, &ViewTransactionsWidget::onFilter);
    connect(clearBtn,  &QPushButton::clicked, this, &ViewTransactionsWidget::onClearFilter);
    connect(deleteBtn, &QPushButton::clicked, this, &ViewTransactionsWidget::onDeleteSelected);

    filterRow->addWidget(catLbl);
    filterRow->addWidget(m_catFilter);
    filterRow->addWidget(fromLbl);
    filterRow->addWidget(m_fromDate);
    filterRow->addWidget(toLbl);
    filterRow->addWidget(m_toDate);
    filterRow->addStretch();
    filterRow->addWidget(filterBtn);
    filterRow->addWidget(clearBtn);
    filterRow->addWidget(deleteBtn);
    root->addWidget(filterCard);

    // ── Table ─────────────────────────────────────────────────────────────────
    m_table = new QTableWidget(0, 6);
    m_table->setObjectName("txTable");
    m_table->setHorizontalHeaderLabels({"ID", "Date", "Category", "Type", "Amount (₹)", "Description"});
    m_table->horizontalHeader()->setSectionResizeMode(5, QHeaderView::Stretch);
    m_table->horizontalHeader()->setObjectName("tableHeader");
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setAlternatingRowColors(true);
    m_table->verticalHeader()->setVisible(false);
    m_table->setColumnWidth(0, 50);
    m_table->setColumnWidth(1, 100);
    m_table->setColumnWidth(2, 120);
    m_table->setColumnWidth(3, 90);
    m_table->setColumnWidth(4, 110);
    root->addWidget(m_table, 1);

    // Count label
    m_countLabel = new QLabel("0 transactions");
    m_countLabel->setObjectName("countLabel");
    root->addWidget(m_countLabel);
}

void ViewTransactionsWidget::refresh() {
    // Rebuild category combo
    m_catFilter->clear();
    m_catFilter->addItem("All");
    for (const auto& c : m_fm->getCategories())
        m_catFilter->addItem(c);

    populateTable(m_fm->getAllTransactions());
}

void ViewTransactionsWidget::populateTable(const std::vector<Transaction>& txns) {
    m_table->setRowCount(0);
    for (const auto& t : txns) {
        int row = m_table->rowCount();
        m_table->insertRow(row);

        auto makeItem = [](const QString& text, Qt::Alignment align = Qt::AlignLeft | Qt::AlignVCenter) {
            auto* item = new QTableWidgetItem(text);
            item->setTextAlignment(align);
            return item;
        };

        bool isInc = (t.getType() == Transaction::INCOME);
        m_table->setItem(row, 0, makeItem(QString::number(t.getId()), Qt::AlignCenter));
        m_table->setItem(row, 1, makeItem(t.getDate().toString("dd-MM-yyyy"), Qt::AlignCenter));
        m_table->setItem(row, 2, makeItem(t.getCategory()));
        m_table->setItem(row, 3, makeItem(t.getTypeString(), Qt::AlignCenter));
        m_table->setItem(row, 4, makeItem(QString::number(t.getAmount(), 'f', 2),
                                          Qt::AlignRight | Qt::AlignVCenter));
        m_table->setItem(row, 5, makeItem(t.getDescription()));

        // Color-code row by type
        QColor rowColor = isInc ? QColor("#E8F5E9") : QColor("#FFEBEE");
        for (int c = 0; c < 6; ++c)
            m_table->item(row, c)->setBackground(rowColor);

        // Store transaction ID in row data
        m_table->item(row, 0)->setData(Qt::UserRole, t.getId());
    }
    m_countLabel->setText(QString("%1 transaction(s) shown").arg(txns.size()));
}

void ViewTransactionsWidget::onFilter() {
    QString cat  = m_catFilter->currentText();
    QDate   from = m_fromDate->date();
    QDate   to   = m_toDate->date();

    auto txns = m_fm->getTransactionsByDateRange(from, to);
    if (cat != "All") {
        std::vector<Transaction> filtered;
        for (const auto& t : txns)
            if (t.getCategory() == cat) filtered.push_back(t);
        populateTable(filtered);
    } else {
        populateTable(txns);
    }
}

void ViewTransactionsWidget::onClearFilter() {
    m_catFilter->setCurrentIndex(0);
    m_fromDate->setDate(QDate::currentDate().addMonths(-1));
    m_toDate->setDate(QDate::currentDate());
    populateTable(m_fm->getAllTransactions());
}

void ViewTransactionsWidget::onDeleteSelected() {
    int row = m_table->currentRow();
    if (row < 0) {
        QMessageBox::information(this, "No Selection", "Please select a row to delete.");
        return;
    }
    int id = m_table->item(row, 0)->data(Qt::UserRole).toInt();
    auto reply = QMessageBox::question(this, "Confirm Delete",
                                       "Delete transaction #" + QString::number(id) + "?",
                                       QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        m_fm->removeTransaction(id);
        refresh();
    }
}
