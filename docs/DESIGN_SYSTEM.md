# 🎨 Design System - Glassmorphism Futuristik

## 📋 Overview
Sistem desain modern dengan pendekatan **Glassmorphism** yang elegan dan futuristik untuk aplikasi monitoring absensi dan manajemen tugas karyawan.

---

## 🎨 Design Philosophy

**Konsep:** "Transparent Elegance in Digital Workspace"

**Karakteristik:**
- ✨ **Glassmorphism** - Efek kaca buram dengan transparansi
- 🌌 **Futuristik** - Gradasi warna modern dengan accent cahaya
- 🎯 **Minimalis** - Clean, fokus pada konten utama
- 💫 **Interactive** - Smooth animations dan micro-interactions

---

## 🎨 Color Palette

### Primary Colors (Blue-Purple Gradient)
```css
--primary-start: #667eea      /* Electric Blue */
--primary-mid: #764ba2        /* Royal Purple */
--primary-end: #8E54E9        /* Vivid Purple */

/* Gradients */
--gradient-primary: linear-gradient(135deg, #667eea 0%, #764ba2 100%)
--gradient-accent: linear-gradient(135deg, #f093fb 0%, #f5576c 100%)
--gradient-success: linear-gradient(135deg, #4facfe 0%, #00f2fe 100%)
--gradient-bg: linear-gradient(135deg, #1e1e2e 0%, #2d2d44 100%)
```

### Neutral Colors (Glassmorphism Base)
```css
--glass-white: rgba(255, 255, 255, 0.1)
--glass-white-hover: rgba(255, 255, 255, 0.15)
--glass-dark: rgba(0, 0, 0, 0.2)
--glass-border: rgba(255, 255, 255, 0.2)

--bg-dark: #0f0f23
--bg-card: rgba(255, 255, 255, 0.05)
--bg-card-hover: rgba(255, 255, 255, 0.08)
```

### Semantic Colors
```css
--success: #4ade80
--warning: #fbbf24
--error: #f87171
--info: #60a5fa

--text-primary: #ffffff
--text-secondary: rgba(255, 255, 255, 0.7)
--text-tertiary: rgba(255, 255, 255, 0.5)
```

---

## 📝 Typography

### Font Family
**Primary:** Inter (sans-serif modern, excellent readability)
**Secondary:** Poppins (headings, friendly & geometric)

```css
--font-primary: 'Inter', -apple-system, BlinkMacSystemFont, 'Segoe UI', sans-serif
--font-secondary: 'Poppins', sans-serif
```

### Font Sizes (Fluid Typography)
```css
--text-xs: clamp(0.75rem, 0.7rem + 0.25vw, 0.875rem)      /* 12-14px */
--text-sm: clamp(0.875rem, 0.8rem + 0.375vw, 1rem)        /* 14-16px */
--text-base: clamp(1rem, 0.9rem + 0.5vw, 1.125rem)        /* 16-18px */
--text-lg: clamp(1.125rem, 1rem + 0.625vw, 1.25rem)       /* 18-20px */
--text-xl: clamp(1.25rem, 1.1rem + 0.75vw, 1.5rem)        /* 20-24px */
--text-2xl: clamp(1.5rem, 1.3rem + 1vw, 2rem)             /* 24-32px */
--text-3xl: clamp(2rem, 1.7rem + 1.5vw, 2.75rem)          /* 32-44px */
```

### Font Weights
```css
--weight-light: 300
--weight-regular: 400
--weight-medium: 500
--weight-semibold: 600
--weight-bold: 700
```

---

## 🎯 Spacing System

### Base Unit: 4px
```css
--space-1: 0.25rem   /* 4px */
--space-2: 0.5rem    /* 8px */
--space-3: 0.75rem   /* 12px */
--space-4: 1rem      /* 16px */
--space-5: 1.25rem   /* 20px */
--space-6: 1.5rem    /* 24px */
--space-8: 2rem      /* 32px */
--space-10: 2.5rem   /* 40px */
--space-12: 3rem     /* 48px */
--space-16: 4rem     /* 64px */
```

---

## 🔲 Border Radius

### Glassmorphism Curves
```css
--radius-sm: 8px      /* Small elements (badges, tags) */
--radius-md: 12px     /* Buttons, inputs */
--radius-lg: 16px     /* Cards, modals */
--radius-xl: 24px     /* Large panels */
--radius-2xl: 32px    /* Hero sections */
--radius-full: 9999px /* Circular elements */
```

---

## 💫 Shadows & Glow

