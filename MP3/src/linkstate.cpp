#include <iostream>
#include <set>
#include <fstream>
#include <vector>

using namespace std;
typedef struct message {
    int src;
    int dest;
    string info;

    message(int src, int dest, string info) : src(src), dest(dest), info(std::move(info)) {}
} message;
vector<message> msgs;

//set is used for
set<int> nodes;
int nodeNum;
vector<vector<int>> graph;
vector<vector<pair<int, int>>> tempGraph;
vector<vector<pair<int, int>>> forwardtable;
ofstream output("output.txt");

void read_in_msgs(char **argv);

void send_msg();

void buildGraph(ifstream &topofile);

int countNodeNum(ifstream &topofile);

void initTemp();

void initForward();

void buildTemp();

void dijkstra();

int main(int argc, char **argv) {
    if (argc != 4) {
        printf("Usage: ./linkstate topofile messagefile changesfile\n");
        return -1;
    }
    read_in_msgs(argv);
    ifstream topofile(argv[1]);
    countNodeNum(topofile);
    ifstream topofile2(argv[1]);
    buildGraph(topofile2);
    initTemp();
    initForward();
    buildTemp();
    dijkstra();
    send_msg();
    ifstream changefile(argv[3]);
    int srcchange, dstchange, newcost;
    while (changefile >> srcchange >> dstchange >> newcost) {
        graph[srcchange][dstchange] = newcost;
        graph[dstchange][srcchange] = newcost;
        buildTemp();
        dijkstra();
        send_msg();
    }
    output.close();
    return 0;
}

void buildGraph(ifstream &topofile) {
    int src_id, dst_id, cost;
    //initialize the value
    for (int i = 0; i < nodeNum + 1; i++) {
        vector<int> temp;
        for (int j = 0; j < nodeNum + 1; j++) {
            if (i == j) {
                temp.push_back(0);
            } else {
                temp.push_back(-999);
            }
        }
        graph.push_back(temp);
    }
    while (topofile >> src_id >> dst_id >> cost) {
        graph[src_id][dst_id] = cost;
        graph[dst_id][src_id] = cost;
    }
}

int countNodeNum(ifstream &topofile) {
    int src_id, dst_id, cost;
    while (topofile >> src_id >> dst_id >> cost) {
        nodes.insert(src_id);
        nodes.insert(dst_id);
    }
    nodeNum = nodes.size();
    return nodes.size();
}

void initForward() {
    for (int i = 0; i < nodeNum + 1; i++) {
        forwardtable.emplace_back();
    }
}

void initTemp() {
    for (int i = 0; i < nodeNum + 1; i++) {
        vector<pair<int, int>> temp;
        for (int j = 0; j < nodeNum + 1; j++) {
            temp.emplace_back(pair<int, int>());
        }
        tempGraph.push_back(temp);
    }
}


void buildTemp() {
    int src_id, dst_id;
    for (int node1:nodes) {
        src_id = node1;
        for (int node : nodes) {
            dst_id = node;
            tempGraph[src_id][dst_id] = make_pair(src_id, graph[src_id][dst_id]);
        }
    }
}

void dijkstra() {
    set<int> visited;
    int src, dst, min_id, mincost;
    int tempid;
    for (int node:nodes) {
        src = node;
        visited.clear();
        min_id = src;
        mincost = 0;
        visited.insert(src);
        for (int i = 1; i <= nodeNum + 1; i++) {
            for (int node2:nodes) {
                dst = node2;
                if (graph[min_id][dst] >= 0
                    && visited.find(dst) == visited.end()
                    && (mincost + graph[min_id][dst] < tempGraph[src][dst].second
                        || tempGraph[src][dst].second < 0)
                        ) {
                    tempGraph[src][dst] = make_pair(min_id, mincost + graph[min_id][dst]);
                }
                if (graph[min_id][dst] >= 0
                    && visited.find(dst) == visited.end()
                    && (mincost + graph[min_id][dst] == tempGraph[src][dst].second
                        || tempGraph[src][dst].second < 0)) {
                    if (min_id < tempGraph[src][dst].first) {
                        tempGraph[src][dst] = make_pair(min_id, mincost + graph[min_id][dst]);
                    }
                }
            }
            mincost = 9999999;
            for (int node2:nodes) {
                dst = node2;
                if (visited.find(dst) == visited.end()
                    && mincost > tempGraph[src][dst].second
                    && tempGraph[src][dst].second >= 0) {
                    mincost = tempGraph[src][dst].second;
                    min_id = dst;
                }
            }
            visited.insert(min_id);
        }
        forwardtable[src] = tempGraph[src];
        for (int m:nodes) {
            dst = m;
            tempid = dst;
            if (forwardtable[src][dst].second >= 0) {
                while (tempGraph[src][tempid].first != src) {
                    tempid = tempGraph[src][tempid].first;
                }
                forwardtable[src][dst].first = tempid;
                output << dst << " " << forwardtable[src][dst].first << " " << forwardtable[src][dst].second << endl;
            }
        }
        output << endl;
    }
}

void send_msg() {
    int src, dest, middle;
    for (auto &msg : msgs) {
        src = msg.src;
        dest = msg.dest;
        middle = src;

        output << "from " << src << " to " << dest << " cost ";
        if (forwardtable[src][dest].second < 0) {
            output << "infinite hops unreachable ";
        } else if (forwardtable[src][dest].second == 0) {
            output << forwardtable[src][dest].second << " hops ";
        } else {
            output << forwardtable[src][dest].second << " hops ";
            while (middle != dest) {
                output << middle << " ";
                middle = forwardtable[middle][dest].first;
            }
        }
        output << "message " << msg.info << endl;
    }
    output << endl;
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
