# 🧠 Page Replacement Algorithm Simulator

![C++](https://img.shields.io/badge/C++-STL-blue)
![Linux](https://img.shields.io/badge/Linux-Terminal-green)
![OS](https://img.shields.io/badge/Operating%20Systems-Memory%20Management-orange)
![Threads](https://img.shields.io/badge/POSIX-pthreads-red)

A **modern Linux-based simulator** for classical **Operating System page replacement algorithms**, built in **C++** using **STL, POSIX threads, and semaphores**.  
Visualizes page hits, faults, and frame states with a clean, interactive CLI.

---

## ✨ Highlights

- Interactive **menu-driven CLI**
- Step-by-step **colored visualization**
- Parallel execution using **pthread**
- Thread-safe output via **semaphores**
- Final **comparison table + best algorithm**
- Linux terminal optimized

---

## 📚 Algorithms

- FIFO
- Optimal
- LRU
- LFU
- Second Chance
- Clock Replacement

---

## 🛠 Tech Stack

- **Language:** C++
- **Platform:** Linux
- **Concepts:** Paging, Threads, Semaphores
- **Containers:** `vector`, `queue`, `unordered_map`, `unordered_set`

---

## ⚙️ Build & Run
**bash**
- g++ -pthread main.cpp -o pager
- ./pager"
