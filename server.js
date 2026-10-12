const express = require('express');
const cors = require('cors');
const crypto = require('crypto');
const path = require('path');
const { MongoClient } = require('mongodb');

const app = express();
const PORT = process.env.PORT || 3000;
const SESSION_TIMEOUT_MS = 10 * 60 * 1000;
const ADMIN_PASSWORD = process.env.ADMIN_PASSWORD || 'BGM@Admin2024!';
const TOKEN_SECRET = process.env.ADMIN_TOKEN_SECRET || ADMIN_PASSWORD;

// MongoDB connection - Direct hardcoded (no env variables needed)
const MONGODB_URI = 'mongodb+srv://rupeshdatabase:rupeshsonam@cluster0.xo0d0ot.mongodb.net/bgmiserver?retryWrites=true&w=majority';
const DB_NAME = 'bgmiserver';

let db;
let keysCollection;
let devicesCollection;
let configCollection;

app.use(cors());
app.use(express.json());
app.use(express.static(path.join(__dirname, 'public')));

function positiveInteger(value, fallback) {
  const number = Number(value);
  return Number.isInteger(number) && number > 0 ? number : fallback;
}

// MongoDB connection
async function connectToMongo() {
  try {
    const client = new MongoClient(MONGODB_URI);
    await client.connect();
    console.log('✓ Connected to MongoDB');
    
    db = client.db(DB_NAME);
    keysCollection = db.collection('keys');
    devicesCollection = db.collection('devices');
    configCollection = db.collection('config');
    
    // Ensure indexes
    await keysCollection.createIndex({ key: 1 }, { unique: true });
    await devicesCollection.createIndex({ deviceId: 1 }, { unique: true });
    await devicesCollection.createIndex({ key: 1 });
    
    // Initialize config if not exists
    const config = await configCollection.findOne({ _id: 'settings' });
    if (!config) {
      await configCollection.insertOne({
        _id: 'settings',
        maxDevicesPerKey: positiveInteger(process.env.MAX_DEVICES_PER_KEY, 1)
      });
    }
    
    return true;
  } catch (error) {
    console.error('✗ MongoDB connection failed:', error.message);
    throw error;
  }
}

async function getConfig() {
  const config = await configCollection.findOne({ _id: 'settings' });
  return config || { maxDevicesPerKey: 1 };
}

async function updateConfig(maxDevicesPerKey) {
  await configCollection.updateOne(
    { _id: 'settings' },
    { $set: { maxDevicesPerKey } },
    { upsert: true }
  );
}

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

async function getActiveDevices(keyData, now = Date.now()) {
  const activeDevices = await devicesCollection.find({
    key: keyData.key,
    lastHeartbeat: { $gt: now - SESSION_TIMEOUT_MS }
  }).toArray();
  return activeDevices.map(d => d.deviceId);
}

