const express = require('express');
const cors = require('cors');
const crypto = require('crypto');
const fs = require('fs');
const path = require('path');

const app = express();
const PORT = process.env.PORT || 3000;
const DATA_DIR = path.join(__dirname, 'data');
const KEYS_FILE = path.join(DATA_DIR, 'keys.json');
const DEVICES_FILE = path.join(DATA_DIR, 'devices.json');
const CONFIG_FILE = path.join(DATA_DIR, 'config.json');
const SESSION_TIMEOUT_MS = 10 * 60 * 1000;
const ADMIN_PASSWORD = process.env.ADMIN_PASSWORD || 'BGM@Admin2024!';
const TOKEN_SECRET = process.env.ADMIN_TOKEN_SECRET || ADMIN_PASSWORD;

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

function ensureDataDir() {
  fs.mkdirSync(DATA_DIR, { recursive: true });
}

function readJson(file, fallback) {
  try {
    return JSON.parse(fs.readFileSync(file, 'utf8'));
  } catch (error) {
    if (error.code !== 'ENOENT') console.error(`Could not read ${file}:`, error.message);
    return fallback;
  }
}

function writeJson(file, value) {
  const tempFile = `${file}.tmp`;
  fs.writeFileSync(tempFile, `${JSON.stringify(value, null, 2)}\n`, 'utf8');
  fs.renameSync(tempFile, file);
}

function positiveInteger(value, fallback) {
  const number = Number(value);
  return Number.isInteger(number) && number > 0 ? number : fallback;
}

ensureDataDir();
const storedConfig = readJson(CONFIG_FILE, {});
let maxDevicesPerKey = positiveInteger(
  process.env.MAX_DEVICES_PER_KEY,
  positiveInteger(storedConfig.maxDevicesPerKey, 1)
);

const storedKeys = readJson(KEYS_FILE, []);
const keys = new Map();
for (const item of Array.isArray(storedKeys) ? storedKeys : []) {
  if (!item || !item.key) continue;
  keys.set(item.key, {
    key: item.key,
    createdAt: item.createdAt,
    expiresAt: item.expiresAt,
    devices: new Set(Array.isArray(item.devices) ? item.devices : []),
    active: item.active !== false
  });
}

const storedDevices = readJson(DEVICES_FILE, []);
const sessions = new Map();
for (const item of Array.isArray(storedDevices) ? storedDevices : []) {
  if (item && item.deviceId && item.key && keys.has(item.key)) {
    sessions.set(item.deviceId, {
      key: item.key,
      lastHeartbeat: item.lastHeartbeat
    });
  }
}

function saveData() {
  writeJson(KEYS_FILE, Array.from(keys.values()).map(data => ({
    key: data.key,
    createdAt: data.createdAt,
    expiresAt: data.expiresAt,
    devices: Array.from(data.devices),
    active: data.active
  })));
  writeJson(DEVICES_FILE, Array.from(sessions.entries()).map(([deviceId, session]) => ({
    deviceId,
    key: session.key,
    lastHeartbeat: session.lastHeartbeat
  })));
  writeJson(CONFIG_FILE, { maxDevicesPerKey });
}

saveData();

function createAdminToken() {
  const payload = Buffer.from(JSON.stringify({
    exp: Date.now() + 30 * 24 * 60 * 60 * 1000
  })).toString('base64url');
  const signature = crypto.createHmac('sha256', TOKEN_SECRET).update(payload).digest('base64url');
  return `${payload}.${signature}`;
}

function isValidAdminToken(token) {
  if (!token || typeof token !== 'string') return false;
  const [payload, signature] = token.split('.');
  if (!payload || !signature) return false;
  const expected = crypto.createHmac('sha256', TOKEN_SECRET).update(payload).digest('base64url');
  if (signature.length !== expected.length || !crypto.timingSafeEqual(Buffer.from(signature), Buffer.from(expected))) return false;
  try {
    return JSON.parse(Buffer.from(payload, 'base64url').toString()).exp > Date.now();
  } catch (_) {
    return false;
  }
}

function adminAuth(req, res, next) {
  const token = req.headers['x-admin-token'] || req.body.adminToken;
  if (!isValidAdminToken(token)) {
    return res.status(401).json({ success: false, message: 'Unauthorized - Invalid or expired admin token' });
  }
  next();
}

function generateKey() {
  const chars = 'ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789';
  let key = 'BGM-';
  for (let i = 0; i < 4; i++) {
    for (let j = 0; j < 4; j++) key += chars.charAt(Math.floor(Math.random() * chars.length));
    if (i < 3) key += '-';
  }
  return key;
}

function isValidKeyFormat(key) {
  return typeof key === 'string' && /^[A-Z0-9\-]{2,50}$/.test(key.toUpperCase());
}

function getActiveDevices(keyData, now = Date.now()) {
  return Array.from(keyData.devices).filter(deviceId => {
    const session = sessions.get(deviceId);
    return session && session.key === keyData.key && now - session.lastHeartbeat < SESSION_TIMEOUT_MS;
  });
}

