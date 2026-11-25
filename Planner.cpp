#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>

using namespace std;

const int MAX_TASKS = 100;      
const int MAX_TOPICS = 20;     
const int UNDO_LIMIT = 100;     


class TextUtil {
public:
    static string trim(const string& s) {
        size_t a = 0, b = s.size();
        while (a < b && isSpace(s[a])) a++;
        while (b > a && isSpace(s[b - 1])) b--;
        return s.substr(a, b - a);
    }


    static string cleanName(const string& s) {
        string r;
        for (size_t i = 0; i < s.size(); i++) {
            char c = s[i];
            if (c == '|' || c == '~' || c == ',' || c == '\t' || c == '\r' || c == '\n') continue;
            r += c;
        }
        return trim(r);
    }


    static bool toInt(const string& s, int& out) {
        string t = trim(s);
        if (t.empty()) return false;
        size_t i = 0;
        bool neg = false;
        if (t[0] == '-') { neg = true; i = 1; }
        if (i >= t.size()) return false;
        long v = 0;
        for (; i < t.size(); i++) {
            if (t[i] < '0' || t[i] > '9') return false;
            v = v * 10 + (t[i] - '0');
            if (v > 1000000) return false;
        }
        out = (int)(neg ? -v : v);
        return true;
    }


    static string fit(const string& s, size_t width) {
        string r = s;
        if (r.size() > width) {
            size_t cut = width - 2;
            while (cut > 0 && ((unsigned char)r[cut] & 0xC0) == 0x80) cut--;
            r = r.substr(0, cut) + "..";
        }
        while (r.size() < width) r += ' ';
        return r;
    }

private:
    static bool isSpace(char c) { return c == ' ' || c == '\t' || c == '\r' || c == '\n'; }
};
class Console {
public:
    static string readLine(const string& prompt) {
        cout << prompt;
        string s;
        if (!getline(cin, s)) {           
            cout << "\nGoodbye!\n";
            exit(0);                      
        }
        return TextUtil::trim(s);
    }

    static int readInt(const string& prompt, int lo, int hi) {
        while (true) {
            int v;
            if (TextUtil::toInt(readLine(prompt), v) && v >= lo && v <= hi) return v;
            cout << "  Please enter a number between " << lo << " and " << hi << ".\n";
        }
    }

    static bool askYes(const string& prompt) {
        while (true) {
            string s = readLine(prompt + " (y/n): ");
            if (s == "y" || s == "Y" || s == "yes") return true;
            if (s == "n" || s == "N" || s == "no") return false;
        }
    }
};
class DirectedGraph {
private:
    static const int MAXV = 50;
    int V;
    int adj[MAXV][MAXV];
    string topics[MAXV];

    void dfs(int v, bool visited[], int stack[], int& stackTop) const {
        visited[v] = true;
        for (int i = 0; i < V; i++) {
            if (adj[v][i] == 1 && !visited[i]) dfs(i, visited, stack, stackTop);
        }
        stack[++stackTop] = v;             
    }

public:
    explicit DirectedGraph(int vertices) : V(vertices) {
        for (int i = 0; i < MAXV; i++)
            for (int j = 0; j < MAXV; j++) adj[i][j] = 0;
    }

    void setTopicName(int index, const string& name) {
        if (index >= 0 && index < V) topics[index] = name;
    }
    const string& topicName(int i) const { return topics[i]; }


    void addEdge(int u, int v) {
        if (u >= 0 && u < V && v >= 0 && v < V) adj[u][v] = 1;
    }


