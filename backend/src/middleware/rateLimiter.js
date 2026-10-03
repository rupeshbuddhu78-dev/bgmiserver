const rateLimit = require('express-rate-limit');

// Disable rate limiting entirely during tests to avoid false failures
const isTest = process.env.NODE_ENV === 'test';
const noOpLimiter = (req, res, next) => next();

const loginLimiter = isTest ? noOpLimiter : rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 5,
  message: {
    success: false,
    message: 'Too many login attempts. Please try again after 15 minutes.'
  },
  standardHeaders: true,
  legacyHeaders: false,
  skipSuccessfulRequests: true
});

const registerLimiter = isTest ? noOpLimiter : rateLimit({
  windowMs: 60 * 60 * 1000, // 1 hour
  max: 3,
  message: {
    success: false,
    message: 'Too many registration attempts. Please try again later.'
  },
  standardHeaders: true,
  legacyHeaders: false
});

const activationLimiter = isTest ? noOpLimiter : rateLimit({
  windowMs: 60 * 60 * 1000, // 1 hour
  max: 10,
  message: {
    success: false,
    message: 'Too many activation attempts. Please try again later.'
  },
  standardHeaders: true,
  legacyHeaders: false
});

const apiLimiter = isTest ? noOpLimiter : rateLimit({
  windowMs: 15 * 60 * 1000, // 15 minutes
  max: 100,
  message: {
    success: false,
    message: 'Too many requests. Please try again later.'
  },
  standardHeaders: true,
  legacyHeaders: false
});

module.exports = {
  loginLimiter,
  registerLimiter,
  activationLimiter,
  apiLimiter
};
