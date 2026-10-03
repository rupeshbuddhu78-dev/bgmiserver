require('dotenv').config();
const express = require('express');
const cors = require('cors');
const helmet = require('helmet');
const connectDB = require('./config/database');
const errorHandler = require('./middleware/errorHandler');
const { apiLimiter } = require('./middleware/rateLimiter');

const authRoutes = require('./routes/auth');
const licenseRoutes = require('./routes/licenses');
const adminRoutes = require('./routes/admin');

const app = express();

// Honor forwarded client IPs only when explicitly configured for a trusted proxy.
if (process.env.TRUST_PROXY) {
  app.set('trust proxy', process.env.TRUST_PROXY === 'true' ? 1 : process.env.TRUST_PROXY);
}

// Security middleware
app.use(helmet());
const allowedOrigins = (process.env.CORS_ORIGIN || 'http://localhost:3000,http://localhost:3001')
  .split(',').map(value => value.trim()).filter(Boolean);
app.use(cors({
  origin(origin, callback) {
    // Native clients may omit Origin; bundled WebViews can send the opaque "null" origin.
    if (!origin || origin === 'null' || allowedOrigins.includes(origin)) return callback(null, true);
    return callback(null, false);
  },
  credentials: true,
  methods: ['GET', 'POST', 'PATCH', 'DELETE', 'OPTIONS'],
  allowedHeaders: ['Content-Type', 'Authorization']
}));

// Body parsing
app.use(express.json({ limit: '10kb' }));
app.use(express.urlencoded({ extended: true }));

// Rate limiting
app.use('/api/', apiLimiter);

// Health check
app.get('/api/health', (req, res) => {
  res.json({
    success: true,
    message: 'API is running',
    timestamp: new Date().toISOString(),
    version: process.env.APP_VERSION || '1.0.0'
  });
});

// App config (public)
app.get('/api/config', (req, res) => {
  res.json({
    success: true,
    data: {
      appName: process.env.APP_NAME || 'Fireplace Loader',
      version: process.env.APP_VERSION || '1.0.0',
      registrationEnabled: process.env.REGISTRATION_ENABLED !== 'false',
      maintenanceMode: process.env.MAINTENANCE_MODE === 'true'
    }
  });
});

// Routes
app.use('/api/auth', authRoutes);
app.use('/api/licenses', licenseRoutes);
app.use('/api/admin', adminRoutes);

// 404 handler
app.use((req, res) => {
  res.status(404).json({
    success: false,
    message: 'Route not found'
  });
});

// Error handler
app.use(errorHandler);

// Fail fast on missing or unsafe deployment configuration.
const validateEnvironment = () => {
  const required = ['MONGODB_URI', 'JWT_SECRET', 'JWT_REFRESH_SECRET'];
  const missing = required.filter(key => !process.env[key] || process.env[key].includes('<'));
  if (missing.length) throw new Error(`Missing required environment variables: ${missing.join(', ')}`);
  if (process.env.NODE_ENV === 'production') {
    for (const key of ['JWT_SECRET', 'JWT_REFRESH_SECRET']) {
      if (process.env[key].length < 32 || /change_me|your_.*_here|secret_here/i.test(process.env[key])) {
        throw new Error(`${key} must be a unique random secret of at least 32 characters in production`);
      }
    }
    if (!process.env.CORS_ORIGIN || process.env.CORS_ORIGIN.split(',').map(value => value.trim()).includes('*')) {
      throw new Error('Set CORS_ORIGIN to a comma-separated allowlist of your website origins in production');
    }
  }
};

// Start server
const PORT = process.env.PORT || 3001;

const startServer = async () => {
  try {
    validateEnvironment();
    await connectDB();
    app.listen(PORT, () => {
      console.log(`Server running on port ${PORT}`);
      console.log(`Environment: ${process.env.NODE_ENV || 'development'}`);
    });
  } catch (error) {
    console.error('Failed to start server:', error);
    process.exit(1);
  }
};

// Only start if not in test mode
if (process.env.NODE_ENV !== 'test') {
  startServer();
}

module.exports = app;
