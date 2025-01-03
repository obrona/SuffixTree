#include <bits/stdc++.h>
using namespace std;

struct Node {
    int l, r = -1;
    Node *slink = nullptr;
    unordered_map<char, Node*> edges;

    Node() {}

    Node(int l, int r): l(l), r(r) {}

    int get_len(int end_ptr) {
        return (r == -1) ? end_ptr - l + 1 : r - l + 1;
    }

    void print(char c, int len) {
        cout << string(len, ' ') << c << " " << l << " " << r << endl;
        for (const auto& [key, node] : edges) {
            node->print(key, len + 1);
        }

    }
};

struct SuffixTree {
    string str;
    int end_ptr = -1;

    Node *root;
    Node *active_node;
    char active_char = '#'; // only '#' if active_len is 0, because we dont have a char
    int active_len = 0;
    int remainder = 0;

    SuffixTree(string& s): str(s), root(new Node(0, 0)), active_node(root) {
        process();
    }

    // move active node.
    // l is the starting index of str, with active_len the length of the substring we are referencing
    // we do not need to worry about exceeding a leaf node, this is not possible
    // as active is the like the prefix of some suffix we are matching, and the pos to match cannot exceed end_ptr
    void walk_down(int l) {
        if (active_len == 0) return;
        
        while (active_len > 0) {
            Node *next = active_node->edges[active_char];
            int len = next->get_len(end_ptr);

            if (len > active_len) break;
            active_node = next;
            active_len -= len;
            active_char = (active_len == 0) ? '#' : str[l + len];
            l += len;
            
            
        }
    }

    // check if we can match str[ptr]
    bool update_active(int ptr, Node *prev) {
        char c = str[ptr];
        if (active_len == 0) {
            if (prev != nullptr) prev->slink = active_node; // if we found a path to the node of 1 char removed suffix
            if (active_node->edges.count(c) == 0) return false;


            Node *edge = active_node->edges[c];
            active_char = c;
            active_len = 1;
            if (edge->get_len(end_ptr) == 1) {
                active_node = edge;
                active_len = 0;
            }

        } else {
            Node *edge = active_node->edges[active_char];
            if (str[edge->l + active_len] != c) return false;
            
            active_len ++;
            if (edge->get_len(end_ptr) == active_len) {
                active_node = edge;
                active_len = 0;
            }

        }
        
        //walk_down(active_node->edges[active_char]->l);
        return true;
    }


    Node* insert(int ptr, Node *to_link) {
        if (active_len == 0) {
            if (to_link != nullptr) to_link->slink = active_node;
            Node *n = new Node(ptr, -1);
            active_node->edges[str[ptr]] = n;
            return n;
        } else {
            Node *edge = active_node->edges[active_char];
            int l = edge->l, r1 = l + active_len - 1, r2 = r1 + 1;
            
            Node *n1 = new Node(l, r1), *n2 = new Node(ptr, -1);
            n1->edges[str[ptr]] = n2;
            n1->edges[str[r2]] = edge;
            edge->l = r2;
            active_node->edges[active_char] = n1;
            if (to_link != nullptr) to_link->slink = n1; // only link if we split an edge, we do not link leafs
            return n1;
        }
    }

    // if active_node path is only 1 char, dont have suffix link, invariant maintained
    // as next insertion is from root
    void process() {
        for (int i = 0; i < str.size(); i ++) {
            end_ptr ++;
            remainder ++;

            
            Node *prev_inserted = nullptr;
            while (remainder > 0) {
                if (update_active(i, prev_inserted)) break;

                int prev_l = (active_len == 0) ? -1 : active_node->edges[active_char]->l;
                prev_inserted = insert(i, prev_inserted);
                if (active_node == root) {
                    active_len = max(0, active_len - 1);
                    active_char = (active_len == 0) ? '#' : str[prev_l + 1];
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
        if (node->edges.size() == 0) return 0;

        int sum = (node == root) ? 0 : node->get_len(end_ptr);
        for (const auto& [c, next] : node->edges) sum += count_repeated_substrings(next);

        return sum;
    }

    void print_tree() {
        //cout << root->edges.size() << endl;
        root->print('#', 0);
    }

    void print_active() {
        cout << active_node << endl;
        cout << active_char << endl;
        cout << active_len << endl;
        cout << remainder << endl;
    }

    
};

int main() {
    int T;
    cin >> T;
    for (int i = 0; i < T; i ++) {
        string s;
        cin >> s;
        s += '$';
        SuffixTree st(s);
        cout << st.count_repeated_substrings(st.root) << '\n';
    }
}