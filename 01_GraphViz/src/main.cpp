#include <iostream>
#include "SimpleGraph.h"
#include <vector>
#include <cmath>
#include <ctime>
#include <fstream>
#include<string>
#include<cstring>
#include<stdio.h>
#include<algorithm>
#include <climits>

using std::cout;
using std::endl;
using std::cin;
using std::min;

unsigned int file_in();

void force();

void detect_big();

void outward_2(size_t); //outward 2
void reset_to_zero(); //reset the change of every note
void reset_speed(); //outward 3

SimpleGraph graph;
double change_x[10000];
double change_y[10000];
std::vector<size_t> adj[10000];
std::string a;
size_t n;
size_t dis[500][500];
size_t m = 0;//outward 2 , choose one point to change its place
bool outward_open = true;

const double k_repel = 0.001; //outward 1
const double k_repel_mid = 0.002; //outward 4
const double k_repel_same = 0.0005; //outward 5
const double k_attract = 0.001; //outward 6
const double kPi = 3.14159265358979323;

int main() {
    while (true) {
        graph.nodes = std::vector<Node>();
        graph.edges = std::vector<Edge>();
        unsigned int timing = file_in();
        time_t startTime = time(NULL); //use the time to determine the when to end
        double elapsedTime = difftime(time(NULL), startTime);
        while (elapsedTime <= timing) {
            force();
            elapsedTime = difftime(time(NULL), startTime);
        } //change the position of each notes
        cin.get();
        cout << "Here is the graph . Do you want another one ? ( Yes or No ) " << endl;
        std::string b;
        getline(cin, b);
        if (b != "Yes") {
            cout << "The end.\n";
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}


unsigned int file_in() {
    unsigned int t;
    cout << "Please enter the name of the file : ";
    getline(cin, a);
    std::ifstream inFile;
    inFile.open(a);
    if (!inFile.is_open()) {
        cout << "Could not open the file " << a << endl;
        cout << "Program terminating.\n";
        exit(EXIT_FAILURE);
    }
    cout << "Please enter the operation time:(note : please the outward is a little slow , please enter a little longer time if the graph is not simple ) ";
    while (true) {
        std::cin >> t;
        if (std::cin.fail()) {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            std::cout << "Invalid input. Please enter a number: ";
        } else if (t < 1 || t > INT_MAX) {
            std::cout << "Please enter a time between 1 and " << INT_MAX << ": ";
        } else {
            break;
        }
    } //check if the time is right , or call you to enter again utill a right inload .
    cout<<"Do you want to close the outward to make it faster ? (especailly 10grid or some others if you find it is too slow)";
    std::string c;
    cin>>c;
    outward_open = true;
    if(c == "Yes"){
        outward_open = false ;
    }
    inFile >> n;
    //the fist paw is the number of your points
    for (size_t i = 0; i < n; i++) {
        graph.nodes.push_back(Node{0.38 * cos(2 * kPi * i / n), 0.38 * sin(2 * kPi * i / n)});
    } //draw them on a circle
    for (size_t i = 0; i <= 499; i++)
        for (size_t j = 0; j <= 499; j++) {
            if (i == j) dis[i][i] = 0;
            else dis[i][j] = 10;
        } //floyd(1)--find their distance-->to complete outward 6
    while (!inFile.eof()) {
        size_t p, q;
        inFile >> p >> q;
        graph.edges.push_back(Edge{p, q});
        dis[p][q] = 1;
        dis[q][p] = 1;
        adj[p].push_back(q);
        adj[q].push_back(p);
    } //floyd(2)--find their distance-->to complete outward 6
    for (size_t k = 1; k <= n; k++) {
        for (size_t x = 1; x <= n; x++) {
            for (size_t y = 1; y <= n; y++) {
                dis[x][y] = min(dis[x][y], dis[x][k] + dis[k][y]);
            }
        }
    } //floyd(3)--find their distance-->to complete outward 6
    InitGraphVisualizer(graph);
    DrawGraph(graph);
    inFile.close();
    return t;
}


void force() {
    reset_speed();
    for (size_t i = 0; i < graph.nodes.size(); ++i) {
        for (size_t j = i + 1; j < graph.nodes.size(); ++j) {
            double x0 = graph.nodes[i].x;
            double x1 = graph.nodes[j].x;
            double y0 = graph.nodes[i].y;
            double y1 = graph.nodes[j].y;
            double F_repel = k_repel / sqrt(pow(x0 - x1, 2) + pow(y0 - y1, 2));
            double angel = atan2(y1 - y0, x1 - x0);
            change_x[i] -= F_repel * cos(angel);
            change_y[i] -= F_repel * sin(angel);
            change_x[j] += F_repel * cos(angel);
            change_y[j] += F_repel * sin(angel); //repel every point depend on their distance
            double F_attract = k_attract * (pow(x0 - x1, 2) + pow(y0 - y1, 2));
            angel = atan2(y1 - y0, x1 - x0);
            if (dis[i][j] == 1) {
                change_x[i] += F_attract * cos(angel);
                change_y[i] += F_attract * sin(angel);
                change_x[j] -= F_attract * cos(angel);
                change_y[j] -= F_attract * sin(angel);
            } else if (dis[i][j] > 1 && dis[i][j] < 7 && outward_open ) {
                F_attract *= pow(0.0005, dis[i][j]);//determine by the distance
                for (auto t: adj[i]) {
                    for (auto s: adj[j]) {
                        change_x[t] += F_attract * cos(angel);
                        change_y[t] += F_attract * sin(angel);
                        change_x[s] -= F_attract * cos(angel);
                        change_y[s] -= F_attract * sin(angel);
                    }
                }
            } //outward 6---use the distance of two points to determine their attract
        }
    }

    for (auto i: graph.edges) {
        for (auto j: graph.edges) {
            if ((i.start != j.start || i.end != j.end) && (i.start != j.end || i.end != j.start)) {

                if (i.start == j.start && outward_open) {
                    double x0 = graph.nodes[i.start].x;
                    double y0 = graph.nodes[i.start].y;
                    double x1 = graph.nodes[i.end].x;
                    double y1 = graph.nodes[i.end].y;
                    double x2 = graph.nodes[j.end].x;
                    double y2 = graph.nodes[j.end].y;
                    double theta = atan2(x1 - x0, y1 - y0) - atan2(x2 - x0, y2 - y0);
                    if (theta > kPi)
                        theta -= 2 * kPi;
                    if (theta < -kPi)
                        theta += 2 * kPi;
                    if (fabs(theta) < kPi / 2) {
                        double F_repel = k_repel_same / sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
                        double angel = atan2(y2 - y1, x2 - x1);
                        change_x[i.end] -= F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[i.end] -= F_repel * sin(angel) * (1 - 2 * angel / kPi);
                        change_x[j.end] += F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[j.end] += F_repel * sin(angel) * (1 - 2 * angel / kPi);
                    }
                } //outward 5(1)---if two edges'angel is too small,use a repel to open it
                else if (i.end == j.start && outward_open) {
                    double x0 = graph.nodes[i.end].x;
                    double y0 = graph.nodes[i.end].y;
                    double x1 = graph.nodes[i.start].x;
                    double y1 = graph.nodes[i.start].y;
                    double x2 = graph.nodes[j.end].x;
                    double y2 = graph.nodes[j.end].y;
                    double theta = atan2(x1 - x0, y1 - y0) - atan2(x2 - x0, y2 - y0);
                    if (theta > kPi)
                        theta -= 2 * kPi;
                    if (theta < -kPi)
                        theta += 2 * kPi;
                    if (fabs(theta) < kPi / 2) {
                        double F_repel = k_repel_same / sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
                        double angel = atan2(y2 - y1, x2 - x1);
                        change_x[i.start] -= F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[i.start] -= F_repel * sin(angel) * (1 - 2 * angel / kPi);
                        change_x[j.end] += F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[j.end] += F_repel * sin(angel) * (1 - 2 * angel / kPi);
                    }
                } //outward 5(2)---if two edges'angel is too small,use a repel to open it
                else if (i.start == j.end && outward_open) {
                    double x0 = graph.nodes[i.start].x;
                    double y0 = graph.nodes[i.start].y;
                    double x1 = graph.nodes[i.end].x;
                    double y1 = graph.nodes[i.end].y;
                    double x2 = graph.nodes[j.start].x;
                    double y2 = graph.nodes[j.start].y;
                    double theta = atan2(x1 - x0, y1 - y0) - atan2(x2 - x0, y2 - y0);
                    if (theta > kPi)
                        theta -= 2 * kPi;
                    if (theta < -kPi)
                        theta += 2 * kPi;
                    if (fabs(theta) < kPi / 2) {
                        double F_repel = k_repel_same / sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
                        double angel = atan2(y2 - y1, x2 - x1);
                        change_x[i.end] -= F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[i.end] -= F_repel * sin(angel) * (1 - 2 * angel / kPi);
                        change_x[j.start] += F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[j.start] += F_repel * sin(angel) * (1 - 2 * angel / kPi);
                    }
                } //outward 5(3)---if two edges'angel is too small,use a repel to open it
                else if (i.end == j.end && outward_open) {
                    double x0 = graph.nodes[i.end].x;
                    double y0 = graph.nodes[i.end].y;
                    double x1 = graph.nodes[i.start].x;
                    double y1 = graph.nodes[i.start].y;
                    double x2 = graph.nodes[j.start].x;
                    double y2 = graph.nodes[j.start].y;
                    double theta = atan2(x1 - x0, y1 - y0) - atan2(x2 - x0, y2 - y0);
                    if (theta > kPi)
                        theta -= 2 * kPi;
                    if (theta < -kPi)
                        theta += 2 * kPi;
                    if (fabs(theta) < kPi / 2) {
                        double F_repel = k_repel_same / sqrt(pow(x1 - x2, 2) + pow(y1 - y2, 2));
                        double angel = atan2(y2 - y1, x2 - x1);
                        change_x[i.start] -= F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[i.start] -= F_repel * sin(angel) * (1 - 2 * angel / kPi);
                        change_x[j.start] += F_repel * cos(angel) * (1 - 2 * angel / kPi);
                        change_y[j.start] += F_repel * sin(angel) * (1 - 2 * angel / kPi);
                    }
                } //outward 5(4)---if two edges'angel is too small,use a repel to open it
                else if(i.start != j.start && i.end != j.start && i.start != j.end && i.end != j.end){
                    double x4 = graph.nodes[i.start].x;
                    double x5 = graph.nodes[i.end].x;
                    double y4 = graph.nodes[i.start].y;
                    double y5 = graph.nodes[i.end].y;
                    double x0_mid = (x4 + x5) / 2;
                    double y0_mid = (y4 + y5) / 2;
                    double x2 = graph.nodes[j.start].x;
                    double x3 = graph.nodes[j.end].x;
                    double y2 = graph.nodes[j.start].y;
                    double y3 = graph.nodes[j.end].y;
                    double x1_mid = (x2 + x3) / 2;
                    double y1_mid = (y2 + y3) / 2;
                    if (pow(x0_mid - x1_mid, 2) + pow(y0_mid - y1_mid, 2) == 0) {
                        continue;
                    }
                    double F_repel_mid = k_repel_mid / sqrt(pow(x0_mid - x1_mid, 2) + pow(y0_mid - y1_mid, 2));
                    double angel = atan2(y1_mid - y0_mid, x1_mid - x0_mid);
                    change_x[i.start] -= F_repel_mid * cos(angel);
                    change_y[i.start] -= F_repel_mid * sin(angel);
                    change_x[i.end] -= F_repel_mid * cos(angel);
                    change_y[i.end] -= F_repel_mid * sin(angel);
                    change_x[j.start] += F_repel_mid * cos(angel);
                    change_y[j.start] += F_repel_mid * sin(angel);
                    change_x[j.end] += F_repel_mid * cos(angel);
                    change_y[j.end] += F_repel_mid * sin(angel);
                } //outward 4---use two center invisible point to force away two edges
            }
        }
    }

    for (size_t i = 0; i < graph.nodes.size(); ++i) {
        graph.nodes[i].x += change_x[i];
        graph.nodes[i].y += change_y[i];
    } //move at last
    //next time , we need to change a point to move
    if(outward_open){
        m++;
        outward_2(m);
        detect_big(); //if some points'possitions are too big,make them more visible
    }
    DrawGraph(graph); //draw
}

void outward_2(size_t m) {
    m %= graph.nodes.size();
    graph.nodes[m].x += 0.0001;
    graph.nodes[m].y += 0.0001;
}//outward 2

void reset_to_zero() {
    memset(change_x, 0, sizeof (change_x));
    memset(change_y, 0, sizeof (change_y));
} //while don't use spped(outward 3),use this please

void reset_speed() {
    for (size_t i = 0; i < graph.nodes.size(); ++i) {
        change_x[i] /= 4;
        change_y[i] /= 4;
    }
} //outward 3--make their speed

void detect_big() {
    bool detect1 = false;
    for (size_t i = 0; i < graph.nodes.size(); ++i) {
        if (fabs(graph.nodes[i].x) >= 10 || fabs(graph.nodes[i].y) >= 10)
            detect1 = true;
    }
    while (detect1) {
        for (size_t i = 0; i < graph.nodes.size(); ++i) {
            graph.nodes[i].x /= 2;
            graph.nodes[i].y /= 2;
        }//make it smaller
        detect1 = false;
        for (size_t i = 0; i < graph.nodes.size(); ++i) {
            if (fabs(graph.nodes[i].x) >= 10 || fabs(graph.nodes[i].y) >= 10)
                detect1 = true;
        }//if it is still big ,we have to run this operation some times
    }
}//detect--if the graph is too big , wo have to make it smaller to be visualable
