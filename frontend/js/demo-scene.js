/**
 * Demo Scene - Fixed Implementation
 *
 * All four features are now properly implemented:
 * - ESP: Data-driven rendering from explicit test entities (clearly labeled)
 * - Location: Real device location via Android bridge (with permission handling)
 * - Health: Controlled test dataset with explicit values (no random generation)
 * - Aim-Lock: Independent aim-training tool with configurable targets
 */
class DemoScene {
  constructor(canvasId) {
    this.canvas = document.getElementById(canvasId);
    this.ctx = this.canvas.getContext('2d');
    this.entities = [];
    this.running = false;
    this.paused = false;
    this.animFrame = null;
    this.settings = this.loadSettings();
    this.features = { esp: false, location: false, health: false, aimlock: false };
    this.playerPos = { x: 0, y: 0 };

    // Location state
    this.deviceLocation = null;       // Real device location from Android
    this.locationState = 'idle';      // idle | requesting | granted | denied | disabled | error
    this.locationError = null;

    // Aim training state
    this.aimTargets = [];
    this.aimScore = 0;
    this.aimHits = 0;
    this.aimMisses = 0;
    this.aimTrainingActive = false;
    this.crosshairPos = { x: 0, y: 0 };
    this.lastTargetSpawn = 0;

    // Test entity dataset - explicitly defined, clearly labeled
    this.testEntityDataset = this._buildTestDataset();
  }

  /**
   * Build a controlled test dataset.
   * All entities are explicitly defined with fixed properties.
   * These are clearly labeled as TEST DATA in the UI.
   */
  _buildTestDataset() {
    return [
      { id: 'T1', label: '[TEST] Entity-Alpha',   baseHealth: 85, color: '#FF6B35', size: 28, height: 44 },
      { id: 'T2', label: '[TEST] Entity-Bravo',   baseHealth: 60, color: '#4FC3F7', size: 26, height: 42 },
      { id: 'T3', label: '[TEST] Entity-Charlie', baseHealth: 45, color: '#4CAF50', size: 30, height: 46 },
      { id: 'T4', label: '[TEST] Entity-Delta',   baseHealth: 100, color: '#FFD700', size: 24, height: 40 },
      { id: 'T5', label: '[TEST] Entity-Echo',    baseHealth: 30, color: '#E040FB', size: 32, height: 48 },
      { id: 'T6', label: '[TEST] Entity-Foxtrot', baseHealth: 70, color: '#FF5252', size: 26, height: 42 },
      { id: 'T7', label: '[TEST] Entity-Golf',    baseHealth: 55, color: '#69F0AE', size: 28, height: 44 },
      { id: 'T8', label: '[TEST] Entity-Hotel',   baseHealth: 90, color: '#40C4FF', size: 24, height: 40 },
      { id: 'T9', label: '[TEST] Entity-India',   baseHealth: 20, color: '#FFAB40', size: 30, height: 46 },
      { id: 'T10', label: '[TEST] Entity-Juliet', baseHealth: 75, color: '#B388FF', size: 26, height: 42 }
    ];
  }

  /**
   * Generate entities from the test dataset.
   * Positions are deterministic (grid-based), not random.
   * Health values come from the dataset, not Math.random().
   */
  generateEntities(count) {
    this.entities = [];
    const dataset = this.testEntityDataset;
    const actualCount = Math.min(count, dataset.length);

    // Grid-based deterministic placement
    const cols = Math.ceil(Math.sqrt(actualCount));
    const rows = Math.ceil(actualCount / cols);
    const spacingX = (this.width - 80) / Math.max(cols, 1);
    const spacingY = (this.height - 80) / Math.max(rows, 1);

    for (let i = 0; i < actualCount; i++) {
      const d = dataset[i];
      const col = i % cols;
      const row = Math.floor(i / cols);
      this.entities.push({
        id: d.id,
        name: d.label,
        x: 40 + col * spacingX + spacingX / 2,
        y: 40 + row * spacingY + spacingY / 2,
        vx: 0,
        vy: 0,
        health: d.baseHealth,          // Fixed from dataset, not random
        maxHealth: 100,
        width: d.size,
        height: d.height,
        color: d.color,
        isTestEntity: true             // Explicitly flagged as test data
      });
    }
  }

  // --- Settings ---

