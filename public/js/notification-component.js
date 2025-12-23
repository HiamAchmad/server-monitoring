// ================================================
// NOTIFICATION COMPONENT
// Komponen notifikasi untuk admin dan user
// ================================================

class NotificationComponent {
    constructor(options = {}) {
        this.isAdmin = options.isAdmin || false;
        this.pegawaiId = options.pegawaiId || null;
        this.containerId = options.containerId || 'notification-bell';
        this.notifications = [];
        this.unreadCount = 0;
        this.isOpen = false;

        this.init();
    }

    init() {
        this.createBellIcon();
        this.loadNotifications();
        this.setupSocketIO();
        this.setupClickOutside();
    }

    createBellIcon() {
        const container = document.getElementById(this.containerId);
        if (!container) {
            console.warn('Notification container not found');
            return;
        }

        // Store reference in window for onclick handlers
        window.notificationComponent = this;

        container.innerHTML = `
            <div class="notif-bell" onclick="window.notificationComponent.toggleDropdown()">
                <svg xmlns="http://www.w3.org/2000/svg" width="24" height="24" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2" stroke-linecap="round" stroke-linejoin="round">
                    <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"></path>
                    <path d="M13.73 21a2 2 0 0 1-3.46 0"></path>
                </svg>
                <span class="notif-badge" id="notif-badge" style="display: none;">0</span>
            </div>
            <div class="notif-dropdown" id="notif-dropdown">
                <div class="notif-header">
                    <h4>Notifikasi</h4>
                    <div class="notif-actions">
                        <button onclick="window.notificationComponent.markAllRead()" title="Tandai semua dibaca">
                            <svg xmlns="http://www.w3.org/2000/svg" width="16" height="16" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
                                <polyline points="20 6 9 17 4 12"></polyline>
                            </svg>
                        </button>
                    </div>
                </div>
                <div class="notif-list" id="notif-list">
                    <div class="notif-empty">Memuat...</div>
                </div>
            </div>
        `;

        this.injectStyles();
    }

    injectStyles() {
        if (document.getElementById('notif-styles')) return;

        const styles = document.createElement('style');
        styles.id = 'notif-styles';
        styles.textContent = `
            .notif-bell {
                position: relative;
                cursor: pointer;
                padding: 8px;
                border-radius: 50%;
                transition: all 0.3s ease;
                display: flex;
                align-items: center;
                justify-content: center;
            }

            .notif-bell:hover {
                background: rgba(255, 255, 255, 0.1);
            }

            .notif-bell svg {
                width: 24px;
                height: 24px;
                color: var(--text-secondary, #a0aec0);
                transition: color 0.3s ease;
            }

            .notif-bell:hover svg {
                color: var(--text-primary, #fff);
            }

            .notif-badge {
                position: absolute;
                top: 2px;
                right: 2px;
                background: linear-gradient(135deg, #f56565, #e53e3e);
                color: white;
                font-size: 10px;
                font-weight: 700;
                min-width: 18px;
                height: 18px;
                border-radius: 9px;
                display: flex;
                align-items: center;
                justify-content: center;
                padding: 0 4px;
                box-shadow: 0 2px 8px rgba(245, 101, 101, 0.5);
                animation: pulse-badge 2s infinite;
            }

            @keyframes pulse-badge {
                0%, 100% { transform: scale(1); }
                50% { transform: scale(1.1); }
            }

            .notif-dropdown {
                position: absolute;
                top: 100%;
                right: 0;
                width: 360px;
                max-height: 480px;
                background: rgba(30, 30, 46, 0.98);
                backdrop-filter: blur(20px);
                border: 1px solid rgba(255, 255, 255, 0.1);
                border-radius: 16px;
                box-shadow: 0 20px 60px rgba(0, 0, 0, 0.4);
                z-index: 1000;
                opacity: 0;
                visibility: hidden;
                transform: translateY(-10px);
                transition: all 0.3s ease;
                overflow: hidden;
            }

            .notif-dropdown.show {
                opacity: 1;
                visibility: visible;
                transform: translateY(0);
            }

            .notif-header {
                display: flex;
                justify-content: space-between;
                align-items: center;
                padding: 16px 20px;
                border-bottom: 1px solid rgba(255, 255, 255, 0.1);
                background: rgba(255, 255, 255, 0.03);
            }

            .notif-header h4 {
                margin: 0;
                font-size: 16px;
                font-weight: 600;
                color: var(--text-primary, #fff);
            }

            .notif-actions button {
                background: rgba(255, 255, 255, 0.1);
                border: none;
                padding: 6px;
                border-radius: 8px;
                cursor: pointer;
                color: var(--text-secondary, #a0aec0);
                transition: all 0.2s ease;
            }

            .notif-actions button:hover {
                background: rgba(102, 126, 234, 0.3);
                color: var(--primary-start, #667eea);
            }

            .notif-list {
                max-height: 400px;
                overflow-y: auto;
            }

            .notif-list::-webkit-scrollbar {
                width: 6px;
            }

            .notif-list::-webkit-scrollbar-thumb {
                background: rgba(255, 255, 255, 0.2);
                border-radius: 3px;
            }

            .notif-item {
                display: flex;
                gap: 12px;
                padding: 14px 20px;
                border-bottom: 1px solid rgba(255, 255, 255, 0.05);
            }

            .notif-item.unread {
                background: rgba(102, 126, 234, 0.1);
                border-left: 3px solid var(--primary-start, #667eea);
            }

            .notif-icon {
                width: 40px;
                height: 40px;
                border-radius: 12px;
                display: flex;
                align-items: center;
                justify-content: center;
                font-size: 18px;
                flex-shrink: 0;
            }

            .notif-icon.info { background: rgba(66, 153, 225, 0.2); }
            .notif-icon.success { background: rgba(72, 187, 120, 0.2); }
            .notif-icon.warning { background: rgba(237, 137, 54, 0.2); }
            .notif-icon.error { background: rgba(245, 101, 101, 0.2); }

            .notif-content {
                flex: 1;
                min-width: 0;
            }

            .notif-title {
                font-size: 14px;
                font-weight: 600;
                color: var(--text-primary, #fff);
                margin-bottom: 4px;
                white-space: nowrap;
                overflow: hidden;
                text-overflow: ellipsis;
            }

            .notif-message {
                font-size: 12px;
                color: var(--text-secondary, #a0aec0);
                line-height: 1.4;
                display: -webkit-box;
                -webkit-line-clamp: 2;
                -webkit-box-orient: vertical;
                overflow: hidden;
            }

            .notif-time {
                font-size: 11px;
                color: var(--text-tertiary, #718096);
                margin-top: 6px;
            }

            .notif-empty {
                padding: 40px 20px;
                text-align: center;
                color: var(--text-tertiary, #718096);
                font-size: 14px;
            }

            .notif-empty svg {
                width: 48px;
                height: 48px;
                margin-bottom: 12px;
                opacity: 0.5;
            }

            @media (max-width: 480px) {
                .notif-dropdown {
                    width: calc(100vw - 32px);
                    right: -60px;
                }
            }
        `;
        document.head.appendChild(styles);
    }

