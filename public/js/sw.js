// Service Worker for Sistem Absensi PWA
const CACHE_NAME = 'absensi-pwa-v8.0.0';
const RUNTIME_CACHE = 'absensi-runtime-v8.0.0';

// Files to cache on install
const STATIC_CACHE_URLS = [
  '/',
  '/user/user-login.html',
  '/user/user-dashboard.html',
  '/public/css/glass-style.css',
  '/manifest.json',
  '/public/icons/icon-192x192.png',
  '/public/icons/icon-512x512.png'
];

// Install event - cache static assets
self.addEventListener('install', (event) => {
  console.log('[Service Worker] Installing...');

  event.waitUntil(
    caches.open(CACHE_NAME)
      .then((cache) => {
        console.log('[Service Worker] Caching static assets');
        return cache.addAll(STATIC_CACHE_URLS);
      })
      .then(() => {
        console.log('[Service Worker] Skip waiting');
        return self.skipWaiting();
      })
  );
});

// Activate event - clean up old caches
self.addEventListener('activate', (event) => {
  console.log('[Service Worker] Activating...');

  event.waitUntil(
    caches.keys()
      .then((cacheNames) => {
        return Promise.all(
          cacheNames.map((cacheName) => {
            if (cacheName !== CACHE_NAME && cacheName !== RUNTIME_CACHE) {
              console.log('[Service Worker] Deleting old cache:', cacheName);
              return caches.delete(cacheName);
            }
          })
        );
      })
      .then(() => {
        console.log('[Service Worker] Claiming clients');
        return self.clients.claim();
      })
  );
});

// Fetch event - network first, fallback to cache
self.addEventListener('fetch', (event) => {
  const { request } = event;
  const url = new URL(request.url);

  // Skip cross-origin requests
  if (url.origin !== location.origin) {
    return;
  }

  // API requests - Network First strategy
  if (request.url.includes('/api/') ||
      request.url.includes('/absensi') ||
      request.url.includes('/pegawai')) {

    event.respondWith(
      fetch(request)
        .then((response) => {
          // Only cache GET requests (Cache API doesn't support POST/PUT/DELETE)
          if (request.method === 'GET') {
            const responseClone = response.clone();
            caches.open(RUNTIME_CACHE).then((cache) => {
              cache.put(request, responseClone);
            });
          }
          return response;
        })
        .catch(() => {
          // Fallback to cache if network fails
          return caches.match(request).then((cachedResponse) => {
            if (cachedResponse) {
              return cachedResponse;
            }
            // Return offline page or error
            return new Response(
              JSON.stringify({
                success: false,
                message: 'Anda sedang offline. Data tidak dapat dimuat.',
                offline: true
              }),
              {
                headers: { 'Content-Type': 'application/json' }
              }
            );
          });
        })
    );
    return;
  }

  // Static assets - Cache First strategy
  event.respondWith(
    caches.match(request)
      .then((cachedResponse) => {
        if (cachedResponse) {
          return cachedResponse;
        }

        // Not in cache, fetch from network
        return fetch(request)
          .then((response) => {
            // Don't cache if not successful
            if (!response || response.status !== 200) {
              return response;
            }

            // Only cache GET requests (Cache API doesn't support POST/PUT/DELETE)
            if (request.method === 'GET') {
              const responseClone = response.clone();
              caches.open(RUNTIME_CACHE).then((cache) => {
                cache.put(request, responseClone);
              });
            }

            return response;
          })
          .catch(() => {
            // Return offline fallback page
            if (request.destination === 'document') {
              return caches.match('/user/user-login.html');
            }
            return new Response('Offline - Resource not available', {
              status: 503,
              statusText: 'Service Unavailable'
            });
          });
      })
  );
});

// Background sync for offline absensi (future feature)
self.addEventListener('sync', (event) => {
  console.log('[Service Worker] Background sync:', event.tag);

  if (event.tag === 'sync-absensi') {
    event.waitUntil(syncAbsensi());
  }
});

// Push notification handler
self.addEventListener('push', (event) => {
  console.log('[Service Worker] Push received');

  let notificationData = {
    title: 'Sistem Absensi',
    body: 'Anda memiliki notifikasi baru',
    icon: '/icons/icon-192x192.png',
    badge: '/icons/icon-72x72.png'
  };

  if (event.data) {
    try {
      const data = event.data.json();
      notificationData = {
        title: data.title || notificationData.title,
        body: data.body || notificationData.body,
        icon: data.icon || notificationData.icon,
        badge: notificationData.badge,
        data: data.data || {}
      };
    } catch (e) {
      console.error('[Service Worker] Error parsing push data:', e);
    }
  }

  event.waitUntil(
    self.registration.showNotification(notificationData.title, {
      body: notificationData.body,
      icon: notificationData.icon,
      badge: notificationData.badge,
      data: notificationData.data,
      vibrate: [200, 100, 200],
      tag: 'absensi-notification',
      requireInteraction: false
    })
  );
});

// Notification click handler
self.addEventListener('notificationclick', (event) => {
  console.log('[Service Worker] Notification clicked');

  event.notification.close();

  event.waitUntil(
    clients.openWindow('/user/user-dashboard.html')
  );
});

// Helper function for background sync
async function syncAbsensi() {
  try {
    // Get pending absensi from IndexedDB (to be implemented)
    console.log('[Service Worker] Syncing offline absensi data');

    // This is a placeholder for future implementation
    // Will sync offline stored absensi to server when online

    return Promise.resolve();
  } catch (error) {
    console.error('[Service Worker] Sync error:', error);
    return Promise.reject(error);
  }
}

// Message handler for communication with pages
self.addEventListener('message', (event) => {
  console.log('[Service Worker] Message received:', event.data);

  if (event.data && event.data.type === 'SKIP_WAITING') {
    self.skipWaiting();
  }

  if (event.data && event.data.type === 'CLEAR_CACHE') {
    event.waitUntil(
      caches.keys().then((cacheNames) => {
        return Promise.all(
          cacheNames.map((cacheName) => {
            return caches.delete(cacheName);
          })
        );
      }).then(() => {
        event.ports[0].postMessage({ success: true });
      })
    );
  }
});

console.log('[Service Worker] Loaded');
