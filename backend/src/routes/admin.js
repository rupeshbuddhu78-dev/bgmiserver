const express = require('express');
const router = express.Router();
const { body } = require('express-validator');
const adminController = require('../controllers/adminController');
const { auth, adminOnly } = require('../middleware/auth');

// All admin routes require auth + admin role
router.use(auth, adminOnly);

// User management
router.get('/users', adminController.getUsers);
router.patch('/users/:id/status',
  [body('status').isIn(['active', 'suspended', 'banned'])],
  adminController.updateUserStatus
);

// License management
router.post('/licenses',
  [
    body('plan').isIn(['daily', 'weekly', 'monthly', 'lifetime']).withMessage('Invalid plan'),
    body('activationLimit').optional().isInt({ min: 1 }).withMessage('Activation limit must be >= 1'),
    body('expiresInDays').optional().isInt({ min: 1 }).withMessage('Expiry days must be >= 1')
  ],
  adminController.createLicense
);
router.get('/licenses', adminController.getLicenses);
router.patch('/licenses/:id/revoke', adminController.revokeLicense);
router.patch('/licenses/:id/suspend', adminController.suspendLicense);

// Audit logs
router.get('/audit-logs', adminController.getAuditLogs);

// Stats
router.get('/stats', adminController.getStats);

module.exports = router;
