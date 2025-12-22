module.exports = {
  apps: [{
    name: 'absensi',
    script: 'server.js',
    env: {
      NODE_ENV: 'production',
      // OwnCloud Configuration (ubah sesuai kebutuhan)
      OWNCLOUD_URL: process.env.OWNCLOUD_URL || '',
      OWNCLOUD_USERNAME: process.env.OWNCLOUD_USERNAME || '',
      OWNCLOUD_PASSWORD: process.env.OWNCLOUD_PASSWORD || ''
    }
  }]
};
