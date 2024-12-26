#include <bits/stdc++.h>
using namespace std;



struct Node {
    int l, r = -1;
    Node *suffix_link = nullptr;
    unordered_map<char, Node*> next;

    Node() {}

    Node(int l, int r): l(l), r(r) {}
};

struct SuffixTree {
    string str;

    Node *root;
    Node *active_node;
    char active_char = '#';
    int active_len = 0;
    int remainder = 0;
    int end_ptr = -1;

    SuffixTree(string& str): str(str), root(new Node()), active_node(root) {
        process();
    }

    void walk_down(int l, int len) {
        if (len == 0) return; // dont do anything if active_len is 0
        while (1) {
            int start = active_node->next[active_char]->l;
            int end = active_node->next[active_char]->r;
            int sz = (end == -1) ? end_ptr - start + 1 : end - start + 1;

            // we do not 'go down' if it is a leaf node, because leaf nodes will be extended in the next iteration
            if (sz > len || end == -1) break;
            active_node = active_node->next[active_char];
            active_char = str[l + sz];
            active_len -= sz;
        }
    }

    bool update_active(int ptr) {
        if (active_len == 0) {
            if (active_node->next.count(str[ptr]) == 0) return false;
            active_char = str[ptr];
            active_len ++;
        } else {
            if (str[active_node->next[active_char]->l + active_len] != str[ptr]) return false;
            active_len ++;
        }

        walk_down(active_node->next[active_char]->l, active_len);
        return true;
    }

    // s[ptr] is the curr char we want to insert
    Node* insert_node(int ptr, Node* prev) {
        if (active_len == 0) {
            Node *node = new Node(ptr, -1);
            active_node->next[str[ptr]] = node;
            return node;
        } else {
            int l = active_node->next[active_char]->l;
            int r1 = l + active_len - 1;
            int r2 = r1 + 1;
            Node *node1 = new Node(l, r1), *node2 = new Node(ptr, -1), *prev = active_node->next[active_char];
            node1->next[str[ptr]] = node2;
            node1->next[str[r2]] = prev;
            prev->l = r2;
            active_node->next[active_char] = node1;
            if (prev != nullptr) prev->suffix_link = node1; // only connect suffix link if we SPLIT AN EDGE
            return node1;
        }
    }

    void process() {
        for (int ptr = 0; ptr < str.size(); ptr ++) {
           
            end_ptr ++;
            remainder ++;
            Node *prev = nullptr;
            while (remainder > 0) {
                if (update_active(ptr)) break;
                
                prev = insert_node(ptr, prev);
                if (active_node == root) {
                    active_char = (active_len > 1) ? str[active_node->next[active_char]->l + 1] : '#';
                    active_len = max(0, active_len - 1);
                    if (active_len != 0) walk_down(active_node->next[active_char]->l, active_len);
                } else {
                    active_node = (active_node->suffix_link == nullptr) ? root : active_node->suffix_link;
                    if (active_len != 0) walk_down(active_node->next[active_char]->l, active_len);
                }

                remainder --;
            }
        }
    }

    int count_repeated(Node* node) {
        if (node->next.size() == 0) return 0;

        int sum = node->r - node->l + 1; // no need to care when r == -1, as we never count leafs
        for (const auto& [key, value] : node->next) {
            sum += count_repeated(value);
        }
        
        return sum;


    }
};

int main() {
    string s = "abbabbx";
   
    SuffixTree st(s);

    for (const auto& [key, value] : st.root->next) {
        cout << key << " ";
        cout << value->l << " " << value->r << endl;
    }

    cout << st.count_repeated(st.root) << endl;
    
}