/* ═══════════════════════════════════════════════════════
   MoneyMap – app.js   (localStorage persistence)
═══════════════════════════════════════════════════════ */

// ── Data ─────────────────────────────────────────────────
const INCOME_CATS  = ['Salary', 'Parents Sent', 'Other'];
const EXPENSE_CATS = ['Food', 'Transport', 'Entertainment',
                      'Healthcare', 'Utilities', 'Shopping', 'Other'];
const ALL_MONTHS   = ['January','February','March','April','May','June',
                      'July','August','September','October','November','December'];
const TIPS = [
  '💡 Follow the 50/30/20 rule: 50% needs, 30% wants, 20% savings.',
  '💡 Track every small expense – Rs 100 daily = Rs 3,000/month.',
  '💡 Pay yourself first: transfer savings before spending.',
  '💡 Build an emergency fund covering 3-6 months of expenses.',
  '💡 Avoid lifestyle inflation as your income grows.',
  '💡 Review subscriptions monthly – cancel what you don\'t use.',
  '💡 Invest early – compounding works best over the long term.',
  '💡 Set SMART goals: Specific, Measurable, Achievable, Relevant, Time-bound.',
  '💡 Use cash for discretionary spending to stay within limits.',
  '💡 Automate your savings – set it and forget it.',
  '💡 Compare prices before buying – even small savings add up.',
  '💡 Keep your wants and needs list separate.',
];

let transactions = [];
let budgets      = {};      // { category: limit }
let nextId       = 1;
let currentType  = 'income';
let dateOpt      = 'today';

// ── Persistence ───────────────────────────────────────────
function save() {
  localStorage.setItem('mm_tx',      JSON.stringify(transactions));
  localStorage.setItem('mm_budgets', JSON.stringify(budgets));
  localStorage.setItem('mm_nextId',  nextId);
}

function load() {
  transactions = JSON.parse(localStorage.getItem('mm_tx')      || '[]');
  budgets      = JSON.parse(localStorage.getItem('mm_budgets') || '{}');
  nextId       = parseInt(localStorage.getItem('mm_nextId')    || '1', 10);
}

// ── Helpers ───────────────────────────────────────────────
const fmt  = n => 'Rs ' + parseFloat(n).toLocaleString('en-IN', {minimumFractionDigits:2, maximumFractionDigits:2});
const today = () => new Date().toISOString().slice(0, 10);

function monthName(m) {
  return ['Jan','Feb','Mar','Apr','May','Jun',
          'Jul','Aug','Sep','Oct','Nov','Dec'][m-1];
}

function getMonthlyTotals(year, month) {
  const prefix = `${year}-${String(month).padStart(2,'0')}`;
  let inc = 0, exp = 0;
  transactions.forEach(t => {
    if (!t.date.startsWith(prefix)) return;
    if (t.type === 'income')  inc += t.amount;
    else                      exp += t.amount;
  });
  return { inc, exp, sav: inc - exp, pct: inc > 0 ? ((inc-exp)/inc)*100 : 0 };
}

function getBudgetUsage(year, month) {
  const prefix = `${year}-${String(month).padStart(2,'0')}`;
  const spent = {};
  transactions.forEach(t => {
    if (t.type === 'expense' && t.date.startsWith(prefix))
      spent[t.category] = (spent[t.category] || 0) + t.amount;
  });
  return spent;
}

// ── Navigation ────────────────────────────────────────────
const PAGE_TITLES = {
  dashboard: ['Dashboard', "Welcome back! Here's your financial overview."],
  add:       ['Add Transaction', 'Record a new income or expense.'],
  transactions: ['Transactions', 'Browse and filter your transaction history.'],
  budget:    ['Budget Manager', 'Set monthly limits and track spending.'],
  report:    ['Monthly Report', 'Detailed breakdown of your finances.'],
};

function navigate(page) {
  document.querySelectorAll('.page').forEach(p => p.classList.remove('active'));
  document.querySelectorAll('.nav-btn').forEach(b => b.classList.remove('active'));
  document.getElementById('page-' + page).classList.add('active');
  document.querySelector(`[data-page="${page}"]`).classList.add('active');
  const [title, sub] = PAGE_TITLES[page];
  document.getElementById('pageTitle').textContent    = title;
  document.getElementById('pageSubtitle').textContent = sub;

  if (page === 'dashboard')    renderDashboard();
  if (page === 'transactions') renderTransactions();
  if (page === 'budget')       renderBudget();
  if (page === 'report')       renderReport();
  if (page === 'add')          refreshCatSelect();
}

