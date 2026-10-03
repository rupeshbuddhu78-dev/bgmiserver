const { validationResult } = require('express-validator');
const User = require('../models/User');
const License = require('../models/License');
const Subscription = require('../models/Subscription');
const AuditLog = require('../models/AuditLog');

// GET /api/admin/users
exports.getUsers = async (req, res) => {
  try {
    const page = parseInt(req.query.page) || 1;
    const limit = Math.min(parseInt(req.query.limit) || 20, 100);
    const skip = (page - 1) * limit;
    const search = String(req.query.search || '').slice(0, 100);

    let query = {};
    if (search) {
      query = {
        $or: [
          { username: { $regex: search.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), $options: 'i' } },
          { email: { $regex: search.replace(/[.*+?^${}()|[\]\\]/g, '\\$&'), $options: 'i' } }
        ]
      };
    }

    const [users, total] = await Promise.all([
      User.find(query)
        .select('-passwordHash -refreshToken')
        .sort({ createdAt: -1 })
        .skip(skip)
        .limit(limit),
      User.countDocuments(query)
    ]);

    res.json({
      success: true,
      data: {
        users,
        pagination: {
          page,
          limit,
          total,
          pages: Math.ceil(total / limit)
        }
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to fetch users',
    });
  }
};

// PATCH /api/admin/users/:id/status
exports.updateUserStatus = async (req, res) => {
  try {
    const { status } = req.body;
    if (!['active', 'suspended', 'banned'].includes(status)) {
      return res.status(400).json({
        success: false,
        message: 'Invalid status value'
      });
    }

    const user = await User.findById(req.params.id);
    if (!user) {
      return res.status(404).json({
        success: false,
        message: 'User not found'
      });
    }

    if (user._id.toString() === req.user._id.toString()) {
      return res.status(400).json({
        success: false,
        message: 'Cannot change your own status'
      });
    }

    const oldStatus = user.status;
    user.status = status;
    await user.save();

    const eventType = status === 'active' ? 'USER_ACTIVATED' : 'USER_SUSPENDED';
    await AuditLog.create({
      eventType,
      user: user._id,
      performedBy: req.user._id,
      ipAddress: req.ip,
      metadata: { oldStatus, newStatus: status }
    });

    res.json({
      success: true,
      message: `User status updated to ${status}`
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to update user status',
    });
  }
};

