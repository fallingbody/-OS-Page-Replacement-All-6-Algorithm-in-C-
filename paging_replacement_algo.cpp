#include <iostream>
#include <pthread.h>
#include <semaphore.h>
#include <vector>
#include <queue>
#include <unordered_set>
#include <unordered_map>
#include <algorithm>
#include <cstring>
#include <iomanip>
#include <climits>

#define N 6

using namespace std;

// ANSI Color Codes
const string RESET = "\033[0m";
const string RED = "\033[1;31m";
const string GREEN = "\033[1;32m";
const string YELLOW = "\033[1;33m";
const string BLUE = "\033[1;34m";
const string CYAN = "\033[1;36m";
const string BOLD = "\033[1m";

vector<vector<int>> result(N, vector<int>(2, 0));
sem_t mem_lock;

// --- Display Helpers ---

void printHeader(string title)
{
    cout << "\n"
         << CYAN << "==============================================================" << RESET << endl;
    cout << "   " << BOLD << title << RESET << endl;
    cout << CYAN << "==============================================================" << RESET << endl;
    cout << setw(10) << left << "Step" << setw(10) << "Page" << setw(15) << "Status" << "Frames" << endl;
    cout << "--------------------------------------------------------------" << endl;
}

void displayStep(int step, int page, bool hit, const vector<int> &frames, int capacity)
{
    cout << setw(10) << left << step << setw(10) << page;

    if (hit)
        cout << GREEN << setw(15) << "[ HIT  ] " << RESET;
    else
        cout << RED << setw(15) << "[ FAULT] " << RESET;

    for (int i = 0; i < capacity; i++)
    {
        if (frames[i] == -1)
            cout << "[ ] ";
        else
            cout << "[" << frames[i] << "] ";
    }
    cout << endl;
}

void displaySummary(int faults, int hits, int n)
{
    cout << "--------------------------------------------------------------" << endl;
    cout << YELLOW << "   Summary Statistics" << RESET << endl;
    cout << "   ------------------" << endl;
    cout << "   Page Faults : " << RED << faults << RESET << endl;
    cout << "   Page Hits   : " << GREEN << hits << RESET << endl;
    cout << "   Fault Ratio : " << fixed << setprecision(2) << ((float)faults / n) << endl;
    cout << "   Hit Ratio   : " << fixed << setprecision(2) << ((float)hits / n) << endl;
    cout << "==============================================================" << endl;
}

bool search(int key, vector<int> &fr)
{
    for (int i : fr)
        if (i == key)
            return true;
    return false;
}

void welcomePage()
{
    system("clear");
    cout << CYAN << "*******************************************************************************" << RESET << "\n";
    cout << CYAN << "*" << RESET << "                                                                             " << CYAN << "*" << RESET << "\n";
    cout << CYAN << "*" << RESET << BOLD << "          PAGE REPLACEMENT ALGORITHM SIMULATOR (LINUX EDITION)               " << RESET << CYAN << "*" << RESET << "\n";
    cout << CYAN << "*" << RESET << "                                                                             " << CYAN << "*" << RESET << "\n";
    cout << CYAN << "*******************************************************************************" << RESET << "\n\n";
    cout << "Algorithms Supported:\n";
    cout << "  1. FIFO\n  2. Optimal\n  3. LRU\n  4. LFU\n  5. Second Chance\n  6. Clock\n\n";
    cout << YELLOW << "Press ENTER to initialize system..." << RESET;
    cin.ignore();
    cin.get();
}

// --- Algorithms ---

void FIFO(int pages[], int n, int capacity)
{
    unordered_set<int> s;
    queue<int> indexes;
    int page_faults = 0, page_hits = 0;
    vector<int> frames(capacity, -1);

    printHeader("FIFO ALGORITHM");

    for (int i = 0; i < n; i++)
    {
        bool hit = false;
        if (s.size() < capacity)
        {
            if (s.find(pages[i]) == s.end())
            {
                s.insert(pages[i]);
                indexes.push(pages[i]);
                frames[indexes.size() - 1] = pages[i];
                page_faults++;
            }
            else
            {
                page_hits++;
                hit = true;
            }
        }
        else
        {
            if (s.find(pages[i]) == s.end())
            {
                int val = indexes.front();
                indexes.pop();
                s.erase(val);
                s.insert(pages[i]);
                indexes.push(pages[i]);
                auto it = find(frames.begin(), frames.end(), val);
                if (it != frames.end())
                    *it = pages[i];
                page_faults++;
            }
            else
            {
                page_hits++;
                hit = true;
            }
        }
        displayStep(i + 1, pages[i], hit, frames, capacity);
    }
    result[0][0] = page_faults;
    result[0][1] = page_hits;
    displaySummary(page_faults, page_hits, n);
}

