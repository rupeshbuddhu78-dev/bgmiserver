const mongoose = require('mongoose');
const crypto = require('crypto');

const licenseSchema = new mongoose.Schema({
  keyHash: {
    type: String,
    required: [true, 'License key hash is required'],
    unique: true
  },
  keyPrefix: {
    type: String,
    required: true,
    index: true
  },
  owner: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    default: null
  },
  status: {
    type: String,
    enum: ['inactive', 'active', 'expired', 'suspended', 'revoked'],
    default: 'inactive'
  },
  plan: {
    type: String,
    enum: ['daily', 'weekly', 'monthly', 'lifetime'],
    default: 'monthly'
  },
  activationLimit: {
    type: Number,
    default: 1
  },
  currentActivations: {
    type: Number,
    default: 0
  },
  activatedDevices: [{
    deviceId: String,
    activatedAt: Date,
    lastSeen: Date,
    ip: String
  }],
  expiresAt: {
    type: Date,
    default: null
  },
  activatedAt: {
    type: Date,
    default: null
  },
  revokedAt: {
    type: Date,
    default: null
  },
  revokedBy: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    default: null
  },
  notes: {
    type: String,
    maxlength: [500, 'Notes cannot exceed 500 characters']
  }
}, {
  timestamps: true
});

// Indexes
licenseSchema.index({ status: 1 });
licenseSchema.index({ owner: 1 });
licenseSchema.index({ expiresAt: 1 });

// Generate a secure license key
licenseSchema.statics.generateKey = function() {
  const numSegments = 4;
  const charsPerSegment = 5;
  const chars = 'ABCDEFGHJKLMNPQRSTUVWXYZ23456789';
  const parts = [];
  
  for (let i = 0; i < numSegments; i++) {
    let segment = '';
    const bytes = crypto.randomBytes(charsPerSegment);
    for (let j = 0; j < charsPerSegment; j++) {
      segment += chars[bytes[j] % chars.length];
    }
    parts.push(segment);
  }
  
  return parts.join('-');
};

// Hash the license key for storage
licenseSchema.statics.hashKey = function(key) {
  return crypto.createHash('sha256').update(key).digest('hex');
};

// Check if license is valid
licenseSchema.methods.isValid = function(deviceId) {
  if (this.status === 'revoked' || this.status === 'suspended' || this.status === 'expired') return false;
  if (this.expiresAt && this.expiresAt < new Date()) return false;
  // An already-registered device may reconnect even when the activation limit is full.
  const alreadyActivated = deviceId && this.activatedDevices.some(d => d.deviceId === deviceId);
  if (!alreadyActivated && this.currentActivations >= this.activationLimit) return false;
  return true;
};

// Activate license
licenseSchema.methods.activate = function(deviceId, ip) {
  if (!this.isValid(deviceId)) {
    return { success: false, error: 'License is not valid for activation' };
  }
  
  const existingDevice = this.activatedDevices.find(d => d.deviceId === deviceId);
  if (existingDevice) {
    existingDevice.lastSeen = new Date();
    existingDevice.ip = ip;
    return { success: true, alreadyActivated: true };
  }
  
  if (this.currentActivations >= this.activationLimit) {
    return { success: false, error: 'Activation limit reached' };
  }
  
  this.activatedDevices.push({
    deviceId,
    activatedAt: new Date(),
    lastSeen: new Date(),
    ip
  });
  this.currentActivations += 1;
  
  if (!this.activatedAt) {
    this.activatedAt = new Date();
  }
  
  this.status = 'active';
  return { success: true, alreadyActivated: false };
};

// Deactivate a device
licenseSchema.methods.deactivateDevice = function(deviceId) {
  const idx = this.activatedDevices.findIndex(d => d.deviceId === deviceId);
  if (idx === -1) return false;
  
  this.activatedDevices.splice(idx, 1);
  this.currentActivations = Math.max(0, this.currentActivations - 1);
  return true;
};

module.exports = mongoose.model('License', licenseSchema);
