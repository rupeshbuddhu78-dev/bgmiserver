/**
 * API Client for Fireplace Loader
 * Handles all server communication
 */
class ApiClient {
  constructor(baseUrl) {
    this.baseUrl = baseUrl || this.detectBaseUrl();
    this.accessToken = localStorage.getItem('accessToken');
    this.refreshToken = localStorage.getItem('refreshToken');
  }

  detectBaseUrl() {
    // Check if running inside Android WebView
    if (window.AndroidBridge && window.AndroidBridge.getApiUrl) {
      return window.AndroidBridge.getApiUrl();
    }
    // Base URL is the server origin only (no trailing /api); request() adds /api/... routes.
    return (localStorage.getItem('apiBaseUrl') || 'https://bgmiserver-tev5.onrender.com').replace(/\/$/, '');
  }

  setTokens(accessToken, refreshToken) {
    this.accessToken = accessToken;
    this.refreshToken = refreshToken;
    if (accessToken) localStorage.setItem('accessToken', accessToken);
    if (refreshToken) localStorage.setItem('refreshToken', refreshToken);
  }

  clearTokens() {
    this.accessToken = null;
    this.refreshToken = null;
    localStorage.removeItem('accessToken');
    localStorage.removeItem('refreshToken');
  }

  async request(endpoint, options = {}) {
    const url = `${this.baseUrl}${endpoint}`;
    const headers = {
      'Content-Type': 'application/json',
      ...options.headers
    };

    if (this.accessToken) {
      headers['Authorization'] = `Bearer ${this.accessToken}`;
    }

    try {
      const response = await fetch(url, {
        ...options,
        headers,
        body: options.body ? JSON.stringify(options.body) : undefined
      });

      const data = await response.json();

      if (response.status === 401 && this.refreshToken) {
        const refreshed = await this.tryRefresh();
        if (refreshed) {
          headers['Authorization'] = `Bearer ${this.accessToken}`;
          const retryResponse = await fetch(url, { ...options, headers, body: options.body ? JSON.stringify(options.body) : undefined });
          return await retryResponse.json();
        }
      }

      return data;
    } catch (error) {
      return { success: false, message: 'Network error. Please check your connection.' };
    }
  }

  async tryRefresh() {
    try {
      const response = await fetch(`${this.baseUrl}/api/auth/refresh`, {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ refreshToken: this.refreshToken })
      });

      const data = await response.json();
      if (data.success) {
        this.setTokens(data.data.accessToken, data.data.refreshToken);
        return true;
      }
    } catch (e) { /* ignore */ }
    
    this.clearTokens();
    return false;
  }

  // Auth
  async register(username, email, password) {
    return this.request('/api/auth/register', {
      method: 'POST',
      body: { username, email, password }
    });
  }

  async login(login, password) {
    return this.request('/api/auth/login', {
      method: 'POST',
      body: { login, password }
    });
  }

  async logout() {
    const result = await this.request('/api/auth/logout', { method: 'POST' });
    this.clearTokens();
    return result;
  }

  async getMe() {
    return this.request('/api/auth/me');
  }

  // Licenses
  async activateLicense(licenseKey, deviceId) {
    return this.request('/api/licenses/activate', {
      method: 'POST',
      body: { licenseKey: licenseKey.toUpperCase(), deviceId }
    });
  }

  async getLicenseStatus() {
    return this.request('/api/licenses/status');
  }

  async validateLicense() {
    return this.request('/api/licenses/validate', { method: 'POST' });
  }

  // Config
  async getConfig() {
    return this.request('/api/config');
  }

  async getHealth() {
    return this.request('/api/health');
  }

  // Admin
  async adminGetUsers(page = 1, search = '') {
    return this.request(`/api/admin/users?page=${page}&search=${encodeURIComponent(search)}`);
  }

  async adminUpdateUserStatus(userId, status) {
    return this.request(`/api/admin/users/${userId}/status`, {
      method: 'PATCH',
      body: { status }
    });
  }

  async adminCreateLicense(plan, activationLimit, expiresInDays, notes) {
    return this.request('/api/admin/licenses', {
      method: 'POST',
      body: { plan, activationLimit, expiresInDays, notes }
    });
  }

  async adminGetLicenses(page = 1, status = '') {
    return this.request(`/api/admin/licenses?page=${page}&status=${status}`);
  }

  async adminRevokeLicense(id) {
    return this.request(`/api/admin/licenses/${id}/revoke`, { method: 'PATCH' });
  }

  async adminSuspendLicense(id) {
    return this.request(`/api/admin/licenses/${id}/suspend`, { method: 'PATCH' });
  }

  async adminGetAuditLogs(page = 1, eventType = '') {
    return this.request(`/api/admin/audit-logs?page=${page}&eventType=${eventType}`);
  }

  async adminGetStats() {
    return this.request('/api/admin/stats');
  }
}

// Global instance
const api = new ApiClient();