int predict(int pages[], vector<int> &fr, int pn, int index)
{
    int res = -1, farthest = index;
    for (int i = 0; i < fr.size(); i++)
    {
        int j;
        for (j = index; j < pn; j++)
        {
            if (fr[i] == pages[j])
            {
                if (j > farthest)
                {
                    farthest = j;
                    res = i;
                }
                break;
            }
        }
        if (j == pn)
            return i;
    }
    return (res == -1) ? 0 : res;
}

void optimalPage(int pages[], int n, int capacity)
{
    vector<int> fr(capacity, -1);
    int hit = 0, page_faults = 0;

    printHeader("OPTIMAL ALGORITHM");

    for (int i = 0; i < n; i++)
    {
        bool isHit = false;
        if (search(pages[i], fr))
        {
            hit++;
            isHit = true;
        }
        else
        {
            page_faults++;
            if (find(fr.begin(), fr.end(), -1) != fr.end())
            {
                auto it = find(fr.begin(), fr.end(), -1);
                *it = pages[i];
            }
            else
            {
                int j = predict(pages, fr, n, i + 1);
                fr[j] = pages[i];
            }
        }
        displayStep(i + 1, pages[i], isHit, fr, capacity);
    }
    result[1][0] = page_faults;
    result[1][1] = hit;
    displaySummary(page_faults, hit, n);
}

void LRU(int pages[], int n, int capacity)
{
    unordered_set<int> s;
    unordered_map<int, int> indexes;
    int page_faults = 0, page_hits = 0;
    vector<int> frames(capacity, -1);

    printHeader("LRU ALGORITHM");

    for (int i = 0; i < n; i++)
    {
        bool hit = false;
        if (s.size() < capacity)
        {
            if (s.find(pages[i]) == s.end())
            {
                s.insert(pages[i]);
                frames[s.size() - 1] = pages[i];
                page_faults++;
            }
            else
            {
                page_hits++;
                hit = true;
            }
            indexes[pages[i]] = i;
        }
        else
        {
            if (s.find(pages[i]) == s.end())
            {
                int lru = INT_MAX, val;
                for (auto it = s.begin(); it != s.end(); it++)
                {
                    if (indexes[*it] < lru)
                    {
                        lru = indexes[*it];
                        val = *it;
                    }
                }
                s.erase(val);
                s.insert(pages[i]);
                indexes[pages[i]] = i;
                auto it = find(frames.begin(), frames.end(), val);
                if (it != frames.end())
                    *it = pages[i];
                page_faults++;
            }
            else
            {
                page_hits++;
                hit = true;
            }
            indexes[pages[i]] = i;
        }
        displayStep(i + 1, pages[i], hit, frames, capacity);
    }
    result[2][0] = page_faults;
    result[2][1] = n - page_faults;
    displaySummary(page_faults, n - page_faults, n);
}

void LFU(int pages[], int n, int capacity)
{
    vector<int> frames(capacity, -1);
    unordered_map<int, int> freq, lastUsed;
    int page_faults = 0, time = 0;

    printHeader("LFU ALGORITHM");

    for (int i = 0; i < n; i++)
    {
        bool hit = false;
        if (find(frames.begin(), frames.end(), pages[i]) != frames.end())
        {
            freq[pages[i]]++;
            hit = true;
        }
        else
        {
            page_faults++;
            if (find(frames.begin(), frames.end(), -1) != frames.end())
            {
                auto it = find(frames.begin(), frames.end(), -1);
                *it = pages[i];
            }
            else
            {
                int lfu = INT_MAX, lru_time = INT_MAX, replace_index = -1;
                for (int j = 0; j < capacity; j++)
                {
                    if (freq[frames[j]] < lfu || (freq[frames[j]] == lfu && lastUsed[frames[j]] < lru_time))
                    {
                        lfu = freq[frames[j]];
                        lru_time = lastUsed[frames[j]];
                        replace_index = j;
                    }
                }
                freq.erase(frames[replace_index]);
                frames[replace_index] = pages[i];
            }
            freq[pages[i]]++;
        }
        lastUsed[pages[i]] = ++time;
        displayStep(i + 1, pages[i], hit, frames, capacity);
    }
    result[3][0] = page_faults;
    result[3][1] = n - page_faults;
    displaySummary(page_faults, n - page_faults, n);
}

