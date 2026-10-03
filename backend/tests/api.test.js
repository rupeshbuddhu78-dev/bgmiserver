const mongoose = require('mongoose');
const { MongoMemoryServer } = require('mongodb-memory-server');
const request = require('supertest');
const app = require('../src/server');
const User = require('../src/models/User');
const License = require('../src/models/License');

let mongoServer;

beforeAll(async () => {
  mongoServer = await MongoMemoryServer.create();
  const uri = mongoServer.getUri();
  process.env.MONGODB_URI = uri;
  process.env.JWT_SECRET = 'test_secret_key_12345';
  process.env.JWT_REFRESH_SECRET = 'test_refresh_secret_12345';
  process.env.NODE_ENV = 'test';
  process.env.REGISTRATION_ENABLED = 'true';
  
  await mongoose.connect(uri);
});

afterAll(async () => {
  await mongoose.disconnect();
  await mongoServer.stop();
});

afterEach(async () => {
  await User.deleteMany({});
  await License.deleteMany({});
});

describe('Health & Config', () => {
  test('GET /api/health returns success', async () => {
    const res = await request(app).get('/api/health');
    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
  });

  test('GET /api/config returns app config', async () => {
    const res = await request(app).get('/api/config');
    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
    expect(res.body.data).toHaveProperty('appName');
    expect(res.body.data).toHaveProperty('version');
  });
});

describe('Auth - Registration', () => {
  test('POST /api/auth/register creates a new user', async () => {
    const res = await request(app)
      .post('/api/auth/register')
      .send({
        username: 'testuser',
        email: 'test@example.com',
        password: 'Password123'
      });

    expect(res.status).toBe(201);
    expect(res.body.success).toBe(true);
    expect(res.body.data).toHaveProperty('accessToken');
    expect(res.body.data).toHaveProperty('refreshToken');
    expect(res.body.data.user.username).toBe('testuser');
  });

  test('POST /api/auth/register rejects duplicate email', async () => {
    await request(app).post('/api/auth/register').send({
      username: 'user1',
      email: 'dup@example.com',
      password: 'Password123'
    });

    const res = await request(app).post('/api/auth/register').send({
      username: 'user2',
      email: 'dup@example.com',
      password: 'Password123'
    });

    expect(res.status).toBe(409);
    expect(res.body.success).toBe(false);
  });

  test('POST /api/auth/register rejects weak password', async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'testuser',
      email: 'test@example.com',
      password: 'weak'
    });

    expect(res.status).toBe(400);
  });

  test('POST /api/auth/register rejects invalid email', async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'testuser',
      email: 'notanemail',
      password: 'Password123'
    });

    expect(res.status).toBe(400);
  });

  test('POST /api/auth/register rejects short username', async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'ab',
      email: 'test@example.com',
      password: 'Password123'
    });

    expect(res.status).toBe(400);
  });
});

describe('Auth - Login', () => {
  beforeEach(async () => {
    await request(app).post('/api/auth/register').send({
      username: 'loginuser',
      email: 'login@example.com',
      password: 'Password123'
    });
  });

  test('POST /api/auth/login with valid credentials', async () => {
    const res = await request(app)
      .post('/api/auth/login')
      .send({ login: 'loginuser', password: 'Password123' });

    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
    expect(res.body.data).toHaveProperty('accessToken');
  });

  test('POST /api/auth/login with email', async () => {
    const res = await request(app)
      .post('/api/auth/login')
      .send({ login: 'login@example.com', password: 'Password123' });

    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
  });

  test('POST /api/auth/login with wrong password', async () => {
    const res = await request(app)
      .post('/api/auth/login')
      .send({ login: 'loginuser', password: 'WrongPassword1' });

    expect(res.status).toBe(401);
    expect(res.body.success).toBe(false);
  });

  test('POST /api/auth/login with non-existent user', async () => {
    const res = await request(app)
      .post('/api/auth/login')
      .send({ login: 'nouser', password: 'Password123' });

    expect(res.status).toBe(401);
  });
});

describe('Auth - Protected Routes', () => {
  let token;

  beforeEach(async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'protecteduser',
      email: 'protected@example.com',
      password: 'Password123'
    });
    token = res.body.data.accessToken;
  });

  test('GET /api/auth/me with valid token', async () => {
    const res = await request(app)
      .get('/api/auth/me')
      .set('Authorization', `Bearer ${token}`);

    expect(res.status).toBe(200);
    expect(res.body.data.user.username).toBe('protecteduser');
  });

  test('GET /api/auth/me without token returns 401', async () => {
    const res = await request(app).get('/api/auth/me');
    expect(res.status).toBe(401);
  });

  test('GET /api/auth/me with invalid token returns 401', async () => {
    const res = await request(app)
      .get('/api/auth/me')
      .set('Authorization', 'Bearer invalidtoken');
    expect(res.status).toBe(401);
  });

  test('POST /api/auth/logout clears refresh token', async () => {
    const res = await request(app)
      .post('/api/auth/logout')
      .set('Authorization', `Bearer ${token}`);

    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
  });
});

describe('Auth - Token Refresh', () => {
  let refreshToken;

  beforeEach(async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'refreshuser',
      email: 'refresh@example.com',
      password: 'Password123'
    });
    refreshToken = res.body.data.refreshToken;
  });

  test('POST /api/auth/refresh with valid refresh token', async () => {
    const res = await request(app)
      .post('/api/auth/refresh')
      .send({ refreshToken });

    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
    expect(res.body.data).toHaveProperty('accessToken');
    expect(res.body.data).toHaveProperty('refreshToken');
  });

  test('POST /api/auth/refresh with invalid token returns 401', async () => {
    const res = await request(app)
      .post('/api/auth/refresh')
      .send({ refreshToken: 'invalidtoken' });

    expect(res.status).toBe(401);
  });
});

