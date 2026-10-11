const express = require('express');
const cors = require('cors');
const crypto = require('crypto');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

// Admin password - change this! Set via env var ADMIN_PASSWORD or use default
const ADMIN_PASSWORD = process.env.ADMIN_PASSWORD || 'BGM@Admin2024!';
const MAX_DEVICES_PER_KEY = 1; // Only 1 device per key

app.use(cors());
app.use(express.json());
app.use(express.static('public'));

// In-memory storage
const keys = new Map();
const sessions = new Map();

// Admin authentication middleware
function adminAuth(req, res, next) {
  const token = req.headers['x-admin-token'] || req.body.adminToken;
  if (token !== ADMIN_PASSWORD) {
    return res.status(401).json({ success: false, message: 'Unauthorized - Invalid admin token' });
  }
  next();
}

// Generate random key
function generateKey() {
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  let key = 'BGM-';
  for (let i = 0; i < 4; i++) {
    for (let j = 0; j < 4; j++) {
      key += chars.charAt(Math.floor(Math.random() * chars.length));
    }
    if (i < 3) key += '-';
  }
  return key;
}

// API: Generate new key (admin only - requires auth)
app.post('/api/generate', adminAuth, (req, res) => {
  const hours = req.body.hours || 25;
  const key = generateKey();
  const expiresAt = Date.now() + (hours * 60 * 60 * 1000);
  
  keys.set(key, {
    key: key,
    createdAt: Date.now(),
    expiresAt: expiresAt,
    devices: new Set(),
    active: true
  });
  
  res.json({
    success: true,
    key: key,
    expiresAt: expiresAt,
    hours: hours
  });
});

// API: Verify key (client - no auth needed)
app.post('/api/verify', (req, res) => {
  const { key, deviceId } = req.body;
  
  if (!key || !deviceId) {
    return res.json({ success: false, message: 'Missing key or deviceId' });
  }
  
  const keyData = keys.get(key);
  
  if (!keyData) {
    return res.json({ success: false, message: 'Invalid key - not found' });
  }
  
  if (!keyData.active) {
    return res.json({ success: false, message: 'Key has been disabled by admin' });
  }
  
  if (Date.now() > keyData.expiresAt) {
    return res.json({ success: false, message: 'Key has expired' });
  }
  
  // Check device limit
  const now = Date.now();
  const activeDevices = Array.from(keyData.devices).filter(dId => {
    const session = sessions.get(dId);
    return session && (now - session.lastHeartbeat) < 10 * 60 * 1000;
  });
  
  // If this device is not already registered and we're at the limit
  if (!keyData.devices.has(deviceId) && activeDevices.length >= MAX_DEVICES_PER_KEY) {
    return res.json({ 
      success: false, 
      message: 'Device limit reached (' + MAX_DEVICES_PER_KEY + ' device max). Key is already in use on another device.' 
    });
  }
  
  // Add device to key
  keyData.devices.add(deviceId);
  
  // Create/update session
  sessions.set(deviceId, {
    key: key,
    lastHeartbeat: Date.now()
  });
  
  const timeLeft = keyData.expiresAt - Date.now();
  
  res.json({
    success: true,
    message: 'Key verified',
    expiresAt: keyData.expiresAt,
    timeLeft: timeLeft
  });
});

// API: Heartbeat (client - no auth needed)
app.post('/api/heartbeat', (req, res) => {
  const { key, deviceId } = req.body;
  
  if (!key || !deviceId) {
    return res.json({ success: false, message: 'Missing data' });
  }
  
  const keyData = keys.get(key);
  
  if (!keyData || !keyData.active || Date.now() > keyData.expiresAt) {
    return res.json({ success: false, message: 'Key invalid or expired' });
  }
  
  // Verify this device is registered to this key
  if (!keyData.devices.has(deviceId)) {
    return res.json({ success: false, message: 'Device not registered for this key' });
  }
  
  // Update session
  const session = sessions.get(deviceId);
  if (session && session.key === key) {
    session.lastHeartbeat = Date.now();
  }
  
  const timeLeft = keyData.expiresAt - Date.now();
  
  res.json({
    success: true,
    timeLeft: timeLeft
  });
});

// API: Admin login
app.post('/api/admin/login', (req, res) => {
  const { password } = req.body;
  if (password === ADMIN_PASSWORD) {
    res.json({ success: true, token: ADMIN_PASSWORD });
  } else {
    res.json({ success: false, message: 'Wrong password' });
  }
});

// API: Get all keys (admin only)
app.get('/api/keys', adminAuth, (req, res) => {
  const now = Date.now();
  const keyList = [];
  
  keys.forEach((data, key) => {
    const activeDevices = Array.from(data.devices).filter(deviceId => {
      const session = sessions.get(deviceId);
      return session && (now - session.lastHeartbeat) < 10 * 60 * 1000;
    });
    
    keyList.push({
      key: data.key,
      createdAt: data.createdAt,
      expiresAt: data.expiresAt,
      active: data.active,
      expired: now > data.expiresAt,
      deviceCount: activeDevices.length,
      maxDevices: MAX_DEVICES_PER_KEY,
      timeLeft: Math.max(0, data.expiresAt - now)
    });
  });
  
  keyList.sort((a, b) => b.createdAt - a.createdAt);
  
  res.json(keyList);
});

// API: Disable key (admin only)
app.post('/api/disable', adminAuth, (req, res) => {
  const { key } = req.body;
  
  const keyData = keys.get(key);
  if (keyData) {
    keyData.active = false;
    res.json({ success: true, message: 'Key disabled' });
  } else {
    res.json({ success: false, message: 'Key not found' });
  }
});

// API: Enable key (admin only)
app.post('/api/enable', adminAuth, (req, res) => {
  const { key } = req.body;
  
  const keyData = keys.get(key);
  if (keyData) {
    keyData.active = true;
    res.json({ success: true, message: 'Key enabled' });
  } else {
    res.json({ success: false, message: 'Key not found' });
  }
});

// API: Delete key (admin only)
app.post('/api/delete', adminAuth, (req, res) => {
  const { key } = req.body;
  
  if (keys.has(key)) {
    keys.delete(key);
    res.json({ success: true, message: 'Key deleted' });
  } else {
    res.json({ success: false, message: 'Key not found' });
  }
});

// Cleanup inactive sessions every minute
setInterval(() => {
  const now = Date.now();
  
  sessions.forEach((session, deviceId) => {
    if (now - session.lastHeartbeat > 10 * 60 * 1000) {
      sessions.delete(deviceId);
      
      const keyData = keys.get(session.key);
      if (keyData) {
        keyData.devices.delete(deviceId);
      }
    }
  });
}, 60 * 1000);

app.listen(PORT, () => {
  console.log(`BGMi Key Server running on port ${PORT}`);
  console.log(`Admin password: ${ADMIN_PASSWORD.substring(0, 3)}***`);
  console.log(`Max devices per key: ${MAX_DEVICES_PER_KEY}`);
});