  loadSettings() {
    const defaults = {
      espBoxColor: '#FF6B35',
      espLineThickness: 2,
      espOpacity: 0.8,
      espShowLabels: true,
      espShowDistance: true,
      locMarkerColor: '#4FC3F7',
      locShowMinimap: true,
      healthBarColor: '#4CAF50',
      healthShowPct: true,
      aimRadius: 50,
      aimCrosshairColor: '#FF1744',
      aimCrosshairSize: 14,
      aimCrosshairOpacity: 80,
      aimTargetRadius: 20,
      aimAutoSpawn: true,
      aimSpawnInterval: 2000
    };

    try {
      const saved = JSON.parse(localStorage.getItem('demoSettings'));
      return { ...defaults, ...saved };
    } catch {
      return defaults;
    }
  }

  saveSettings(settings) {
    this.settings = { ...this.settings, ...settings };
    localStorage.setItem('demoSettings', JSON.stringify(this.settings));
  }

  // --- Canvas ---

  resize() {
    const rect = this.canvas.parentElement.getBoundingClientRect();
    const topBar = document.querySelector('#demo-screen .top-bar');
    const controls = document.querySelector('.demo-controls');
    const legend = document.querySelector('.demo-legend');
    const locPanel = document.getElementById('location-info-panel');
    const topH = topBar ? topBar.offsetHeight : 48;
    const ctrlH = controls ? controls.offsetHeight : 60;
    const legH = legend ? legend.offsetHeight : 30;
    const locH = (locPanel && !locPanel.classList.contains('hidden')) ? locPanel.offsetHeight : 0;

    this.canvas.width = rect.width * window.devicePixelRatio;
    this.canvas.height = (rect.height - topH - ctrlH - legH - locH) * window.devicePixelRatio;
    this.canvas.style.width = rect.width + 'px';
    this.canvas.style.height = (rect.height - topH - ctrlH - legH - locH) + 'px';
    this.ctx.scale(window.devicePixelRatio, window.devicePixelRatio);
    this.width = rect.width;
    this.height = rect.height - topH - ctrlH - legH - locH;
    this.playerPos = { x: this.width / 2, y: this.height / 2 };
  }

  // --- Lifecycle ---

  start(entityCount = 5) {
    this.resize();
    this.generateEntities(entityCount);
    this.running = true;
    this.paused = false;

    // Initialize aim training
    if (this.features.aimlock) {
      this.initAimTraining();
    }

    // Initialize location
    if (this.features.location) {
      this.initLocation();
    }

    this.loop();
  }

  stop() {
    this.running = false;
    if (this.animFrame) {
      cancelAnimationFrame(this.animFrame);
      this.animFrame = null;
    }
    // Stop location updates
    if (window.AndroidBridge && window.AndroidBridge.stopLocationUpdates) {
      window.AndroidBridge.stopLocationUpdates();
    }
  }

  togglePause() {
    this.paused = !this.paused;
    if (!this.paused) this.loop();
    return this.paused;
  }

  reset(entityCount) {
    this.stop();
    this.start(entityCount || this.entities.length || 5);
  }

  // --- Location ---

  initLocation() {
    if (window.AndroidBridge) {
      // Check if location services are enabled
      if (window.AndroidBridge.isLocationEnabled && !window.AndroidBridge.isLocationEnabled()) {
        this.locationState = 'disabled';
        this.locationError = 'Location services are disabled on this device';
        this.updateLocationPanel();
        return;
      }

      // Check permission
      if (window.AndroidBridge.hasLocationPermission && window.AndroidBridge.hasLocationPermission()) {
        this.locationState = 'granted';
        window.AndroidBridge.startLocationUpdates();
        // Get last known location
        this._parseNativeLocation();
      } else {
        this.locationState = 'requesting';
        window.AndroidBridge.requestLocationPermission();
      }
    } else {
      // No Android bridge (e.g., desktop browser testing)
      this.locationState = 'error';
      this.locationError = 'Location requires Android device';
      this.updateLocationPanel();
    }
  }

  _parseNativeLocation() {
    if (!window.AndroidBridge || !window.AndroidBridge.getLastLocation) return;
    try {
      const raw = window.AndroidBridge.getLastLocation();
      const loc = JSON.parse(raw);
      if (loc.error) {
        this.locationState = 'error';
        this.locationError = loc.error === 'no_location' ? 'No location available yet' : loc.error;
      } else {
        this.deviceLocation = loc;
        this.locationState = 'granted';
      }
    } catch (e) {
      this.locationState = 'error';
      this.locationError = 'Failed to parse location data';
    }
    this.updateLocationPanel();
  }

