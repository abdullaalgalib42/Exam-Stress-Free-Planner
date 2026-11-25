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