### Glass Shadows (Soft & Layered)
```css
--shadow-glass-sm:
  0 2px 4px rgba(0, 0, 0, 0.1),
  0 1px 2px rgba(0, 0, 0, 0.06),
  inset 0 1px 1px rgba(255, 255, 255, 0.1)

--shadow-glass-md:
  0 4px 6px rgba(0, 0, 0, 0.1),
  0 2px 4px rgba(0, 0, 0, 0.06),
  inset 0 1px 1px rgba(255, 255, 255, 0.1)

--shadow-glass-lg:
  0 10px 15px rgba(0, 0, 0, 0.1),
  0 4px 6px rgba(0, 0, 0, 0.05),
  inset 0 1px 2px rgba(255, 255, 255, 0.1)

--shadow-glass-xl:
  0 20px 25px rgba(0, 0, 0, 0.15),
  0 10px 10px rgba(0, 0, 0, 0.04),
  inset 0 2px 4px rgba(255, 255, 255, 0.1)
```

### Neon Glow Effects
```css
--glow-primary: 0 0 20px rgba(102, 126, 234, 0.4)
--glow-accent: 0 0 20px rgba(245, 87, 108, 0.4)
--glow-success: 0 0 20px rgba(74, 222, 128, 0.4)
```

---

## 🧩 Component Styles

### Glass Card
```css
.glass-card {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: var(--radius-lg);
  box-shadow: var(--shadow-glass-md);
  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);
}

.glass-card:hover {
  background: rgba(255, 255, 255, 0.08);
  border-color: rgba(255, 255, 255, 0.2);
  box-shadow: var(--shadow-glass-lg);
  transform: translateY(-2px);
}
```

### Glass Button
```css
.glass-btn {
  background: linear-gradient(135deg, rgba(102, 126, 234, 0.2), rgba(118, 75, 162, 0.2));
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.2);
  border-radius: var(--radius-md);
  padding: 0.75rem 1.5rem;
  color: white;
  font-weight: 600;
  transition: all 0.3s ease;
}

.glass-btn:hover {
  background: linear-gradient(135deg, rgba(102, 126, 234, 0.4), rgba(118, 75, 162, 0.4));
  box-shadow: 0 0 20px rgba(102, 126, 234, 0.5);
  transform: translateY(-2px);
}
```

### Glass Input
```css
.glass-input {
  background: rgba(255, 255, 255, 0.05);
  backdrop-filter: blur(10px);
  border: 1px solid rgba(255, 255, 255, 0.1);
  border-radius: var(--radius-md);
  padding: 0.75rem 1rem;
  color: white;
  transition: all 0.3s ease;
}

.glass-input:focus {
  background: rgba(255, 255, 255, 0.08);
  border-color: rgba(102, 126, 234, 0.5);
  box-shadow: 0 0 20px rgba(102, 126, 234, 0.3);
  outline: none;
}
```

---

## 📱 Layout Structure

### Desktop Layout (1200px+)
```
┌─────────────────────────────────────────────┐
│  Navbar (Glass, Fixed Top)                 │
├──────┬──────────────────────────────────────┤
│      │  Main Content Area                  │
│ Side │  ┌──────────────┬──────────────┐    │
│ bar  │  │  Card 1      │  Card 2      │    │
│      │  │  (Glass)     │  (Glass)     │    │
│(Gla  │  └──────────────┴──────────────┘    │
│ ss)  │  ┌─────────────────────────────┐    │
│      │  │  Chart Area (Glass)         │    │
│      │  └─────────────────────────────┘    │
└──────┴──────────────────────────────────────┘
```

### Mobile Layout (< 768px)
```
┌─────────────────────┐
│  Navbar (Collapsed) │
├─────────────────────┤
│                     │
│  Content (Stack)    │
│  ┌───────────────┐  │
│  │  Card 1       │  │
│  └───────────────┘  │
│  ┌───────────────┐  │
│  │  Card 2       │  │
│  └───────────────┘  │
│                     │
├─────────────────────┤
│  Bottom Nav (Glass) │
└─────────────────────┘
```

---

## 🎭 Animation & Transitions

### Transition Timings
```css
--transition-fast: 150ms cubic-bezier(0.4, 0, 0.2, 1)
--transition-base: 300ms cubic-bezier(0.4, 0, 0.2, 1)
--transition-slow: 500ms cubic-bezier(0.4, 0, 0.2, 1)
```

### Hover Effects
```css
/* Scale on hover */
.scale-hover:hover {
  transform: scale(1.02);
}

/* Lift on hover */
.lift-hover:hover {
  transform: translateY(-4px);
}

/* Glow on hover */
.glow-hover:hover {
  box-shadow: 0 0 30px rgba(102, 126, 234, 0.6);
}
```

### Loading Animations
```css
@keyframes shimmer {
  0% { background-position: -1000px 0; }
  100% { background-position: 1000px 0; }
}

@keyframes pulse-glow {
  0%, 100% { box-shadow: 0 0 20px rgba(102, 126, 234, 0.4); }
  50% { box-shadow: 0 0 40px rgba(102, 126, 234, 0.8); }
}
```

---

## 📊 Dashboard Components

### Stat Card (KPI Cards)
- **Size:** 250px x 150px (desktop), full-width (mobile)
- **Background:** Glass with gradient overlay
- **Content:**
  - Large number (--text-3xl, --weight-bold)
  - Label (--text-sm, --text-secondary)
  - Icon (top-right, gradient fill)
  - Trend indicator (+/- with color)