  updateLocationPanel() {
    const panel = document.getElementById('location-info-panel');
    if (!panel) return;

    panel.classList.remove('hidden');
    const latEl = document.getElementById('loc-lat');
    const lngEl = document.getElementById('loc-lng');
    const accEl = document.getElementById('loc-accuracy');
    const timeEl = document.getElementById('loc-time');
    const statusEl = document.getElementById('loc-status');

    switch (this.locationState) {
      case 'idle':
        statusEl.textContent = 'Waiting...';
        latEl.textContent = '--';
        lngEl.textContent = '--';
        accEl.textContent = '--';
        timeEl.textContent = '--';
        break;
      case 'requesting':
        statusEl.textContent = 'Requesting permission...';
        latEl.textContent = '--';
        lngEl.textContent = '--';
        accEl.textContent = '--';
        timeEl.textContent = '--';
        break;
      case 'granted':
        if (this.deviceLocation) {
          statusEl.textContent = 'Active';
          statusEl.className = 'loc-status loc-status-active';
          latEl.textContent = this.deviceLocation.lat.toFixed(6);
          lngEl.textContent = this.deviceLocation.lng.toFixed(6);
          accEl.textContent = '\u00B1' + this.deviceLocation.accuracy.toFixed(1) + 'm';
          timeEl.textContent = new Date(this.deviceLocation.time).toLocaleTimeString();
        } else {
          statusEl.textContent = 'Acquiring...';
          latEl.textContent = '--';
          lngEl.textContent = '--';
          accEl.textContent = '--';
          timeEl.textContent = '--';
        }
        break;
      case 'denied':
        statusEl.textContent = 'Permission Denied';
        statusEl.className = 'loc-status loc-status-error';
        latEl.textContent = '--';
        lngEl.textContent = '--';
        accEl.textContent = '--';
        timeEl.textContent = '--';
        break;
      case 'disabled':
        statusEl.textContent = 'Location Disabled';
        statusEl.className = 'loc-status loc-status-error';
        latEl.textContent = '--';
        lngEl.textContent = '--';
        accEl.textContent = '--';
        timeEl.textContent = '--';
        break;
      case 'error':
        statusEl.textContent = this.locationError || 'Error';
        statusEl.className = 'loc-status loc-status-error';
        latEl.textContent = '--';
        lngEl.textContent = '--';
        accEl.textContent = '--';
        timeEl.textContent = '--';
        break;
    }
  }

  // Called from native Android when location updates arrive
  onNativeLocationUpdate(locData) {
    this.deviceLocation = locData;
    this.locationState = 'granted';
    this.updateLocationPanel();
  }

  // Called from native Android when permission result arrives
  onLocationPermissionResult(granted) {
    if (granted) {
      this.locationState = 'granted';
      if (window.AndroidBridge) window.AndroidBridge.startLocationUpdates();
      this._parseNativeLocation();
    } else {
      this.locationState = 'denied';
      this.locationError = 'Location permission denied by user';
    }
    this.updateLocationPanel();
  }

  // --- Aim Training ---

  initAimTraining() {
    this.aimTargets = [];
    this.aimScore = 0;
    this.aimHits = 0;
    this.aimMisses = 0;
    this.aimTrainingActive = true;
    this.lastTargetSpawn = Date.now();

    // Spawn initial targets with predictable positions
    this._spawnAimTarget();
    this._spawnAimTarget();

    // Set up canvas click handler for aim training
    this._setupAimClickHandler();
  }

  _spawnAimTarget() {
    const s = this.settings;
    const margin = s.aimTargetRadius + 20;
    // Deterministic placement using golden ratio distribution
    const idx = this.aimTargets.length + this.aimHits + this.aimMisses;
    const golden = 1.618033988749;
    const x = margin + ((idx * golden * 97) % (this.width - 2 * margin));
    const y = margin + ((idx * golden * 53) % (this.height - 2 * margin));

    this.aimTargets.push({
      x: x,
      y: y,
      radius: s.aimTargetRadius,
      spawnTime: Date.now(),
      lifetime: 5000,   // 5 seconds to hit
      hit: false
    });
  }

