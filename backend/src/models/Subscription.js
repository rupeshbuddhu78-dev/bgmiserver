const mongoose = require('mongoose');

const subscriptionSchema = new mongoose.Schema({
  user: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'User',
    required: true
  },
  plan: {
    type: String,
    enum: ['free', 'daily', 'weekly', 'monthly', 'lifetime'],
    required: true,
    default: 'free'
  },
  status: {
    type: String,
    enum: ['active', 'expired', 'cancelled', 'pending'],
    default: 'pending'
  },
  startDate: {
    type: Date,
    required: true
  },
  expiryDate: {
    type: Date,
    required: true
  },
  cancelledAt: {
    type: Date,
    default: null
  },
  licenseId: {
    type: mongoose.Schema.Types.ObjectId,
    ref: 'License',
    default: null
  }
}, {
  timestamps: true
});

subscriptionSchema.index({ user: 1, status: 1 });
subscriptionSchema.index({ expiryDate: 1 });

subscriptionSchema.methods.isExpired = function() {
  return this.expiryDate < new Date();
};

module.exports = mongoose.model('Subscription', subscriptionSchema);