// ── Dashboard ─────────────────────────────────────────────
let barChartInst = null;
function renderDashboard() {
  const now = new Date();
  const { inc, exp, sav, pct } = getMonthlyTotals(now.getFullYear(), now.getMonth()+1);

  const grid = document.getElementById('statGrid');
  grid.innerHTML = `
    <div class="stat-card green">
      <div class="stat-icon">📈</div>
      <div class="stat-label">Income (${monthName(now.getMonth()+1)})</div>
      <div class="stat-value green">${fmt(inc)}</div>
    </div>
    <div class="stat-card red">
      <div class="stat-icon">📉</div>
      <div class="stat-label">Expenses (${monthName(now.getMonth()+1)})</div>
      <div class="stat-value red">${fmt(exp)}</div>
    </div>
    <div class="stat-card purple">
      <div class="stat-icon">💰</div>
      <div class="stat-label">Savings</div>
      <div class="stat-value purple">${fmt(sav)}</div>
    </div>
    <div class="stat-card yellow">
      <div class="stat-icon">📊</div>
      <div class="stat-label">Savings Rate</div>
      <div class="stat-value yellow">${pct.toFixed(1)}%</div>
    </div>
  `;

  // Recent transactions
  const recent = [...transactions].reverse().slice(0, 6);
  const recEl = document.getElementById('recentList');
  if (!recent.length) {
    recEl.innerHTML = '<p class="empty-msg">No transactions yet. Add your first one!</p>';
  } else {
    recEl.innerHTML = recent.map(t => `
      <div class="recent-item">
        <div>
          <div class="recent-cat">${t.category}</div>
          <div class="recent-desc">${t.date} · ${t.desc || ''}</div>
        </div>
        <div class="recent-amt ${t.type === 'income' ? 'inc' : 'exp'}">
          ${t.type === 'income' ? '+' : '-'}${fmt(t.amount)}
        </div>
      </div>
    `).join('');
  }

  // Bar chart – last 6 months
  const labels = [], incData = [], expData = [];
  for (let i = 5; i >= 0; i--) {
    const d = new Date(now.getFullYear(), now.getMonth() - i, 1);
    const { inc: mi, exp: me } = getMonthlyTotals(d.getFullYear(), d.getMonth()+1);
    labels.push(monthName(d.getMonth()+1));
    incData.push(mi); expData.push(me);
  }

  const ctx = document.getElementById('barChart').getContext('2d');
  if (barChartInst) barChartInst.destroy();
  barChartInst = new Chart(ctx, {
    type: 'bar',
    data: {
      labels,
      datasets: [
        { label: 'Income',  data: incData, backgroundColor: 'rgba(34,197,94,0.7)',  borderRadius: 6 },
        { label: 'Expense', data: expData, backgroundColor: 'rgba(239,68,68,0.7)',  borderRadius: 6 },
      ]
    },
    options: {
      responsive: true,
      plugins: { legend: { labels: { color: '#94a3b8', font: { family: 'Inter' } } } },
      scales: {
        x: { ticks: { color: '#94a3b8' }, grid: { color: 'rgba(255,255,255,0.05)' } },
        y: { ticks: { color: '#94a3b8', callback: v => 'Rs'+v }, grid: { color: 'rgba(255,255,255,0.05)' } }
      }
    }
  });
}

// ── Add Transaction ───────────────────────────────────────
function setType(type) {
  currentType = type;
  document.getElementById('btnIncome').classList.toggle('active',  type === 'income');
  document.getElementById('btnExpense').classList.toggle('active', type === 'expense');
  document.getElementById('addBtnLabel').textContent = type === 'income' ? 'Add Income' : 'Add Expense';
  refreshCatSelect();
}

function setDateOpt(opt) {
  dateOpt = opt;
  document.getElementById('dateTodayBtn').classList.toggle('active', opt === 'today');
  document.getElementById('dateCustomBtn').classList.toggle('active', opt === 'custom');
  document.getElementById('fDate').classList.toggle('hidden', opt === 'today');
}

function refreshCatSelect() {
  const sel  = document.getElementById('fCategory');
  const cats = currentType === 'income' ? INCOME_CATS : EXPENSE_CATS;
  sel.innerHTML = cats.map(c => `<option value="${c}">${c}</option>`).join('');
}

