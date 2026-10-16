#include "httplib.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <mutex>

// Structure representing one student's record
struct StudentRecord {
    std::string id;
    double education;
    double entertainment;
    double social_media;
    double communication;

    double total() const {
        return education + entertainment + social_media + communication;
    }
};

// Structure holding the 7 statistical metrics
struct CategoryStats {
    int count = 0;
    double mean = 0.0;
    double median = 0.0;
    std::string mode = "No mode";
    double range = 0.0;
    double stdDev = 0.0;
    double cv = 0.0;
    double skewness = 0.0;
    double min = 0.0;
    double max = 0.0;
};

// Global in-memory dataset and synchronization lock
static std::vector<StudentRecord> g_students;
static std::mutex g_data_mutex;
static const std::string CSV_FILE_PATH = "data.csv";

// Ideal student benchmarks
static const double IDEAL_EDUCATION = 4.0;
static const double IDEAL_ENTERTAINMENT = 1.5;
static const double IDEAL_SOCIAL_MEDIA = 1.0;
static const double IDEAL_COMMUNICATION = 1.0; // max target 1.5h

// Helper to trim strings
static std::string trim(const std::string& s) {
    size_t start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    size_t end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

// Load data.csv into memory
static bool load_csv() {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    g_students.clear();

    std::ifstream file(CSV_FILE_PATH);
    if (!file.is_open()) {
        std::cerr << "[WARN] Could not open " << CSV_FILE_PATH << ". Starting with empty dataset." << std::endl;
        return false;
    }

    std::string line;
    bool is_header = true;
    while (std::getline(file, line)) {
        line = trim(line);
        if (line.empty()) continue;
        if (is_header) {
            is_header = false;
            continue;
        }

        std::stringstream ss(line);
        std::string sid, s_edu, s_ent, s_soc, s_com;
        if (std::getline(ss, sid, ',') &&
            std::getline(ss, s_edu, ',') &&
            std::getline(ss, s_ent, ',') &&
            std::getline(ss, s_soc, ',') &&
            std::getline(ss, s_com, ',')) {
            try {
                StudentRecord rec;
                rec.id = trim(sid);
                rec.education = std::stod(trim(s_edu));
                rec.entertainment = std::stod(trim(s_ent));
                rec.social_media = std::stod(trim(s_soc));
                rec.communication = std::stod(trim(s_com));
                g_students.push_back(rec);
            } catch (...) {
                // Ignore parse errors on malformed rows
            }
        }
    }
    std::cout << "[INFO] Loaded " << g_students.size() << " student records from " << CSV_FILE_PATH << std::endl;
    return true;
}

// Persist in-memory records to data.csv
static bool save_csv() {
    std::ofstream file(CSV_FILE_PATH, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "[ERROR] Failed to open " << CSV_FILE_PATH << " for writing!" << std::endl;
        return false;
    }

    file << "student_id,education,entertainment,social_media,communication\n";
    for (const auto& r : g_students) {
        file << r.id << ","
             << std::fixed << std::setprecision(1) << r.education << ","
             << std::fixed << std::setprecision(1) << r.entertainment << ","
             << std::fixed << std::setprecision(1) << r.social_media << ","
             << std::fixed << std::setprecision(1) << r.communication << "\n";
    }
    return true;
}

// Core Mathematical Engine: Computes all 7 descriptive statistical metrics
static CategoryStats compute_stats(const std::vector<double>& raw_vals) {
    CategoryStats st;
    st.count = static_cast<int>(raw_vals.size());
    if (st.count == 0) return st;

    std::vector<double> vals = raw_vals;
    std::sort(vals.begin(), vals.end());

    st.min = vals.front();
    st.max = vals.back();
    st.range = st.max - st.min;

    // 1. Mean
    double sum = 0.0;
    for (double v : vals) sum += v;
    st.mean = sum / st.count;

    // 2. Median
    if (st.count % 2 == 1) {
        st.median = vals[st.count / 2];
    } else {
        st.median = (vals[st.count / 2 - 1] + vals[st.count / 2]) / 2.0;
    }

    // 3. Mode (grouped by rounding to 1 decimal place)
    std::map<int, int> freq_map;
    int max_freq = 0;
    for (double v : vals) {
        int key = static_cast<int>(std::round(v * 10.0));
        freq_map[key]++;
        if (freq_map[key] > max_freq) max_freq = freq_map[key];
    }

    if (max_freq <= 1) {
        st.mode = "No mode";
    } else {
        std::vector<double> modes;
        for (const auto& pair : freq_map) {
            if (pair.second == max_freq) {
                modes.push_back(pair.first / 10.0);
            }
        }
        if (modes.size() > 3) {
            st.mode = "Multi-modal";
        } else {
            std::ostringstream mode_oss;
            for (size_t i = 0; i < modes.size(); ++i) {
                if (i > 0) mode_oss << ", ";
                mode_oss << std::fixed << std::setprecision(1) << modes[i];
            }
            st.mode = mode_oss.str();
        }
    }

    // 4. Sample Standard Deviation
    if (st.count > 1) {
        double sq_sum = 0.0;
        for (double v : vals) {
            sq_sum += (v - st.mean) * (v - st.mean);
        }
        double sample_var = sq_sum / (st.count - 1);
        st.stdDev = std::sqrt(sample_var);
    } else {
        st.stdDev = 0.0;
    }

    // 5. Coefficient of Variation (CV%)
    if (std::abs(st.mean) > 1e-9) {
        st.cv = (st.stdDev / st.mean) * 100.0;
    } else {
        st.cv = 0.0;
    }

    // 6. Fisher-Pearson Sample Skewness
    if (st.count >= 3 && st.stdDev > 1e-9) {
        double m3 = 0.0;
        for (double v : vals) {
            double diff = v - st.mean;
            m3 += diff * diff * diff;
        }
        m3 /= st.count;
        double s3 = st.stdDev * st.stdDev * st.stdDev;
        double g1 = m3 / s3;
        double n = static_cast<double>(st.count);
        st.skewness = (std::sqrt(n * (n - 1.0)) / (n - 2.0)) * g1;
    } else {
        st.skewness = 0.0;
    }

    return st;
}

// Compute metrics for all 4 categories
struct AllStats {
    CategoryStats education;
    CategoryStats entertainment;
    CategoryStats social_media;
    CategoryStats communication;
    double total_avg_screen_time = 0.0;
    int total_students = 0;
};

static AllStats get_all_stats() {
    std::lock_guard<std::mutex> lock(g_data_mutex);
    std::vector<double> v_edu, v_ent, v_soc, v_com;
    for (const auto& s : g_students) {
        v_edu.push_back(s.education);
        v_ent.push_back(s.entertainment);
        v_soc.push_back(s.social_media);
        v_com.push_back(s.communication);
    }

    AllStats res;
    res.education = compute_stats(v_edu);
    res.entertainment = compute_stats(v_ent);
    res.social_media = compute_stats(v_soc);
    res.communication = compute_stats(v_com);
    res.total_students = static_cast<int>(g_students.size());
    res.total_avg_screen_time = res.education.mean + res.entertainment.mean + res.social_media.mean + res.communication.mean;
    return res;
}

// Lightweight JSON helper to serialize stats
static std::string serialize_category_stats(const CategoryStats& st) {
    std::ostringstream oss;
    oss << "{"
        << "\"count\":" << st.count << ","
        << "\"mean\":" << std::fixed << std::setprecision(4) << st.mean << ","
        << "\"median\":" << std::fixed << std::setprecision(4) << st.median << ","
        << "\"mode\":\"" << st.mode << "\","
        << "\"range\":" << std::fixed << std::setprecision(4) << st.range << ","
        << "\"stdDev\":" << std::fixed << std::setprecision(4) << st.stdDev << ","
        << "\"cv\":" << std::fixed << std::setprecision(2) << st.cv << ","
        << "\"skewness\":" << std::fixed << std::setprecision(4) << st.skewness << ","
        << "\"min\":" << std::fixed << std::setprecision(2) << st.min << ","
        << "\"max\":" << std::fixed << std::setprecision(2) << st.max
        << "}";
    return oss.str();
}

// Lightweight JSON field extractors
static double extract_double(const std::string& body, const std::string& key, double def_val = 0.0) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = body.find(pattern);
    if (pos == std::string::npos) return def_val;
    pos = body.find(':', pos);
    if (pos == std::string::npos) return def_val;
    pos++;
    while (pos < body.size() && (body[pos] == ' ' || body[pos] == '\"')) pos++;
    size_t end = body.find_first_of(",}\" \r\n", pos);
    if (end == std::string::npos) end = body.size();
    try {
        return std::stod(body.substr(pos, end - pos));
    } catch (...) {
        return def_val;
    }
}