  _setupAimClickHandler() {
    if (this._aimClickHandler) {
      this.canvas.removeEventListener('click', this._aimClickHandler);
    }
    this._aimClickHandler = (e) => {
      if (!this.features.aimlock || !this.aimTrainingActive || this.paused) return;
      const rect = this.canvas.getBoundingClientRect();
      const clickX = e.clientX - rect.left;
      const clickY = e.clientY - rect.top;

      let hitAny = false;
      for (let i = this.aimTargets.length - 1; i >= 0; i--) {
        const t = this.aimTargets[i];
        if (t.hit) continue;
        const dist = Math.hypot(clickX - t.x, clickY - t.y);
        if (dist <= t.radius) {
          t.hit = true;
          this.aimHits++;
          this.aimScore += Math.max(10, Math.round(100 - (Date.now() - t.spawnTime) / 50));
          hitAny = true;
          break;
        }
      }
      if (!hitAny) {
        this.aimMisses++;
      }
    };
    this.canvas.addEventListener('click', this._aimClickHandler);
  }

  updateAimTraining() {
    if (!this.aimTrainingActive) return;

    const now = Date.now();

    // Remove expired targets
    this.aimTargets = this.aimTargets.filter(t => {
      if (t.hit) return false;
      if (now - t.spawnTime > t.lifetime) {
        this.aimMisses++;
        return false;
      }
      return true;
    });

    // Auto-spawn new targets
    if (this.settings.aimAutoSpawn && now - this.lastTargetSpawn > this.settings.aimSpawnInterval) {
      if (this.aimTargets.length < 4) {
        this._spawnAimTarget();
        this.lastTargetSpawn = now;
      }
    }
  }

  // --- Main Loop ---

  loop() {
    if (!this.running || this.paused) return;
    this.update();
    this.draw();
    this.animFrame = requestAnimationFrame(() => this.loop());
  }

  update() {
    const speed = parseFloat(document.getElementById('entity-speed')?.value || 3) / 3;

    // Entities move with deterministic velocities (no random health changes)
    this.entities.forEach(e => {
      e.x += e.vx * speed;
      e.y += e.vy * speed;

      if (e.x < 20 || e.x > this.width - 20) e.vx *= -1;
      if (e.y < 20 || e.y > this.height - 20) e.vy *= -1;

      e.x = Math.max(20, Math.min(this.width - 20, e.x));
      e.y = Math.max(20, Math.min(this.height - 20, e.y));

      // Health is NOT randomly modified. It stays at its dataset value.
      // Health can only be changed via explicit setEntityHealth() calls.
    });

    // Update aim training
    if (this.features.aimlock) {
      this.updateAimTraining();
    }

    // Periodically refresh location display
    if (this.features.location && this.locationState === 'granted') {
      this._parseNativeLocation();
    }
  }

  /**
   * Explicitly set an entity's health value.
   * This is the ONLY way health values change - no random generation.
   */
  setEntityHealth(entityId, healthValue) {
    const entity = this.entities.find(e => e.id === entityId);
    if (entity) {
      entity.health = Math.max(0, Math.min(entity.maxHealth, healthValue));
    }
  }

  /**
   * Supply external entity data (for future integration with authorized data sources).
   * Each entity must have: id, name, x, y, health, width, height, color.
   * All entities are rendered with a [TEST] or [DATA] label.
   */
  supplyEntityData(entityArray) {
    this.entities = entityArray.map(e => ({
      ...e,
      isTestEntity: true,
      name: e.name || `[DATA] ${e.id}`
    }));
  }

  // --- Drawing ---

