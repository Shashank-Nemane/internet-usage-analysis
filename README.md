# NetPulse: Student Internet Analytics Dashboard & C++ Backend

> **A modern, responsive full-stack statistical analysis dashboard powered by a high-performance C++ REST API engine with static GitHub Pages resilience.**  
> Academic project by **Shashank Mahadev Nemane** · Roll No: **B25CE1037** · Subject: **Mathematical & Data Modeling (MDM)** · Domain: **Student Life**

---

## 🌐 Dual Deployment & Execution Modes

NetPulse supports two complementary runtime modes:
1. **Full-Stack Mode (Local / High-Performance C++ API)**:
   - Backend runs on `http://localhost:8080` powered by `server.cpp` and `cpp-httplib`.
   - Dynamic real-time calculations directly on `data.csv` (200 student records).
   - Live visual status: `🟢 C++ Backend Connected (Port 8080)`.
2. **Static / GitHub Pages Resilience Mode**:
   - Zero-dependency client-side fallback using modern Vanilla JS and `localStorage` / `data.csv`.
   - If the C++ server is offline, the frontend falls back seamlessly without error.
   - Live visual status: `🟡 Offline / Client-Side Mode`.

---

## 📁 Project Architecture & Files

```
c:\MDM mini project\
├── server.cpp        # High-performance C++ backend HTTP server (cpp-httplib)
├── httplib.h         # Header-only cpp-httplib HTTP/HTTPS library (keep untouched)
├── server.exe        # Compiled Windows binary (or 'server' for Linux/macOS)
├── data.csv          # Persistent CSV dataset: 200 student records (S001 - S200)
├── index.html        # Interactive frontend dashboard (HTML5 + Chart.js + Vanilla JS)
├── style.css         # Modern dark/light responsive CSS stylesheet
├── README.md         # Documentation, math formulations, and run instructions
└── .nojekyll         # GitHub Pages configuration to allow raw file serving
```

---

## 🛠️ C++ Backend Compilation & Execution

### Windows (MinGW / MSYS2 / GCC)
```powershell
# Compile server.cpp linking against Windows Sockets (ws2_32)
g++ -O2 -std=c++17 server.cpp -o server.exe -lws2_32

# Run the server
.\server.exe
```

### Linux / macOS
```bash
# Compile server.cpp with POSIX threads
g++ -O2 -std=c++17 server.cpp -o server -lpthread

# Run the server
./server
```

Once running, the backend listens at `http://localhost:8080` and also serves the static frontend directly.

---

## 📡 REST API Endpoints

All endpoints have CORS enabled (`Access-Control-Allow-Origin: *`, `GET, POST, DELETE, OPTIONS`).

| Method | Endpoint | Description | Payload / Response |
|---|---|---|---|
| `GET` | `/api/stats` | Computes & returns all 7 statistical metrics across the 4 categories | Returns JSON object with calculated metrics |
| `GET` | `/api/students` | Retrieves all student records directly from `data.csv` | Returns JSON array of student records |
| `POST` | `/api/students` | Appends a new student record to `data.csv` & syncs state | Body: `{"student_id":"S201","education":4.5,...}` |
| `DELETE` | `/api/students/:id` | Deletes a student record by ID and updates `data.csv` | URL parameter `:id` (e.g., `S015`) |
| `POST` | `/api/simulate` | Evaluates habits against ideal student benchmarks & score | Body: `{"education":4.0,...}` → Plan + Score |

---

## 📊 Core Statistical Engine (7 Metrics × 4 Categories)

Computed across **Education**, **Entertainment**, **Social Media**, and **Communication**:

1. **Mean ($\bar{x}$):**
   $$\bar{x} = \frac{1}{n}\sum_{i=1}^{n} x_i$$
2. **Median:** Middle value of sorted data (or mean of the two central values for even $n$).
3. **Mode:** Most frequent observation (with multi-modal / continuous binning support).
4. **Range:**
   $$R = x_{\max} - x_{\min}$$
5. **Sample Standard Deviation ($s$):**
   $$s = \sqrt{\frac{1}{n-1}\sum_{i=1}^{n}(x_i - \bar{x})^2}$$
6. **Coefficient of Variation ($CV\%$):**
   $$CV\% = \left(\frac{s}{\bar{x}}\right) \times 100\%$$
7. **Fisher-Pearson Skewness ($g_1$):**
   $$g_1 = \frac{n}{(n-1)(n-2)} \sum_{i=1}^{n} \left(\frac{x_i - \bar{x}}{s}\right)^3$$

---

## 🎯 Key Application Features

- **Full-Screen Landing Overlay ("The Idea"):** Academic motivation, research problem statement, and "Enter Dashboard" trigger.
- **Top Navbar:** Quick category selector, connection status pill (`🟢 C++ Backend Connected` / `🟡 Offline Mode`), project info modal, and theme toggle.
- **Executive KPI Cards:** Dynamic metrics reflecting all categories or isolated for the active category filter.
- **Usage Breakdown & Distribution Charts:** Real-time Chart.js doughnut, grouped bar, and student distribution lines.
- **Quick Habit Simulator with 24h Daily Limit:**
  - Strict total daily cap of 24.0 hours across sliders.
  - Live 3-way comparison table: **Your Hours** vs **Student Average** vs **Ideal Benchmark**.
  - 0–100 Digital Wellness Score with personalized, actionable habit recommendations.
- **Automated Dynamic Insights:** Statistically generated findings highlighting usage drivers, dispersion, and skewness alerts.
- **CRUD Student Records Table:** Instant table sorting, searching, pagination, new record creation, and deletion with disk sync.

---

## 👨‍🎓 Academic Metadata

- **Student Name:** Shashank Mahadev Nemane
- **Roll Number:** B25CE1037
- **Course / Subject:** Mathematical & Data Modeling (MDM) / GenAI-MP
- **Domain:** Student Life & Behavioral Screen Analytics
- **Academic Year:** 2025–2026

---

*Built with precision for the MDM Mini Project.*