describe('License - Activation', () => {
  let userToken;
  let deviceId;

  beforeEach(async () => {
    const regRes = await request(app).post('/api/auth/register').send({
      username: 'licenseuser',
      email: 'license@example.com',
      password: 'Password123'
    });
    userToken = regRes.body.data.accessToken;
    deviceId = 'test_device_123';

    // Create a license directly in DB
    const rawKey = License.generateKey();
    const keyHash = License.hashKey(rawKey);
    await License.create({
      keyHash,
      keyPrefix: rawKey.substring(0, 5),
      status: 'inactive',
      plan: 'monthly',
      activationLimit: 1,
      expiresAt: new Date(Date.now() + 30 * 24 * 60 * 60 * 1000)
    });
    
    // Store the raw key for test use
    global._testLicenseKey = rawKey;
  });

  test('POST /api/licenses/activate with valid key', async () => {
    const res = await request(app)
      .post('/api/licenses/activate')
      .set('Authorization', `Bearer ${userToken}`)
      .send({ licenseKey: global._testLicenseKey, deviceId });

    expect(res.status).toBe(200);
    expect(res.body.success).toBe(true);
  });

  test('POST /api/licenses/activate without auth returns 401', async () => {
    const res = await request(app)
      .post('/api/licenses/activate')
      .send({ licenseKey: global._testLicenseKey, deviceId });

    expect(res.status).toBe(401);
  });

  test('POST /api/licenses/activate with invalid key', async () => {
    const res = await request(app)
      .post('/api/licenses/activate')
      .set('Authorization', `Bearer ${userToken}`)
      .send({ licenseKey: 'XXXXX-XXXXX-XXXXX-XXXXX', deviceId });

    expect(res.status).toBe(404);
  });

  test('GET /api/licenses/status returns license info', async () => {
    // Activate first
    await request(app)
      .post('/api/licenses/activate')
      .set('Authorization', `Bearer ${userToken}`)
      .send({ licenseKey: global._testLicenseKey, deviceId });

    const res = await request(app)
      .get('/api/licenses/status')
      .set('Authorization', `Bearer ${userToken}`);

    expect(res.status).toBe(200);
    expect(res.body.data.hasLicense).toBe(true);
  });
});

describe('Admin - Access Control', () => {
  let userToken;

  beforeEach(async () => {
    const res = await request(app).post('/api/auth/register').send({
      username: 'regularuser',
      email: 'regular@example.com',
      password: 'Password123'
    });
    userToken = res.body.data.accessToken;
  });

  test('Regular user cannot access admin routes', async () => {
    const res = await request(app)
      .get('/api/admin/users')
      .set('Authorization', `Bearer ${userToken}`);

    expect(res.status).toBe(403);
  });

  test('Unauthenticated user cannot access admin routes', async () => {
    const res = await request(app).get('/api/admin/users');
    expect(res.status).toBe(401);
  });
});

describe('Admin - Full Access', () => {
  let adminToken;

  beforeEach(async () => {
    // Create admin user directly
    const user = new User({
      username: 'admin',
      email: 'admin@example.com',
      passwordHash: 'Password123',
      role: 'admin'
    });
    await user.save();

    const res = await request(app).post('/api/auth/login').send({
      login: 'admin',
      password: 'Password123'
    });
    adminToken = res.body.data.accessToken;
  });

  test('GET /api/admin/users returns user list', async () => {
    const res = await request(app)
      .get('/api/admin/users')
      .set('Authorization', `Bearer ${adminToken}`);

    expect(res.status).toBe(200);
    expect(res.body.data).toHaveProperty('users');
    expect(res.body.data).toHaveProperty('pagination');
  });

  test('POST /api/admin/licenses creates a license', async () => {
    const res = await request(app)
      .post('/api/admin/licenses')
      .set('Authorization', `Bearer ${adminToken}`)
      .send({ plan: 'monthly', activationLimit: 1, expiresInDays: 30 });

    expect(res.status).toBe(201);
    expect(res.body.data).toHaveProperty('licenseKey');
    expect(res.body.data.licenseKey).toMatch(/^[A-Z0-9]{5}-[A-Z0-9]{5}-[A-Z0-9]{5}-[A-Z0-9]{5}$/);
  });

  test('GET /api/admin/stats returns statistics', async () => {
    const res = await request(app)
      .get('/api/admin/stats')
      .set('Authorization', `Bearer ${adminToken}`);

    expect(res.status).toBe(200);
    expect(res.body.data).toHaveProperty('totalUsers');
    expect(res.body.data).toHaveProperty('activeUsers');
  });

  test('GET /api/admin/audit-logs returns logs', async () => {
    const res = await request(app)
      .get('/api/admin/audit-logs')
      .set('Authorization', `Bearer ${adminToken}`);

    expect(res.status).toBe(200);
    expect(res.body.data).toHaveProperty('logs');
  });

  test('GET /api/admin/licenses returns license list', async () => {
    const res = await request(app)
      .get('/api/admin/licenses')
      .set('Authorization', `Bearer ${adminToken}`);

    expect(res.status).toBe(200);
    expect(res.body.data).toHaveProperty('licenses');
  });
});

describe('404 Handler', () => {
  test('Unknown route returns 404', async () => {
    const res = await request(app).get('/api/nonexistent');
    expect(res.status).toBe(404);
    expect(res.body.success).toBe(false);
  });
});