static std::string extract_string(const std::string& body, const std::string& key) {
    std::string pattern = "\"" + key + "\"";
    size_t pos = body.find(pattern);
    if (pos == std::string::npos) return "";
    pos = body.find(':', pos);
    if (pos == std::string::npos) return "";
    pos = body.find('\"', pos);
    if (pos == std::string::npos) return "";
    pos++;
    size_t end = body.find('\"', pos);
    if (end == std::string::npos) return "";
    return body.substr(pos, end - pos);
}

// Wellness score calculation & Personal Action Plan
struct SimulationResult {
    int score = 0;
    std::string statusText;
    std::string statusColor;
    std::vector<std::string> plan;
};

static SimulationResult simulate(double edu, double ent, double soc, double com) {
    SimulationResult res;

    // Scoring weights: Education (30 pts), Entertainment (25 pts), Social (25 pts), Communication (20 pts)
    double eduScore = std::min(edu / IDEAL_EDUCATION, 1.0) * 30.0;
    double entScore = (ent <= IDEAL_ENTERTAINMENT) ? 25.0 : std::max(0.0, 25.0 * (1.0 - (ent - IDEAL_ENTERTAINMENT) / 3.0));
    double socScore = (soc <= IDEAL_SOCIAL_MEDIA) ? 25.0 : std::max(0.0, 25.0 * (1.0 - (soc - IDEAL_SOCIAL_MEDIA) / 2.0));
    
    double comScore = 0.0;
    if (com >= 1.0 && com <= 1.5) {
        comScore = 20.0;
    } else if (com < 1.0) {
        comScore = (com / 1.0) * 20.0;
    } else {
        comScore = std::max(0.0, 20.0 * (1.0 - (com - 1.5) / 2.5));
    }

    res.score = std::min(100, static_cast<int>(std::round(eduScore + entScore + socScore + comScore)));

    if (res.score >= 80) {
        res.statusText = "Optimal Routine 🟢";
        res.statusColor = "#2dd4a0";
    } else if (res.score >= 60) {
        res.statusText = "Needs Rebalancing 🟡";
        res.statusColor = "#f59e0b";
    } else {
        res.statusText = "High Screen-Fatigue Risk 🔴";
        res.statusColor = "#ef4444";
    }

    // Dynamic Action Plan
    std::ostringstream ss;
    if (edu < IDEAL_EDUCATION) {
        ss << "Increase Education by +" << std::fixed << std::setprecision(1) << (IDEAL_EDUCATION - edu)
           << " hrs/day to reach the ideal study benchmark of " << IDEAL_EDUCATION << " hrs.";
        res.plan.push_back(ss.str());
        ss.str("");
    } else {
        res.plan.push_back("Optimal Study Habit! Your education screen time meets or exceeds the ideal 4.0 hrs benchmark.");
    }

    double excEnt = ent - IDEAL_ENTERTAINMENT;
    double excSoc = soc - IDEAL_SOCIAL_MEDIA;
    if (excEnt > 0.0 || excSoc > 0.0) {
        std::string rec = "Reduce ";
        if (excEnt > 0.0) {
            ss << "Entertainment by -" << std::fixed << std::setprecision(1) << excEnt << " hrs/day";
            rec += ss.str();
            ss.str("");
        }
        if (excSoc > 0.0) {
            if (excEnt > 0.0) rec += " and ";
            ss << "Social Media by -" << std::fixed << std::setprecision(1) << excSoc << " hrs/day";
            rec += ss.str();
            ss.str("");
        }
        rec += " to meet healthy screen-time limits (≤1.5h & ≤1.0h).";
        res.plan.push_back(rec);
    } else {
        res.plan.push_back("Entertainment & Social Media are well within healthy limits. Keep it up!");
    }

    if (com > 2.0) {
        ss << "Consider trimming passive messaging — reduce Communication by -"
           << std::fixed << std::setprecision(1) << (com - 1.5) << " hrs/day.";
        res.plan.push_back(ss.str());
    } else {
        res.plan.push_back("Communication time is well-balanced.");
    }

    return res;
}

