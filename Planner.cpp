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