    async loadNotifications() {
        try {
            const endpoint = this.isAdmin
                ? '/api/notifikasi/admin'
                : `/api/notifikasi/user/${this.pegawaiId}`;

            const response = await fetch(endpoint);
            const result = await response.json();

            if (result.success) {
                this.notifications = result.data;
                this.unreadCount = result.unread_count;
                this.updateBadge();
                this.renderNotifications();
            }
        } catch (error) {
            console.error('Error loading notifications:', error);
        }
    }

    updateBadge() {
        const badge = document.getElementById('notif-badge');
        if (badge) {
            if (this.unreadCount > 0) {
                badge.textContent = this.unreadCount > 99 ? '99+' : this.unreadCount;
                badge.style.display = 'flex';
            } else {
                badge.style.display = 'none';
            }
        }
    }

    renderNotifications() {
        const list = document.getElementById('notif-list');
        if (!list) return;

        if (this.notifications.length === 0) {
            list.innerHTML = `
                <div class="notif-empty">
                    <svg xmlns="http://www.w3.org/2000/svg" viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="1.5">
                        <path d="M18 8A6 6 0 0 0 6 8c0 7-3 9-3 9h18s-3-2-3-9"></path>
                        <path d="M13.73 21a2 2 0 0 1-3.46 0"></path>
                    </svg>
                    <div>Tidak ada notifikasi</div>
                </div>
            `;
            return;
        }

        list.innerHTML = this.notifications.map(notif => `
            <div class="notif-item ${notif.is_read ? '' : 'unread'}">
                <div class="notif-icon ${notif.tipe}">
                    ${this.getIcon(notif.tipe)}
                </div>
                <div class="notif-content">
                    <div class="notif-title">${this.escapeHtml(notif.judul)}</div>
                    <div class="notif-message">${this.escapeHtml(notif.pesan)}</div>
                    <div class="notif-time">${this.formatTime(notif.created_at)}</div>
                </div>
            </div>
        `).join('');
    }

    getIcon(tipe) {
        const icons = {
            info: '📢',
            success: '✅',
            warning: '⚠️',
            error: '❌'
        };
        return icons[tipe] || '🔔';
    }

    formatTime(timestamp) {
        const date = new Date(timestamp);
        const now = new Date();
        const diff = now - date;

        const minutes = Math.floor(diff / 60000);
        const hours = Math.floor(diff / 3600000);
        const days = Math.floor(diff / 86400000);

        if (minutes < 1) return 'Baru saja';
        if (minutes < 60) return `${minutes} menit lalu`;
        if (hours < 24) return `${hours} jam lalu`;
        if (days < 7) return `${days} hari lalu`;

        return date.toLocaleDateString('id-ID', {
            day: 'numeric',
            month: 'short',
            year: date.getFullYear() !== now.getFullYear() ? 'numeric' : undefined
        });
    }

    escapeHtml(text) {
        const div = document.createElement('div');
        div.textContent = text;
        return div.innerHTML;
    }

