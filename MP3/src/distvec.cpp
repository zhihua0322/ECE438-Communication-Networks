#include <iostream>
#include <fstream>
#include <utility>
#include <vector>
#include <set>
#include <string>

using namespace std;

vector<vector<int>> topology;
vector<vector<pair<int, int>>> forward_table;

// nodes
set<int> nodes;

// message
typedef struct message {
    int src;
    int dest;
    string info;

    message(int src, int dest, string info) : src(src), dest(dest), info(std::move(info)) {}
} message;
vector<message> msgs;

// output file
ofstream outfile("output.txt");

// function
void read_in_msgs(char **argv);

void read_in(ifstream &topofile);

void init_forward_table();

void build_forward_table();

void send_msg();

void update_forward_table();

void print_topo();

void countNodeNum(ifstream &topofile);


int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: ./distvec topofile messagefile changesfile\n");
        return -1;
    }

    // read in messages
    read_in_msgs(argv);

    // read in data
    ifstream topofile(argv[1]);
    countNodeNum(topofile);
    ifstream topofile2(argv[1]);
    read_in(topofile2);

    print_topo();
    // initialization
    init_forward_table();

    // Bellman-Ford Equation
    build_forward_table();

    // send messages
    send_msg();
    int src, dest, cost;

    // change
    ifstream changesfile(argv[3]);
    while (changesfile >> src >> dest >> cost) {
        topology[src][dest] = cost;
        topology[dest][src] = cost;
        print_topo();
        update_forward_table();
        build_forward_table();
        send_msg();
    }

    outfile.close();

    return 0;
}

void print_topo() {
    for (int i = 0; i < topology.size(); i++) {
        for (int j = 0; j < topology[0].size(); j++)
            cout << topology[i][j] << " ";
        cout << endl;
    }
}

void send_msg() {
    int src, dest, middle;
    for (auto &msg : msgs) {
        src = msg.src;
        dest = msg.dest;
        middle = src;

        outfile << "from " << src << " to " << dest << " cost ";
        if (forward_table[src][dest].second < 0) {
            outfile << "infinite hops unreachable ";
        } else if (forward_table[src][dest].second == 0) {
            outfile << forward_table[src][dest].second << " hops ";
        } else {
            outfile << forward_table[src][dest].second << " hops ";
            while (middle != dest) {
                outfile << middle << " ";
                middle = forward_table[middle][dest].first;
            }
        }
        outfile << "message " << msg.info << endl;
    }
    outfile << endl;
}

void build_forward_table() {
    int min_id, min_cost, next_hop, cur_cost, temp_cost;
    int nodes_num = nodes.size();


    for (int it = 1; it <= nodes_num; it++) {
        for (int src: nodes) {
            for (int dest: nodes) {
                next_hop = forward_table[src][dest].first;
                cur_cost = forward_table[src][dest].second;
                min_id = next_hop;
                min_cost = cur_cost;
                for (int middle: nodes) {
                    if (topology[src][middle] >= 0 && forward_table[middle][dest].second >= 0) {
                        temp_cost = topology[src][middle] + forward_table[middle][dest].second;
                        if (min_cost < 0 || temp_cost < min_cost) {
                            min_id = middle;
                            min_cost = temp_cost;
                        }
                    }
                }
                forward_table[src][dest] = make_pair(min_id, min_cost);
            }
        }
    }

    for (int src: nodes) {
        for (int dest: nodes) {
            outfile << dest << " " << forward_table[src][dest].first << " " << forward_table[dest][src].second
                    << endl;
        }
    }
}


void init_forward_table() {
    for (int src : nodes) {
        for (int dest : nodes) {
            if (topology[src][dest] >= 0) {
                forward_table[src][dest] = make_pair(dest, topology[src][dest]);
            } else {
                forward_table[src][dest] = make_pair(-999, topology[src][dest]);
            }
        }
    }
}

void update_forward_table() {
    for (int src : nodes) {
        for (int dest : nodes) {
            if (topology[src][dest] >= 0) {
                forward_table[src][dest] = make_pair(dest, topology[src][dest]);
            } else {
                forward_table[src][dest] = make_pair(-999, topology[src][dest]);
            }
        }
    }
}

void countNodeNum(ifstream &topofile) {
    int src_id, dst_id, cost;    // send messages
    while (topofile >> src_id >> dst_id >> cost) {
        nodes.insert(src_id);
        nodes.insert(dst_id);
    }
}


void read_in(ifstream &topofile) {
    int src, dest, cost;
    int nodes_num = nodes.size();

    for (int i = 0; i <= nodes_num; i++) {
        forward_table.emplace_back();
        forward_table[i].resize(nodes_num + 1);
        topology.emplace_back();
        topology[i].resize(nodes_num + 1);
    }

    for (int i = 1; i <= nodes_num; i++) {
        for (int j = 1; j <= nodes_num; j++) {
            topology[i][j] = -999;
            if (i == j) {
                topology[i][j] = 0;
            }
        }
    }

    // initialize graph
    while (topofile >> src >> dest >> cost) {
        topology[src][dest] = cost;
        topology[dest][src] = cost;
    }
}

void read_in_msgs(char **argv) {
    int src, dest;

    ifstream messagefile(argv[2]);

    string line, info;
    while (getline(messagefile, line)) {
        if (!line.empty()) {
            sscanf(line.c_str(), "%d %d %*s", &src, &dest);
            line = line.substr(line.find(' ') + 1);     // find first space
            info = line.substr(line.find(' ') + 1);  // find second space
            message msg(src, dest, info);
            msgs.push_back(msg);
        }
    }
}