app.post('/api/generate', adminAuth, (req, res) => {
  const hours = positiveInteger(req.body.hours, 25);
  const customKey = req.body.customKey;
  let key;
  if (customKey) {
    const upperKey = customKey.toUpperCase().trim();
    if (!isValidKeyFormat(upperKey)) {
      return res.json({ success: false, message: 'Invalid key. Use at least 2 characters (letters/numbers/hyphens only)' });
    }
    if (keys.has(upperKey)) return res.json({ success: false, message: 'Key already exists. Please use a different key.' });
    key = upperKey;
  } else {
    do { key = generateKey(); } while (keys.has(key));
  }

  const now = Date.now();
  keys.set(key, { key, createdAt: now, expiresAt: now + hours * 60 * 60 * 1000, devices: new Set(), active: true });
  saveData();
  res.json({ success: true, key, expiresAt: now + hours * 60 * 60 * 1000, hours, isCustom: !!customKey });
});

function verifyKeyLogic(key, deviceId, res) {
  if (!key || !deviceId) return res.json({ success: false, message: 'Missing key or deviceId' });
  const keyData = keys.get(key);
  if (!keyData) return res.json({ success: false, message: 'Invalid key - not found' });
  if (!keyData.active) return res.json({ success: false, message: 'Key has been disabled by admin' });
  if (Date.now() > keyData.expiresAt) return res.json({ success: false, message: 'Key has expired' });

  const activeDevices = getActiveDevices(keyData);
  const existingSession = sessions.get(deviceId);
  const alreadyRegistered = keyData.devices.has(deviceId) && existingSession && existingSession.key === key;
  if (!alreadyRegistered && activeDevices.length >= maxDevicesPerKey) {
    return res.json({ success: false, message: `Device limit reached (${maxDevicesPerKey} device${maxDevicesPerKey === 1 ? '' : 's'} max).` });
  }

  keyData.devices.add(deviceId);
  sessions.set(deviceId, { key, lastHeartbeat: Date.now() });
  saveData();
  res.json({ success: true, message: 'Key verified', expiresAt: keyData.expiresAt, timeLeft: keyData.expiresAt - Date.now() });
}

app.get('/api/verify', (req, res) => verifyKeyLogic(req.query.key, req.query.deviceId, res));
app.post('/api/verify', (req, res) => verifyKeyLogic(req.body.key, req.body.deviceId, res));

app.post('/api/heartbeat', (req, res) => {
  const { key, deviceId } = req.body;
  const keyData = keys.get(key);
  const session = sessions.get(deviceId);
  if (!key || !deviceId || !keyData || !keyData.active || Date.now() > keyData.expiresAt) {
    return res.json({ success: false, message: 'Key invalid or expired' });
  }
  if (!keyData.devices.has(deviceId) || !session || session.key !== key) {
    return res.json({ success: false, message: 'Device not registered for this key' });
  }
  session.lastHeartbeat = Date.now();
  saveData();
  res.json({ success: true, timeLeft: keyData.expiresAt - Date.now() });
});

app.post('/api/admin/login', (req, res) => {
  if (req.body.password === ADMIN_PASSWORD) return res.json({ success: true, token: createAdminToken() });
  res.json({ success: false, message: 'Wrong password' });
});

app.get('/api/settings', adminAuth, (req, res) => res.json({ maxDevicesPerKey }));

app.post('/api/settings', adminAuth, (req, res) => {
  const requestedLimit = positiveInteger(req.body.maxDevicesPerKey, 0);
  if (!requestedLimit || requestedLimit > 100000) {
    return res.status(400).json({ success: false, message: 'maxDevicesPerKey must be a whole number from 1 to 100000' });
  }
  maxDevicesPerKey = requestedLimit;
  saveData();
  res.json({ success: true, maxDevicesPerKey });
});

app.get('/api/keys', adminAuth, (req, res) => {
  const now = Date.now();
  const keyList = Array.from(keys.values()).map(data => ({
    key: data.key,
    createdAt: data.createdAt,
    expiresAt: data.expiresAt,
    active: data.active,
    expired: now > data.expiresAt,
    deviceCount: getActiveDevices(data, now).length,
    maxDevices: maxDevicesPerKey,
    timeLeft: Math.max(0, data.expiresAt - now)
  })).sort((a, b) => b.createdAt - a.createdAt);
  res.json(keyList);
});

app.post('/api/disable', adminAuth, (req, res) => {
  const keyData = keys.get(req.body.key);
  if (!keyData) return res.json({ success: false, message: 'Key not found' });
  keyData.active = false;
  saveData();
  res.json({ success: true, message: 'Key disabled' });
});

app.post('/api/enable', adminAuth, (req, res) => {
  const keyData = keys.get(req.body.key);
  if (!keyData) return res.json({ success: false, message: 'Key not found' });
  keyData.active = true;
  saveData();
  res.json({ success: true, message: 'Key enabled' });
});

app.post('/api/delete', adminAuth, (req, res) => {
  if (!keys.has(req.body.key)) return res.json({ success: false, message: 'Key not found' });
  keys.delete(req.body.key);
  for (const [deviceId, session] of sessions) if (session.key === req.body.key) sessions.delete(deviceId);
  saveData();
  res.json({ success: true, message: 'Key deleted' });
});

setInterval(() => {
  const now = Date.now();
  let changed = false;
  for (const [deviceId, session] of sessions) {
    if (now - session.lastHeartbeat > SESSION_TIMEOUT_MS || !keys.has(session.key)) {
      sessions.delete(deviceId);
      const keyData = keys.get(session.key);
      if (keyData) keyData.devices.delete(deviceId);
      changed = true;
    }
  }
  if (changed) saveData();
}, 60 * 1000);

app.listen(PORT, () => {
  console.log(`BGMi Key Server running on port ${PORT}`);
  console.log(`Max devices per key: ${maxDevicesPerKey}`);
  console.log(`JSON data directory: ${DATA_DIR}`);
});
