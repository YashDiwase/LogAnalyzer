# 📊 Enterprise Log Analyzer (C++ + C# Hybrid)

A high-performance log analysis desktop application built using **C++ (Core Engine)**, **C++/CLI (Interop Bridge)**, and **WPF (C# UI)**.

This project demonstrates **multi-threaded log parsing**, **native-managed interoperability**, and a **modern dashboard UI**.

---

## 🧠 Architecture
WPF (C# UI)
↓
C++/CLI Bridge (Interop Layer)
↓
Native C++ Engine (Core Parsing Logic)

---

## ✨ Features

- 📂 Load and analyze large log files
- ⚡ Multi-threaded log processing
- 📊 Summary dashboard:
  - INFO count
  - WARN count
  - ERROR count
- 📋 Module-wise error breakdown
- 🧪 Malformed log detection
- 🚫 Prevents duplicate analysis execution
- 🎨 Modern dark-themed UI
- 📉 (Optional) Chart visualization

---

## 📄 Log Format Supported
YYYY-MM-DD HH:MM:SS,ms LEVEL Module - Message
Example:
2025-01-20 10:15:32,123 ERROR OrderService - Order failed for ID=1023

---

## 🛠️ Tech Stack

| Layer | Technology |
|------|--------|
| UI | WPF (C#) |
| ViewModel | MVVM |
| Interop | C++/CLI |
| Core Engine | Native C++ |
| Concurrency | std::thread / async |
| Charts | LiveCharts2 (optional) |

---

## ⚙️ How It Works

1. User selects a log file
2. C# UI calls C++/CLI bridge
3. Bridge invokes native C++ parser
4. Parser processes logs using multi-threading
5. Results are returned to UI
6. Dashboard updates instantly

---

## 🚀 Getting Started

### Prerequisites

- Visual Studio 2022
- .NET (WPF support)
- C++ Desktop Development workload

---

### Steps

1. Clone the repo:

```bash
git clone https://github.com/your-username/log-analyzer.git