    toggleDropdown() {
        const dropdown = document.getElementById('notif-dropdown');
        if (dropdown) {
            this.isOpen = !this.isOpen;
            dropdown.classList.toggle('show', this.isOpen);

            // Otomatis tandai semua sebagai dibaca saat dropdown dibuka
            if (this.isOpen && this.unreadCount > 0) {
                this.markAllRead();
            }
        }
    }

    closeDropdown() {
        const dropdown = document.getElementById('notif-dropdown');
        if (dropdown) {
            this.isOpen = false;
            dropdown.classList.remove('show');
        }
    }

    setupClickOutside() {
        document.addEventListener('click', (e) => {
            const container = document.getElementById(this.containerId);
            if (container && !container.contains(e.target)) {
                this.closeDropdown();
            }
        });
    }

    async handleClick(id, link) {
        // Mark as read
        try {
            await fetch(`/api/notifikasi/${id}/read`, { method: 'PUT' });
            const notif = this.notifications.find(n => n.id_notifikasi === id);
            if (notif && !notif.is_read) {
                notif.is_read = true;
                this.unreadCount = Math.max(0, this.unreadCount - 1);
                this.updateBadge();
                this.renderNotifications();
            }
        } catch (error) {
            console.error('Error marking notification read:', error);
        }

        // Navigate if link exists
        if (link) {
            window.location.href = link;
        }
    }

    async markAllRead() {
        try {
            const endpoint = this.isAdmin
                ? '/api/notifikasi/admin/read-all'
                : `/api/notifikasi/user/${this.pegawaiId}/read-all`;

            await fetch(endpoint, { method: 'PUT' });

            this.notifications.forEach(n => n.is_read = true);
            this.unreadCount = 0;
            this.updateBadge();
            this.renderNotifications();
        } catch (error) {
            console.error('Error marking all read:', error);
        }
    }

    setupSocketIO() {
        if (typeof io === 'undefined') return;

        const socket = io();

        // Listen for new notifications
        socket.on('admin-notification', (notif) => {
            if (this.isAdmin) {
                this.addNotification(notif);
            }
        });

        socket.on(`user-notification-${this.pegawaiId}`, (notif) => {
            if (!this.isAdmin) {
                this.addNotification(notif);
            }
        });

        socket.on('new-notification', (notif) => {
            // Refresh notification list
            this.loadNotifications();
        });
    }

    addNotification(notif) {
        this.notifications.unshift(notif);
        if (!notif.is_read) {
            this.unreadCount++;
        }
        this.updateBadge();
        this.renderNotifications();

        // Show toast
        this.showToast(notif);
    }

    showToast(notif) {
        const toast = document.createElement('div');
        toast.className = 'notif-toast';
        toast.innerHTML = `
            <div class="notif-toast-icon ${notif.tipe}">${this.getIcon(notif.tipe)}</div>
            <div class="notif-toast-content">
                <div class="notif-toast-title">${this.escapeHtml(notif.judul)}</div>
                <div class="notif-toast-message">${this.escapeHtml(notif.pesan)}</div>
            </div>
        `;

        // Add toast styles if not exists
        if (!document.getElementById('notif-toast-styles')) {
            const toastStyles = document.createElement('style');
            toastStyles.id = 'notif-toast-styles';
            toastStyles.textContent = `
                .notif-toast {
                    position: fixed;
                    top: 20px;
                    right: 20px;
                    display: flex;
                    gap: 12px;
                    padding: 16px 20px;
                    background: rgba(30, 30, 46, 0.98);
                    backdrop-filter: blur(20px);
                    border: 1px solid rgba(255, 255, 255, 0.1);
                    border-radius: 12px;
                    box-shadow: 0 10px 40px rgba(0, 0, 0, 0.3);
                    z-index: 10000;
                    animation: slideIn 0.3s ease, slideOut 0.3s ease 4.7s;
                    max-width: 360px;
                }

                @keyframes slideIn {
                    from { transform: translateX(100%); opacity: 0; }
                    to { transform: translateX(0); opacity: 1; }
                }

                @keyframes slideOut {
                    from { transform: translateX(0); opacity: 1; }
                    to { transform: translateX(100%); opacity: 0; }
                }

                .notif-toast-icon {
                    width: 40px;
                    height: 40px;
                    border-radius: 10px;
                    display: flex;
                    align-items: center;
                    justify-content: center;
                    font-size: 20px;
                }

                .notif-toast-icon.info { background: rgba(66, 153, 225, 0.2); }
                .notif-toast-icon.success { background: rgba(72, 187, 120, 0.2); }
                .notif-toast-icon.warning { background: rgba(237, 137, 54, 0.2); }
                .notif-toast-icon.error { background: rgba(245, 101, 101, 0.2); }

                .notif-toast-title {
                    font-weight: 600;
                    color: #fff;
                    margin-bottom: 4px;
                }

                .notif-toast-message {
                    font-size: 13px;
                    color: #a0aec0;
                }
            `;
            document.head.appendChild(toastStyles);
        }

        document.body.appendChild(toast);

        setTimeout(() => {
            toast.remove();
        }, 5000);
    }
}

// Note: notificationComponent should be declared in each page that uses this component
// e.g., var notificationComponent = null; before creating new NotificationComponent()