// POST /api/admin/licenses
exports.createLicense = async (req, res) => {
  try {
    const errors = validationResult(req);
    if (!errors.isEmpty()) {
      return res.status(400).json({
        success: false,
        message: 'Validation failed',
        errors: errors.array()
      });
    }

    const { plan, activationLimit, expiresInDays, notes } = req.body;

    const rawKey = License.generateKey();
    const keyHash = License.hashKey(rawKey);
    const keyPrefix = rawKey.substring(0, 5);

    const expiresAt = expiresInDays
      ? new Date(Date.now() + parseInt(expiresInDays) * 24 * 60 * 60 * 1000)
      : null;

    const license = new License({
      keyHash,
      keyPrefix,
      status: 'inactive',
      plan: plan || 'monthly',
      activationLimit: activationLimit || 1,
      expiresAt,
      notes: notes || ''
    });

    await license.save();

    await AuditLog.create({
      eventType: 'LICENSE_CREATED',
      performedBy: req.user._id,
      ipAddress: req.ip,
      metadata: {
        keyPrefix,
        plan: license.plan,
        activationLimit: license.activationLimit,
        expiresInDays
      }
    });

    res.status(201).json({
      success: true,
      message: 'License created successfully',
      data: {
        licenseKey: rawKey,
        keyPrefix,
        plan: license.plan,
        activationLimit: license.activationLimit,
        expiresAt: license.expiresAt,
        status: license.status
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to create license',
    });
  }
};

// GET /api/admin/licenses
exports.getLicenses = async (req, res) => {
  try {
    const page = parseInt(req.query.page) || 1;
    const limit = Math.min(parseInt(req.query.limit) || 20, 100);
    const skip = (page - 1) * limit;
    const status = req.query.status;

    let query = {};
    if (status) {
      query.status = status;
    }

    const [licenses, total] = await Promise.all([
      License.find(query)
        .populate('owner', 'username email')
        .sort({ createdAt: -1 })
        .skip(skip)
        .limit(limit),
      License.countDocuments(query)
    ]);

    res.json({
      success: true,
      data: {
        licenses,
        pagination: {
          page,
          limit,
          total,
          pages: Math.ceil(total / limit)
        }
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to fetch licenses',
    });
  }
};

// PATCH /api/admin/licenses/:id/revoke
exports.revokeLicense = async (req, res) => {
  try {
    const license = await License.findById(req.params.id);
    if (!license) {
      return res.status(404).json({
        success: false,
        message: 'License not found'
      });
    }

    license.status = 'revoked';
    license.revokedAt = new Date();
    license.revokedBy = req.user._id;
    await license.save();

    await AuditLog.create({
      eventType: 'LICENSE_REVOKED',
      user: license.owner,
      performedBy: req.user._id,
      ipAddress: req.ip,
      metadata: { keyPrefix: license.keyPrefix }
    });

    res.json({
      success: true,
      message: 'License revoked successfully'
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to revoke license',
    });
  }
};

// PATCH /api/admin/licenses/:id/suspend
exports.suspendLicense = async (req, res) => {
  try {
    const license = await License.findById(req.params.id);
    if (!license) {
      return res.status(404).json({
        success: false,
        message: 'License not found'
      });
    }

    license.status = 'suspended';
    await license.save();

    await AuditLog.create({
      eventType: 'LICENSE_SUSPENDED',
      user: license.owner,
      performedBy: req.user._id,
      ipAddress: req.ip,
      metadata: { keyPrefix: license.keyPrefix }
    });

    res.json({
      success: true,
      message: 'License suspended successfully'
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to suspend license',
    });
  }
};

// GET /api/admin/audit-logs
exports.getAuditLogs = async (req, res) => {
  try {
    const page = parseInt(req.query.page) || 1;
    const limit = Math.min(parseInt(req.query.limit) || 50, 200);
    const skip = (page - 1) * limit;
    const eventType = req.query.eventType;

    let query = {};
    if (eventType) {
      query.eventType = eventType;
    }

    const [logs, total] = await Promise.all([
      AuditLog.find(query)
        .populate('user', 'username email')
        .populate('performedBy', 'username email')
        .sort({ createdAt: -1 })
        .skip(skip)
        .limit(limit),
      AuditLog.countDocuments(query)
    ]);

    res.json({
      success: true,
      data: {
        logs,
        pagination: {
          page,
          limit,
          total,
          pages: Math.ceil(total / limit)
        }
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to fetch audit logs',
    });
  }
};

// GET /api/admin/stats
exports.getStats = async (req, res) => {
  try {
    const [
      totalUsers,
      activeUsers,
      totalLicenses,
      activeLicenses,
      recentUsers,
      recentLogins
    ] = await Promise.all([
      User.countDocuments(),
      User.countDocuments({ status: 'active' }),
      License.countDocuments(),
      License.countDocuments({ status: 'active' }),
      User.countDocuments({
        createdAt: { $gte: new Date(Date.now() - 7 * 24 * 60 * 60 * 1000) }
      }),
      AuditLog.countDocuments({
        eventType: 'USER_LOGIN',
        createdAt: { $gte: new Date(Date.now() - 24 * 60 * 60 * 1000) }
      })
    ]);

    res.json({
      success: true,
      data: {
        totalUsers,
        activeUsers,
        totalLicenses,
        activeLicenses,
        recentUsers,
        recentLogins
      }
    });
  } catch (error) {
    res.status(500).json({
      success: false,
      message: 'Failed to fetch stats',
    });
  }
};
