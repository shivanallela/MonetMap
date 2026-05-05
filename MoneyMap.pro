QT += core gui widgets

CONFIG += c++17

TARGET = MoneyMap
TEMPLATE = app

SOURCES += \
    main.cpp \
    Transaction.cpp \
    Budget.cpp \
    FinanceManager.cpp \
    MainWindow.cpp \
    DashboardWidget.cpp \
    AddTransactionWidget.cpp \
    ViewTransactionsWidget.cpp \
    BudgetWidget.cpp \
    ReportWidget.cpp

HEADERS += \
    Transaction.h \
    Budget.h \
    FinanceManager.h \
    MainWindow.h \
    DashboardWidget.h \
    AddTransactionWidget.h \
    ViewTransactionsWidget.h \
    BudgetWidget.h \
    ReportWidget.h

RESOURCES += resources.qrc