    bool hasPath(int from, int to) const {
        if (from < 0 || from >= V || to < 0 || to >= V) return false;
        bool visited[MAXV] = {false};
        int stack[MAXV];
        int top = -1;
        stack[++top] = from;
        visited[from] = true;
        while (top >= 0) {
            int v = stack[top--];
            if (v == to) return true;
            for (int i = 0; i < V; i++) {
                if (adj[v][i] == 1 && !visited[i]) {
                    visited[i] = true;
                    stack[++top] = i;
                }
            }
        }
        return false;
    }

  
    int studySequence(int order[]) const {
        bool visited[MAXV] = {false};
        int stack[MAXV];
        int top = -1;
        for (int i = 0; i < V; i++) {
            if (!visited[i] && !topics[i].empty()) dfs(i, visited, stack, top);
        }
        int n = 0;
        while (top >= 0) order[n++] = stack[top--];
        return n;
    }
};
class TopicSet {
public:
    enum AddResult { ADD_OK, ADD_EMPTY, ADD_FULL, ADD_DUPLICATE };
    enum LinkResult { LINK_OK, LINK_SAME, LINK_EXISTS, LINK_LOOP };

private:
    string names[MAX_TOPICS];
    bool before[MAX_TOPICS][MAX_TOPICS];
    int n;

    void toGraph(DirectedGraph& g) const {
        for (int i = 0; i < n; i++) g.setTopicName(i, names[i]);
        for (int i = 0; i < n; i++)
            for (int j = 0; j < n; j++)
                if (before[i][j]) g.addEdge(j, i);
    }

public:
    TopicSet() : n(0) {
        for (int i = 0; i < MAX_TOPICS; i++)
            for (int j = 0; j < MAX_TOPICS; j++) before[i][j] = false;
    }

    int size() const { return n; }
    bool isEmpty() const { return n == 0; }
    const string& name(int i) const { return names[i]; }
    bool needs(int i, int j) const { return before[i][j]; }

    AddResult addTopic(const string& rawName) {
        string nm = TextUtil::cleanName(rawName);
        if (nm.empty()) return ADD_EMPTY;
        if (n >= MAX_TOPICS) return ADD_FULL;
        for (int i = 0; i < n; i++)
            if (names[i] == nm) return ADD_DUPLICATE;
        names[n++] = nm;
        return ADD_OK;
    }

    LinkResult addPrerequisite(int a, int b) {
        if (a == b) return LINK_SAME;
        if (before[a][b]) return LINK_EXISTS;
        DirectedGraph g(n);
        toGraph(g);
        if (g.hasPath(a, b)) return LINK_LOOP;
        before[a][b] = true;
        return LINK_OK;
    }

    bool removePrerequisite(int a, int b) {
        if (!before[a][b]) return false;
        before[a][b] = false;
        return true;
    }

    void removeTopic(int k) {
        bool old[MAX_TOPICS][MAX_TOPICS];
        for (int i = 0; i < MAX_TOPICS; i++)
            for (int j = 0; j < MAX_TOPICS; j++) old[i][j] = before[i][j];

        int m = n - 1;
        for (int i = 0; i < m; i++) {
            int oi = (i < k) ? i : i + 1;
            names[i] = names[oi];
            for (int j = 0; j < m; j++) {
                int oj = (j < k) ? j : j + 1;
                before[i][j] = old[oi][oj];
            }
        }
        names[m] = "";
        for (int i = 0; i < MAX_TOPICS; i++) {
            before[m][i] = false;
            before[i][m] = false;
        }
        n = m;
    }

  
    int studyOrder(int order[]) const {
        DirectedGraph g(n);
        toGraph(g);
        return g.studySequence(order);
    }

 
    string serialize() const {
        string out;
        for (int i = 0; i < n; i++) {
            if (i > 0) out += '|';
            out += names[i];
            out += '~';
            bool first = true;
            for (int j = 0; j < n; j++) {
                if (before[i][j]) {
                    if (!first) out += ',';
                    out += to_string(j);
                    first = false;
                }
            }
        }
        return out;
    }

    static TopicSet parse(const string& s) {
        TopicSet ts;
        if (s.empty()) return ts;

   
        string prereqText[MAX_TOPICS];
        size_t pos = 0;
        while (pos <= s.size() && ts.n < MAX_TOPICS) {
            size_t bar = s.find('|', pos);
            string part = s.substr(pos, bar == string::npos ? string::npos : bar - pos);
            if (!part.empty()) {
                size_t tilde = part.find('~');
                string nm = (tilde == string::npos) ? part : part.substr(0, tilde);
                ts.names[ts.n] = nm.empty() ? "(topic)" : nm;
                if (tilde != string::npos) prereqText[ts.n] = part.substr(tilde + 1);
                ts.n++;
            }
            if (bar == string::npos) break;
            pos = bar + 1;
        }

        for (int i = 0; i < ts.n; i++) {
            const string& t = prereqText[i];
            size_t p = 0;
            while (p <= t.size()) {
                size_t comma = t.find(',', p);
                string num = t.substr(p, comma == string::npos ? string::npos : comma - p);
                int v;
                if (TextUtil::toInt(num, v) && v >= 0 && v < ts.n && v != i) ts.before[i][v] = true;
                if (comma == string::npos) break;
                p = comma + 1;
            }
        }
        return ts;
    }
};





