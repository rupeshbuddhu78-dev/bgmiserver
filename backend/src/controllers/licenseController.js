const { validationResult } = require('express-validator');
const License = require('../models/License');
const Subscription = require('../models/Subscription');
const AuditLog = require('../models/AuditLog');

// POST /api/licenses/activate
exports.activateLicense = async (req, res) => {
  try {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
      return res.status(400).json({
        success: false,
        message: 'Validation failed',
        errors: errors.array()
      });
    }

    const { licenseKey, deviceId } = req.body;
    const keyHash = License.hashKey(licenseKey);

    const license = await License.findOne({ keyHash });
    if (!license) {
      return res.status(404).json({
        success: false,
        message: 'Invalid license key'
      });
    }

    // A license already assigned to another account must never be transferred
    // by simply presenting its key.
    if (license.owner && license.owner.toString() !== req.user._id.toString()) {
      return res.status(403).json({
        success: false,
        message: 'License is already assigned to another account'
      });
    }

    if (license.status === 'revoked') {
      return res.status(403).json({
        success: false,
        message: 'License has been revoked'
      });
    }

    if (license.status === 'suspended') {
      return res.status(403).json({
        success: false,
        message: 'License is suspended'
      });
    }

    if (license.status === 'expired' || (license.expiresAt && license.expiresAt < new Date())) {
      license.status = 'expired';
      await license.save();
      return res.status(403).json({
        success: false,
        message: 'License has expired'
      });
    }

    const result = license.activate(deviceId, req.ip);
    if (!result.success) {
      return res.status(400).json({
        success: false,
        message: result.error
      });
    }

    // Assign license to user
    license.owner = req.user._id;
    await license.save();

    // Create or update subscription
    const planDurations = {
      daily: 1,
      weekly: 7,
      monthly: 30,
      lifetime: 36500
    };

    const duration = planDurations[license.plan] || 30;
    const expiryDate = license.plan === 'lifetime'
      ? new Date('2099-12-31')
      : new Date(Date.now() + duration * 24 * 60 * 60 * 1000);

    await Subscription.findOneAndUpdate(
      { user: req.user._id },
      {
        plan: license.plan,
        status: 'active',
        startDate: new Date(),
        expiryDate,
        licenseId: license._id
      },
      { upsert: true, new: true }
    );

    await AuditLog.create({
      eventType: 'LICENSE_ACTIVATED',
      user: req.user._id,
      ipAddress: req.ip,
      userAgent: req.get('User-Agent'),
      metadata: {
        licensePrefix: license.keyPrefix,
        deviceId,
        plan: license.plan
      }
    });

    res.json({
      success: true,
      message: result.alreadyActivated
        ? 'License already activated on this device'
        : 'License activated successfully',
      data: {
        plan: license.plan,
        expiresAt: license.expiresAt,
        status: license.status
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'License activation failed' 
    });
  }
};

// GET /api/licenses/status
exports.getLicenseStatus = async (req, res) => {
  try {
    const subscription = await Subscription.findOne({
      user: req.user._id
    }).sort({ createdAt: -1 });

    const license = await License.findOne({ owner: req.user._id });

    if (!subscription) {
      return res.json({
        success: true,
        data: {
          hasLicense: false,
          subscription: null,
          license: null
        }
      });
    }

    const isExpired = subscription.expiryDate < new Date();
    if (isExpired && subscription.status === 'active') {
      subscription.status = 'expired';
      await subscription.save();
    }

    res.json({
      success: true,
      data: {
        hasLicense: true,
        subscription: {
          plan: subscription.plan,
          status: isExpired ? 'expired' : subscription.status,
          startDate: subscription.startDate,
          expiryDate: subscription.expiryDate
        },
        license: license ? {
          status: license.status,
          plan: license.plan,
          expiresAt: license.expiresAt,
          activatedAt: license.activatedAt,
          activations: license.currentActivations,
          limit: license.activationLimit
        } : null
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to get license status' 
    });
  }
};

// POST /api/licenses/validate
exports.validateLicense = async (req, res) => {
  try {
    const subscription = await Subscription.findOne({
      user: req.user._id,
      status: 'active'
    });

    if (!subscription) {
      return res.json({
        success: false,
        message: 'No active subscription'
      });
    }

    if (subscription.expiryDate < new Date()) {
      return res.json({
        success: false,
        message: 'Subscription expired'
      });
    }

    res.json({
      success: true,
      message: 'License is valid',
      data: {
        plan: subscription.plan,
        expiresAt: subscription.expiryDate
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Validation failed',
      error: error.message
    });
  }
};