void SecondChanceReplacement(int pages[], int n, int capacity)
{
    vector<int> arr(capacity, -1);
    vector<bool> second_chance(capacity);
    int pointer = 0, page_faults = 0;

    printHeader("SECOND CHANCE ALGO");

    for (int i = 0; i < n; i++)
    {
        bool hit = false;
        auto it = find(arr.begin(), arr.end(), pages[i]);
        if (it != arr.end())
        {
            second_chance[it - arr.begin()] = true;
            hit = true;
        }
        else
        {
            page_faults++;
            while (second_chance[pointer])
            {
                second_chance[pointer] = false;
                pointer = (pointer + 1) % capacity;
            }
            arr[pointer] = pages[i];
            second_chance[pointer] = false;
            pointer = (pointer + 1) % capacity;
        }
        displayStep(i + 1, pages[i], hit, arr, capacity);
    }
    result[5][0] = page_faults;
    result[5][1] = n - page_faults;
    displaySummary(page_faults, n - page_faults, n);
}

void ClockPageReplacement(int pages[], int n, int capacity)
{
    vector<int> arr(capacity, -1);
    vector<bool> ref_bit(capacity);
    int hand = 0, page_faults = 0;

    printHeader("CLOCK ALGORITHM");

    for (int i = 0; i < n; i++)
    {
        bool hit = false;
        auto it = find(arr.begin(), arr.end(), pages[i]);
        if (it != arr.end())
        {
            ref_bit[it - arr.begin()] = true;
            hit = true;
        }
        else
        {
            page_faults++;
            while (ref_bit[hand])
            {
                ref_bit[hand] = false;
                hand = (hand + 1) % capacity;
            }
            arr[hand] = pages[i];
            ref_bit[hand] = false;
            hand = (hand + 1) % capacity;
        }
        displayStep(i + 1, pages[i], hit, arr, capacity);
    }
    result[4][0] = page_faults;
    result[4][1] = n - page_faults;
    displaySummary(page_faults, n - page_faults, n);
}

// --- Reporting ---

void printTable()
{
    cout << "\n"
         << BOLD << "FINAL COMPARISON RESULTS" << RESET << endl;
    cout << "+-------------------+--------------+--------------+-----------+" << endl;
    cout << "|" << BOLD << " Algorithm         " << RESET << "| " << RED << "Page Faults " << RESET << " | " << GREEN << "Page Hits   " << RESET << " | " << BLUE << "Hit Ratio " << RESET << "|" << endl;
    cout << "+-------------------+--------------+--------------+-----------+" << endl;

    string algorithms[] = {"FIFO", "Optimal", "LRU", "LFU", "Second Chance", "Clock Replacement"};
    for (int i = 0; i < N; i++)
    {
        int faults = result[i][0];
        int hits = result[i][1];
        float hitRatio = (faults + hits == 0) ? 0.0 : static_cast<float>(hits) / (faults + hits);

        cout << "| " << left << setw(18) << algorithms[i]
             << "| " << right << setw(12) << faults
             << " | " << setw(12) << hits
             << " | " << setw(9) << fixed << setprecision(2) << hitRatio << " |" << endl;
    }
    cout << "+-------------------+--------------+--------------+-----------+" << endl;
}

void concludeComparison(int n)
{
    int min_faults = INT_MAX, best_algo = 0;
    string algorithms[] = {"FIFO", "Optimal", "LRU", "LFU", "Second Chance", "Clock Replacement"};

    for (int i = 0; i < N; i++)
    {
        if (result[i][0] < min_faults && result[i][0] != 0)
        { // Simple check, ideally check if run
            min_faults = result[i][0];
            best_algo = i;
        }
    }
    cout << "\n"
         << BOLD << "CONCLUSION: " << RESET
         << "The most efficient algorithm was " << GREEN << algorithms[best_algo] << RESET
         << " with " << BOLD << min_faults << RESET << " page faults.\n";
}

// --- Thread Wrappers ---

struct Args
{
    int *pages;
    int n;
    int capacity;
};