function addTransaction() {
  const amount   = parseFloat(document.getElementById('fAmount').value);
  const category = document.getElementById('fCategory').value;
  const desc     = document.getElementById('fDesc').value.trim();
  const date     = dateOpt === 'today'
                   ? today()
                   : document.getElementById('fDate').value;

  const msg = document.getElementById('formMsg');

  if (!amount || amount <= 0) return showFormMsg('Please enter a valid amount.', 'error');
  if (!date)                  return showFormMsg('Please select a date.', 'error');

  transactions.push({ id: nextId++, type: currentType, amount, category, desc: desc || '-', date });
  save();

  // Budget warning for expense
  let extra = '';
  if (currentType === 'expense' && budgets[category]) {
    const now = new Date(date);
    const spent = getBudgetUsage(now.getFullYear(), now.getMonth()+1)[category] || 0;
    if (spent > budgets[category])
      extra = ` ⚠️ Budget exceeded for ${category}!`;
  }

  showFormMsg(`${currentType === 'income' ? 'Income' : 'Expense'} of ${fmt(amount)} added!${extra}`,
              extra ? 'error' : 'success');
  document.getElementById('fAmount').value = '';
  document.getElementById('fDesc').value   = '';
  toast(`${currentType === 'income' ? '✅ Income' : '❌ Expense'} saved!`,
        currentType === 'income' ? 'success' : 'error');
}

function showFormMsg(text, type) {
  const el = document.getElementById('formMsg');
  el.textContent = text;
  el.className = `form-msg ${type}`;
  el.classList.remove('hidden');
  setTimeout(() => el.classList.add('hidden'), 4000);
}

// ── Transactions page ─────────────────────────────────────
function renderTransactions() {
  const typeF  = document.getElementById('filterType').value;
  const catF   = document.getElementById('filterCat').value;
  const monthF = document.getElementById('filterMonth').value;

  // Populate category filter
  const allCats = [...new Set(transactions.map(t => t.category))];
  const catSel  = document.getElementById('filterCat');
  const prevCat = catSel.value;
  catSel.innerHTML = '<option value="all">All Categories</option>' +
    allCats.map(c => `<option value="${c}" ${c===prevCat?'selected':''}>${c}</option>`).join('');

  // Populate month filter
  const allMonths = [...new Set(transactions.map(t => t.date.slice(0,7)))].sort().reverse();
  const monSel    = document.getElementById('filterMonth');
  const prevMon   = monSel.value;
  monSel.innerHTML = '<option value="all">All Months</option>' +
    allMonths.map(m => `<option value="${m}" ${m===prevMon?'selected':''}>${m}</option>`).join('');

  let filtered = [...transactions].reverse().filter(t => {
    if (typeF  !== 'all' && t.type     !== typeF)       return false;
    if (catF   !== 'all' && t.category !== catF)        return false;
    if (monthF !== 'all' && !t.date.startsWith(monthF)) return false;
    return true;
  });

  const body  = document.getElementById('txTableBody');
  const empty = document.getElementById('txEmpty');

  if (!filtered.length) {
    body.innerHTML = ''; empty.classList.remove('hidden'); return;
  }
  empty.classList.add('hidden');
  body.innerHTML = filtered.map(t => `
    <tr>
      <td style="color:var(--text2);font-size:.78rem">#${t.id}</td>
      <td>${t.date}</td>
      <td><span class="badge ${t.type==='income'?'inc':'exp'}">${t.type}</span></td>
      <td>${t.category}</td>
      <td class="${t.type==='income'?'amt-inc':'amt-exp'}">${t.type==='income'?'+':'-'}${fmt(t.amount)}</td>
      <td style="color:var(--text2);font-size:.82rem">${t.desc}</td>
      <td><button class="btn-del" onclick="deleteTransaction(${t.id})" title="Delete">🗑</button></td>
    </tr>
  `).join('');
}

function deleteTransaction(id) {
  transactions = transactions.filter(t => t.id !== id);
  save(); renderTransactions();
  toast('Transaction deleted.', 'error');
}

function clearAllData() {
  if (!confirm('Clear ALL transactions and budgets? This cannot be undone.')) return;
  transactions = []; budgets = {}; nextId = 1;
  save(); renderTransactions();
  toast('All data cleared.', 'error');
}

