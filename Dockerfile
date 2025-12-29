# Use Node.js 18 LTS as base image
FROM node:18-alpine

# Set working directory
WORKDIR /app

# Install dependencies first (caching)
COPY package*.json ./
RUN npm install --production

# Copy application source code
COPY . .

# Create uploads directory
RUN mkdir -p uploads && chown -R node:node uploads

# Expose ports
# 3000: Web Server / Socket.IO
# 1883: MQTT Broker
EXPOSE 3000 1883

# Switch to non-root user
USER node

# Start the application
CMD ["npm", "start"]
