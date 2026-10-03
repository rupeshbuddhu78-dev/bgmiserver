/**
 * Fireplace Loader - Main Application (Fixed)
 *
 * Changes from original:
 * - Location toggle now requests real Android permissions
 * - Feature descriptions updated to reflect actual behavior
 * - Removed false success states and fabricated indicators
 * - Added proper loading, unavailable, permission-denied, error states
 * - Aim-lock settings now include crosshair size, opacity, target radius
 * - Demo scene exposes global reference for native callbacks
 */
(function() {
  'use strict';

  let demoScene = null;
  let currentUser = null;

  // ===== Screen Navigation =====
  function showScreen(id) {
    document.querySelectorAll('.screen').forEach(s => s.classList.remove('active'));
    const screen = document.getElementById(id);
    if (screen) screen.classList.add('active');
  }

  // ===== Toast Notifications =====
  function showToast(message, type = 'info', duration = 3000) {
    const container = document.getElementById('toast-container');
    const toast = document.createElement('div');
    toast.className = `toast toast-${type}`;
    toast.textContent = message;
    container.appendChild(toast);
    setTimeout(() => {
      toast.style.opacity = '0';
      toast.style.transform = 'translateY(-10px)';
      setTimeout(() => toast.remove(), 300);
    }, duration);
  }

  // ===== Button Loading State =====
  function setBtnLoading(btn, loading) {
    const text = btn.querySelector('.btn-text');
    const loader = btn.querySelector('.btn-loader');
    if (loading) {
      btn.disabled = true;
      if (text) text.classList.add('hidden');
      if (loader) loader.classList.remove('hidden');
    } else {
      btn.disabled = false;
      if (text) text.classList.remove('hidden');
      if (loader) loader.classList.add('hidden');
    }
  }

  // ===== Splash Screen =====
  function initSplash() {
    showScreen('splash-screen');
    setTimeout(() => {
      checkAuthState();
    }, 2800);
  }

  // ===== Auth State =====
  async function checkAuthState() {
    const token = localStorage.getItem('accessToken');
    if (!token) {
      showScreen('login-screen');
      return;
    }

    const result = await api.getMe();
    if (result.success) {
      currentUser = result.data.user;
      await loadDashboard();
    } else {
      api.clearTokens();
      showScreen('login-screen');
    }
  }

  // ===== Login =====
  function initLogin() {
    const form = document.getElementById('login-form');
    const errorEl = document.getElementById('login-error');
    const btn = document.getElementById('login-btn');

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      errorEl.classList.add('hidden');
      setBtnLoading(btn, true);

      const login = document.getElementById('login-input').value.trim();
      const password = document.getElementById('login-password').value;

      const result = await api.login(login, password);
      setBtnLoading(btn, false);

      if (result.success) {
        api.setTokens(result.data.accessToken, result.data.refreshToken);
        currentUser = result.data.user;
        showToast('Login successful', 'success');
        await loadDashboard();
      } else {
        errorEl.textContent = result.message || 'Login failed';
        errorEl.classList.remove('hidden');
      }
    });

    // Navigation links
    document.getElementById('show-register').addEventListener('click', (e) => {
      e.preventDefault();
      showScreen('register-screen');
    });

    document.getElementById('show-login').addEventListener('click', (e) => {
      e.preventDefault();
      showScreen('login-screen');
    });

    document.getElementById('forgot-password-link').addEventListener('click', (e) => {
      e.preventDefault();
      showToast('Password reset: Contact support for assistance', 'info');
    });

    // Password toggle
    document.querySelectorAll('.toggle-password').forEach(btn => {
      btn.addEventListener('click', () => {
        const input = document.getElementById(btn.dataset.target);
        input.type = input.type === 'password' ? 'text' : 'password';
      });
    });
  }

  // ===== Register =====
  function initRegister() {
    const form = document.getElementById('register-form');
    const errorEl = document.getElementById('register-error');
    const btn = document.getElementById('register-btn');

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      errorEl.classList.add('hidden');

      const username = document.getElementById('reg-username').value.trim();
      const email = document.getElementById('reg-email').value.trim();
      const password = document.getElementById('reg-password').value;
      const confirm = document.getElementById('reg-confirm').value;

      if (password !== confirm) {
        errorEl.textContent = 'Passwords do not match';
        errorEl.classList.remove('hidden');
        return;
      }

      setBtnLoading(btn, true);
      const result = await api.register(username, email, password);
      setBtnLoading(btn, false);

      if (result.success) {
        api.setTokens(result.data.accessToken, result.data.refreshToken);
        currentUser = result.data.user;
        showToast('Account created successfully', 'success');
        showScreen('license-screen');
      } else {
        errorEl.textContent = result.message || 'Registration failed';
        errorEl.classList.remove('hidden');
      }
    });
  }

  // ===== License Activation =====
  function initLicense() {
    const form = document.getElementById('license-form');
    const errorEl = document.getElementById('license-error');
    const btn = document.getElementById('license-btn');

    form.addEventListener('submit', async (e) => {
      e.preventDefault();
      errorEl.classList.add('hidden');
      setBtnLoading(btn, true);

      const key = document.getElementById('license-key').value.trim();
      const deviceId = getDeviceId();

      const result = await api.activateLicense(key, deviceId);
      setBtnLoading(btn, false);

      if (result.success) {
        showToast(result.message, 'success');
        await loadDashboard();
      } else {
        errorEl.textContent = result.message || 'Activation failed';
        errorEl.classList.remove('hidden');
      }
    });

    document.getElementById('skip-license-btn').addEventListener('click', () => {
      loadDashboard();
    });
  }

  // ===== Dashboard =====
  async function loadDashboard() {
    showScreen('dashboard-screen');
    updateProfileUI();
    await checkServerStatus();
    await loadLicenseStatus();
  }

  async function checkServerStatus() {
    const textEl = document.getElementById('server-status-text');
    const badgeEl = document.getElementById('server-badge');

    textEl.textContent = 'Checking...';
    badgeEl.textContent = '...';
    badgeEl.className = 'status-badge';

    try {
      const result = await api.getHealth();
      if (result.success) {
        textEl.textContent = 'Connected';
        badgeEl.textContent = 'ONLINE';
        badgeEl.className = 'status-badge badge-online';
      } else {
        throw new Error();
      }
    } catch {
      textEl.textContent = 'Disconnected';
      badgeEl.textContent = 'OFFLINE';
      badgeEl.className = 'status-badge badge-offline';
    }
  }

  async function loadLicenseStatus() {
    const textEl = document.getElementById('license-status-text');
    const badgeEl = document.getElementById('license-badge');
    const planBadge = document.getElementById('plan-badge');
    const subPlan = document.getElementById('sub-plan');
    const subExpires = document.getElementById('sub-expires');
    const subStatus = document.getElementById('sub-status');

    const result = await api.getLicenseStatus();

    if (result.success && result.data.hasLicense) {
      const sub = result.data.subscription;
      const status = sub.status;
      
      textEl.textContent = status.charAt(0).toUpperCase() + status.slice(1);
      badgeEl.textContent = status.toUpperCase();
      badgeEl.className = `status-badge badge-${status === 'active' ? 'active' : 'expired'}`;
      
      planBadge.textContent = sub.plan;
      subPlan.textContent = sub.plan.charAt(0).toUpperCase() + sub.plan.slice(1);
      subExpires.textContent = sub.expiryDate ? new Date(sub.expiryDate).toLocaleDateString() : 'Never';
      subStatus.textContent = status.charAt(0).toUpperCase() + status.slice(1);
    } else {
      textEl.textContent = 'No License';
      badgeEl.textContent = 'NONE';
      badgeEl.className = 'status-badge badge-inactive';
      planBadge.textContent = 'Free';
      subPlan.textContent = 'Free';
      subExpires.textContent = '--';
      subStatus.textContent = 'None';
    }
  }

  function updateProfileUI() {
    if (!currentUser) return;
    document.getElementById('profile-username').textContent = currentUser.username;
    document.getElementById('profile-email').textContent = currentUser.email;
    document.getElementById('profile-role').textContent = currentUser.role;
    document.getElementById('profile-joined').textContent = new Date(currentUser.createdAt).toLocaleDateString();
    document.getElementById('profile-lastlogin').textContent = currentUser.lastLogin ? new Date(currentUser.lastLogin).toLocaleString() : '--';
    document.getElementById('avatar-initial').textContent = currentUser.username.charAt(0).toUpperCase();
  }

  // ===== Demo Scene =====
  function initDemoScene() {
    demoScene = new DemoScene('demo-canvas');
    // Expose globally for native callbacks
    window.demoScene = demoScene;

    document.getElementById('launch-demo-btn').addEventListener('click', () => {
      showScreen('demo-screen');
      const features = {
        esp: document.getElementById('toggle-esp').checked,
        location: document.getElementById('toggle-location').checked,
        health: document.getElementById('toggle-health').checked,
        aimlock: document.getElementById('toggle-aimlock').checked
      };
      demoScene.features = features;
      
      // Show/hide location panel
      const locPanel = document.getElementById('location-info-panel');
      if (locPanel) {
        if (features.location) {
          locPanel.classList.remove('hidden');
        } else {
          locPanel.classList.add('hidden');
        }
      }

      // Check if any feature is enabled
      const anyFeature = features.esp || features.location || features.health || features.aimlock;
      if (!anyFeature) {
        showToast('Enable at least one feature to launch', 'warning');
        showScreen('dashboard-screen');
        return;
      }

      setTimeout(() => {
        const count = parseInt(document.getElementById('entity-count')?.value || 5);
        demoScene.start(count);
      }, 100);
    });

    document.getElementById('demo-back-btn').addEventListener('click', () => {
      demoScene.stop();
      const locPanel = document.getElementById('location-info-panel');
      if (locPanel) locPanel.classList.add('hidden');
      showScreen('dashboard-screen');
    });

    document.getElementById('demo-reset-btn').addEventListener('click', () => {
      const count = parseInt(document.getElementById('entity-count').value);
      demoScene.reset(count);
    });

    document.getElementById('demo-pause-btn').addEventListener('click', () => {
      const paused = demoScene.togglePause();
      showToast(paused ? 'Demo paused' : 'Demo resumed', 'info', 1500);
    });

    // Range inputs
    const entityCount = document.getElementById('entity-count');
    const entitySpeed = document.getElementById('entity-speed');
    
    entityCount.addEventListener('input', () => {
      document.getElementById('entity-count-val').textContent = entityCount.value;
    });
    entitySpeed.addEventListener('input', () => {
      document.getElementById('entity-speed-val').textContent = entitySpeed.value;
    });

    // Resize handler
    window.addEventListener('resize', () => {
      if (demoScene.running) demoScene.resize();
    });

    // Location toggle - request permission when enabled
    document.getElementById('toggle-location').addEventListener('change', (e) => {
      if (e.target.checked && window.AndroidBridge) {
        if (window.AndroidBridge.isLocationEnabled && !window.AndroidBridge.isLocationEnabled()) {
          showToast('Enable location services in device settings', 'warning');
          e.target.checked = false;
          return;
        }
        if (window.AndroidBridge.hasLocationPermission && !window.AndroidBridge.hasLocationPermission()) {
          showToast('Location permission will be requested when demo launches', 'info');
        }
      }
    });
  }

  // ===== Settings =====
  function initSettings() {
    document.getElementById('settings-nav-btn').addEventListener('click', () => {
      showScreen('settings-screen');
      loadSettingsUI();
    });

    document.getElementById('settings-back-btn').addEventListener('click', () => {
      showScreen('dashboard-screen');
    });

    document.getElementById('save-settings-btn').addEventListener('click', () => {
      const settings = gatherSettings();
      if (demoScene) demoScene.saveSettings(settings);
      else localStorage.setItem('demoSettings', JSON.stringify(settings));
      
      const theme = document.getElementById('theme-select').value;
      document.documentElement.setAttribute('data-theme', theme);
      localStorage.setItem('theme', theme);
      
      showToast('Settings saved', 'success');
    });

    document.getElementById('reset-settings-btn').addEventListener('click', () => {
      localStorage.removeItem('demoSettings');
      loadSettingsUI();
      showToast('Settings reset to defaults', 'info');
    });
  }

  function loadSettingsUI() {
    let settings;
    try {
      settings = JSON.parse(localStorage.getItem('demoSettings')) || {};
    } catch { settings = {}; }

    setVal('esp-box-color', settings.espBoxColor || '#FF6B35');
    setVal('esp-line-thickness', settings.espLineThickness || 2);
    setVal('esp-opacity', settings.espOpacity || 80);
    setChecked('esp-show-labels', settings.espShowLabels !== false);
    setChecked('esp-show-distance', settings.espShowDistance !== false);
    setVal('loc-marker-color', settings.locMarkerColor || '#4FC3F7');
    setChecked('loc-show-minimap', settings.locShowMinimap !== false);
    setVal('health-bar-color', settings.healthBarColor || '#4CAF50');
    setChecked('health-show-pct', settings.healthShowPct !== false);
    setVal('aim-radius', settings.aimRadius || 50);
    setVal('aim-crosshair-color', settings.aimCrosshairColor || '#FF1744');
    setVal('aim-crosshair-size', settings.aimCrosshairSize || 14);
    setVal('aim-crosshair-opacity', settings.aimCrosshairOpacity || 80);
    setVal('aim-target-radius', settings.aimTargetRadius || 20);
    setChecked('aim-auto-spawn', settings.aimAutoSpawn !== false);
    
    const theme = localStorage.getItem('theme') || 'dark';
    document.getElementById('theme-select').value = theme;
  }

  function gatherSettings() {
    return {
      espBoxColor: document.getElementById('esp-box-color').value,
      espLineThickness: parseInt(document.getElementById('esp-line-thickness').value),
      espOpacity: parseInt(document.getElementById('esp-opacity').value),
      espShowLabels: document.getElementById('esp-show-labels').checked,
      espShowDistance: document.getElementById('esp-show-distance').checked,
      locMarkerColor: document.getElementById('loc-marker-color').value,
      locShowMinimap: document.getElementById('loc-show-minimap').checked,
      healthBarColor: document.getElementById('health-bar-color').value,
      healthShowPct: document.getElementById('health-show-pct').checked,
      aimRadius: parseInt(document.getElementById('aim-radius').value),
      aimCrosshairColor: document.getElementById('aim-crosshair-color').value,
      aimCrosshairSize: parseInt(document.getElementById('aim-crosshair-size').value),
      aimCrosshairOpacity: parseInt(document.getElementById('aim-crosshair-opacity').value),
      aimTargetRadius: parseInt(document.getElementById('aim-target-radius').value),
      aimAutoSpawn: document.getElementById('aim-auto-spawn').checked
    };
  }

  function setVal(id, val) {
    const el = document.getElementById(id);
    if (el) el.value = val;
  }

  function setChecked(id, val) {
    const el = document.getElementById(id);
    if (el) el.checked = val;
  }

  // ===== Profile =====
  function initProfile() {
    document.getElementById('profile-btn').addEventListener('click', () => {
      updateProfileUI();
      showScreen('profile-screen');
    });

    document.getElementById('profile-back-btn').addEventListener('click', () => {
      showScreen('dashboard-screen');
    });
  }

  // ===== Logout =====
  function initLogout() {
    const modal = document.getElementById('logout-modal');

    document.getElementById('logout-btn').addEventListener('click', () => {
      modal.classList.remove('hidden');
    });

    document.getElementById('cancel-logout').addEventListener('click', () => {
      modal.classList.add('hidden');
    });

    document.getElementById('confirm-logout').addEventListener('click', async () => {
      modal.classList.add('hidden');
      await api.logout();
      currentUser = null;
      showToast('Logged out', 'info');
      showScreen('login-screen');
    });
  }

  // ===== Support =====
  function initSupport() {
    document.getElementById('support-btn').addEventListener('click', () => {
      showToast('Support: Contact via the official channel', 'info');
    });
  }

  // ===== Helpers =====
  function getDeviceId() {
    // Prefer native device ID from Android bridge
    if (window.AndroidBridge && window.AndroidBridge.getDeviceId) {
      return window.AndroidBridge.getDeviceId();
    }
    let id = localStorage.getItem('deviceId');
    if (!id) {
      id = 'dev_' + Math.random().toString(36).substring(2, 15) + Date.now().toString(36);
      localStorage.setItem('deviceId', id);
    }
    return id;
  }

  // ===== Theme =====
  function loadTheme() {
    const theme = localStorage.getItem('theme') || 'dark';
    document.documentElement.setAttribute('data-theme', theme);
  }

  // ===== Initialize =====
  function init() {
    loadTheme();
    initLogin();
    initRegister();
    initLicense();
    initDemoScene();
    initSettings();
    initProfile();
    initLogout();
    initSupport();
    initSplash();
  }

  // Start when DOM is ready
  if (document.readyState === 'loading') {
    document.addEventListener('DOMContentLoaded', init);
  } else {
    init();
  }
})();
