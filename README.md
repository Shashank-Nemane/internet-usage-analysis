[README.md](https://github.com/user-attachments/files/33503655/README.md)
# NetPulse: Student Internet Analytics Dashboard

> **A fully responsive, production-ready statistical analysis dashboard built for GitHub Pages.**
> Academic project by **Shashank Mahadev Nemane** · Roll No: **B25CE1037** · MDM Mini Project

---

## 🌐 Live Demo

Deploy this project on [GitHub Pages](https://pages.github.com/) — no server required.

---

## ✨ Features

### 🎯 First-View Hero
| Feature | Description |
|---|---|
| **4 Live KPI Cards** | Total avg screen time, Top usage category, Most consistent habit (lowest CV%), Most skewed category |
| **Usage Breakdown Chart** | Toggle between Doughnut & Bar chart in real-time |
| **Habit Simulator** | Slide your own hours and instantly compare vs the student average with colour-coded badges |

### 📊 Statistical Engine (7 Metrics × 4 Categories)
| Metric | Formula |
|---|---|
| **Mean** | $\bar{x} = \frac{\sum x_i}{n}$ |
| **Median** | Middle value of sorted data |
| **Mode** | Most frequent value (multi-modal handled) |
| **Range** | $Max - Min$ |
| **Standard Deviation** | $\sigma = \sqrt{\frac{\sum(x_i - \bar{x})^2}{n-1}}$ (sample) |
| **Coefficient of Variation** | $CV = \frac{\sigma}{\bar{x}} \times 100\%$ |
| **Skewness** | Fisher-Pearson sample skewness coefficient |

All metrics are computed **live** and **dynamically** whenever data changes.

### 📈 Visualizations (Chart.js 4)
- **Master Statistical Comparison Table** — all 7 metrics × 4 categories with highlighted max/min
- **Grouped Bar Chart** — Mean vs Std Dev side-by-side
- **Distribution Line Chart** — sorted spread across all students
- **Frequency Histogram** — binned 0-1h, 1-2h … 7-8h per category
- **Doughnut / Bar Hero Chart** — toggleable usage share

### ⚙️ Data Management
- **Add Student Record** form with instant recalculation
- **Delete** individual records
- **Export CSV** with one click
- **Reset to Default** (40 pre-loaded students)
- **localStorage persistence** — data survives page reloads
- **Optional Supabase integration** — input your URL + anon key to pull live table data

### 🎨 UX & Design
- **Dark / Light mode** toggle with persistence
- **Collapsible Raw Data Table**
- **Auto-generated Statistical Insights** (4 dynamic bullet points)
- **Project Info modal** with academic metadata
- **Toast notifications** for all actions
- Fully **responsive** — mobile, tablet, desktop

---

## 🚀 Deployment (GitHub Pages)

```bash
# 1. Fork or clone this repo
git clone https://github.com/YOUR_USERNAME/netpulse-dashboard.git
cd netpulse-dashboard

# 2. Push index.html to the main branch
git add .
git commit -m "Initial commit"
git push origin main

# 3. Enable GitHub Pages
# Go to repo Settings → Pages → Source: Deploy from branch → main → / (root)
```

Your dashboard will be live at: `https://YOUR_USERNAME.github.io/netpulse-dashboard/`

> **No build step needed.** Everything is in `index.html` — pure HTML, CSS, and JS with CDN dependencies only.

---

## 📁 File Structure

```
netpulse-dashboard/
├── index.html          ← Entire application (self-contained)
├── README.md           ← This file
└── .nojekyll           ← Bypasses GitHub Pages Jekyll processing
```

---

## ☁️ Optional Supabase Integration

1. Create a Supabase project at [supabase.com](https://supabase.com)
2. Create a table with this schema:

```sql
create table student_internet_usage (
  id              serial primary key,
  student_id      text,
  education       float,
  entertainment   float,
  social_media    float,
  communication   float
);

-- Enable public read access
alter table student_internet_usage enable row level security;
create policy "Public read" on student_internet_usage for select using (true);
```

3. Click **⚡ Supabase** in the navbar → enter your Project URL and Anon Key → **Connect & Sync**

---

## 📚 Dataset

40 pre-loaded students (`S01`–`S40`) with realistic daily internet usage values across:
- 📚 **Education** — academic browsing, e-learning, research
- 🎬 **Entertainment** — streaming, gaming, YouTube
- 📱 **Social Media** — Instagram, Twitter/X, TikTok
- 💬 **Communication** — WhatsApp, email, video calls

---

## 👨‍🎓 Academic Details

| Field | Value |
|---|---|
| **Student Name** | Shashank Mahadev Nemane |
| **Roll No.** | B25CE1037 |
| **Domain** | Student Life |
| **Subject** | Mathematical & Data Modeling (MDM) |
| **Academic Year** | 2025–26 |

---

## 🛠️ Tech Stack

- **HTML5 / CSS3 / Vanilla JS** — zero framework dependencies
- **[Chart.js 4.4.4](https://www.chartjs.org/)** — via CDN
- **[Inter Font](https://fonts.google.com/specimen/Inter)** — via Google Fonts
- **[JetBrains Mono](https://fonts.google.com/specimen/JetBrains+Mono)** — monospace for stats
- **localStorage** — client-side data persistence

---