// ── Budget page ───────────────────────────────────────────
function renderBudget() {
  const sel = document.getElementById('budgetCatSel');
  sel.innerHTML = EXPENSE_CATS.map(c => `<option value="${c}">${c}</option>`).join('');

  const now   = new Date();
  const spent = getBudgetUsage(now.getFullYear(), now.getMonth()+1);

  const list = document.getElementById('budgetList');
  if (!Object.keys(budgets).length) {
    list.innerHTML = '<p class="empty-msg" style="padding:20px;color:var(--text2)">No budgets set yet. Set a limit on the left!</p>';
    return;
  }

  list.innerHTML = Object.entries(budgets).map(([cat, lim]) => {
    const sp  = spent[cat] || 0;
    const pct = Math.min((sp / lim) * 100, 100);
    const cls = pct >= 100 ? 'danger' : pct >= 75 ? 'warn' : 'safe';
    const label = pct >= 100 ? '🔴 Over Budget!' : pct >= 75 ? '🟡 Almost at limit' : '🟢 On track';
    return `
      <div class="budget-item">
        <div class="budget-item-header">
          <span class="budget-cat">${cat}</span>
          <span class="budget-nums">
            <span>${fmt(sp)}</span> / ${fmt(lim)}
          </span>
        </div>
        <div class="progress-bar-wrap">
          <div class="progress-bar ${cls}" style="width:${pct}%"></div>
        </div>
        <div class="budget-status ${cls}">${label} · Remaining: ${fmt(Math.max(lim-sp,0))}</div>
      </div>
    `;
  }).join('');
}

function saveBudget() {
  const cat = document.getElementById('budgetCatSel').value;
  const lim = parseFloat(document.getElementById('budgetLimit').value);
  if (!lim || lim <= 0) return toast('Enter a valid limit.', 'error');
  budgets[cat] = lim;
  save(); renderBudget();
  document.getElementById('budgetLimit').value = '';
  toast(`Budget set for ${cat}!`, 'success');
}

// ── Report page ───────────────────────────────────────────
let pieChartInst = null;
function initReportFilters() {
  const now  = new Date();
  const ySel = document.getElementById('reportYear');
  const mSel = document.getElementById('reportMonth');
  ySel.innerHTML = '';
  for (let y = now.getFullYear(); y >= now.getFullYear()-3; y--)
    ySel.innerHTML += `<option value="${y}" ${y===now.getFullYear()?'selected':''}>${y}</option>`;
  mSel.innerHTML = ALL_MONTHS
    .map((m,i) => `<option value="${i+1}" ${i===now.getMonth()?'selected':''}>${m}</option>`).join('');
}

function renderReport() {
  const year  = parseInt(document.getElementById('reportYear').value);
  const month = parseInt(document.getElementById('reportMonth').value);
  const { inc, exp, sav, pct } = getMonthlyTotals(year, month);
  const spent = getBudgetUsage(year, month);
  const prefix = `${year}-${String(month).padStart(2,'0')}`;

  // Summary
  document.getElementById('reportSummary').innerHTML = `
    <h3 class="card-title">Summary – ${monthName(month)} ${year}</h3>
    <div class="report-row">
      <span class="report-row-label">💚 Total Income</span>
      <span class="report-row-val" style="color:var(--green)">${fmt(inc)}</span>
    </div>
    <div class="report-row">
      <span class="report-row-label">❤️ Total Expenses</span>
      <span class="report-row-val" style="color:var(--red)">${fmt(exp)}</span>
    </div>
    <div class="report-row">
      <span class="report-row-label">💜 Savings</span>
      <span class="report-row-val" style="color:${sav>=0?'var(--accent2)':'var(--red)'}">${fmt(sav)}</span>
    </div>
    <div class="report-row" style="flex-direction:column;align-items:flex-start;gap:8px">
      <div style="display:flex;justify-content:space-between;width:100%">
        <span class="report-row-label">📊 Savings Rate</span>
        <span class="report-row-val" style="color:var(--accent2)">${pct.toFixed(1)}%</span>
      </div>
      <div class="savings-bar-wrap" style="width:100%">
        <div class="savings-bar" style="width:${Math.max(0,Math.min(pct,100))}%"></div>
      </div>
    </div>
  `;

  // Pie chart – expense by category
  const catExp = {};
  transactions.filter(t => t.type==='expense' && t.date.startsWith(prefix))
    .forEach(t => catExp[t.category] = (catExp[t.category]||0) + t.amount);

  const ctx = document.getElementById('pieChart').getContext('2d');
  if (pieChartInst) pieChartInst.destroy();
  if (Object.keys(catExp).length) {
    const COLORS = ['#6c63ff','#22c55e','#ef4444','#f59e0b','#06b6d4','#ec4899','#a78bfa','#34d399'];
    pieChartInst = new Chart(ctx, {
      type: 'doughnut',
      data: {
        labels: Object.keys(catExp),
        datasets: [{ data: Object.values(catExp), backgroundColor: COLORS, borderWidth: 2, borderColor: '#0d0f1a' }]
      },
      options: {
        responsive: true,
        plugins: {
          legend: { labels: { color: '#94a3b8', font: { family: 'Inter', size: 12 } }, position: 'bottom' },
          tooltip: { callbacks: { label: ctx => ` ${ctx.label}: Rs ${ctx.parsed.toLocaleString()}` } }
        }
      }
    });
  }

  // Budget usage
  const bEl = document.getElementById('reportBudgets');
  if (!Object.keys(budgets).length) {
    bEl.innerHTML = '<p class="empty-msg">No budgets configured.</p>';
  } else {
    bEl.innerHTML = Object.entries(budgets).map(([cat, lim]) => {
      const sp  = spent[cat] || 0;
      const pct = Math.min((sp/lim)*100, 100);
      const cls = pct>=100?'danger':pct>=75?'warn':'safe';
      return `
        <div style="margin-bottom:14px">
          <div style="display:flex;justify-content:space-between;margin-bottom:6px">
            <span style="font-weight:600;font-size:.88rem">${cat}</span>
            <span style="font-size:.82rem;color:var(--text2)">${fmt(sp)} / ${fmt(lim)}</span>
          </div>
          <div class="progress-bar-wrap">
            <div class="progress-bar ${cls}" style="width:${pct}%"></div>
          </div>
        </div>
      `;
    }).join('');
  }

  // Tip
  const tipIdx = (month - 1) % TIPS.length;
  document.getElementById('tipCard').innerHTML = `
    <h3 class="card-title">💡 Finance Tip</h3>
    <p class="tip-text">${TIPS[tipIdx]}</p>
  `;
}

