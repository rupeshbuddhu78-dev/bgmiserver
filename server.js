require('dotenv').config();
const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const rateLimit = require('express-rate-limit');
const fs = require('fs');
const path = require('path');
const crypto = require('crypto');

const app = express();
const PORT = Number(process.env.PORT || 3000);
const HOST = '0.0.0.0';
const APP_ID = process.env.APP_ID || 'education-demo';
const ADMIN_TOKEN = process.env.ADMIN_TOKEN || 'change-this-token';
const DB_FILE = process.env.DATA_FILE || path.join(__dirname, 'data', 'licenses.json');

fs.mkdirSync(path.dirname(DB_FILE), { recursive: true });
if (!fs.existsSync(DB_FILE)) fs.writeFileSync(DB_FILE, '[]');
const readKeys = () => JSON.parse(fs.readFileSync(DB_FILE, 'utf8'));
const writeKeys = (keys) => fs.writeFileSync(DB_FILE, JSON.stringify(keys, null, 2));
const makeKey = () => `DEMO-${crypto.randomBytes(3).toString('hex').toUpperCase()}-${crypto.randomBytes(3).toString('hex').toUpperCase()}`;
const isExpired = (key) => key.expiresAt && new Date(key.expiresAt).getTime() < Date.now();

const PANEL_LICENSE = process.env.PANEL_LICENSE || 'Vm8Lk7Uj2JmsjCPVPVjrLa7zgfx3uz9E';

app.use(helmet({ contentSecurityPolicy: false }));
app.use(cors({ origin: process.env.CORS_ORIGIN || '*' }));
app.use(express.json({ limit: '20kb' }));
app.use(express.urlencoded({ extended: true }));
app.use(rateLimit({ windowMs: 60_000, limit: 120, standardHeaders: true, legacyHeaders: false }));
app.use(express.static(path.join(__dirname, 'public')));

function adminOnly(req, res, next) {
  if (req.get('authorization') !== `Bearer ${ADMIN_TOKEN}`) return res.status(401).json({ error: 'Admin authorization required' });
  next();
}

app.get('/api/health', (_req, res) => res.json({ ok: true, appId: APP_ID, time: new Date().toISOString() }));

app.get('/api/admin/keys', adminOnly, (_req, res) => res.json({ keys: readKeys() }));

app.post('/api/admin/keys', adminOnly, (req, res) => {
  const days = Math.max(1, Math.min(3650, Number(req.body.days || 30)));
  const appId = String(req.body.appId || APP_ID).trim().slice(0, 80);
  const now = new Date();
  const expiresAt = new Date(now.getTime() + days * 86400000).toISOString();
  const record = { id: crypto.randomUUID(), key: makeKey(), appId, createdAt: now.toISOString(), expiresAt, enabled: true, usageCount: 0, lastUsedAt: null, lastDeviceId: null };
  const keys = readKeys(); keys.push(record); writeKeys(keys);
  res.status(201).json(record);
});

app.patch('/api/admin/keys/:id', adminOnly, (req, res) => {
  const keys = readKeys(); const item = keys.find(k => k.id === req.params.id);
  if (!item) return res.status(404).json({ error: 'Key not found' });
  if (typeof req.body.enabled === 'boolean') item.enabled = req.body.enabled;
  if (req.body.expiresAt) item.expiresAt = new Date(req.body.expiresAt).toISOString();
  writeKeys(keys); res.json(item);
});

app.delete('/api/admin/keys/:id', adminOnly, (req, res) => {
  const keys = readKeys(); const next = keys.filter(k => k.id !== req.params.id);
  if (next.length === keys.length) return res.status(404).json({ error: 'Key not found' });
  writeKeys(next); res.status(204).end();
});

app.post('/api/validate-key', (req, res) => {
  const keyValue = String(req.body.key || '').trim().toUpperCase();
  const appId = String(req.body.appId || '').trim();
  const deviceId = String(req.body.deviceId || '').trim().slice(0, 160);
  if (!keyValue || !appId || !deviceId) return res.status(400).json({ valid: false, message: 'key, appId and deviceId are required' });
  const keys = readKeys(); const item = keys.find(k => k.key === keyValue);
  if (!item) return res.status(404).json({ valid: false, message: 'Invalid key' });
  if (!item.enabled) return res.status(403).json({ valid: false, message: 'Key disabled' });
  if (item.appId !== appId) return res.status(403).json({ valid: false, message: 'Key is for a different app' });
  if (isExpired(item)) return res.status(403).json({ valid: false, message: 'Key expired', expiresAt: item.expiresAt });
  item.usageCount += 1; item.lastUsedAt = new Date().toISOString(); item.lastDeviceId = deviceId; writeKeys(keys);
  res.json({ valid: true, message: 'Key accepted', appId: item.appId, expiresAt: item.expiresAt, usageCount: item.usageCount });
});

// App-compatible validate endpoint (form-urlencoded, returns token+rng format)
app.post('/api/validate', (req, res) => {
  const game = String(req.body.game || '').trim();
  const userKey = String(req.body.user_key || '').trim();
  const serial = String(req.body.serial || '').trim();
  if (!userKey || !serial) {
    return res.json({ status: false, reason: 'Missing user_key or serial' });
  }
  const keys = readKeys();
  const item = keys.find(k => k.key === userKey.toUpperCase());
  if (!item) return res.json({ status: false, reason: 'Invalid key' });
  if (!item.enabled) return res.json({ status: false, reason: 'Key disabled' });
  if (isExpired(item)) return res.json({ status: false, reason: 'Key expired' });

  // Generate token matching what the app expects: MD5(game + "-" + user_key + "-" + serial + "-" + PANEL_LICENSE)
  const authString = game + '-' + userKey + '-' + serial + '-' + PANEL_LICENSE;
  const token = crypto.createHash('md5').update(authString).digest('hex');
  const rng = Math.floor(Date.now() / 1000);

  item.usageCount += 1;
  item.lastUsedAt = new Date().toISOString();
  item.lastDeviceId = serial;
  writeKeys(keys);

  res.json({ status: true, data: { token, rng } });
});

app.get('*', (_req, res) => res.sendFile(path.join(__dirname, 'public', 'index.html')));
app.listen(PORT, HOST, () => console.log(`License demo listening on http://${HOST}:${PORT}`));
