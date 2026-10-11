const express = require('express');
const cors = require('cors');
const crypto = require('crypto');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;

app.use(cors());
app.use(express.json());
app.use(express.static('public'));

// In-memory storage
const keys = new Map();
const sessions = new Map();

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

// Generate unique device ID
function generateDeviceId() {
  return crypto.randomBytes(8).toString('hex');
}

// API: Generate new key (admin)
app.post('/api/generate', (req, res) => {
  const key = generateKey();
  const expiresAt = Date.now() + (25 * 60 * 60 * 1000); // 25 hours
  
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
    expiresAt: expiresAt
  });
});

// API: Verify key (client)
app.post('/api/verify', (req, res) => {
  const { key, deviceId } = req.body;
  
  if (!key || !deviceId) {
    return res.json({ success: false, message: 'Missing key or deviceId' });
  }
  
  const keyData = keys.get(key);
  
  if (!keyData) {
    return res.json({ success: false, message: 'Invalid key' });
  }
  
  if (!keyData.active) {
    return res.json({ success: false, message: 'Key disabled' });
  }
  
  if (Date.now() > keyData.expiresAt) {
    return res.json({ success: false, message: 'Key expired' });
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

// API: Heartbeat (client)
app.post('/api/heartbeat', (req, res) => {
  const { key, deviceId } = req.body;
  
  if (!key || !deviceId) {
    return res.json({ success: false, message: 'Missing data' });
  }
  
  const keyData = keys.get(key);
  
  if (!keyData || !keyData.active || Date.now() > keyData.expiresAt) {
    return res.json({ success: false, message: 'Key invalid or expired' });
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

// API: Get all keys (admin panel)
app.get('/api/keys', (req, res) => {
  const now = Date.now();
  const keyList = [];
  
  keys.forEach((data, key) => {
    // Clean up expired devices (no heartbeat for 5 minutes)
    const activeDevices = Array.from(data.devices).filter(deviceId => {
      const session = sessions.get(deviceId);
      return session && (now - session.lastHeartbeat) < 5 * 60 * 1000;
    });
    
    keyList.push({
      key: data.key,
      createdAt: data.createdAt,
      expiresAt: data.expiresAt,
      active: data.active,
      expired: now > data.expiresAt,
      deviceCount: activeDevices.length,
      timeLeft: Math.max(0, data.expiresAt - now)
    });
  });
  
  // Sort by createdAt (newest first)
  keyList.sort((a, b) => b.createdAt - a.createdAt);
  
  res.json(keyList);
});

// API: Disable key (admin)
app.post('/api/disable', (req, res) => {
  const { key } = req.body;
  
  const keyData = keys.get(key);
  if (keyData) {
    keyData.active = false;
    res.json({ success: true, message: 'Key disabled' });
  } else {
    res.json({ success: false, message: 'Key not found' });
  }
});

// API: Delete key (admin)
app.post('/api/delete', (req, res) => {
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
    // Remove sessions with no heartbeat for 10 minutes
    if (now - session.lastHeartbeat > 10 * 60 * 1000) {
      sessions.delete(deviceId);
      
      // Remove device from key
      const keyData = keys.get(session.key);
      if (keyData) {
        keyData.devices.delete(deviceId);
      }
    }
  });
}, 60 * 1000);

app.listen(PORT, () => {
  console.log(`BGMi Key Server running on port ${PORT}`);
});