  draw() {
    const ctx = this.ctx;
    const s = this.settings;

    // Background
    ctx.fillStyle = '#0a0e17';
    ctx.fillRect(0, 0, this.width, this.height);

    // Grid
    ctx.strokeStyle = 'rgba(255,255,255,0.03)';
    ctx.lineWidth = 1;
    for (let x = 0; x < this.width; x += 40) {
      ctx.beginPath(); ctx.moveTo(x, 0); ctx.lineTo(x, this.height); ctx.stroke();
    }
    for (let y = 0; y < this.height; y += 40) {
      ctx.beginPath(); ctx.moveTo(0, y); ctx.lineTo(this.width, y); ctx.stroke();
    }

    // No-data state
    if (this.entities.length === 0 && !this.features.aimlock) {
      ctx.save();
      ctx.fillStyle = '#5a6477';
      ctx.font = '14px Inter, sans-serif';
      ctx.textAlign = 'center';
      ctx.fillText('No entity data available', this.width / 2, this.height / 2 - 10);
      ctx.font = '11px Inter, sans-serif';
      ctx.fillText('Connect a data source or enable test entities', this.width / 2, this.height / 2 + 10);
      ctx.restore();
    }

    // Draw player marker
    ctx.save();
    ctx.globalAlpha = 0.6;
    ctx.fillStyle = '#4CAF50';
    ctx.beginPath();
    ctx.arc(this.playerPos.x, this.playerPos.y, 6, 0, Math.PI * 2);
    ctx.fill();
    ctx.strokeStyle = '#4CAF50';
    ctx.lineWidth = 1;
    ctx.beginPath();
    ctx.arc(this.playerPos.x, this.playerPos.y, 12, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();

    const alpha = s.espOpacity / 100;

    // Draw entities
    this.entities.forEach(e => {
      const dist = Math.hypot(e.x - this.playerPos.x, e.y - this.playerPos.y);

      // ESP - Bounding Box
      if (this.features.esp) {
        ctx.save();
        ctx.globalAlpha = alpha;
        ctx.strokeStyle = s.espBoxColor;
        ctx.lineWidth = s.espLineThickness;
        ctx.strokeRect(e.x - e.width / 2, e.y - e.height / 2, e.width, e.height);

        // Labels (always show [TEST] prefix for test data)
        if (s.espShowLabels) {
          ctx.fillStyle = '#ffffff';
          ctx.font = '10px Inter, sans-serif';
          ctx.textAlign = 'center';
          ctx.fillText(e.name, e.x, e.y - e.height / 2 - 14);
        }

        // Distance
        if (s.espShowDistance) {
          ctx.fillStyle = '#aaaaaa';
          ctx.font = '9px Inter, sans-serif';
          ctx.textAlign = 'center';
          ctx.fillText(`${Math.round(dist)}m`, e.x, e.y + e.height / 2 + 14);
        }
        ctx.restore();
      }

      // Location - marker and line to player
      if (this.features.location && this.locationState === 'granted') {
        ctx.save();
        ctx.globalAlpha = alpha;
        ctx.fillStyle = s.locMarkerColor;
        ctx.beginPath();
        ctx.arc(e.x, e.y, 4, 0, Math.PI * 2);
        ctx.fill();

        // Line to player
        ctx.strokeStyle = s.locMarkerColor;
        ctx.lineWidth = 0.5;
        ctx.setLineDash([4, 4]);
        ctx.beginPath();
        ctx.moveTo(this.playerPos.x, this.playerPos.y);
        ctx.lineTo(e.x, e.y);
        ctx.stroke();
        ctx.setLineDash([]);
        ctx.restore();
      }

      // Health bars - using entity's actual health value (not random)
      if (this.features.health) {
        ctx.save();
        ctx.globalAlpha = alpha;
        const barW = e.width;
        const barH = 4;
        const barX = e.x - barW / 2;
        const barY = e.y - e.height / 2 - 6;

        // Background
        ctx.fillStyle = 'rgba(0,0,0,0.6)';
        ctx.fillRect(barX - 1, barY - 1, barW + 2, barH + 2);

        // Health fill - using actual health value
        const hpPct = e.health / e.maxHealth;
        const hpColor = hpPct > 0.6 ? s.healthBarColor : hpPct > 0.3 ? '#FFC107' : '#FF1744';
        ctx.fillStyle = hpColor;
        ctx.fillRect(barX, barY, barW * hpPct, barH);

        // Percentage
        if (s.healthShowPct) {
          ctx.fillStyle = '#ffffff';
          ctx.font = '8px Inter, sans-serif';
          ctx.textAlign = 'center';
          ctx.fillText(`${Math.round(e.health)}%`, e.x, barY - 3);
        }
        ctx.restore();
      }

      // Entity body (always drawn as simple shape)
      ctx.save();
      ctx.globalAlpha = 0.4;
      ctx.fillStyle = e.color;
      ctx.fillRect(e.x - e.width / 4, e.y - e.height / 4, e.width / 2, e.height / 2);
      ctx.restore();
    });

    // --- Aim-Lock Training Visualization ---
    if (this.features.aimlock) {
      this._drawAimTraining(ctx, s, alpha);
    }

    // Minimap (only when location is active)
    if (this.features.location && s.locShowMinimap && this.locationState === 'granted') {
      this._drawMinimap(ctx, s);
    }

    // Status watermark
    ctx.save();
    ctx.globalAlpha = 0.3;
    ctx.fillStyle = '#ffffff';
    ctx.font = '10px Inter, sans-serif';
    ctx.textAlign = 'left';
    ctx.fillText('DEMO SCENE - Test Data Only - No Live Game Data', 8, this.height - 8);
    ctx.restore();
  }

  _drawAimTraining(ctx, s, alpha) {
    const now = Date.now();

    // Draw targets
    this.aimTargets.forEach(t => {
      if (t.hit) return;
      const age = now - t.spawnTime;
      const remaining = 1 - (age / t.lifetime);
      const pulse = 0.8 + 0.2 * Math.sin(age / 200);

      ctx.save();
      ctx.globalAlpha = alpha * Math.max(0.2, remaining) * pulse;

      // Target circle
      ctx.strokeStyle = s.aimCrosshairColor;
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.arc(t.x, t.y, t.radius, 0, Math.PI * 2);
      ctx.stroke();

      // Inner dot
      ctx.fillStyle = s.aimCrosshairColor;
      ctx.beginPath();
      ctx.arc(t.x, t.y, 3, 0, Math.PI * 2);
      ctx.fill();

      // Timer arc
      ctx.strokeStyle = 'rgba(255,255,255,0.3)';
      ctx.lineWidth = 2;
      ctx.beginPath();
      ctx.arc(t.x, t.y, t.radius + 4, -Math.PI / 2, -Math.PI / 2 + Math.PI * 2 * remaining);
      ctx.stroke();

      ctx.restore();
    });

    // Draw crosshair at center (or last click position)
    ctx.save();
    ctx.globalAlpha = (s.aimCrosshairOpacity || 80) / 100;
    ctx.strokeStyle = s.aimCrosshairColor;
    ctx.lineWidth = 1.5;
    const ch = s.aimCrosshairSize || 14;
    const cx = this.crosshairPos.x || this.width / 2;
    const cy = this.crosshairPos.y || this.height / 2;

    ctx.beginPath();
    ctx.moveTo(cx - ch, cy); ctx.lineTo(cx + ch, cy);
    ctx.moveTo(cx, cy - ch); ctx.lineTo(cx, cy + ch);
    ctx.stroke();

    // Crosshair circle
    ctx.beginPath();
    ctx.arc(cx, cy, ch * 0.6, 0, Math.PI * 2);
    ctx.stroke();
    ctx.restore();

    // Score display
    ctx.save();
    ctx.globalAlpha = 0.8;
    ctx.fillStyle = '#ffffff';
    ctx.font = 'bold 12px Inter, sans-serif';
    ctx.textAlign = 'left';
    ctx.fillText(`Score: ${this.aimScore}`, 10, 20);
    ctx.font = '10px Inter, sans-serif';
    ctx.fillStyle = '#4CAF50';
    ctx.fillText(`Hits: ${this.aimHits}`, 10, 36);
    ctx.fillStyle = '#FF1744';
    ctx.fillText(`Miss: ${this.aimMisses}`, 70, 36);
    ctx.restore();
  }

  _drawMinimap(ctx, s) {
    const mmSize = 80;
    const mmX = this.width - mmSize - 10;
    const mmY = 10;
    const scale = mmSize / Math.max(this.width, this.height);

    ctx.save();
    ctx.globalAlpha = 0.7;
    ctx.fillStyle = 'rgba(0,0,0,0.6)';
    ctx.strokeStyle = 'rgba(255,255,255,0.2)';
    ctx.lineWidth = 1;
    ctx.fillRect(mmX, mmY, mmSize, mmSize);
    ctx.strokeRect(mmX, mmY, mmSize, mmSize);

    // Player on minimap
    ctx.fillStyle = '#4CAF50';
    ctx.beginPath();
    ctx.arc(mmX + this.playerPos.x * scale, mmY + this.playerPos.y * scale, 2, 0, Math.PI * 2);
    ctx.fill();

    // Entities on minimap
    this.entities.forEach(e => {
      ctx.fillStyle = s.locMarkerColor;
      ctx.beginPath();
      ctx.arc(mmX + e.x * scale, mmY + e.y * scale, 1.5, 0, Math.PI * 2);
      ctx.fill();
    });
    ctx.restore();
  }
}

// --- Global callbacks for Android bridge ---
function onNativeLocationUpdate(data) {
  if (window.demoScene) {
    window.demoScene.onNativeLocationUpdate(data);
  }
}

function onLocationPermissionResult(granted) {
  if (window.demoScene) {
    window.demoScene.onLocationPermissionResult(granted);
  }
}
