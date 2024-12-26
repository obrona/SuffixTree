#include <bits/stdc++.h>
using namespace std;

// -1 means it is the end variable #
struct Edge {
    int l, r = -1;
    int idx; // next node idx
};

struct Node {
    int suffix_link = 0; // just point to root as default
    unordered_map<char, Edge> edges;
};

// ukkonen algorithm
// invariants
// 1. once a leaf, always a leaf, because we never insert at leaf, because leaf contains a suffix i.e [l, end], cannot extemd
// 2. suffix link is to a string path with 1 fewer char from the front
// 3. all nodes at lvl 1 and below, except leafs have suffix link (because we never after a leaf node)
//    because suppose we have a path abc-(ccd, xxy) (split path), then we have a suffix abcccd... and abcxxy...
//    then we have a suffix bccd... and bcxxy...
//    that means path bc must split
//    only exception is if node is at lvl 1 and edge contains 1 char only 

// suffix link for nodes at lvl 1 is to root (implicitly)
// root is always at idx 0

struct SuffixTree {
    string s;

    int active_node_idx;
    char active_edge_char;
    int active_len;
    int remainder;
    int end_ptr;

    vector<Node> nodes;

    SuffixTree(string& s): s(s), nodes(1, Node()) {}


    int new_node() {
        int idx = nodes.size();
        nodes.emplace_back();
        return idx;
    }

    Node& get_node(int idx) {
        return nodes[idx];
    }

    // the substring pointed to starts at l and has alen char, so from [l, l + alen - 1]
    void shift_active(int l, int alen) {
        while (1) {
            Node& node = get_node(active_node_idx);
            Edge& edge = node.edges[active_edge_char];
            int len = (edge.r == -1) ? end_ptr - edge.l + 1 : edge.r - edge.l + 1;

            if (len > alen) break;
            active_node_idx = edge.idx;
            active_edge_char = s[l + len];
            active_len -= len;
        }
    }


    // check whether we can just update active
    // case 1: active_len == 0
    // case 2: active_len > 0
    bool update_active(int ptr) {
        Node& node = get_node(active_node_idx);
        
        if (active_len == 0) {
            if (node.edges.count(s[ptr]) == 0) return false;
            active_edge_char = s[ptr];
            active_len ++;
        } else {
            char c = s[node.edges[active_edge_char].l + active_len];
            if (c != s[ptr]) return false;
            active_len ++;
        }
    }

    // have to insert node, returns idx of the new node that is needed for suffix link
    // case 1. new node needed does not split an edge (e.g active_len == 0), then only 1 node created
    // case 2. need to split an edge, 2 nodes created, have to return the correct node idx needed for suffix link   
    int insert_node(int ptr) {
        Node& active_node = get_node(active_node_idx);
        Edge& active_edge = active_node.edges[active_edge_char];
        
        
        if (active_len == 0) {
            int idx = new_node();
            active_node.edges[s[ptr]] = {ptr, -1, idx};
            return idx;
        } else {
            int node1_idx = new_node();
            int node2_idx = new_node();
            
            Node &n1 = get_node(node1_idx), &n2 = get_node(node2_idx);
            n1.edges[s[ptr]] = {ptr, -1, node2_idx};
            
            
            int r1 = active_edge.l + active_len - 1;
            int r2 = r1 + 1;
            
            n1.edges[s[r2]] = {r2, active_edge.r, active_edge.idx};
            active_edge.r = r1;
            active_edge.idx = node1_idx;

            return node1_idx;
        }
    }

    void update(int ptr) {
        end_ptr ++;
        remainder ++;

        int prev_idx = 0;
        while (remainder > 0) {
            if (update_active(ptr)) break;

            int new_node_idx = insert_node(ptr);
            
            // insert at root
            if (active_node_idx == 0) {
                active_edge_char = s[get_node(active_node_idx).edges[active_edge_char].l + 1];
                active_len --;
                remainder --;
            }


        }

    }
};
