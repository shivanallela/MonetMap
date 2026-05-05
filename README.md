# 💰 MoneyMap – Personal Finance Tracker

MoneyMap is a complete, multi-platform personal finance management system built for course project requirements. It includes three separate interfaces sharing the same core logic: a C++ CLI, a C++ Qt Desktop GUI, and a Modern Web Application.

### 🌐 Live Web Version
**Try it instantly in your browser:**  
👉 **[https://moneymap-beta.vercel.app/](https://moneymap-beta.vercel.app/)**

---

## 🎯 Features

- **Transaction Management:** Add, delete, and view Income & Expenses.
- **Categorization:** Separate categories for Income (Salary, Parents Sent, Other) and Expenses.
- **Budgeting System:** Set monthly limits per category with visual progress tracking and warnings.
- **Monthly Reports:** Automatic calculation of total income, expenses, savings, and savings rate.
- **Data Persistence:** 
  - *Web:* Auto-saves to your browser's local storage.
  - *C++ / Qt:* Reads and writes to `categories.csv`, `transactions.csv`, and `budgets.csv`.

---

## 🖥️ How to Run Locally

### 1. Web Version (HTML/CSS/JS)
The easiest way to view the code. No installation required.
1. Open the `/web` folder.
2. Double-click `index.html` to open it in your browser.

### 2. C++ Command Line Interface (CLI)
A fully functional terminal app using standard C++14.
1. Navigate to the `/CLI` folder.
2. Run the compiled executable:
   ```bash
   moneymap.exe
   ```
3. *(To Recompile)*: `g++ -std=c++14 -O2 -o moneymap.exe main.cpp Transaction.cpp Budget.cpp FinanceManager.cpp Display.cpp`

### 3. C++ Qt Desktop GUI
A beautiful, animated desktop application.
1. Ensure **Qt 6.x** and **MinGW 64-bit** are installed via the Qt Maintenance Tool.
2. Open `MoneyMap.pro` in **Qt Creator**.
3. Press **Ctrl+R** (Run) to compile and launch the GUI.

---

## 🛠️ Technology Stack
- **Web App:** Vanilla HTML5, CSS3 (Glassmorphism design), Vanilla JS (ES6), Chart.js
- **Desktop GUI:** C++17, Qt 6 Widgets (QSS Styling, QPropertyAnimation)
- **CLI:** Standard C++14 (OOP, `std::vector`, `std::map`, file I/O)
- **Deployment:** Vercel (Web)

---
*Developed as a comprehensive course end project.*
