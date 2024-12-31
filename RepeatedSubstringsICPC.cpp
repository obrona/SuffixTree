#include <bits/stdc++.h>
using namespace std;

struct SuffixTree {
    struct Node {
        int l, r = -1;
        Node *slink;
        unordered_map<char, Node*> next;

        Node(int l, int r): l(l), r(r) {}

        int get_len(int end_ptr) {
            return (r == -1) ? end_ptr - l + 1 : r - l + 1;
        }

        void print(char c, int len) {
            cout << string(len, ' ') << c << " " << l << " " << r << endl;
            for (const auto& [key, node] : next) {
                node->print(key, len + 1);
            }
        }
    };

    int end_ptr = -1; // must start from -1
    string text;
    Node *root;
    
    Node *active_node;
    char active_char = '#';
    int active_len = 0; // if active_len == 0, active_char is invalid
    int remainder = 0;


    SuffixTree(string& s): text(s), root(new Node(-1, -1)), active_node(root) {
        process();
    }

    void walk_down(int l) {
        while (active_len > 0) {
            Node *edge = active_node->next[active_char];
            int len = edge->get_len(end_ptr);
            if (len > active_len) break;
            
            active_node = edge;
            active_len -= len;
            active_char = (active_len == 0) ? '#' : text[l + len];
            l += len;
        }
    }

    // path to prev node represents the matched chars, but without text[p]
    // returns true if we can match the new char
    bool update_active(int p, Node *prev) {
        char c = text[p];
        if (active_len == 0) {
            if (prev != nullptr) prev->slink = active_node;
            if (active_node->next.count(c) == 0) return false;

            active_char = c;
            active_len = 1;
            Node *edge = active_node->next[c];
            if (edge->get_len(end_ptr) == 1) {
                active_node = edge;
                active_len = 0;
            }
        } else {
            Node *edge = active_node->next[active_char];
            if (text[edge->l + active_len] != c) return false; 

            active_len += 1;
            if (active_len == edge->get_len(end_ptr)) {
                active_node = edge;
                active_len = 0;
            }
        }
        return true;
    }

    // only call this after checking for match using update_active
    // returns the node whose path represents the matched char (for this active_point)
    Node* insert(int p, Node *prev) {
        if (active_len == 0) {
            Node *n = new Node(p, -1);
            active_node->next[text[p]] = n;
            return active_node;
        } else {
            Node *edge = active_node->next[active_char];
            int l = edge->l, r1 = l + active_len - 1, r2 = r1 + 1;
            
            Node *n1 = new Node(l, r1), *n2 = new Node(p, -1);
            edge->l = r2;
            active_node->next[active_char] = n1;
            n1->next[text[r2]] = edge;
            n1->next[text[p]] = n2;
            
            if (prev != nullptr) prev->slink = n1;
            return n1;
        }
    }

    void process() {
        for (int i = 0; i < text.size(); i ++) {
            end_ptr ++;
            remainder ++;

            Node *prev = nullptr;
            while (remainder > 0) {
                if (update_active(i, prev)) break;
                
                prev = insert(i, prev);
                int prev_l = (active_len == 0) ? -1 : active_node->next[active_char]->l;
                if (active_node == root) {
                    active_len = max(0, active_len - 1);
                    active_char = (active_len == 0) ? '#' : text[prev_l + 1];
                    walk_down(prev_l + 1);
                } else {
                    active_node = (active_node->slink == nullptr) ? root : active_node->slink;
                    walk_down(prev_l);
                }
                remainder --;
            }

        }
    }

    int count_repeated_substrings(Node *node) {
        if (node->next.size() == 0) return 0;

        int sum = (node == root) ? 0 : node->get_len(end_ptr);
        for (const auto& [key, edge] : node->next) {
            sum += count_repeated_substrings(edge);
        }
        return sum;
    }

    void print_tree() {
        root->print('#', 0);
    }

    int find_longest_repeated_substring(Node *node, unordered_map<Node*, Node*>& store) {
        int best = 0;
        Node *next = nullptr;
        for (int i = 0; i < 26; i ++) {
            char c = 'a' + i;
            if (node->next.count(c) == 0) continue;

            Node *n = node->next[c];
            if (n->r == -1) continue;
            
            int ans = n->get_len(end_ptr) + find_longest_repeated_substring(n, store);
            if (ans > best) {
                best = ans;
                next = n;
            }
        }
        

        if (best != 0) {
            store[node] = next;
        }
        return best;

    }

    string get_longest_repeated_substring() {
        unordered_map<Node*, Node*> store;
        find_longest_repeated_substring(root, store);
        
        string out = "";
        Node *curr = root;
        while (store.count(curr) > 0) {
            curr = store[curr];
            out.append(text, curr->l, curr->r - curr->l + 1);
        }
        return out;
    }

};

int main() {
    string s;
    cin >> s;
    s += '$';
    SuffixTree st(s);
    cout << st.get_longest_repeated_substring() << endl;
} 