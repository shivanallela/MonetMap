#pragma once
#include <QWidget>
#include "FinanceManager.h"

class QComboBox;
class QSpinBox;
class QVBoxLayout;
class QLabel;

/**
 * @brief Monthly report screen – shows income/expense/savings summary
 *        and category-wise budget usage table.
 */
class ReportWidget : public QWidget {
    Q_OBJECT
public:
    explicit ReportWidget(FinanceManager* fm, QWidget* parent = nullptr);
    void refresh();

private slots:
    void onGenerateReport();

private:
    void buildUI();
    void showReport(int year, int month);

    FinanceManager* m_fm;

    QComboBox*   m_monthBox  = nullptr;
    QSpinBox*    m_yearSpin  = nullptr;
    QLabel*      m_incLbl    = nullptr;
    QLabel*      m_expLbl    = nullptr;
    QLabel*      m_savLbl    = nullptr;
    QLabel*      m_pctLbl    = nullptr;
    QVBoxLayout* m_catLayout = nullptr;
    QWidget*     m_catArea   = nullptr;
    QLabel*      m_tipLbl    = nullptr;
};
