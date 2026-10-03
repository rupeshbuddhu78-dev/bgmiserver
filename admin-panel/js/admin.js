/**
 * Admin Panel JavaScript
 */
(function() {
  'use strict';

  const API_URL = localStorage.getItem('adminApiUrl') || 'http://localhost:3001';
  let accessToken = localStorage.getItem('adminAccessToken');
  let currentPage = { users: 1, licenses: 1, logs: 1 };

  // ===== API Helper =====
  async function apiRequest(endpoint, options = {}) {
    const headers = { 'Content-Type': 'application/json' };
    if (accessToken) headers['Authorization'] = `Bearer ${accessToken}`;

    try {
      const res = await fetch(`${API_URL}${endpoint}`, {
        ...options,
        headers,
        body: options.body ? JSON.stringify(options.body) : undefined
      });
      return await res.json();
    } catch {
      return { success: false, message: 'Network error' };
    }
  }

  // ===== Toast =====
  function toast(msg, type = 'info') {
    const c = document.getElementById('toast-container');
    const t = document.createElement('div');
    t.className = `toast toast-${type}`;
    t.textContent = msg;
    c.appendChild(t);
    setTimeout(() => { t.style.opacity = '0'; setTimeout(() => t.remove(), 300); }, 3000);
  }

  // ===== Auth =====
  function initAuth() {
    const form = document.getElementById('admin-login-form');
    const errorEl = document.getElementById('admin-login-error');

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      errorEl.classList.add('hidden');

      const login = document.getElementById('admin-login-input').value.trim();
      const password = document.getElementById('admin-login-pass').value;

      const result = await apiRequest('/api/auth/login', {
        method: 'POST',
        body: { login, password }
      });

      if (result.success) {
        if (result.data.user.role !== 'admin') {
          errorEl.textContent = 'Admin access required';
          errorEl.classList.remove('hidden');
          return;
        }
        accessToken = result.data.accessToken;
        localStorage.setItem('adminAccessToken', accessToken);
        showDashboard();
      } else {
        errorEl.textContent = result.message || 'Login failed';
        errorEl.classList.remove('hidden');
      }
    });

    document.getElementById('admin-logout-btn').addEventListener('click', () => {
      accessToken = null;
      localStorage.removeItem('adminAccessToken');
      document.getElementById('admin-dashboard').classList.remove('active');
      document.getElementById('admin-login').classList.add('active');
    });
  }

  function showDashboard() {
    document.getElementById('admin-login').classList.remove('active');
    document.getElementById('admin-dashboard').classList.add('active');
    loadStats();
  }

  // ===== Navigation =====
  function initNav() {
    document.querySelectorAll('.nav-item').forEach(item => {
      item.addEventListener('click', (e) => {
        e.preventDefault();
        document.querySelectorAll('.nav-item').forEach(n => n.classList.remove('active'));
        item.classList.add('active');
        
        const tab = item.dataset.tab;
        document.querySelectorAll('.tab-content').forEach(t => t.classList.remove('active'));
        document.getElementById(`tab-${tab}`).classList.add('active');

        if (tab === 'stats') loadStats();
        if (tab === 'users') loadUsers();
        if (tab === 'licenses') loadLicenses();
        if (tab === 'logs') loadLogs();
      });
    });
  }

  // ===== Stats =====
  async function loadStats() {
    const result = await apiRequest('/api/admin/stats');
    if (result.success) {
      const d = result.data;
      document.getElementById('stat-total-users').textContent = d.totalUsers;
      document.getElementById('stat-active-users').textContent = d.activeUsers;
      document.getElementById('stat-total-licenses').textContent = d.totalLicenses;
      document.getElementById('stat-active-licenses').textContent = d.activeLicenses;
      document.getElementById('stat-recent-users').textContent = d.recentUsers;
      document.getElementById('stat-recent-logins').textContent = d.recentLogins;
    }
  }

  // ===== Users =====
  async function loadUsers(page = 1, search = '') {
    currentPage.users = page;
    const result = await apiRequest(`/api/admin/users?page=${page}&search=${encodeURIComponent(search)}`);
    
    if (!result.success) return;
    
    const tbody = document.getElementById('users-tbody');
    tbody.innerHTML = '';

    result.data.users.forEach(u => {
      const tr = document.createElement('tr');
      tr.innerHTML = `
        <td>${esc(u.username)}</td>
        <td>${esc(u.email)}</td>
        <td><span class="status-pill status-${u.role === 'admin' ? 'active' : 'inactive'}">${u.role}</span></td>
        <td><span class="status-pill status-${u.status}">${u.status}</span></td>
        <td>${formatDate(u.createdAt)}</td>
        <td>
          ${u.status !== 'active' ? `<button class="btn btn-success btn-sm" onclick="adminActions.updateUserStatus('${u._id}','active')">Activate</button>` : ''}
          ${u.status === 'active' ? `<button class="btn btn-danger btn-sm" onclick="adminActions.updateUserStatus('${u._id}','suspended')">Suspend</button>` : ''}
        </td>
      `;
      tbody.appendChild(tr);
    });

    renderPagination('users-pagination', result.data.pagination, (p) => loadUsers(p, search));
  }

  // ===== Licenses =====
  async function loadLicenses(page = 1, status = '') {
    currentPage.licenses = page;
    const result = await apiRequest(`/api/admin/licenses?page=${page}&status=${status}`);
    
    if (!result.success) return;
    
    const tbody = document.getElementById('licenses-tbody');
    tbody.innerHTML = '';

    result.data.licenses.forEach(l => {
      const tr = document.createElement('tr');
      const owner = l.owner ? esc(l.owner.username) : 'Unassigned';
      tr.innerHTML = `
        <td><code>${esc(l.keyPrefix)}...</code></td>
        <td>${l.plan}</td>
        <td><span class="status-pill status-${l.status}">${l.status}</span></td>
        <td>${owner}</td>
        <td>${l.currentActivations}/${l.activationLimit}</td>
        <td>${l.expiresAt ? formatDate(l.expiresAt) : 'Never'}</td>
        <td>
          ${l.status === 'active' ? `<button class="btn btn-danger btn-sm" onclick="adminActions.revokeLicense('${l._id}')">Revoke</button>` : ''}
          ${l.status === 'active' ? `<button class="btn btn-ghost btn-sm" onclick="adminActions.suspendLicense('${l._id}')">Suspend</button>` : ''}
        </td>
      `;
      tbody.appendChild(tr);
    });

    renderPagination('licenses-pagination', result.data.pagination, (p) => loadLicenses(p, status));
  }

  // ===== Audit Logs =====
  async function loadLogs(page = 1, eventType = '') {
    currentPage.logs = page;
    const result = await apiRequest(`/api/admin/audit-logs?page=${page}&eventType=${eventType}`);
    
    if (!result.success) return;
    
    const tbody = document.getElementById('logs-tbody');
    tbody.innerHTML = '';

    result.data.logs.forEach(l => {
      const tr = document.createElement('tr');
      const user = l.user ? esc(l.user.username) : '--';
      const meta = l.metadata ? JSON.stringify(l.metadata).substring(0, 60) : '--';
      tr.innerHTML = `
        <td><code style="font-size:0.75rem">${l.eventType}</code></td>
        <td>${user}</td>
        <td>${l.ipAddress || '--'}</td>
        <td>${formatDate(l.createdAt)}</td>
        <td style="max-width:200px;overflow:hidden;text-overflow:ellipsis" title="${esc(meta)}">${esc(meta)}</td>
      `;
      tbody.appendChild(tr);
    });

    renderPagination('logs-pagination', result.data.pagination, (p) => loadLogs(p, eventType));
  }

  // ===== Create License =====
  function initCreateLicense() {
    const modal = document.getElementById('create-license-modal');
    const form = document.getElementById('create-license-form');
    const display = document.getElementById('created-key-display');

    document.getElementById('create-license-btn').addEventListener('click', () => {
      display.classList.add('hidden');
      form.reset();
      modal.classList.remove('hidden');
    });

    document.getElementById('cancel-create-license').addEventListener('click', () => {
      modal.classList.add('hidden');
    });

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      const btn = document.getElementById('submit-create-license');
      btn.disabled = true;
      btn.textContent = 'Generating...';

      const result = await apiRequest('/api/admin/licenses', {
        method: 'POST',
        body: {
          plan: document.getElementById('new-license-plan').value,
          activationLimit: parseInt(document.getElementById('new-license-limit').value),
          expiresInDays: parseInt(document.getElementById('new-license-expiry').value),
          notes: document.getElementById('new-license-notes').value
        }
      });

      btn.disabled = false;
      btn.textContent = 'Generate';

      if (result.success) {
        document.getElementById('generated-key-value').textContent = result.data.licenseKey;
        display.classList.remove('hidden');
        toast('License created', 'success');
        loadLicenses();
      } else {
        toast(result.message || 'Failed to create license', 'error');
      }
    });

    document.getElementById('copy-key-btn').addEventListener('click', () => {
      const key = document.getElementById('generated-key-value').textContent;
      navigator.clipboard.writeText(key).then(() => toast('Key copied', 'success'));
    });
  }

  // ===== Search & Filter =====
  function initFilters() {
    let searchTimeout;
    document.getElementById('user-search').addEventListener('input', (e) => {
      clearTimeout(searchTimeout);
      searchTimeout = setTimeout(() => loadUsers(1, e.target.value), 400);
    });

    document.getElementById('license-filter').addEventListener('change', (e) => {
      loadLicenses(1, e.target.value);
    });

    document.getElementById('log-filter').addEventListener('change', (e) => {
      loadLogs(1, e.target.value);
    });
  }

  // ===== Pagination =====
  function renderPagination(containerId, pagination, callback) {
    const container = document.getElementById(containerId);
    container.innerHTML = '';

    if (pagination.pages <= 1) return;

    for (let i = 1; i <= pagination.pages; i++) {
      const btn = document.createElement('button');
      btn.className = `page-btn ${i === pagination.page ? 'active' : ''}`;
      btn.textContent = i;
      btn.addEventListener('click', () => callback(i));
      container.appendChild(btn);
    }
  }

  // ===== Helpers =====
  function esc(str) {
    if (!str) return '';
    const div = document.createElement('div');
    div.textContent = str;
    return div.innerHTML;
  }

  function formatDate(dateStr) {
    if (!dateStr) return '--';
    return new Date(dateStr).toLocaleDateString('en-US', {
      year: 'numeric', month: 'short', day: 'numeric'
    });
  }

  // ===== Global Actions (for inline onclick) =====
  window.adminActions = {
    async updateUserStatus(userId, status) {
      const result = await apiRequest(`/api/admin/users/${userId}/status`, {
        method: 'PATCH',
        body: { status }
      });
      if (result.success) {
        toast(`User ${status}`, 'success');
        loadUsers(currentPage.users);
      } else {
        toast(result.message, 'error');
      }
    },
    async revokeLicense(id) {
      if (!confirm('Revoke this license?')) return;
      const result = await apiRequest(`/api/admin/licenses/${id}/revoke`, { method: 'PATCH' });
      if (result.success) {
        toast('License revoked', 'success');
        loadLicenses(currentPage.licenses);
      } else {
        toast(result.message, 'error');
      }
    },
    async suspendLicense(id) {
      const result = await apiRequest(`/api/admin/licenses/${id}/suspend`, { method: 'PATCH' });
      if (result.success) {
        toast('License suspended', 'success');
        loadLicenses(currentPage.licenses);
      } else {
        toast(result.message, 'error');
      }
    }
  };

  // ===== Init =====
  function init() {
    initAuth();
    initNav();
    initCreateLicense();
    initFilters();

    if (accessToken) {
      // Verify token is still valid
      apiRequest('/api/auth/me').then(result => {
        if (result.success && result.data.user.role === 'admin') {
          showDashboard();
        } else {
          accessToken = null;
          localStorage.removeItem('adminAccessToken');
        }
      });
    }
  }

  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
  } else {
    init();
  }
})();