void *runFIFO(void *arg)
{
    sem_wait(&mem_lock);
    FIFO(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}
void *runOPT(void *arg)
{
    sem_wait(&mem_lock);
    optimalPage(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}
void *runLRU(void *arg)
{
    sem_wait(&mem_lock);
    LRU(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}
void *runLFU(void *arg)
{
    sem_wait(&mem_lock);
    LFU(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}
void *runSC(void *arg)
{
    sem_wait(&mem_lock);
    SecondChanceReplacement(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}
void *runClock(void *arg)
{
    sem_wait(&mem_lock);
    ClockPageReplacement(((Args *)arg)->pages, ((Args *)arg)->n, ((Args *)arg)->capacity);
    sem_post(&mem_lock);
    return nullptr;
}

void CompareAll(int pages[], int n, int capacity)
{
    cout << "\n[INFO] Initializing threads for parallel simulation...\n";
    pthread_t threads[6];
    Args args = {pages, n, capacity};

    pthread_create(&threads[0], nullptr, runFIFO, &args);
    pthread_create(&threads[1], nullptr, runOPT, &args);
    pthread_create(&threads[2], nullptr, runLRU, &args);
    pthread_create(&threads[3], nullptr, runLFU, &args);
    pthread_create(&threads[4], nullptr, runSC, &args);
    pthread_create(&threads[5], nullptr, runClock, &args);

    for (int i = 0; i < 6; i++)
        pthread_join(threads[i], nullptr);
    printTable();
    concludeComparison(n);
}

void helpFunction()
{
    system("clear");
    cout << "\n"
         << BOLD << "ALGORITHM REFERENCE GUIDE" << RESET << endl;
    cout << CYAN << "-------------------------" << RESET << endl;
    cout << BOLD << "1. FIFO:" << RESET << " Replaces oldest page in memory. Simple but suffers from Belady's Anomaly.\n";
    cout << BOLD << "2. Optimal:" << RESET << " Replaces page not used for longest future time. Best theoretical performance.\n";
    cout << BOLD << "3. LRU:" << RESET << " Replaces page not used for longest past time. Good approximation of Optimal.\n";
    cout << BOLD << "4. LFU:" << RESET << " Replaces page with lowest frequency count. Good for stable patterns.\n";
    cout << BOLD << "5. Second Chance:" << RESET << " FIFO + reference bit. Gives frequently used pages a 'second life'.\n";
    cout << BOLD << "6. Clock:" << RESET << " Efficient implementation of Second Chance using a circular pointer.\n";
    cout << "\nPress ENTER to return.";
    cin.ignore();
    cin.get();
}

int main()
{
    welcomePage();
    sem_init(&mem_lock, 0, 1);

    int n, capacity;
    cout << BOLD << "Configuration Setup:\n"
         << RESET;
    cout << ">> Enter number of pages: ";
    cin >> n;

    int *pages = new int[n];
    cout << ">> Enter the pages (space separated): ";
    for (int i = 0; i < n; i++)
        cin >> pages[i];

    cout << ">> Enter frame capacity: ";
    cin >> capacity;

    system("clear");

    while (true)
    {
        cout << "\n"
             << CYAN << "============== MAIN MENU ==============" << RESET << endl;
        cout << " 1. FIFO" << endl;
        cout << " 2. Optimal" << endl;
        cout << " 3. LRU" << endl;
        cout << " 4. LFU" << endl;
        cout << " 5. Second Chance" << endl;
        cout << " 6. Clock" << endl;
        cout << " 7. COMPARE ALL" << endl;
        cout << " 8. Help" << endl;
        cout << " 9. " << RED << "Exit" << RESET << endl;
        cout << "=======================================" << endl;
        cout << "Select Option: ";

        char ch;
        cin >> ch;
        switch (ch)
        {
        case '1':
            FIFO(pages, n, capacity);
            break;
        case '2':
            optimalPage(pages, n, capacity);
            break;
        case '3':
            LRU(pages, n, capacity);
            break;
        case '4':
            LFU(pages, n, capacity);
            break;
        case '5':
            SecondChanceReplacement(pages, n, capacity);
            break;
        case '6':
            ClockPageReplacement(pages, n, capacity);
            break;
        case '7':
            CompareAll(pages, n, capacity);
            break;
        case '8':
            helpFunction();
            break;
        case '9':
            delete[] pages;
            return 0;
        default:
            cout << RED << "Invalid choice!" << RESET << endl;
        }

        cout << "\n[M] Menu  [E] Exit: ";
        char c;
        cin >> c;
        if (c == 'e' || c == 'E')
            break;
        system("clear");
    }
    delete[] pages;
    return 0;
}