// ── Toast ─────────────────────────────────────────────────
let toastTimer;
function toast(msg, type='success') {
  const el = document.getElementById('toast');
  el.textContent = msg;
  el.className   = `toast ${type}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => el.classList.add('hidden'), 3000);
}

// ── AI Advisor ─────────────────────────────────────────────
async function getAISuggestion() {
  const btn = document.getElementById('btnAiAdvisor');
  const textEl = document.getElementById('aiSuggestionText');
  
  btn.disabled = true;
  btn.textContent = '⏳ Thinking...';
  
  try {
    const now = new Date();
    const { inc, exp, sav } = getMonthlyTotals(now.getFullYear(), now.getMonth() + 1);
    
    // Create prompt
    const prompt = `I have a total income of Rs ${inc}, and total expenses of Rs ${exp}. My current savings are Rs ${sav}. Based on this, give me practical financial suggestions in a concise format (just 3-4 bullet points, use "Rs" for currency). Keep it very brief and actionable. Use emojis where appropriate.`;

    let apiKey = localStorage.getItem('mm_groq_key');
    if (!apiKey) {
      apiKey = prompt('Please enter your Groq API Key to use the AI Advisor:');
      if (apiKey) localStorage.setItem('mm_groq_key', apiKey);
      else throw new Error('API Key required');
    }

    const response = await fetch('https://api.groq.com/openai/v1/chat/completions', {
      method: 'POST',
      headers: {
        'Authorization': `Bearer ${apiKey}`,
        'Content-Type': 'application/json'
      },
      body: JSON.stringify({
        model: 'llama-3.1-8b-instant',
        messages: [{ role: 'user', content: prompt }]
      })
    });

    if (!response.ok) throw new Error('API Error');
    const data = await response.json();
    const advice = data.choices[0].message.content;
    
    textEl.innerHTML = advice.replace(/\n/g, '<br>');
    textEl.style.color = 'var(--text)';
  } catch (err) {
    textEl.textContent = '❌ Failed to get AI advice. Check your API key or connection.';
    textEl.style.color = 'var(--red)';
  } finally {
    btn.disabled = false;
    btn.textContent = '✨ Get New Advice';
  }
}

// ── Init ──────────────────────────────────────────────────
(function init() {
  load();
  // Today badge
  const d = new Date();
  document.getElementById('todayBadge').textContent =
    d.toLocaleDateString('en-IN', { weekday:'short', year:'numeric', month:'short', day:'numeric' });

  // Set today as default date
  document.getElementById('fDate').value = today();

  initReportFilters();
  refreshCatSelect();
  renderDashboard();
})();