### Chart Container
- **Background:** Glass card
- **Padding:** var(--space-6)
- **Chart Style:**
  - Gradient fills for areas
  - Soft grid lines (rgba(255,255,255,0.1))
  - Tooltips with glass background

### Table (Attendance List)
- **Header:** Sticky, glass background
- **Rows:** Hover effect with glass-white-hover
- **Borders:** Subtle (rgba(255,255,255,0.1))
- **Alternate rows:** Slight transparency difference

---

## 📤 Upload Page Components

### Drag & Drop Zone
```css
.drop-zone {
  background: rgba(255, 255, 255, 0.03);
  border: 2px dashed rgba(102, 126, 234, 0.4);
  border-radius: var(--radius-xl);
  padding: var(--space-12);
  text-align: center;
  transition: all 0.3s ease;
}

.drop-zone.drag-over {
  background: rgba(102, 126, 234, 0.1);
  border-color: rgba(102, 126, 234, 0.8);
  box-shadow: 0 0 30px rgba(102, 126, 234, 0.4);
}
```

### Progress Bar
```css
.progress-bar {
  height: 8px;
  background: rgba(255, 255, 255, 0.1);
  border-radius: var(--radius-full);
  overflow: hidden;
}

.progress-fill {
  height: 100%;
  background: linear-gradient(90deg, #667eea, #764ba2);
  border-radius: var(--radius-full);
  box-shadow: 0 0 10px rgba(102, 126, 234, 0.5);
  animation: shimmer 2s infinite;
}
```

---

## 📱 Mobile Specific

### Bottom Navigation
```css
.bottom-nav {
  position: fixed;
  bottom: 0;
  left: 0;
  right: 0;
  background: rgba(15, 15, 35, 0.9);
  backdrop-filter: blur(20px);
  border-top: 1px solid rgba(255, 255, 255, 0.1);
  padding: var(--space-3);
  display: flex;
  justify-content: space-around;
  z-index: 1000;
}

.nav-item {
  display: flex;
  flex-direction: column;
  align-items: center;
  gap: var(--space-1);
  color: var(--text-secondary);
  transition: all 0.3s ease;
}

.nav-item.active {
  color: var(--primary-start);
  text-shadow: 0 0 10px rgba(102, 126, 234, 0.8);
}
```

### Touch Interactions
- **Tap target:** Minimum 44x44px
- **Ripple effect:** On touch
- **Pull-to-refresh:** Gradient indicator
- **Swipe gestures:** Card dismissal, navigation

---

## ✨ Micro-interactions

### Button Press
```css
.btn-press:active {
  transform: scale(0.98);
  box-shadow: inset 0 2px 4px rgba(0, 0, 0, 0.2);
}
```

### Card Reveal
```css
@keyframes fadeInUp {
  from {
    opacity: 0;
    transform: translateY(20px);
  }
  to {
    opacity: 1;
    transform: translateY(0);
  }
}

.card-reveal {
  animation: fadeInUp 0.6s cubic-bezier(0.4, 0, 0.2, 1);
}
```

### Success Feedback
```css
@keyframes success-pulse {
  0%, 100% {
    box-shadow: 0 0 0 0 rgba(74, 222, 128, 0.7);
  }
  50% {
    box-shadow: 0 0 0 20px rgba(74, 222, 128, 0);
  }
}
```

---

## 🎯 Accessibility

### Focus States
```css
.focusable:focus-visible {
  outline: 2px solid rgba(102, 126, 234, 0.8);
  outline-offset: 2px;
  box-shadow: 0 0 20px rgba(102, 126, 234, 0.5);
}
```

### High Contrast Mode
```css
@media (prefers-contrast: high) {
  .glass-card {
    border-width: 2px;
    border-color: rgba(255, 255, 255, 0.4);
  }
}
```

### Reduced Motion
```css
@media (prefers-reduced-motion: reduce) {
  *,
  *::before,
  *::after {
    animation-duration: 0.01ms !important;
    animation-iteration-count: 1 !important;
    transition-duration: 0.01ms !important;
  }
}
```

---

## 📐 Breakpoints

```css
--breakpoint-sm: 640px   /* Mobile landscape */
--breakpoint-md: 768px   /* Tablet */
--breakpoint-lg: 1024px  /* Desktop */
--breakpoint-xl: 1280px  /* Large desktop */
--breakpoint-2xl: 1536px /* Extra large */
```

---

## 🎨 Implementation Notes

1. **Backdrop Filter Support:** Use `-webkit-backdrop-filter` for Safari
2. **Performance:** Limit blur effects on mobile (reduce from 10px to 5px)
3. **Dark Mode Only:** Design optimized for dark background
4. **GPU Acceleration:** Use `transform: translateZ(0)` for smooth animations
5. **Font Loading:** Use `font-display: swap` to prevent FOIT

---

**Design Version:** 1.0.0
**Last Updated:** November 2025
**Designer:** Glassmorphism Futuristik Theme
