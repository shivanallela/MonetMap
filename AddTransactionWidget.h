#pragma once
#include <QWidget>
#include "FinanceManager.h"

class QLineEdit;
class QComboBox;
class QDateEdit;
class QRadioButton;
class QLabel;

/**
 * @brief Form for adding a new income / expense transaction.
 */
class AddTransactionWidget : public QWidget {
    Q_OBJECT
public:
    explicit AddTransactionWidget(FinanceManager* fm, QWidget* parent = nullptr);
    void resetForm();

signals:
    void transactionAdded();

private slots:
    void onSubmit();
    void onAddCategory();

private:
    void buildUI();
    void populateCategoryCombo();

    FinanceManager* m_fm;

    QLineEdit*    m_amountEdit  = nullptr;
    QComboBox*    m_categoryBox = nullptr;
    QLineEdit*    m_descEdit    = nullptr;
    QDateEdit*    m_dateEdit    = nullptr;
    QRadioButton* m_incomeRad   = nullptr;
    QRadioButton* m_expenseRad  = nullptr;
    QLineEdit*    m_newCatEdit  = nullptr;
    QLabel*       m_statusLabel = nullptr;
};
