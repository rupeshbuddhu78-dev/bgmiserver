const express = require('express');
const router = express.Router();
const { body } = require('express-validator');
const licenseController = require('../controllers/licenseController');
const { auth } = require('../middleware/auth');
const { activationLimiter } = require('../middleware/rateLimiter');

router.post('/activate',
  auth,
  activationLimiter,
  [
    body('licenseKey')
      .trim()
      .notEmpty()
      .withMessage('License key is required')
      .matches(/^[A-Z0-9]{5}-[A-Z0-9]{5}-[A-Z0-9]{5}-[A-Z0-9]{5}$/)
      .withMessage('Invalid license key format'),
    body('deviceId')
      .trim()
      .notEmpty()
      .withMessage('Device ID is required')
  ],
  licenseController.activateLicense
);

router.get('/status', auth, licenseController.getLicenseStatus);
router.post('/validate', auth, licenseController.validateLicense);

module.exports = router;