int main() {
    load_csv();

    httplib::Server svr;

    // Apply global CORS headers to every response
    svr.set_default_headers({
        {"Access-Control-Allow-Origin", "*"},
        {"Access-Control-Allow-Methods", "GET, POST, DELETE, OPTIONS"},
        {"Access-Control-Allow-Headers", "Content-Type, Authorization"}
    });

    // Handle OPTIONS preflight requests for any path
    svr.Options(".*", [](const httplib::Request&, httplib::Response& res) {
        res.status = 204;
    });

    // 1. GET /api/stats : Computes and returns the 7 metrics across all 4 categories in JSON format
    svr.Get("/api/stats", [](const httplib::Request&, httplib::Response& res) {
        AllStats st = get_all_stats();

        std::ostringstream json;
        json << "{"
             << "\"status\":\"success\","
             << "\"total_students\":" << st.total_students << ","
             << "\"total_avg_screen_time\":" << std::fixed << std::setprecision(4) << st.total_avg_screen_time << ","
             << "\"metrics\":{"
             << "\"education\":" << serialize_category_stats(st.education) << ","
             << "\"entertainment\":" << serialize_category_stats(st.entertainment) << ","
             << "\"social_media\":" << serialize_category_stats(st.social_media) << ","
             << "\"communication\":" << serialize_category_stats(st.communication)
             << "},"
             << "\"benchmarks\":{"
             << "\"education\":" << IDEAL_EDUCATION << ","
             << "\"entertainment\":" << IDEAL_ENTERTAINMENT << ","
             << "\"social_media\":" << IDEAL_SOCIAL_MEDIA << ","
             << "\"communication\":" << IDEAL_COMMUNICATION
             << "}"
             << "}";

        res.set_content(json.str(), "application/json");
    });

    // 2. GET /api/students : Returns all records from data.csv in JSON format
    svr.Get("/api/students", [](const httplib::Request&, httplib::Response& res) {
        std::lock_guard<std::mutex> lock(g_data_mutex);
        std::ostringstream json;
        json << "{\"status\":\"success\",\"count\":" << g_students.size() << ",\"data\":[";
        for (size_t i = 0; i < g_students.size(); ++i) {
            if (i > 0) json << ",";
            const auto& r = g_students[i];
            json << "{"
                 << "\"student_id\":\"" << r.id << "\","
                 << "\"education\":" << std::fixed << std::setprecision(1) << r.education << ","
                 << "\"entertainment\":" << std::fixed << std::setprecision(1) << r.entertainment << ","
                 << "\"social_media\":" << std::fixed << std::setprecision(1) << r.social_media << ","
                 << "\"communication\":" << std::fixed << std::setprecision(1) << r.communication
                 << "}";
        }
        json << "]}";
        res.set_content(json.str(), "application/json");
    });

    // 3. POST /api/students : Appends a new student record to data.csv & updates in-memory state
    svr.Post("/api/students", [](const httplib::Request& req, httplib::Response& res) {
        double edu = extract_double(req.body, "education", -1.0);
        double ent = extract_double(req.body, "entertainment", -1.0);
        double soc = extract_double(req.body, "social_media", -1.0);
        double com = extract_double(req.body, "communication", -1.0);
        std::string sid = extract_string(req.body, "student_id");

        if (edu < 0 || ent < 0 || soc < 0 || com < 0 || edu > 24 || ent > 24 || soc > 24 || com > 24) {
            res.status = 400;
            res.set_content("{\"status\":\"error\",\"message\":\"Hours must be between 0 and 24.\"}", "application/json");
            return;
        }

        std::lock_guard<std::mutex> lock(g_data_mutex);
        if (sid.empty()) {
            std::ostringstream id_ss;
            id_ss << "S" << std::setw(3) << std::setfill('0') << (g_students.size() + 1);
            sid = id_ss.str();
        }

        StudentRecord rec;
        rec.id = sid;
        rec.education = edu;
        rec.entertainment = ent;
        rec.social_media = soc;
        rec.communication = com;
        g_students.push_back(rec);
        save_csv();

        std::ostringstream json;
        json << "{\"status\":\"success\",\"message\":\"Student record added successfully\",\"student\":{"
             << "\"student_id\":\"" << rec.id << "\","
             << "\"education\":" << rec.education << ","
             << "\"entertainment\":" << rec.entertainment << ","
             << "\"social_media\":" << rec.social_media << ","
             << "\"communication\":" << rec.communication
             << "}}";
        res.set_content(json.str(), "application/json");
    });

    // 4. DELETE /api/students/:id : Deletes a record from data.csv by student ID
    svr.Delete(R"(/api/students/([A-Za-z0-9_-]+))", [](const httplib::Request& req, httplib::Response& res) {
        std::string target_id = req.matches[1];
        std::lock_guard<std::mutex> lock(g_data_mutex);

        auto it = std::find_if(g_students.begin(), g_students.end(), [&](const StudentRecord& r) {
            return r.id == target_id;
        });

        if (it != g_students.end()) {
            g_students.erase(it);
            save_csv();
            std::ostringstream json;
            json << "{\"status\":\"success\",\"message\":\"Student record deleted successfully\",\"id\":\"" << target_id << "\"}";
            res.set_content(json.str(), "application/json");
        } else {
            res.status = 404;
            res.set_content("{\"status\":\"error\",\"message\":\"Student ID not found\"}", "application/json");
        }
    });

    // 5. POST /api/simulate : Personal simulator comparison vs ideal student benchmarks
    svr.Post("/api/simulate", [](const httplib::Request& req, httplib::Response& res) {
        double edu = extract_double(req.body, "education", 2.5);
        double ent = extract_double(req.body, "entertainment", 3.0);
        double soc = extract_double(req.body, "social_media", 2.0);
        double com = extract_double(req.body, "communication", 1.5);

        SimulationResult sim = simulate(edu, ent, soc, com);

        std::ostringstream json;
        json << "{"
             << "\"status\":\"success\","
             << "\"score\":" << sim.score << ","
             << "\"statusText\":\"" << sim.statusText << "\","
             << "\"statusColor\":\"" << sim.statusColor << "\","
             << "\"plan\":[";
        for (size_t i = 0; i < sim.plan.size(); ++i) {
            if (i > 0) json << ",";
            json << "\"" << sim.plan[i] << "\"";
        }
        json << "],"
             << "\"benchmarks\":{"
             << "\"education\":" << IDEAL_EDUCATION << ","
             << "\"entertainment\":" << IDEAL_ENTERTAINMENT << ","
             << "\"social_media\":" << IDEAL_SOCIAL_MEDIA << ","
             << "\"communication\":" << IDEAL_COMMUNICATION
             << "}"
             << "}";

        res.set_content(json.str(), "application/json");
    });

    // Static file serving: Serve index.html, style.css, data.csv from directory
    svr.set_mount_point("/", ".");

    int port = 8080;
    std::cout << "========================================================\n";
    std::cout << "  NetPulse C++ Statistical Analysis Server\n";
    std::cout << "  Listening on: http://localhost:" << port << "\n";
    std::cout << "  Loaded " << g_students.size() << " records from data.csv\n";
    std::cout << "========================================================" << std::endl;

    if (!svr.listen("0.0.0.0", port)) {
        std::cerr << "[FATAL] Failed to bind to port " << port << std::endl;
        return 1;
    }
    return 0;
}