app.post('/api/generate', adminAuth, async (req, res) => {
  try {
    const hours = positiveInteger(req.body.hours, 25);
    const customKey = req.body.customKey;
    let key;
    
    if (customKey) {
      const upperKey = customKey.toUpperCase().trim();
      if (!isValidKeyFormat(upperKey)) {
        return res.json({ success: false, message: 'Invalid key. Use at least 2 characters (letters/numbers/hyphens only)' });
      }
      const existing = await keysCollection.findOne({ key: upperKey });
      if (existing) return res.json({ success: false, message: 'Key already exists. Please use a different key.' });
      key = upperKey;
    } else {
      do { 
        key = generateKey(); 
        const existing = await keysCollection.findOne({ key });
        if (!existing) break;
      } while (true);
    }

    const now = Date.now();
    await keysCollection.insertOne({
      key,
      createdAt: now,
      expiresAt: now + hours * 60 * 60 * 1000,
      devices: [],
      active: true
    });
    
    res.json({ success: true, key, expiresAt: now + hours * 60 * 60 * 1000, hours, isCustom: !!customKey });
  } catch (error) {
    console.error('Generate error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

async function verifyKeyLogic(key, deviceId, res) {
  try {
    if (!key || !deviceId) return res.json({ success: false, message: 'Missing key or deviceId' });
    
    const keyData = await keysCollection.findOne({ key });
    if (!keyData) return res.json({ success: false, message: 'Invalid key - not found' });
    if (!keyData.active) return res.json({ success: false, message: 'Key has been disabled by admin' });
    if (Date.now() > keyData.expiresAt) return res.json({ success: false, message: 'Key has expired' });

    const config = await getConfig();
    const activeDevices = await getActiveDevices(keyData);
    const existingSession = await devicesCollection.findOne({ deviceId });
    const alreadyRegistered = keyData.devices.includes(deviceId) && existingSession && existingSession.key === key;
    
    if (!alreadyRegistered && activeDevices.length >= config.maxDevicesPerKey) {
      return res.json({ success: false, message: `Device limit reached (${config.maxDevicesPerKey} device${config.maxDevicesPerKey === 1 ? '' : 's'} max).` });
    }

    // Add device to key's device list if not already there
    if (!keyData.devices.includes(deviceId)) {
      await keysCollection.updateOne(
        { key },
        { $addToSet: { devices: deviceId } }
      );
    }
    
    // Update or create device session
    await devicesCollection.updateOne(
      { deviceId },
      { 
        $set: { 
          key, 
          lastHeartbeat: Date.now() 
        } 
      },
      { upsert: true }
    );
    
    res.json({ success: true, message: 'Key verified', expiresAt: keyData.expiresAt, timeLeft: keyData.expiresAt - Date.now() });
  } catch (error) {
    console.error('Verify error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
}

app.get('/api/verify', (req, res) => verifyKeyLogic(req.query.key, req.query.deviceId, res));
app.post('/api/verify', (req, res) => verifyKeyLogic(req.body.key, req.body.deviceId, res));

app.post('/api/heartbeat', async (req, res) => {
  try {
    const { key, deviceId } = req.body;
    const keyData = await keysCollection.findOne({ key });
    const session = await devicesCollection.findOne({ deviceId });
    
    if (!key || !deviceId || !keyData || !keyData.active || Date.now() > keyData.expiresAt) {
      return res.json({ success: false, message: 'Key invalid or expired' });
    }
    if (!keyData.devices.includes(deviceId) || !session || session.key !== key) {
      return res.json({ success: false, message: 'Device not registered for this key' });
    }
    
    await devicesCollection.updateOne(
      { deviceId },
      { $set: { lastHeartbeat: Date.now() } }
    );
    
    res.json({ success: true, timeLeft: keyData.expiresAt - Date.now() });
  } catch (error) {
    console.error('Heartbeat error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.post('/api/admin/login', (req, res) => {
  if (req.body.password === ADMIN_PASSWORD) return res.json({ success: true, token: createAdminToken() });
  res.json({ success: false, message: 'Wrong password' });
});

app.get('/api/settings', adminAuth, async (req, res) => {
  try {
    const config = await getConfig();
    res.json({ maxDevicesPerKey: config.maxDevicesPerKey });
  } catch (error) {
    console.error('Settings error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.post('/api/settings', adminAuth, async (req, res) => {
  try {
    const requestedLimit = positiveInteger(req.body.maxDevicesPerKey, 0);
    if (!requestedLimit || requestedLimit > 100000) {
      return res.status(400).json({ success: false, message: 'maxDevicesPerKey must be a whole number from 1 to 100000' });
    }
    await updateConfig(requestedLimit);
    res.json({ success: true, maxDevicesPerKey: requestedLimit });
  } catch (error) {
    console.error('Update settings error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.get('/api/keys', adminAuth, async (req, res) => {
  try {
    const now = Date.now();
    const allKeys = await keysCollection.find().toArray();
    
    const keyList = [];
    for (const data of allKeys) {
      const activeDevices = await getActiveDevices(data, now);
      keyList.push({
        key: data.key,
        createdAt: data.createdAt,
        expiresAt: data.expiresAt,
        active: data.active,
        expired: now > data.expiresAt,
        deviceCount: activeDevices.length,
        maxDevices: (await getConfig()).maxDevicesPerKey,
        timeLeft: Math.max(0, data.expiresAt - now)
      });
    }
    
    keyList.sort((a, b) => b.createdAt - a.createdAt);
    res.json(keyList);
  } catch (error) {
    console.error('List keys error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.post('/api/disable', adminAuth, async (req, res) => {
  try {
    const result = await keysCollection.updateOne(
      { key: req.body.key },
      { $set: { active: false } }
    );
    if (result.matchedCount === 0) return res.json({ success: false, message: 'Key not found' });
    res.json({ success: true, message: 'Key disabled' });
  } catch (error) {
    console.error('Disable error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.post('/api/enable', adminAuth, async (req, res) => {
  try {
    const result = await keysCollection.updateOne(
      { key: req.body.key },
      { $set: { active: true } }
    );
    if (result.matchedCount === 0) return res.json({ success: false, message: 'Key not found' });
    res.json({ success: true, message: 'Key enabled' });
  } catch (error) {
    console.error('Enable error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

app.post('/api/delete', adminAuth, async (req, res) => {
  try {
    const result = await keysCollection.deleteOne({ key: req.body.key });
    if (result.deletedCount === 0) return res.json({ success: false, message: 'Key not found' });
    
    // Remove all devices associated with this key
    await devicesCollection.deleteMany({ key: req.body.key });
    
    res.json({ success: true, message: 'Key deleted' });
  } catch (error) {
    console.error('Delete error:', error);
    res.status(500).json({ success: false, message: 'Server error' });
  }
});

// Cleanup inactive sessions
setInterval(async () => {
  try {
    const now = Date.now();
    
    // Find expired sessions
    const expiredSessions = await devicesCollection.find({
      $or: [
        { lastHeartbeat: { $lt: now - SESSION_TIMEOUT_MS } }
      ]
    }).toArray();
    
    for (const session of expiredSessions) {
      // Remove device from key's device list
      await keysCollection.updateOne(
        { key: session.key },
        { $pull: { devices: session.deviceId } }
      );
      
      // Delete the session
      await devicesCollection.deleteOne({ deviceId: session.deviceId });
    }
    
    if (expiredSessions.length > 0) {
      console.log(`Cleaned up ${expiredSessions.length} expired sessions`);
    }
  } catch (error) {
    console.error('Cleanup error:', error);
  }
}, 60 * 1000);

// Start server
connectToMongo().then(() => {
  app.listen(PORT, async () => {
    const config = await getConfig();
    console.log(`BGMi Key Server running on port ${PORT}`);
    console.log(`Max devices per key: ${config.maxDevicesPerKey}`);
    console.log(`Database: MongoDB (${DB_NAME})`);
  });
}).catch(error => {
  console.error('Failed to start server:', error);
  process.exit(1);
});