class Task {
private:
    string name_;
    string course_;
    int daysLeft_;
    TopicSet topics_;

public:
    Task() : daysLeft_(0) {}
    Task(const string& name, const string& course, int daysLeft)
        : name_(name), course_(course), daysLeft_(daysLeft) {}

    const string& name() const { return name_; }
    const string& course() const { return course_; }
    int daysLeft() const { return daysLeft_; }
    const TopicSet& topics() const { return topics_; }
    void setTopics(const TopicSet& t) { topics_ = t; }

    bool sameAs(const Task& o) const {
        return name_ == o.name_ && course_ == o.course_ && daysLeft_ == o.daysLeft_;
    }
};
class MinHeap {
private:
    Task* arr;
    int capacity;
    int count;

    void swapTasks(Task& a, Task& b) {
        Task t = a;
        a = b;
        b = t;
    }

    void heapifyUp(int i) {
        while (i > 0 && arr[(i - 1) / 2].daysLeft() > arr[i].daysLeft()) {
            swapTasks(arr[i], arr[(i - 1) / 2]);
            i = (i - 1) / 2;
        }
    }

    void heapifyDown(int i) {
        int smallest = i;
        int l = 2 * i + 1;
        int r = 2 * i + 2;
        if (l < count && arr[l].daysLeft() < arr[smallest].daysLeft()) smallest = l;
        if (r < count && arr[r].daysLeft() < arr[smallest].daysLeft()) smallest = r;
        if (smallest != i) {
            swapTasks(arr[i], arr[smallest]);
            heapifyDown(smallest);
        }
    }

public:
    explicit MinHeap(int cap) : arr(new Task[cap]), capacity(cap), count(0) {}
    ~MinHeap() { delete[] arr; }

    MinHeap(const MinHeap& o) : arr(new Task[o.capacity]), capacity(o.capacity), count(o.count) {
        for (int i = 0; i < count; i++) arr[i] = o.arr[i];
    }
    MinHeap& operator=(const MinHeap& o) {
        if (this != &o) {
            delete[] arr;
            arr = new Task[o.capacity];
            capacity = o.capacity;
            count = o.count;
            for (int i = 0; i < count; i++) arr[i] = o.arr[i];
        }
        return *this;
    }

    bool isEmpty() const { return count == 0; }
    bool isFull() const { return count == capacity; }
    int size() const { return count; }

    bool peek(Task& out) const {
        if (isEmpty()) return false;
        out = arr[0];
        return true;
    }

    bool insert(const Task& t) {                      
        if (isFull()) return false;
        arr[count] = t;
        count++;
        heapifyUp(count - 1);
        return true;
    }

    bool extractMin(Task& out) {                    
        if (isEmpty()) return false;
        out = arr[0];
        arr[0] = arr[count - 1];
        count--;
        heapifyDown(0);
        return true;
    }

    int indexOf(const Task& t) const {              
        for (int i = 0; i < count; i++)
            if (arr[i].sameAs(t)) return i;
        return -1;
    }

    bool removeAt(int i) {                            
        if (i < 0 || i >= count) return false;
        arr[i] = arr[count - 1];
        count--;
        if (i < count) {
            heapifyDown(i);
            heapifyUp(i);
        }
        return true;
    }

    bool remove(const Task& t) { return removeAt(indexOf(t)); }

    bool updateTopics(const Task& t, const TopicSet& topics) {
        int i = indexOf(t);
        if (i < 0) return false;
        arr[i].setTopics(topics);
        return true;
    }

    const Task& at(int i) const { return arr[i]; }
};
