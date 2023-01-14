// ConsoleApplication1.cpp : This file contains the 'main' function. Program execution begins and ends there.
//export PKG_CONFIG_PATH=C:/MinGW/msys/1.0/home/ASUS/Ibex/ibex-2.8.9/share/pkgconfig   
#include "ibex.h"
#include <iostream>
#include <string> // for string class
#include <fstream>
#include <map>
#include <vector>
#include <algorithm>   
#include <json/json.h>
#include "jsoncpp.cpp"
#include <cmath>
#include <queue>
#include <bitset>
#include <numeric>
#include <ctime>
#include <tuple>
#include <chrono>



#define MAX  100
#define FILE_PATH "C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\test_generation\\testing_roads\\"
#define EXT ".json"

using namespace std;
using namespace ibex;
using namespace std::chrono;
static int identifier = 0;

static float boxEps = 1e-02;
static int numberSolutions = 1;
static float limitTime = 7;
static int limitRun = 3;




using namespace std;
using namespace ibex;


struct my_comparatoor
{
    // queue elements are vectors so we need to compare those
    bool operator()(IntervalVector const& a, IntervalVector const& b) const
    {
        return a.max_diam() < b.max_diam();
    }
};

using my_priority_queuee = std::priority_queue<IntervalVector, std::vector<IntervalVector>, my_comparatoor>;

class Scenarioo {
public:
    string fileName;					// name of the minibex file
    int nVar;							// number of variables
    double eps = 1e-2;				// criterion to stop bisection: do not bisect domains smaller than eps
    int nBoxes = 1;					// criterion to stop bisection: maximum number of boxes of the partition cover
    System* sys;						// pointer to the constraint system
    Ctc* ctc;							// pointer to the constraint contractor
    vector<IntervalVector> partition; 	// a box partition that contains all possible solutions
    vector<IntervalVector> solutions;	// one solution for each box in the partition

    Scenarioo(string name);				// class constructor: reads the minibex system and defines the contractor
    void setHC4Contractor();			// a contractor
    void setFixPointContractor();		// a contractor
    void prune(IntervalVector& box);	// prune a box with the defined contractor
    void setEps(double e) { eps = e; };	// redefine the eps parameter
    void setNBoxes(int n) { nBoxes = n; };	// redefine the nBoxes parameter
    IntervalVector searchSolution(IntervalVector box);	// searches a solution within a box
    void solve();						// computes a partition with nBoxes and a solution within each box
    void showVariables();				// shows the variables
    void showPartition();				// shows the boxes of the partition
    void showSolutions();				// shows the solution for each box of the partition
    void showAll();						// shows the boxes of the partition and respective solutions
};


Scenarioo::Scenarioo(string name) {
    fileName = name;
    ifstream myfile(fileName);
    static System ss(&fileName[0]);
    sys = &ss;
    // choose/implement the adequate contractor
    setHC4Contractor();
    //setFixPointContractor();
    nVar = sys->nb_var;
    partition.clear();
    partition.push_back(sys->box);
    solutions.clear();
}

void Scenarioo::setHC4Contractor() {
    static CtcHC4 hc(*sys);
    ctc = &hc;
}

void Scenarioo::setFixPointContractor() {
    int nCstr = sys->nb_ctr;
    static Array<Ctc> vctc(nCstr);
    for (int i = 0; i < nCstr; i++) vctc.set_ref(i, *new CtcHC4(sys->ctrs[i]));
    static CtcCompo compo(vctc);
    static CtcFixPoint fix(compo);
    ctc = &fix;
}

void Scenarioo::showVariables() {
    cout << "variables: " << nVar << endl;
    std::vector<std::string> varNames = sys->var_names();
    IntervalVector box = sys->box;
    for (int i = 0; i < nVar; i++) cout << "   " << varNames[i] << " in " << box[i] << endl;
}

void Scenarioo::showPartition() {
    IntervalVector box;
    cout << "partition with " << partition.size() << " boxes:" << endl;
    for (int i = 0; i < partition.size(); i++) {
        box = partition[i];
        cout << "   box " << i << " -> " << box << " size: " << box.max_diam() << endl;
    }
}

void Scenarioo::showSolutions() {
    IntervalVector box;
    cout << solutions.size() << " solutions:" << endl;
    for (int i = 0; i < solutions.size(); i++) {
        cout << solutions[0] << endl;
        cout << "yoyo" << endl;
        box = solutions[i];
        cout << "   sol " << i << " -> " << box << " size: " << box.max_diam() << endl;
    }
}

void Scenarioo::showAll() {
    IntervalVector box;
    cout << "partition with " << partition.size() << " boxes:" << endl;
    for (int i = 0; i < partition.size(); i++) {
        box = partition[i];
        cout << "   box " << i << " -> " << box << " size: " << box.max_diam() << endl;
        box = solutions[i];
        cout << "   sol " << i << " -> " << box << " size: " << box.max_diam() << endl;
    }
}

void Scenarioo::prune(IntervalVector& box) {
    ctc->contract(box);
}

IntervalVector Scenarioo::searchSolution(IntervalVector box) {
    //cout << "+++ search: " << box << " size: " << box.max_diam() << endl;
    prune(box);
    if (box.is_empty())	return box;
    //cout << "+++ contra: " << box << " size: " << box.max_diam() << endl;
    if (box.max_diam() < eps) return box;
    IntervalVector B = box;
    IntervalVector sol;
    int i = box.extr_diam_index(false);
    box[i] = Interval(box[i].mid());
    sol = searchSolution(box);
    if (!sol.is_empty()) return sol;
    pair<IntervalVector, IntervalVector> p = B.bisect(i);
    sol = searchSolution(p.first);
    if (!sol.is_empty()) return sol;
    sol = searchSolution(p.second);
    return sol;
}

void Scenarioo::solve() {
    my_priority_queuee s;
    IntervalVector box = sys->box;
    s.push(box);
    double boxSize = box.max_diam();
    while (!s.empty() && s.size() < nBoxes && boxSize > eps) {
        // get the largest box
        box = s.top();
        boxSize = box.max_diam();
        s.pop();
        cout << "--- Pop I = " << box;
        // prune the box
        prune(box);
        cout << " -> " << box << endl;
        if (!box.is_empty()) {
            // if box was not discarded
            if (box.max_diam() <= eps) {
                // if box is small enough
                cout << "--- zero " << box << " (width: " << box.max_diam() << ")" << endl;
                // keep the whole box
                s.push(box);
            }
            else {
                // if box is splitable
                // find the largest interval domain
                int i = box.extr_diam_index(false);
                // bisect the box in the midpoint of this domain
                pair<IntervalVector, IntervalVector> p = box.bisect(i);
                cout << "--- split " << box << " -> " << p.first << " ; " << p.second << endl;
                // keep both boxes
                s.push(p.first);
                s.push(p.second);
            }
        }
        else {
            // the box is discarded
            cout << "--- delete " << box << endl;
        }
    }
    int n = s.size();
    partition.clear();
    for (int i = 0; i < n; i++) {
        box = s.top();
        s.pop();
        partition.push_back(box);
        solutions.push_back(searchSolution(box));
    }
}







void createIntersections(int id) {
    Json::Value root;
    Json::Reader reader;
    Json::StyledStreamWriter writer;
    std::ifstream test(FILE_PATH + to_string(id) + EXT, std::ifstream::binary);
    bool parsingSuccessful = reader.parse(test, root, false);
    if (!parsingSuccessful) {
        cout << "error!" << endl;
        //cout << FILE_PATH + to_string(id) << endl;
    }
    int count = 1;
    Json::Value vec(Json::arrayValue);
    float h0, h1, v0, v1, l0, l1;
    vector<pair<float, float>> xIntervals0, xIntervals1;
    vector<pair<float, float>> yIntervals0, yIntervals1;
    for (Json::Value::ArrayIndex i = 0; i != root["roads"].size(); i++) {
        for (Json::Value::ArrayIndex j = i + 1; j != root["roads"].size(); j++) {
            assert(root["roads"][i]["id"].asInt() != root["roads"][j]["id"].asInt());
            h0 = abs((int)(root["roads"][i]["h_axis0"].asFloat() - root["roads"][i]["h_axis1"].asFloat()));
            h1 = abs((int)(root["roads"][j]["h_axis0"].asFloat() - root["roads"][j]["h_axis1"].asFloat()));
            v0 = abs((int)(root["roads"][i]["v_axis0"].asFloat() - root["roads"][i]["v_axis1"].asFloat()));
            v1 = abs((int)(root["roads"][j]["v_axis0"].asFloat() - root["roads"][j]["v_axis1"].asFloat()));
            l0 = root["roads"][i]["n_lanes"].asInt();
            l1 = root["roads"][j]["n_lanes"].asInt();
            //v_axis0 >= h_axis0 and v_axis1 >= h_axis1 or v_axis0 <= h_axis0 and v_axis1 <= h_axis1)
            if ((h0 > v0 and h1 > v1) or (v1 > h1 and v0 > h0))
                continue;
            xIntervals0.clear();
            yIntervals0.clear();
            for (int l = 0; l < l0; l++) {
                if (h0 < v0) {
                    xIntervals0.push_back(make_pair(root["roads"][i]["h_axis0"].asFloat() + h0 * (l)/l0, root["roads"][i]["h_axis0"].asFloat() + h0 * (l + 1)/l0));
                    yIntervals0.push_back(make_pair(root["roads"][i]["v_axis0"].asFloat(), root["roads"][i]["v_axis1"].asFloat()));
                }
                else {
                    yIntervals0.push_back(make_pair(root["roads"][i]["v_axis0"].asFloat() + v0 * (l)/l0, root["roads"][i]["v_axis0"].asFloat() + v0 * (l + 1)/l0));
                    xIntervals0.push_back(make_pair(root["roads"][i]["h_axis0"].asFloat(), root["roads"][i]["h_axis1"].asFloat()));
                  
                }
            }
            xIntervals1.clear();
            yIntervals1.clear();
            for (int l = 0; l < l1; l++) {
                if (h1 < v1) {
                    xIntervals1.push_back(make_pair(root["roads"][j]["h_axis0"].asFloat() + h1 * (l)/l1, root["roads"][j]["h_axis0"].asFloat() + h1 * (l + 1)/l1));
                    yIntervals1.push_back(make_pair(root["roads"][j]["v_axis0"].asFloat(), root["roads"][j]["v_axis1"].asFloat()));
                }
                else {
                    yIntervals1.push_back(make_pair(root["roads"][j]["v_axis0"].asFloat() + v1 * (l)/l1, root["roads"][j]["v_axis0"].asFloat() + v1 * (l + 1)/l1));
                    xIntervals1.push_back(make_pair(root["roads"][j]["h_axis0"].asFloat(), root["roads"][j]["h_axis1"].asFloat()));
                }
            }

            for (int ii = 0; ii < xIntervals0.size(); ii++) {
                for (int jj = 0; jj < xIntervals1.size(); jj++) {
                    // x1.lb <= x2.ub and x2.lb <= x1.ub
                    if (!((xIntervals0[ii].first <= xIntervals1[jj].second && xIntervals1[jj].first <= xIntervals0[ii].second) &&
                        (yIntervals0[ii].first <= yIntervals1[jj].second && yIntervals1[jj].first <= yIntervals0[ii].second)))
                        continue;

                    char buffer[1000];
                    sprintf(buffer, "  {\n"
                        "    \"id\" : %d,\n"
                        "    \"road0\" : %d,\n"
                        "    \"lane0\" : %d,\n"
                        "    \"road1\" : %d,\n"
                        "    \"lane1\" : %d,\n"
                        "    \"x0\" : %f,\n"
                        "    \"x1\" : %f,\n"
                        "    \"y0\" : %f,\n"
                        "    \"y1\" : %f,\n"
                        "  }", count, root["roads"][i]["id"].asInt(), ii, root["roads"][j]["id"].asInt(), jj,
                        max(xIntervals0[ii].first, xIntervals1[jj].first), min(xIntervals0[ii].second, xIntervals1[jj].second),
                        max(yIntervals0[ii].first, yIntervals1[jj].first), min(yIntervals0[ii].second, yIntervals1[jj].second));
                    count++;
                    
                    string str(buffer);
                    Json::Value intersection;
                    reader.parse(str, intersection);
                    vec.append(intersection);
                }
            }
        }
    }
    root["intersections"] = vec;
    std::ofstream test1(FILE_PATH +to_string(id) + "withIntersections" + EXT);;
    writer.write(test1, root);


}

vector<vector<int>> cartesian(vector<vector<int> >& v) {
    vector<vector<int>> result;
    auto product = [](long long a, vector<int>& b) { return a * b.size(); };
    const long long N = accumulate(v.begin(), v.end(), 1LL, product);
    vector<int> u(v.size());
    for (long long n = 0; n < N; ++n) {
        lldiv_t q{ n, 0 };
        for (long long i = v.size() - 1; 0 <= i; --i) {
            q = div(q.quot, v[i].size());
            u[i] = v[i][q.rem];
        }
        result.push_back(u);
    }
    return result;
}

struct my_comparator
{
    // queue elements are vectors so we need to compare those
    bool operator()(IntervalVector const& a, IntervalVector const& b) const
    {
        return a.max_diam() < b.max_diam();
    }
};

using my_priority_queue = std::priority_queue<IntervalVector, std::vector<IntervalVector>, my_comparator>;

class Relation {
public:
    vector<string> c;
    vector<pair<string, Variable>> vars;
    string c1, vc1, c2, vc2, t, t1;
    string type;
    int trj_idx0, trj_idx1;
    Relation(string c1, string c2, int idxc1, int idxc2, string type, int trj_idx0, int trj_idx1) {
        this->c1 = c1;
        this->c2 = c2;
        vc1 = c1 + "coll_" + to_string(idxc1);
        vc2 = c2 + "coll_" + to_string(idxc2);
        t = "t" + c1 + "_" + c2 + "coll_" + to_string(idxc2);
        t1 = "t" + c2 + "_" + c1 + "coll_" + to_string(idxc2);
        Variable v_c1(vc1.c_str());
        Variable v_c2(vc2.c_str());
        Variable v_t(t.c_str());
        Variable v_t1(t1.c_str());
        vars.push_back(make_pair(vc1, v_c1));
        vars.push_back(make_pair(vc2, v_c2));
        vars.push_back(make_pair(t, v_t));
        vars.push_back(make_pair(t1, v_t1));
        this->type = type;
        this->trj_idx0 = trj_idx0;
        this->trj_idx1 = trj_idx1;
    }
    Relation(string c1, string c2, int idxc1, int idxc2, string type) {
        this->c1 = c1;
        this->c2 = c2;
        vc1 = c1 + "coll_" + to_string(idxc1);
        vc2 = c2 + "coll_" + to_string(idxc2);
        t = "t" + c1 + "_" + c2 + "coll_" + to_string(idxc2);
        Variable v_c1(vc1.c_str());
        Variable v_c2(vc2.c_str());
        Variable v_t(t.c_str());
        vars.push_back(make_pair(vc1, v_c1));
        vars.push_back(make_pair(vc2, v_c2));
        vars.push_back(make_pair(t, v_t));
        this->type = type;
    }
    Relation(string c1, string c2, int idxc1, int idxc2) {
        this->c1 = c1;
        this->c2 = c2;
        vc1 = c1 + "coll_" + to_string(idxc1);
        vc2 = c2 + "coll_" + to_string(idxc2);
        t = "t"+c1+"_"+c2+"coll_" + to_string(idxc2);
        Variable v_c1(vc1.c_str());
        Variable v_c2(vc2.c_str());
        Variable v_t(t.c_str());
        vars.push_back(make_pair(vc1, v_c1));
        vars.push_back(make_pair(vc2, v_c2));
        vars.push_back(make_pair(t, v_t));
    }
    Relation(vector<string> c) {
        this->c = c;
    }
    bool is_cut() {
        return type == "cut";
    }
    Variable get_first_car() {
        return get_value(vc1);
    }
    Variable get_second_car() {
        return get_value(vc2);
    }
    Variable get_time() {
        return get_value(t);
    }
    Variable get_time1() {
        return get_value(t1);
    }
    Variable get_value(string v) {
        for (pair<string, Variable> pair : vars)
            if (pair.first == v)
                return pair.second;
    }
    string get_first_car_name() {
        return c1;
    }
    string get_second_car_name() {
        return c2;
    }
    string get_third_time() {
        return t;
    }
    string get_third_time1() {
        return t1;
    }
    string get_type() {
        return type;
    }
    bool is_between(string car1, string car2) {
        return (car1 == get_first_car_name() and car2 == get_second_car_name()) or (car2 == get_first_car_name() and car1 == get_second_car_name());
    }
};

class Chasing_Relation {
public:
    string car1, car2;
    vector<string> c;
    vector<pair<string, Variable>> vars;
    vector<Relation*> final;
    vector<Relation*> v1, v2;

    Chasing_Relation(string car1, string car2, vector<Relation*> v1, vector<Relation*> v2) {
        this->v1 = v1;
        this->v2 = v2;
        this->car1 = car1;
        this->car2 = car2;
        merge();
    }

    int get_size() {
        return final.size();
    }

    void merge() {
        for (Relation* r : v1)
            final.push_back(r);
        for (Relation* r : v2)
            final.push_back(r);
    }
};

class Crossing_Relation {
public:
    string car1, car2;
    vector<string> c;
    vector<pair<string, Variable>> vars;
    IntervalVector cross;
    int trj_idx0_in, trj_idx0_out, trj_idx1_in, trj_idx1_out;
    int index1, index2;
    int lo, ro, ld, rd;
    Crossing_Relation(string car1, string car2, int lo, int ro, int ld, int rd, int index1, int index2, IntervalVector cross, int trj_idx0_in, int trj_idx0_out, int trj_idx1_in, int trj_idx1_out) {
        this->car1 = car1;
        this->car2 = car2;
        this->lo = lo;
        this->ld = ld;
        this->ro = ro;
        this->rd = rd;
        this->cross = cross;
        this->index1 = index1;
        this->index2 = index2;
        this->trj_idx0_in = trj_idx0_in;
        this->trj_idx0_out= trj_idx0_out;
        this->trj_idx1_in = trj_idx1_in;
        this->trj_idx1_out = trj_idx1_out;
        process_vars();
    }
    string get_first_car_tin_name() {
        return vars[0].first;
    }
    string get_first_car_tout_name() {
        return vars[1].first;
    }
    string get_second_car_tin_name() {
        return vars[2].first;
    }
    string get_second_car_tout_name() {
        return vars[3].first;
    }
    Variable get_first_car_tin() {
        return vars[0].second;
    }
    Variable get_first_car_tout() {
        return vars[1].second;
    }
    Variable get_second_car_tin() {
        return vars[2].second;
    }
    Variable get_second_car_tout() {
        return vars[3].second;
    }
    void process_vars() {
        string tin1 = "t"+car1 + "_in" + to_string(index1);
        string tout1= "t"+car1 + "_out" + to_string(index1);
        string tin2 = "t" + car2 + "_in" + to_string(index2);
        string tout2 = "t" + car2 + "_out" + to_string(index2);
        Variable t_in1(tin1.c_str());
        Variable t_out1(tout1.c_str());
        Variable t_in2(tin2.c_str());
        Variable t_out2(tout2.c_str());
        vars.push_back(make_pair(tin1, t_in1));
        vars.push_back(make_pair(tout1, t_out1));
        vars.push_back(make_pair(tin2, t_in2));
        vars.push_back(make_pair(tout2, t_out2));
    }
};

class Relation_Set {
public:
    string car1, car2;
    vector<string> c;
    vector<pair<string, Variable>> vars;
    vector<Crossing_Relation*> final;
    vector<Crossing_Relation*> v1, v2;

    Relation_Set(string car1, string car2, vector<Crossing_Relation*> v1, vector<Crossing_Relation*> v2) {
        this->v1 = v1;
        this->v2 = v2;
        this->car1 = car1;
        this->car2 = car2;
        merge();
    }
    int get_size() {
        return final.size();
    }
    void merge() {
        for (Crossing_Relation* r : v1)
            final.push_back(r);
        for (Crossing_Relation* r : v2)
            final.push_back(r);
    }
};


string str(Variable var) {
    // returns the name of a variable
    stringstream buffer;
    buffer << var;
    string s = buffer.str();
    buffer.str("");
    return s;
}

string str(NumConstraint cstr) {
    // returns the description of a constraint
    stringstream buffer;
    buffer << cstr;
    string s = buffer.str();
    buffer.str("");
    return s;
}

Interval checkNull(float v0, float v1) {
    if (v0 == 0 && v1 == 0)
        return Interval(0, POS_INFINITY);
    return Interval(v0, v1);
}

class Trajectory {
public:
    int id;
    float v0, v1, p0, p1, t0, t1, a;
    int road, lane;
    bool universal, hasAcc, in, out;
    Trajectory(int id, float v0, float v1, float p0, float p1, float t0, float t1, float a, int road, int lane, bool universal, bool hasAcc, bool in, bool out) {
        this->v0 = v0;
        this->v1 = v1;
        this->p0 = p0;
        this->p1 = p1;
        this->t0 = t0;
        this->t1 = t1;
        this->road = road;
        this->lane = lane;
        this->id = id;
        this->universal = universal;
        this->a = a;
        this->hasAcc = hasAcc;
        this->in = in;
        this->out = out;
    }
    Trajectory(int id, float v0, float v1, float p0, float p1, float t0, float t1, float a, int road, int lane, bool universal, bool hasAcc) {
        this->v0 = v0;
        this->v1 = v1;
        this->p0 = p0;
        this->p1 = p1;
        this->t0 = t0;
        this->t1 = t1;
        this->road = road;
        this->lane = lane;
        this->id = id;
        this->universal = universal;
        this->a = a;
        this->hasAcc = hasAcc;
        this->in = false;
        this->out = false;
    }
    Trajectory(int id, float v0, float v1, float p0, float p1, float t0, float t1, int road, int lane, bool universal, bool hasAcc) {
        this->v0 = v0;
        this->v1 = v1;
        this->p0 = p0;
        this->p1 = p1;
        this->t0 = t0;
        this->t1 = t1;
        this->road = road;
        this->lane = lane;
        this->id = id;
        this->universal = universal;
        this->hasAcc = hasAcc;
        this->in = false;
        this->out = false;
    }
    Trajectory(int id, float v0, float v1, float t0, float t1, bool universal) {
        this->v0 = v0;
        this->v1 = v1;
        this->t0 = t0;
        this->t1 = t1;
        this->id = id;
        this->universal = universal;
        this->in = false;
        this->out = false;
    }
    int getId() {
        return id;
    }
    float getV0() {
        return v0;
    }
    float getV1() {
        return v1;
    }
    float getP0() {
        return p0;
    }
    float getP1() {
        return p1;
    }
    float getT0() {
        return t0;
    }
    float getT1() {
        return t1;
    }
    int getRoad() {
        return road;
    }
    int getLane() {
        return lane;
    }
};

class Car {
    private:
        string id;
    public:
        vector<pair<string, Variable>> t;
        vector<pair<string, Variable>> x;
        vector<pair<string, Variable>> v;
        vector<pair<string, Variable>> c_in;
        vector<pair<string, Variable>> c_out;
        vector<pair<string, Relation*>> coll;
        vector<pair<string, Variable>> xcuts_init;
        vector<pair<string, Variable>> vcuts_init;
        vector<pair<string, Variable>> tcuts_init;
        vector<pair<string, Variable>> xcuts_fin;
        vector<pair<string, Variable>> vcuts_fin;
        vector<pair<string, Variable>> tcuts_fin;
        vector<pair<string, Variable>> angles;
        vector<Variable> r;
        vector<int> lanes_trjs;
        vector<bool> changing_lane;
        vector<Interval> lane_ints;
        vector<Interval> angle_ints;
        vector<string> cars_front;
        vector<pair<Interval, Interval>> mapIntCoords; //map coords to correspond each c_in and c_out to each collision area
        vector<IntervalVector> cutInts; //cut intervals
        vector<IntervalVector> collInts; //collision intervals
        vector<IntervalVector> trjInts;
        float width, length;
        int road, lane;
        std::vector<Trajectory*> trajectories;
        std::vector<Trajectory*> uniTrajectories;

    Car(string id, float width, float length, std::vector<Trajectory*> trajectories, std::vector<Trajectory*> uniTrajectories) {
        this->width = width;
        this->length = length;
        this->id = id;
        this->trajectories = trajectories;
        this->uniTrajectories = uniTrajectories;
        addCar();
    }
    Car(string id, float width, float length) {
        this->width = width;
        this->length = length;
        this->id = id;
    }
    Car(float width, float length) {
        this->width = width;
        this->length = length;
    }
    bool varExists(string id) {
        for (pair<string, Variable> p : x)
            if (p.first == id) 
                return true;
        for (pair<string, Variable> p : t)
            if (p.first == id) 
                return true;
        for (pair<string, Variable> p : v)
            if (p.first == id) 
                return true;
        for (pair<string, Variable> p : c_in)
            if (p.first == id) 
                return true;
        for (pair<string, Variable> p : c_out)
            if (p.first == id)
                return true;
        for (pair<string, Variable> p : angles)
            if (p.first == id)
                return true;
        return false;
    }
    Variable getVar(string id) {
        for (pair<string, Variable> p : x)
            if (p.first == id) {
                return p.second;
            }
        for (pair<string, Variable> p : t)
            if (p.first == id) {
                return p.second;
            }
        for (pair<string, Variable> p : v)
            if (p.first == id) {
                return p.second;
            }
        for (pair<string, Variable> p : c_in)
            if (p.first == id) {
                return p.second;
            }
        for (pair<string, Variable> p : c_out)
            if (p.first == id) {
                return p.second;
            }
        for (pair<string, Variable> p : angles)
            if (p.first == id) {
                return p.second;
            }
    }
    vector<pair<int, int>> get_lanes_roads() {
        vector<pair<int, int>> roads_lanes;
        for (int i = 0; i < trajectories.size(); i++)
            if (!pair_exists(roads_lanes, make_pair(trajectories[i]->getRoad(), trajectories[i]->getLane())))
                roads_lanes.push_back(make_pair(trajectories[i]->getRoad(),trajectories[i]->getLane()));
        return roads_lanes;
    }
    bool changes_lane() {
        int lane = trajectories[0]->getLane();
        for (int i = 0; i < trajectories.size(); i++)
            if (lane != trajectories[i]->getLane())
                return true;
        return false;
    }
    bool pair_exists(vector<pair<int, int>> pairs, pair<int,int> p) {
        for (pair<int, int> pp : pairs) {
            if (pp.first == p.first and pp.second == p.second)
                return true;
        }
        return false;
    }
    string getId() {
        return id;
    }
    float getWidth() {
        return width;
    }
    float getLength() {
        return length;
    }
    float getX0() {
        return trajectories[0]->getP0();
    }
    float getX1() {
        return trajectories[0]->getP1();
    }
    int get_road(int index) {
        return trajectories[index]->getRoad();
    }
    int get_lane(int index) {
        return trajectories[index]->getLane();
    }
    Trajectory* getTrajectory(int index) {
        if (index > trajectories.size())
            throw std::invalid_argument("Invalid argument!");
        return trajectories.at(index);
    }
    vector<Trajectory*> getTrajectories() {
        return trajectories;
    }
    void addTrajectory(Trajectory* traj) {
        trajectories.push_back(traj);
    }
    pair<int, int> get_lane_transition() {
        int lane_destin = get_cutlane_index();
        int lane_origin = lane_destin-1;
        if (lane_destin == -1)
            return make_pair(trajectories[0]->getLane(), trajectories[0]->getLane());
        return make_pair(trajectories[lane_origin]->getLane(), trajectories[lane_destin]->getLane());
    }
    int get_cutlane_index() {
        for (int i = 0; i < changing_lane.size(); i++) {
            if (changing_lane[i])
                return i;
        }
        return -1;
    }
    Interval get_intersection(float as, float ae, float bs, float be) {
        //Ia=[as,ae]  and Ib=[bs,be]
        if (bs > ae or as > be) { 
            return Interval(bs, be); 
        }
        else {

            float os = max(as, bs);
            float oe = min(ae, be);
            return Interval(os, oe);
        }
    }
    Interval process_velocity_reg_time(Interval t, Interval v) {
        //universal constraints
        bool check = false;
        Interval new_v = v;
        for (int i = 0; i < uniTrajectories.size(); i++) {
                if (uniTrajectories[i]->getT0() <= t.ub() && t.lb() <= uniTrajectories[i]->getT1()) {
                    new_v = get_intersection(new_v.lb(), new_v.ub(), uniTrajectories[i]->getV0() / 3.6, uniTrajectories[i]->getV1() / 3.6);
                }
        }
        return new_v;
    }

    void addCar() {
        //cout << "################ " << this->id << "################ " << endl;
        vector<Trajectory*> trjs = this->trajectories;
        //cout << "size of the trajectory: " << trjs.size() << endl;
        //cout << "size of the uni trajectories: " << uniTrajectories.size() << endl;
        //cout << "-----------------------" << endl;

        x.clear();
        t.clear();
        v.clear();
        string str_i, t_i, x_i, v_i, i_lane;

        IntervalVector txv(3);

        if (trjs.size() > 0 && trjs[0]->getT0() != 0)
            return;
        int lane = trjs[0]->getLane();
        bool change_lane = false;
        for (int i = 0; i < trjs.size(); i++) {
            change_lane = false;
            lanes_trjs.push_back(trjs[i]->getLane());
            IntervalVector txv(3);
            if (i < trjs.size() - 1) {
                if (trjs[i]->getLane() != trjs[i + 1]->getLane())
                    change_lane = true;
            }
            
            lane = trjs[i]->getLane();
            str_i = std::to_string(i);
            
            t_i = "t" + this->getId() + "_" + str_i;
            x_i = "x" + this->getId() + "_" + str_i;
            v_i = "v" + this->getId() + "_" + str_i;
           
            if(i<trjs.size()-1)
                changing_lane.push_back(change_lane);

            Variable ti(t_i.c_str());
            Variable xi(x_i.c_str());
            Variable vi(v_i.c_str());
            t.push_back(make_pair(t_i,ti));
            x.push_back(make_pair(x_i,xi));
            v.push_back(make_pair(v_i, vi));
            
            txv[0] = Interval(trjs[i]->getT0(), trjs[i]->getT1());
            txv[1] = Interval(trjs[i]->getP0(), trjs[i]->getP1());
            txv[2] = process_velocity_reg_time(txv[0], Interval(trjs[i]->getV0(), trjs[i]->getV1())/Interval(3.6));

            trjInts.push_back(txv);
            
            /*cout << ti << " t[" << i << "] = " << trjInts[i][0] << endl;
            cout << xi << " x[" << i << "] = " << trjInts[i][1] << endl;
            cout << vi << " v[" << i << "] = " << trjInts[i][2]*3.6 << endl;
            cout << "----------------" << endl;*/
        }

    }
};


    class Intersection {
    private:
        int id;
    public:
        int road0, road1;
        int lane0, lane1;
        float x0, x1, y0, y1;
        bool horizontal;
        Intersection(int id, int road0, int lane0, int road1, int lane1, float x0, float x1, float y0, float y1) {
            this->id = id;
            this->x0 = x0;
            this->x1 = x1;
            this->y0 = y0;
            this->y1 = y1;
            this->road0 = road0;
            this->lane0 = lane0;
            this->road1 = road1;
            this->lane1 = lane1;
        }
        float getX0() {
            return x0;
        }
        float getY0() {
            return y0;
        }
        float getX1() {
            return x1;
        }
        float getY1() {
            return y1;
        }
        float getRoad0() {
            return road0;
        }
        float getRoad1() {
            return road1;
        }
        float getLane0() {
            return lane0;
        }
        float getLane1() {
            return lane1;
        }
        IntervalVector get_interval_vec() {
            IntervalVector vec(2);
            vec[0] = Interval(x0, x1);
            vec[1] = Interval(y0, y1);
            return vec;
        }
        bool equals(Intersection* inter){
            return this->x0 == inter->x0 and this->x1 == inter->x1 and this->y0 == inter->y0 and this->y1 == inter->y1;
        }
    };

    class Road {
    public:
        int id;
        bool dir;
        float h_axis0, h_axis1, v_axis0, v_axis1;
        int n_lanes;
        Road(int id, bool dir, float h_axis0, float h_axis1, float v_axis0, float v_axis1, int n_lanes) {
            this->id = id;
            this->dir = dir;
            this->h_axis0 = h_axis0;
            this->h_axis1 = h_axis1;
            this->v_axis0 = v_axis0;
            this->v_axis1 = v_axis1;
            this->n_lanes = n_lanes;
        }
        bool isHorizontal() {
            return (this->h_axis1 - this->h_axis0) > (this->v_axis1 - this->v_axis0);
        }
    };

    class Goal {
    public:
        string id;
        std::vector<string> cars;
        bool collides;
        Goal(bool collides, std::vector<string> cars) {
            this->collides = collides;
            this->cars = cars;
        }
        void addCar(string car) {
            cars.push_back(car);
        }
        vector<string> getCars() {
            return cars;
        }
        int getNumberCollisions(string car) {
            int ctr = 0;
            for (string c : cars)
                if (c == car)
                    ctr++;
            return ctr;
        }
    };

    class RoadNetwork {
    public:
        int id;
        vector<Intersection*> intersections;
        vector<Road*> roads;
        float h_axis, v_axis;
        RoadNetwork(int id, vector<Intersection*> intersections, vector<Road*> roads, float h_axis, float v_axis) {
            this->id = id;
            this->intersections = intersections;
            this->roads = roads;
            this->h_axis = h_axis;
            this->v_axis = v_axis;
        }
        float get_lanediff_width(int road, int origin, int dest) {
            return abs(origin - dest) * get_interlane_width(road);
        }
        float get_interlane_width(int road) {
            Road* r = get_road(road);
            if (r->isHorizontal()) {
                return abs(r->v_axis1 - r->v_axis0) / r->n_lanes;
            } else {
                return abs(r->h_axis1 - r->h_axis0) / r->n_lanes;
            }
        }
        vector<Intersection*> getAllIntersections(int road, int lane) {
            vector<Intersection*> ints;
            for (Intersection* i : intersections)
                if ((i->getLane0() == lane && i->getRoad0() == road) || (i->getLane1() == lane && i->getRoad1() == road))
                    ints.push_back(i);
            return ints;
        }
        bool intersect(int road1, int lane1, int road, int lane) {
            for (Intersection* i : intersections){
                if ((i->getLane0() == lane && i->getRoad0() == road) && (i->getLane1() == lane1 && i->getRoad1() == road1) ||
                    ((i->getLane0() == lane1 && i->getRoad0() == road1) && (i->getLane1() == lane && i->getRoad1() == road))) {
                    return 1;
                }
            }
            return 0;
        }
        Road* get_road(int road) {
            for (Road* r : roads) {
                if (r->id== road)
                    return r;
            }
        }
        IntervalVector get_intersection_interval(int road0, int lane0, int road1, int lane1) {
            Intersection* i = getIntersection(road0, lane0, road1, lane1);
            Road* r = get_road(road0);
            IntervalVector iv(2);
            if (r->isHorizontal()) {
                iv[0] = Interval(i->getX0(), i->getX1());
                iv[1] = Interval(i->getY0(), i->getY1());
            }
            else {
                iv[0] = Interval(i->getY0(), i->getY1());
                iv[1] = Interval(i->getX0(), i->getX1());
            }
            return iv;
        }
        Intersection* getIntersection(int road1, int lane1, int road, int lane) {
            for (Intersection* i : intersections)
                if ((i->getLane0() == lane && i->getRoad0() == road) && (i->getLane1() == lane1 && i->getRoad1() == road1) ||
                    ((i->getLane0() == lane1 && i->getRoad0() == road1) && (i->getLane1() == lane && i->getRoad1() == road)))
                    return i;
        }
        bool isHorizontal(int id) {

            for (Road* r : roads) {
                if (r->id == id) {
                    return r->isHorizontal();
                }
            }
            return false;
        }
        vector<float> getRefPosition(int road, float pos, float x0, float x1, float y0, float y1) {
            vector<float> p;
            if (isHorizontal(road))
                p = { x0,x1 };
            else
                p = { y0,y1 };
            return p;
        }
    };

    class Scenario {
    private:
        string id;
    public:
        std::map<string, Car*> cars;
        float duration;
        RoadNetwork* roadNet;
        vector<Relation*> relations;
        std::vector<Goal*> goals;
        vector<Chasing_Relation*> global_relations;
        vector<Relation_Set*> cross_global_relations;
        vector<vector<string>> chasing_clusters;
        vector<string> processed_cars;
        vector<Variable> coll_chasing_variable;
        string collision_var;
        int collision_index;
        Scenario(string id, float duration, std::map<string, Car*> cars, std::vector<Goal*> goals, RoadNetwork* roadNet) {
            this->duration = duration;
            this->id = id;
            this->cars = cars;
            this->goals = goals;
            this->roadNet = roadNet;
            process_chasing_collisions();
            process_crossing_collisions();
        }
        string getId() {
            return id;
        }
        float getDuration() {
            return duration;
        }
        bool relation_id_exists(string id) {

            for (Relation* r : relations) {
                if (r->vc1 == id)
                    return true;
                else if (r->vc2 == id)
                    return true;
                else if (r->t == id)
                    return true;
            }
        }
        Car* getCar(string id) {
            auto found = cars.find(id);
            if (found == cars.end())
                throw std::invalid_argument("Invalid argument!");
            return found->second;
        }
        map<string, Car*> getCars() {
            return cars;
        }
        void addCar(Car* car) {
            cars.insert(std::pair<string, Car*>(car->getId(), car));
        }
        vector<Goal*> getGoals() {
            return goals;
        }
        bool pair_exists(vector<pair<int, int>> pairs, int v1, int v2) {
            for (pair<int, int> p : pairs) {
                if (p.first == v1 and p.second == v2)
                    return true;
            }
            return false;
        }
        vector<Intersection*> getIntersections(Car*c, int road, int lane) {
            //vector<Intersection*>
            vector<Intersection*> iv;
            map<string, Car*>::iterator it;
            vector<int> roads;
            vector<pair<int, int>> pairs;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->first == c->getId())
                    continue;
                bool check = roadNet->intersect(road, lane, it->second->getTrajectories()[0]->road, it->second->getTrajectories()[0]->lane);
                if (!check)
                    continue;
                if (pair_exists(pairs, it->second->getTrajectories()[0]->road, it->second->getTrajectories()[0]->lane))
                    continue;
                Intersection* aux = roadNet->getIntersection(road, lane, it->second->getTrajectories()[0]->road, it->second->getTrajectories()[0]->lane);
                pairs.push_back(make_pair(it->second->getTrajectories()[0]->road, it->second->getTrajectories()[0]->lane));
                iv.push_back(aux);
            }
            return iv;
        }
        vector<Intersection*> getIntersectionsAlt(int road, int lane) {
            map<string, Car*>::iterator it;
            
            for (it = cars.begin(); it != cars.end(); it++) {

            }
        }
        vector<Intersection*> getAllIntersections(Car* c) {
            vector<Intersection*> iv;
            map<string, Car*>::iterator it;
            vector<int> roads;
            vector<pair<int, int>> pairs;
            vector<pair<int, int>> cr_cl = c->get_lanes_roads();
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->first == c->getId())
                    continue;
                for (pair<int, int> crr_cll : cr_cl) {
                    for (pair<int, int> r_l : it->second->get_lanes_roads()) {
                        if (crr_cll.first == r_l.first)
                            continue;
                        //cout << "checking collisions between " << crr_cll.first << ", " << crr_cll.second << " and " << r_l.first << ", " << r_l.second << endl;
                        bool check = roadNet->intersect(crr_cll.first, crr_cll.second, r_l.first, r_l.second);
                        if (!check)
                            continue;
                        if (pair_exists(pairs, r_l.first, r_l.second))
                            continue;
                        Intersection* aux = roadNet->getIntersection(crr_cll.first, crr_cll.second, r_l.first, r_l.second);
                        pairs.push_back(make_pair(r_l.first, r_l.second));
                        iv.push_back(aux);
                    }
                }
            }
            return iv;
        }
        bool same_road(Car* c1, Car* c2) {
            return get_road(c1) == get_road(c2);
        }
        bool same_lane(Car* c1, Car* c2) {
            return get_road(c1) == get_road(c2) and get_lane(c1) == get_lane(c2);
        }
        int get_road(Car* c) {
            return c->getTrajectories()[0]->getRoad();
        }
        int get_lane(Car* c) {
            return c->getTrajectories()[0]->getLane();
        }
        void processGoals() {
            for (Goal* goal : goals) {
                int nt = 0;
                Car* c;
                for (string carId : goal->getCars()) {
                    c = cars[carId];
                    string str1 = "t" + c->getId() + "_in"+to_string(nt);
                    string str2 = "t" + c->getId() + "_out"+ to_string(nt);
                    Variable t_in(str1.c_str());
                    Variable t_out(str2.c_str());
                    c->c_in.push_back(make_pair(str1.c_str(), t_in));
                    c->c_out.push_back(make_pair(str2.c_str(), t_out));
                }
            }
        }

        vector<Car*> get_cars_same_road(Car* c) {
            vector<Car*> aux;

            map<string, Car*>::iterator it;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->first == c->getId())
                    continue;
                if(same_road(it->second, c) and same_lane(it->second, c))
                    aux.push_back(it->second);
            }
            return aux;
        }
        bool changes_lane(Car* c, int idx) {
            if (idx == c->getTrajectories().size())
                return false;
            return c->getTrajectory(idx)->getLane() != c->getTrajectory(idx + 1)->getLane();
        }
        bool tuple_exists(vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> tuples, tuple<pair<int, int>, pair<int, int>, pair<int, int>> tuple) {
            
            pair<int, int> idxs = get<0>(tuple);;
            pair<int, int> roads = get<1>(tuple);
            pair<int, int> lanes = get<2>(tuple);

            for (int i = 0; i < tuples.size(); i++) {

                pair<int, int> idxs_c = get<0>(tuples[i]);
                pair<int, int> roads_c = get<1>(tuples[i]);
                pair<int, int> lanes_c = get<2>(tuples[i]);

                if (((idxs_c.first == idxs.first and idxs_c.second == idxs.second) or (idxs_c.second == idxs.first and idxs_c.first == idxs.second)) and ((roads_c.first == roads.first and roads_c.second == roads.second) or (roads_c.second == roads.first and roads_c.first == roads.second))
                    and ((lanes_c.first == lanes.first and lanes_c.second == lanes.second) or (lanes_c.second == lanes.first and lanes_c.first == lanes.second))) {
                    return true;
                }

            }

            return false;
        }
        vector<Car*> get_cars_potentially_cross(string car) {
            Car* c = cars[car];
            vector<Car*> aux;
            map<string, Car*>::iterator it;

            vector < pair<int, int>> roads;
            vector<pair<int, int>> lanes;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) == c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                // if there is a x0, x1, road0, road1, lane0, lane1
                std::tuple<int, int, int> tpl;
                vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> props_eval;

                vector<pair<pair<int, int>, pair<int, int>>> vec;
                vector<pair<int, int>> idxs;
                for (int i = 0; i < it->second->trajectories.size(); i++) {
                    for (int j = 0; j < c->trajectories.size(); j++) {
                        for (int x1 = 0; x1 < it->second->trajectories.size() - 1; x1++) {
                            for (int y1 = x1; y1 < it->second->trajectories.size()-1; y1++) {
                                for (int x2 = 0; x2 < c->trajectories.size() - 1; x2++) {
                                    for (int y2 = x2; y2 < c->trajectories.size()-1; y2++) {
                                        
                                        if (roadNet->intersect(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j))
                                            ) {
                                            if (tuple_exists(props_eval, make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j)))))
                                                continue;

                                            props_eval.push_back(make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j))));

                                            aux.push_back(it->second);
                                            idxs.push_back(make_pair(x1, x2));
                                            roads.push_back(make_pair(it->second->get_road(i), c->get_road(j)));
                                            lanes.push_back(make_pair(it->second->get_lane(i), c->get_lane(j)));

                                            vec.push_back(make_pair(make_pair(i, j), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return aux;
        }
        vector<IntervalVector> get_cars_potentially_cross_intersections_intervals(string car) {
            Car* c = cars[car];
            vector<pair<int, int>> aux;
            map<string, Car*>::iterator it;
            vector<IntervalVector> ints;

            vector < pair<int, int>> roads;
            vector<pair<int, int>> lanes;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) == c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                vector<pair<int, int>> idxs;

                vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> props_eval;

                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i = 0; i < it->second->trajectories.size(); i++) {
                    for (int j = 0; j < c->trajectories.size(); j++) {
                        for (int x1 = 0; x1 < it->second->trajectories.size() - 1; x1++) {
                            for (int y1 = x1; y1 < it->second->trajectories.size()-1; y1++) {
                                for (int x2 = 0; x2 < c->trajectories.size() - 1; x2++) {
                                    for (int y2 = x2; y2 < c->trajectories.size()-1; y2++) {
                                        //if ( pair_exists(roads, it->second->get_road(i), c->get_road(j)) and pair_exists(lanes, it->second->get_lane(i), c->get_lane(j)))
                                        //    continue;
                                        if (roadNet->intersect(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j))
                                            ) {
                                            if (tuple_exists(props_eval, make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j)))))
                                                continue;
                                            props_eval.push_back(make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j))));

                                            ints.push_back(roadNet->get_intersection_interval(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j)));
                                            vec.push_back(make_pair(make_pair(x1, x2), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                            idxs.push_back(make_pair(x1, x2));
                                            roads.push_back(make_pair(it->second->get_road(i), c->get_road(j)));
                                            lanes.push_back(make_pair(it->second->get_lane(i), c->get_lane(j)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return ints;
        }

        vector<Intersection*> get_cars_potentially_cross_intersections(string car) {
            Car* c = cars[car];
            vector<pair<int, int>> aux;
            map<string, Car*>::iterator it;
            vector<Intersection*> ints;
            vector < pair<int, int>> roads;
            vector<pair<int, int>> lanes;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) == c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                vector<pair<int, int>> idxs;

                vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> props_eval;
                
                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i = 0; i < it->second->trajectories.size(); i++) {
                    for (int j = 0; j < c->trajectories.size(); j++) {
                        for (int x1 = 0; x1 < it->second->trajectories.size() - 1; x1++) {
                            for (int y1 = x1; y1 < it->second->trajectories.size()-1; y1++) {
                                for (int x2 = 0; x2 < c->trajectories.size() - 1; x2++) {
                                    for (int y2 = x2; y2 < c->trajectories.size()-1; y2++) {
                                        //if (pair_exists(roads, it->second->get_road(i), c->get_road(j)) and pair_exists(lanes, it->second->get_lane(i), c->get_lane(j)))
                                        //    continue;
                                        if (roadNet->intersect(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j))) {
                                            if (tuple_exists(props_eval, make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j)))))
                                                continue;
                                            props_eval.push_back(make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j))));

                                            aux.push_back(make_pair(x1, y1));
                                            Intersection* in = roadNet->getIntersection(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j));
                                            //Intersection* in = roadNet->getIntersection(c->get_road(y1), c->get_lane(y1), it->second->get_road(x1), it->second->get_lane(x1));
                                            ints.push_back(in);
                                            vec.push_back(make_pair(make_pair(x1, x2), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                            //vec.push_back(make_pair(make_pair(j - 1, i - 1), make_pair(c->get_lane(j_aux), it->second->get_lane(i_aux))));
                                            idxs.push_back(make_pair(x1, x2));
                                            roads.push_back(make_pair(it->second->get_road(i), c->get_road(j)));
                                            lanes.push_back(make_pair(it->second->get_lane(i), c->get_lane(j)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return ints;
        }
        vector<pair<int, int>> get_cars_potentially_cross_idxs_in(string car) {
            Car* c = cars[car];
            vector<pair<int, int>> aux;
            map<string, Car*>::iterator it;

            vector < pair<int, int>> roads;
            vector<pair<int, int>> lanes;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) == c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                vector<pair<int, int>> idxs;

                vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> props_eval;
                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i = 0; i < it->second->trajectories.size(); i++) {
                    for (int j = 0; j < c->trajectories.size(); j++) {
                        for (int x1 = 0; x1 < it->second->trajectories.size() - 1; x1++) {
                            for (int y1 = x1; y1 < it->second->trajectories.size()-1; y1++) {
                                for (int x2 = 0; x2 < c->trajectories.size() - 1; x2++) {
                                    for (int y2 = x2; y2 < c->trajectories.size()-1; y2++) {
                                        //cout << it->first << " in x0: " << x1 << endl;
                                        //cout << c->getId() << " in x0: " << x2 << endl;
                                        //if (pair_exists(roads, it->second->get_road(i), c->get_road(j)) and pair_exists(lanes, it->second->get_lane(i), c->get_lane(j)))
                                        //    continue;
                                        if (roadNet->intersect(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j))) {
                                            if (tuple_exists(props_eval, make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j)))))
                                                continue;
                                            props_eval.push_back(make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j))));

                                            aux.push_back(make_pair(x1, x2));
                                            vec.push_back(make_pair(make_pair(x1, x2), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                            idxs.push_back(make_pair(x1, x2));
                                            roads.push_back(make_pair(it->second->get_road(i), c->get_road(j)));
                                            lanes.push_back(make_pair(it->second->get_lane(i), c->get_lane(j)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return aux;
        }
        vector<pair<int, int>> get_cars_potentially_cross_idxs_out(string car) {
            Car* c = cars[car];
            vector<pair<int, int>> aux;
            map<string, Car*>::iterator it;
            vector < pair<int, int>> roads;
            vector<pair<int, int>> lanes;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) == c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                vector<pair<int, int>> idxs;

                vector<tuple<pair<int, int>, pair<int, int>, pair<int, int>>> props_eval;
                
                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i = 0; i < it->second->trajectories.size(); i++) {
                    for (int j = 0; j < c->trajectories.size(); j++) {
                        for (int x1 = 0; x1 < it->second->trajectories.size() - 1; x1++) {
                            for (int y1 = x1; y1 < it->second->trajectories.size()-1; y1++) {
                                for (int x2 = 0; x2 < c->trajectories.size() - 1; x2++) {
                                    for (int y2 = x2; y2 < c->trajectories.size()-1; y2++) {
                                        if (roadNet->intersect(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j))) {
                                            if (tuple_exists(props_eval, make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j)))))
                                                continue;
                                            props_eval.push_back(make_tuple(make_pair(x1, x2), make_pair(it->second->get_road(i), c->get_road(j)), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                            
                                            aux.push_back(make_pair(y1, y2));
                                            vec.push_back(make_pair(make_pair(x1, x2), make_pair(it->second->get_lane(i), c->get_lane(j))));
                                            idxs.push_back(make_pair(x1, x2));
                                            roads.push_back(make_pair(it->second->get_road(i), c->get_road(j)));
                                            lanes.push_back(make_pair(it->second->get_lane(i), c->get_lane(j)));
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return aux;
        }
        vector<Car*> get_cars_potentially_infront(string car) {
            Car* c = cars[car];
            vector<Car*> aux;
            map<string, Car*>::iterator it;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) != c->get_road(0))
                    continue;
                if (it->second->getId() == c->getId())
                    continue;
                //vector<pair<pair<idx1, idx2>, pair<lane1, lane2>>> vec; there are no duplicates of trajs <c1-c2, t1-t2>, will reduce complexity
                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i_aux = 0; i_aux < it->second->trajectories.size(); i_aux++) {
                    for (int j_aux = 0; j_aux < c->trajectories.size(); j_aux++) {
                        for (int i = 1; i < it->second->trajectories.size(); i++) {
                            for (int j = 1; j < c->trajectories.size(); j++) {
                                if (pair_of_pair_exists(vec, j - 1, i - 1, c->get_lane(j_aux), it->second->get_lane(i_aux)))
                                    continue;
                                //roadNet->getIntersection(it->second->get_road(i), it->second->get_lane(i), c->get_road(j), c->get_lane(j));
                                if (it->second->get_lane(i_aux) == c->get_lane(j_aux)) {
                                    aux.push_back(it->second);
                                    vec.push_back(make_pair(make_pair(j - 1, i - 1), make_pair(c->get_lane(j_aux), it->second->get_lane(i_aux))));
                                }
                            }
                        }
                    }
                }
            }
            return aux;
        }
        bool pair_of_pair_exists(vector<pair<pair<int, int>, pair<int, int>>> vec, int c1_t, int c2_t, int idx1, int idx2) {
            if (vec.size() == 0)
                return false;
            for (pair<pair<int, int>, pair<int, int>> pair : vec) {
                if ((pair.first.first == c1_t and pair.first.second == c2_t and pair.second.first == idx1 and pair.second.second == idx2)  or (pair.first.first == c2_t and pair.first.second == c1_t and pair.second.first == idx2 and pair.second.second == idx1)) {
                    return true;
                }
            }
            return false;
        }
        vector<pair<int,int>> get_cars_potentially_infront_idxs(string car) {
            //notice that upon two cars on the same lane, this says that c1->c2 but also that c2->c1 due to the chasing properties and due to the cut properties, a cut can have two ways but a chasing can have only one, this should be eliminated by the constraint checking
            Car* c = cars[car];
            vector<pair<int,int>> aux;
            map<string, Car*>::iterator it;
            for (it = cars.begin(); it != cars.end(); it++) {
                if (it->second->get_road(0) != c->get_road(0))
                    continue;
                if (it->second->getId() == car)
                    continue;
                //vector<pair<pair<c1_t, c2_t>, pair<lane1, lane2>>> vec; there are no duplicates of trajs <c1-c2, t1-t2>, will reduce complexity
                vector<pair<pair<int, int>, pair<int, int>>> vec;
                for (int i_aux = 0; i_aux < it->second->trajectories.size(); i_aux++) {
                    for (int j_aux = 0; j_aux < c->trajectories.size(); j_aux++) {
                        for (int i = 1; i < it->second->trajectories.size(); i++) {
                            for (int j = 1; j < c->trajectories.size(); j++) {
                                if (pair_of_pair_exists(vec, j - 1, i - 1, c->get_lane(j_aux), it->second->get_lane(i_aux)))
                                    continue;
                                if (it->second->get_lane(i_aux) == c->get_lane(j_aux)) {
                                    vec.push_back(make_pair(make_pair(j - 1, i - 1), make_pair(c->get_lane(j_aux), it->second->get_lane(i_aux))));
                                    aux.push_back(make_pair(j - 1, i - 1));
                                }
                            }
                        }
                    }
                }
            }
            return aux;
        }
        bool belongs_to_matrix(vector<vector<string>> vectors, string str) {
            for (vector<string> ch_aux : vectors)
                       if (count(ch_aux.begin(), ch_aux.end(), str))
                            return true;
            return false;
        }
        Interval get_interval(Intersection* in, bool isHorizontal) {
            if (isHorizontal)
                return in->get_interval_vec()[0];
            else
                return in->get_interval_vec()[1];
        }
        int get_pair_road_lane_count(map<string, vector<pair<pair<int, int>, pair<int, int>>>> m, pair<pair<int, int>, pair<int, int>> p, string id) {
            if (!m.count(id))
                return 0;
            vector<pair<pair<int, int>, pair<int, int>>> vec = m[id];
            for (int i = 0; i < vec.size(); i++) {
                if (vec[i].first.first == p.first.first and vec[i].first.second == p.first.second and vec[i].second.first == p.second.first and vec[i].second.second == p.second.second)
                    return i;
                if (vec[i].second.first == p.first.first and vec[i].second.second == p.first.second and vec[i].first.first == p.second.first and vec[i].first.second == p.second.second)
                    return i;
            }
        }
        void process_crossing_collisions() {
            map<string, Car*>::iterator it, it_aux;
            int i0, i1;
            string str1;
            string mixed = "";
            map<string, vector<Crossing_Relation*>> relats;
            vector<pair<string, string>> processed;
            // map that for each car, presents the vars in which he can collides (pair<road0, lane0>, pair<road1, lane1>)
            map<string, vector<pair<pair<int, int>, pair<int, int>>>> info_road_lanes;
            //cout << "----------------- process crossing relations -----------------" << endl;
            for (it = cars.begin(); it != cars.end(); it++) {
                vector<Car*> poss_cars = get_cars_potentially_cross(it->first);
                vector<pair<int, int>> poss_cars_idxs_in = get_cars_potentially_cross_idxs_in(it->first);
                vector<pair<int, int>> poss_cars_idxs_out = get_cars_potentially_cross_idxs_out(it->first);
                vector<IntervalVector> poss_cars_intervals = get_cars_potentially_cross_intersections_intervals(it->first);
                vector<Intersection*> poss_cars_ints = get_cars_potentially_cross_intersections(it->first);
                //cout << "POSS_CARS: " << poss_cars.size() << ", IDXS: " << poss_cars_idxs_in.size() << ", INTS: " << poss_cars_ints.size() << endl;
                
                for (int i = 0; i < poss_cars.size(); i++) {
                    Car* c = poss_cars[i];
                    if (!info_road_lanes.count(c->getId())) {
                        info_road_lanes[c->getId()];
                    }
                    if (!info_road_lanes.count(it->first)) {
                        info_road_lanes[it->first];
                    }
                    if (info_road_lanes.count(it->first) and !pair_of_pair_exists(info_road_lanes[it->first], it->second->get_road(poss_cars_idxs_in[i].second), it->second->get_lane(poss_cars_idxs_in[i].second)
                        , c->get_road(poss_cars_idxs_in[i].first), c->get_lane(poss_cars_idxs_in[i].first))) {
                        info_road_lanes[it->first].push_back(make_pair(make_pair(it->second->get_road(poss_cars_idxs_in[i].second), it->second->get_lane(poss_cars_idxs_in[i].second)),
                            make_pair(c->get_road(poss_cars_idxs_in[i].first), c->get_lane(poss_cars_idxs_in[i].first))));
                    }

                    if (info_road_lanes.count(c->getId()) and !pair_of_pair_exists(info_road_lanes[c->getId()], it->second->get_road(poss_cars_idxs_in[i].second), it->second->get_lane(poss_cars_idxs_in[i].second)
                        , c->get_road(poss_cars_idxs_in[i].first), c->get_lane(poss_cars_idxs_in[i].first))) {
                        info_road_lanes[c->getId()].push_back(make_pair(make_pair(it->second->get_road(poss_cars_idxs_in[i].second), it->second->get_lane(poss_cars_idxs_in[i].second)),
                            make_pair(c->get_road(poss_cars_idxs_in[i].first), c->get_lane(poss_cars_idxs_in[i].first))));
                    }

                    i0 = count_collisions_calculated(it->first, relats);
                    i1 = count_collisions_calculated(poss_cars[i]->getId(), relats);
                    Intersection* alt = roadNet->getIntersection(it->second->get_road(poss_cars_idxs_in[i].second), it->second->get_lane(poss_cars_idxs_in[i].second), c->get_road(poss_cars_idxs_in[i].first), c->get_lane(poss_cars_idxs_in[i].first));
                    
                    //cout << "i0: " << i0 << ", i1: " << i1 << endl;

                    //cout << alt->get_interval_vec() << endl;
                    //i1 = front_cars[i]->coll.size();
                    mixed = it->first + "_" + poss_cars[i]->getId();
                    //(string car1, string car2, int lo, int ro, int ld, int rd, int index1, int index2, IntervalVector cross, int trj_idx0, int trj_idx1)
                    IntervalVector ivec(2);
                    //correct here
                    ivec[0] = get_interval(alt, roadNet->get_road(c->get_road(poss_cars_idxs_in[i].first))->isHorizontal());
                    ivec[1] = get_interval(alt, roadNet->get_road(it->second->get_road(poss_cars_idxs_in[i].second))->isHorizontal());
                    
                    Crossing_Relation* c_r = new Crossing_Relation(poss_cars[i]->getId(),it->first, c->get_lane(poss_cars_idxs_in[i].first), c->get_road(poss_cars_idxs_in[i].first),
                        it->second->get_lane(poss_cars_idxs_in[i].second), it->second->get_road(poss_cars_idxs_in[i].second), i0, i1, ivec, poss_cars_idxs_in[i].first, poss_cars_idxs_out[i].first, poss_cars_idxs_in[i].second, poss_cars_idxs_out[i].second);
                    
                    //cout << " ------------------- " << endl;
                    //(string car1, string car2, int lo, int ro, int ld, int rd, int index1, int index2, IntervalVector cross, int trj_idx0_in, int trj_idx0_out, int trj_idx1_in, int trj_idx1_out)
                    relats[it->first + "_" + poss_cars[i]->getId()].push_back(c_r);
                    processed.push_back(make_pair(poss_cars[i]->getId(), it->first));
                    //cout << c_r->get_first_car_name() << ", car2: " << r->get_second_car_name() << " || " << r->get_first_car() << " | " << r->get_second_car() << " | " << r->get_time() << endl;
                }
                //cout << "---------------------------" << endl;
            }
            if (!processed.size())
                return;
            vector<pair<string, string>> pairs;
            for (it = cars.begin(); it != cars.end(); it++) {
                for (it_aux = cars.begin(); it_aux != cars.end(); it_aux++) {
                    if (it->first == it_aux->first)
                        continue;
                    if (!pair_exists(processed, it->first, it_aux->first))
                        continue;
                    if (pair_exists(pairs, it->first, it_aux->first))
                        continue;
                    cross_global_relations.push_back(new Relation_Set(it->first, it_aux->first, relats[it->first + "_" + it_aux->first], relats[it_aux->first + "_" + it->first]));
                    pairs.push_back(make_pair(it->first, it_aux->first));
                }
                //cout << "SIZE OF THE WHOLE CROSS GLOBAL RELATIONS: " << cross_global_relations[0]->get_size() << endl;
            }
        }
        int count_collisions_calculated(string car1, map<string,vector<Relation*>> relations) {
            map<string, vector<Relation*>>::iterator it;
            int count = 0;
            bool check = false;
            vector<pair<string, string>> pairs;
            for (it = relations.begin(); it != relations.end(); it++) {
                if (strstr(it->first.c_str(), car1.c_str()))
                    count++;

            }
            return count;
        }
        int count_collisions_calculated(string car1, map<string, vector<Crossing_Relation*>> relations) {
            map<string, vector<Crossing_Relation*>>::iterator it;
            int count = 0;
            bool check = false;
            vector<pair<string, string>> pairs;
            for (it = relations.begin(); it != relations.end(); it++) {
                if (strstr(it->first.c_str(), car1.c_str()))
                    count++;

            }
            return count;
        }
        void process_chasing_collisions() {
            //cout << "----------------- process chasing relations -----------------" << endl;
            map<string, Car*>::iterator it, it_aux;
            int i0, i1;
            string str1;
            string mixed = "";
            vector<pair<string, string>> processed;
            map<string, vector<Relation*>> relats;
            for (it = cars.begin(); it != cars.end(); it++) {
                vector<Car*> front_cars = get_cars_potentially_infront(it->first);
                vector<pair<int,int>> front_cars_idxs = get_cars_potentially_infront_idxs(it->first);
                //cout << "car: " << it->first << endl;
                //cout << "size: " << front_cars_idxs.size() << " and " << front_cars.size() << endl;
                for (int i = 0; i < front_cars.size(); i++) {
                    
                    //i0 = it->second->coll.size();
                    i0 = count_collisions_calculated(it->first, relats);
                    i1 = count_collisions_calculated(front_cars[i]->getId(), relats);
                    //i1 = front_cars[i]->coll.size();
                    mixed = it->first + "_" + front_cars[i]->getId();
                    //cout << "process it for " << mixed << endl;
                    //Relation* r = new Relation(it->first, front_cars[i]->getId(), i0, i1, "", front_cars_idxs[i].first, front_cars_idxs[i].second);
                    Relation* r = new Relation(it->first, front_cars[i]->getId(), i0, i1, "", front_cars_idxs[i].first, front_cars_idxs[i].second);
                    it->second->coll.push_back(make_pair("tcoll_" + i0, r));
                    front_cars[i]->coll.push_back(make_pair("tcoll_" + i1, r));
                    //relations.push_back(r);
                    relats[it->first + "_" + front_cars[i]->getId()].push_back(r);
                    processed.push_back(make_pair(it->first, front_cars[i]->getId()));
                    //cout << "car1: " << r->get_first_car_name() << ", car2: " << r->get_second_car_name() << " || " << r->get_first_car() << " | " << r->get_second_car() << " | " << r->get_time() << endl;
                    
                }
            }
            if (!processed.size())
                return;
            vector<pair<string, string>> pairs;
            int index = 0;
            for (it = cars.begin(); it != cars.end(); it++) {
                for (it_aux = cars.begin(); it_aux != cars.end(); it_aux++) {
                    if (it->first == it_aux->first)
                        continue;
                    if (!pair_exists(processed, it->first, it_aux->first))
                        continue;
                    if (pair_exists(pairs, it->first, it_aux->first))
                        continue;
                    //if (relats.find(it->first + "_" + it_aux->first) == relats.end() and relats.find(it_aux->first + "_" + it->first) == relats.end())
                    //    continue;
                    //cout << "size: " << relats[it->first + "_" + it_aux->first].size() << " " << (relats.find(it->first + "_" + it_aux->first) != relats.end()) << endl;
                    global_relations.push_back(new Chasing_Relation(it->first, it_aux->first, relats[it->first + "_" + it_aux->first], relats[it_aux->first + "_" + it->first]));
                    
                    pairs.push_back(make_pair(it->first, it_aux->first));
                    
                    if ((goals[0]->cars[0] == it->first and goals[0]->cars[1] == it_aux->first) or (goals[0]->cars[0] == it_aux->first and goals[0]->cars[1] == it->first)) {
                        collision_index = index;
                        string coll_var = "tcoll_" + goals[0]->cars[0] + "_" + goals[0]->cars[1];
                        Variable cvar(coll_var.c_str());
                        coll_chasing_variable.push_back(cvar);
                        collision_var = coll_var;
                    }
                    //cout << "size growing: " << coll_chasing_variable.size() << endl;
                    index++;
                }
            }

            //cout << "size of the Chasing_Relation: " << global_relations.size() << endl;

        }

        bool pair_exists(vector<pair<string, string>> pairs, string s1, string s2) {
            for (pair<string, string> p : pairs)
                if ((p.first == s1 and p.second == s2) or (p.first == s2 and p.second == s1))
                    return true;
            return false;
        }
    };

    class ScenarioSolve {
    public:
        int id;
        SystemFactory fac;
        System* sys;						// pointer to the constraint system
        Ctc* ctc;							// pointer to the constraint contractor
        Scenario* scn;
        double eps = 0.01;				// criterion to stop bisection: do not bisect domains smaller t
        vector<IntervalVector> partition; 	// a box partition that contains all possible solutions
        vector<IntervalVector> solutions;	// one solution for each box in the partition
        vector<string> ords;
        string coll_vars;
        bool isCollision;
        vector<ScenarioSolve*> tocheck_scenarios;
        vector<pair<pair<string, int>, pair<string, int>>> relations_check;
        vector<tuple<pair<string, int>, pair<string, int>, pair<float, float>, pair<float, float>>> crossings_checks;
        map<string, vector<ScenarioSolve*>> moves;
        vector<string> independent_cars;
        int nBoxes = numberSolutions;
        int timesRan = 0;
        int limitRan = limitRun;
        double stop;
        bool valid;
        bool shrink_interval = false;
        bool ranOut = false;
        float last_eps = 2;
        const float default_time = limitTime;
        float time = limitTime;
        // finished is if any solution was found, valid is if the solution is valid in terms of results (prediction of the trajectory)
        ScenarioSolve(Scenario* scn, System* sys, string coll_vars, bool isCollision, vector<pair<pair<string, int>, pair<string, int>>> relations_check,
            vector<tuple<pair<string, int>, pair<string, int>, pair<float, float>, pair<float, float>>> cross_checks) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->ords = ords;
            this->coll_vars = coll_vars;
            this->isCollision = isCollision;
            this->ords = sys->var_names();
            this->relations_check = relations_check;
            this->crossings_checks = cross_checks;
        }
        ScenarioSolve(Scenario* scn, System* sys, vector<string> ords, string coll_vars, bool isCollision) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->ords = ords;
            this->coll_vars = coll_vars;
            this->isCollision = isCollision;
        }
        ScenarioSolve(Scenario* scn, System* sys, vector<string> ords, string coll_vars) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->ords = ords;
            this->coll_vars = coll_vars;
            isCollision = false;
        }
        ScenarioSolve(Scenario* scn, System* sys, string coll_vars, bool isCollision, vector<pair<pair<string, int>, pair<string, int>>> relations_check) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->coll_vars = coll_vars;
            this->isCollision = isCollision;
            this->relations_check = relations_check;
            this->ords = sys->var_names();
        }
        ScenarioSolve(Scenario* scn, System* sys, string coll_vars, bool isCollision) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->coll_vars = coll_vars;
            this->isCollision = isCollision;
            this->ords = sys->var_names();
        }
        ScenarioSolve(Scenario* scn, System* sys) {
            this->id = identifier++;
            stop = scn->duration;
            this->scn = scn;
            this->sys = sys;
            this->isCollision = false;
            this->ords = sys->var_names();
        }
        ScenarioSolve(System* sys) {
            this->id = identifier++;
            this->sys = sys;
            this->isCollision = false;
            this->ords = sys->var_names();
        }
        void add_var_order(vector<string> ords){
            this->ords = ords;
        }
        void set_time_limit(time_t duration) {
            this->time = duration;
        }
        void setHC4Contractor() {
            ctc = new CtcHC4(*sys);
        }
        bool eps_valid() {
            return solutions.size() > 0 && (solutions[0].max_diam() <= eps);
        }
        void add_main_vars(SystemFactory& fac, vector<string> vars) {
            bool check = false;
            map<string, Car*>::iterator it;
            for (int k = 0; k < vars.size(); k++) {
                check = false;
                for (it = scn->cars.begin(); it != scn->cars.end(); it++) {
                    check = it->second->varExists(vars[k].c_str());
                    if (check) {
                        fac.add_var(it->second->getVar(vars[k].c_str()));
                        break;
                    }
                }
            }
            //cout << endl;
        }
        void load_moves(map<string, vector<ScenarioSolve*>> moves, vector<string> ind) {
            this->moves = moves;
            this->independent_cars = ind;
            map<string, Car*>::iterator it;
            for (string s : ind) {
                    moves[s][0]->setHC4Contractor();
                    moves[s][0]->solve(clock());
                    moves[s][0]->showAll();
            }
        }
        bool first_validation(int solutionId) {

            for (pair<pair<string, int>, pair<string, int>> pair : relations_check) {
                
                
                Car* c1 = scn->cars[pair.first.first];
                Car* c2 = scn->cars[pair.second.first];
                if (c1->get_lane(pair.first.second) != c2->get_lane(pair.second.second))
                    continue;
                float x1_1 = c1->trajectories[pair.first.second]->getP1();
                float x2_0 = c2->trajectories[pair.second.second]->getP0();

                if (x1_1 > x2_0 - c2->getLength()) {
                    return false;
                }
            }
            return true;
        }
        void ran() {
            timesRan++;
        }
        //updates the time available for a system to be solved
        void update_time_to_run() {
            if (timesRan == 0) {
                time = default_time;
                return;
            }
            time = time * 2;
            ranOut = false;
        }
        // check if the chasings are realistic or not regarding the time each car collides or not with another car
        // return yes if some chasing is incorrect, no otherwise
        bool surpasses(Car* c1, Car* c2, int idx0, int idx1, int solutionId) {
            //cout << "----------------------- chasing checkings -----------------------" << endl;
            float x1_0 = getValue(c1->x[idx0].first, solutionId);
            float t1_0 = getValue(c1->t[idx0].first, solutionId);
            float v1_0 = getValue(c1->v[idx0].first, solutionId);
            float x1_1 = getValue(c1->x[idx0+1].first, solutionId);
            float t1_1 = getValue(c1->t[idx0+1].first, solutionId);
            float v1_1 = getValue(c1->v[idx0+1].first, solutionId);

            float x2_0 = getValue(c2->x[idx1].first, solutionId);
            float t2_0 = getValue(c2->t[idx1].first, solutionId);
            float v2_0 = getValue(c2->v[idx1].first, solutionId);
            float x2_1 = getValue(c2->x[idx1+1].first, solutionId);
            float t2_1 = getValue(c2->t[idx1+1].first, solutionId);
            float v2_1 = getValue(c2->v[idx1+1].first, solutionId);
            cout << "car1: " << c1->getId() << ", car2: " << c2->getId() << endl;
            if (x2_0 <= x1_0)
                return true;

            if (c1->get_lane(idx0) != c2->get_lane(idx1) and (c1->get_lane(idx0)!=c2->get_lane(idx1+1) and c1->get_lane(idx0 + 1) != c2->get_lane(idx1) and c1->get_lane(idx0 + 1) != c2->get_lane(idx1+1)))
                return false;

            bool is_coll_rel = coll_relationship(c1, c2);   

            float time = 0;
            float x1 = 0;
            float x2 = 0;
            //until there is a collision this cant happen
            float final_t = min(getValue(coll_vars, solutionId), min(getValue(c2->t[idx1 + 1].first, solutionId), getValue(c1->t[idx0 + 1].first, solutionId)));

            while (time < final_t-0.5) {
                x1 = x1_0 + v1_0 * (time-t1_0) + 0.5 * ((v1_1 - v1_0) / (t1_1 - t1_0)) * (time-t1_0) * (time - t1_0);
                x2 = x2_0 + v2_0 * (time-t2_0) + 0.5 * ((v2_1 - v2_0) / (t2_1 - t2_0)) * (time- t2_0) * (time- t2_0);

                if (x1 > x2 - c2->getLength()) {
                    return true;
                }


                /*if (is_coll_rel) {

                    if (x1 > x2 - c2->getLength()) {
                        shrink_interval = true;
                        return true;
                    }
                }*/
                
                time += 0.1;
            }
            /*if(is_coll_rel and (x2 - c2->getLength()) - x1 > 1) {

                //cout << "car is far out" << endl;
                    return true;
            }*/
            return false;

        }
        // check if the crossings are realistic or not regarding the time each car enters the crossing
        // return yes if some crossing is incorrect, no otherwise
        bool crossing_incorrect(Car* c1, Car* c2, int idx0, int idx1, int solutionId, float in0, float out0, float in1, float out1) {
            //cout << "----------------------- crossing checkings -----------------------" << endl;
            float x1_0 = getValue(c1->x[idx0].first, solutionId);
            float t1_0 = getValue(c1->t[idx0].first, solutionId);
            float v1_0 = getValue(c1->v[idx0].first, solutionId);
            float x1_1 = getValue(c1->x[idx0 + 1].first, solutionId);
            float t1_1 = getValue(c1->t[idx0 + 1].first, solutionId);
            float v1_1 = getValue(c1->v[idx0 + 1].first, solutionId);

            float x2_0 = getValue(c2->x[idx1].first, solutionId);
            float t2_0 = getValue(c2->t[idx1].first, solutionId);
            float v2_0 = getValue(c2->v[idx1].first, solutionId);
            float x2_1 = getValue(c2->x[idx1 + 1].first, solutionId);
            float t2_1 = getValue(c2->t[idx1 + 1].first, solutionId);
            float v2_1 = getValue(c2->v[idx1 + 1].first, solutionId);

            float time = 0;
            float x1 = 0;
            float x2 = 0;
            //until there is a collision this cant happen
            float final_t = min(getValue(coll_vars, solutionId), min(getValue(c2->t[idx1 + 1].first, solutionId), getValue(c1->t[idx0 + 1].first, solutionId)));
            float t_in0, t_out0, t_in1, t_out1;

            bool checkout0 = false;
            bool checkin0 = false;
            bool checkout1 = false;
            bool checkin1 = false;

            bool is_coll_rel = coll_relationship(c1, c2);
            //cout << "final time:" << final_t << endl;
            /*if (!is_coll_rel)
                return false;*/

            //cout << c1->getId() << " and " << c2->getId() << endl;
            while (time <= final_t+0.1) {
                x1 = x1_0 + v1_0 * (time)+0.5 * ((v1_1 - v1_0) / (t1_1 - t1_0)) * (time) * (time);
                x2 = x2_0 + v2_0 * (time)+0.5 * ((v2_1 - v2_0) / (t2_1 - t2_0)) * (time) * (time);

                /*cout << "x1: " << x1 << endl;
                cout << "x2: " << x2 << endl;
                cout << "time: " << time << " of " << final_t << endl;*/
                if (x1 > in0 and !checkin0) {
                    cout << time << endl;
                    t_in0 = time;
                    checkin0 = true;
                    //cout << c1->getId() << " arrives in " << in0 << " in " << t_in0 << endl;
                }
                if (x1 - c1->getLength() > out0 and !checkout0) {
                    cout << time << endl;
                    t_out0 = time;
                    checkout0 = true;
                    //cout << c1->getId() << " departs in " << out0 << " in " << t_out0 << endl;
                }
                if (x2 > in1 and !checkin1) {
                    cout << time << endl;
                    t_in1 = time;
                    checkin1 = true;
                    //cout << c2->getId() << " arrives in " << in1 << " in " << t_in1 << endl;
                }
                if (x2 - c2->getLength() > out1 and !checkout1) {
                    cout << time << endl;
                    t_out1 = time;
                    checkout1 = true;
                    //cout << c2->getId() << " departs in " << out1 << " in " << t_out1 << endl;
                }
                //only evalautes the possibility of collision 1 second to the actual collision
                
                time += 0.1;
            }

            if (is_coll_rel) {
                if ((checkout0 and !checkin1) or (checkout1 and !checkin0) or (!checkin0 and !checkin1)) {
                    
                    return true;
                }
                
                //cout << t_in1 << " and " << t_in0 << endl;
                return false;
            }

            /*if (checkout0 and checkin1) {
                return t_out0 > t_in1;
            }*/

            return false;
        }
        bool coll_relationship(Car* c1, Car* c2) {
            return count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c1->getId()) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c2->getId())
                or count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c2->getId()) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c1->getId());
        }
        bool check_feasibility() {
            for (pair<pair<string, int>, pair<string, int>> pair : relations_check) {
                if(count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), pair.first.first) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), pair.second.first))
                    if (scn->cars[pair.first.first]->get_lane(pair.first.second) != scn->cars[pair.second.first]->get_lane(pair.second.second) and !scn->changes_lane(scn->cars[pair.first.first], pair.first.second)
                         and !scn->changes_lane(scn->cars[pair.second.first], pair.second.second))
                        return false;
            }
            return true;
        }
        bool check_scenario(int solution_id) {

            cout << "check!" << endl;
            //if (timesRan > limitRan)
            //    return true;
            for (pair<pair<string, int>, pair<string, int>> pair: relations_check) {
                //cout << pair.first.first << " and " << pair.second.first << " are related." << endl;
                if (surpasses(scn->cars[pair.first.first], scn->cars[pair.second.first], pair.first.second, pair.second.second, solution_id))
                    return false;
            }   
            for (tuple<pair<string, int>, pair<string, int>, pair<float, float>, pair<float, float>> tup : crossings_checks) {
                if (crossing_incorrect(scn->cars[get<0>(tup).first], scn->cars[get<1>(tup).first], get<0>(tup).second, get<1>(tup).second, solution_id,get<2>(tup).first, get<2>(tup).second, get<3>(tup).first, get<3>(tup).second))
                    return false;
            }
            cout << "it is valid!" << endl;
            return true;
        }
        void defineSearchSpace() {

            map<string, Car*>::iterator it;
            int j, n = 0;
            for (it = scn->cars.begin(); it != scn->cars.end(); it++) {
                for (int i = 0; i < it->second->trjInts.size(); i++) {
                    sys->box[3 * i + 3 * j * n] = it->second->trjInts[i][0];
                    sys->box[3 * i + 1 + 3 * j * n] = it->second->trjInts[i][1];
                    sys->box[3 * i + 2 + 3 * j * n] = it->second->trjInts[i][2];
                }
                j++;
                n = it->second->trjInts.size();
            }

        }
        void setFixPointContractor() {
            int nCstr = sys->nb_ctr;
            Array<Ctc> vctc(nCstr);
            for (int i = 0; i < nCstr; i++) vctc.set_ref(i, *new CtcHC4(sys->ctrs[i]));
            CtcCompo* compo = new CtcCompo(vctc);
            ctc = new CtcFixPoint(*compo);
        }
        void prune(IntervalVector& box) {
            ctc->contract(box);
        }
        IntervalVector searchSolution(IntervalVector box, time_t ori) {
            prune(box);
            
            if (box.is_empty())
                return box;
            if (box.max_diam() < eps)
                return box;
            if (clock() - ori > time * CLOCKS_PER_SEC) {
                //cout << "ALERT: TIME RAN OUT!!!!! in search solution" << endl;
                return box;
            }
            IntervalVector B = box;
            IntervalVector sol;
            int i = box.extr_diam_index(false);
            box[i] = Interval(box[i].mid());
            sol = searchSolution(box, ori);
            if (!sol.is_empty()) return sol;
            pair<IntervalVector, IntervalVector> p = B.bisect(i);
            sol = searchSolution(p.first, ori);
            if (!sol.is_empty()) return sol;
            sol = searchSolution(p.second, ori);
            return sol;
        }
        void solve(time_t ori) {
            //if (!first_validation(0))
              //  return;
            my_priority_queue s;
            IntervalVector box = sys->box;
            s.push(box);
            double boxSize = box.max_diam();
            while (!s.empty() && s.size() <= nBoxes) {
                // get the largest box        
                box = s.top();
                boxSize = box.max_diam();
                s.pop();
                cout << "--- Pop I = " << box;
                // prune the box
                prune(box);
                cout << " -> " << box << endl;
                if (!box.is_empty()) {
                    // if box was not discarded
                    cout << box.max_diam() << endl;
                    if (box.max_diam() <= eps) {
                        // if box is small enough
                        cout << "--- zero " << box << " (width: " << box.max_diam() << ")" << endl;
                        // keep the whole box
                        s.push(box);
                    }
                    else {
                        // if box is splitable
                        // find the largest interval domain
                        const int i = box.extr_diam_index(false);
                        // bisect the box in the midpoint of this domain
                        pair<IntervalVector, IntervalVector> p = box.bisect(i);
                        cout << "--- split " << box << " -> " << p.first << " ; " << p.second << endl;
                        // keep both boxes
                        if (!searchSolution(p.first, ori).is_empty())  s.push(p.first); // certify that there is at least one solution
                        if (!searchSolution(p.second, ori).is_empty()) s.push(p.second);// may be too strong
                        //s.push(p.first);
                        //s.push(p.second);
                    }
                }
                else {
                    // the box is discarded (was pruned)
                    cout << "--- delete (was pruned)" << box << endl;
                }
                time_t x = clock();  
                if ((x - ori) > time * CLOCKS_PER_SEC)
                    break;
            }
            int n = s.size();
            partition.clear();
            solutions.clear();
            valid = true;
            for (int i = 0; i < n; i++) {
                box = s.top();
                s.pop();
                partition.push_back(box);
                solutions.push_back(searchSolution(box, ori));
                last_eps = solutions[solutions.size() - 1].max_diam();
                if (!check_scenario(solutions.size() - 1)) {
                    cout << "remove" << endl;
                    solutions.pop_back();
                    //valid = valid && check_scenario(solutions.size() - 1);
                }
                else {
                    cout << "accepted!" << endl;
                }

                //cout << "is empty: " << solutions[solutions.size() - 1].is_empty() << endl;
            }
            
            return;
        }
        void run() {
            if (this->timesRan > 0 and shrink_interval) {
                sys->box[getIndex(coll_vars)] = Interval(0, getValue(coll_vars, 0)-0.1);
                cout << " new interval in index " << getIndex(coll_vars) << " is " << sys->box[getIndex(coll_vars)] << endl;
            }
            setHC4Contractor();
            solve(clock());
            update_time_to_run();
            ran();
        }
        void showPartition() {
            IntervalVector box;
            cout << "partition with " << partition.size() << " boxes:" << endl;
            for (int i = 0; i < partition.size(); i++) {
                box = partition[i];
                cout << "   box " << i << " -> " << box << " size: " << box.max_diam() << endl;
            }
        }
        void showSolutions() {
            IntervalVector box;
            cout << solutions.size() << " solutions:" << endl;
            for (int i = 0; i < solutions.size(); i++) {
                box = solutions[i];
                if (box.is_empty()) cout << "   sol " << i << " -> empty" << endl;
                else cout << "   sol " << i << " -> " << box << " size: " << box.max_diam() << endl;
            }
        }
        void showAll() {
            IntervalVector box;
            cout << *sys << endl;
            for (int i = 0; i < partition.size(); i++) {
                box = partition[i];
                cout << "   box " << i << " -> " << box << " size: " << box.max_diam() << endl;
                if (solutions.size() > 0) {
                    if (solutions.size() - 1 < i)
                        box = solutions[solutions.size() - 1];
                    else
                        box = solutions[i];
                    //box = solutions[std::min(solutions.size()-1,i)];
                    if (box.is_empty()) cout << "   sol " << i << " -> empty" << endl;
                    else cout << "   sol " << i << " -> " << box << " size: " << box.max_diam() << endl;
                }
            }
        }
        int getIndex(string valueName) {
            for (int i = 0; i < ords.size(); i++) {
                //cout << "ord[" << i << "]: " << ords[i] << ", for " << valueName << " and size: " << ords.size() << endl;
                if (valueName == ords[i])
                    return i;
            }
            return -1;
        }
        float getValue(string valueName, int solutionId) {
            for (int i = 0; i < ords.size(); i++) {
                //cout << "ord[" << i << "]: " << ords[i] << ", for " << valueName << " and size: " << ords.size() << endl;
                if (valueName == ords[i])
                    return (solutions[solutionId][i].ub() + solutions[solutionId][i].lb())/2;
            }
            map<string, vector<ScenarioSolve*>>::iterator it;
            for (string s : independent_cars) {
                //cout << "ind car: " << s << endl;
                float ret = moves[s][0]->getValue(valueName, 0);
                if (ret != -1)
                    return ret;
            }
            return -1;
        }
    };

    class Problem {
    public:
        Scenario* scn;
        vector<ScenarioSolve*> scns;
        map<string, ScenarioSolve*> ind_scns;
        map<string, vector<System*>> systems;
        map<string, vector<ScenarioSolve*>> systems_aux;
        vector<vector<string>> ords;
        vector<string> coll_vars;
        vector<ScenarioSolve*> tocheck_systems;
        map<string, System*> indep_cars;
        vector<string> depend_cars;
        vector<System*> s;
        vector<ScenarioSolve*> s_aux;

        Problem(Scenario* scn) {
            this->scn = scn;
        }

        void compile_movements() {
            //cout << "--------------------------------- MOVEMENT COMPILATION ---------------------------------" << endl;
            map<string, Car*>::iterator it;
            int idx = 0;
            for (it = scn->cars.begin(); it != scn->cars.end(); it++) {
                //cout << "system compiled for " << it->first << endl;
                // new
                vector<ScenarioSolve*> s;
                SystemFactory fac;
                addCarVars(fac, it->second);
                addCarConstraints(fac, it->second);
                System sys(fac);
                defineSearchSpace(&sys, it->second);
                // new
                s.push_back(new ScenarioSolve(scn, new System(sys)));
                // new
                systems_aux[it->first] = s;
                //cout << sys << endl;
            }
            //cout << "Compiled movements successfully!" << endl;
        }
        vector<string> get_independent_cars() {
            map<string, vector<ScenarioSolve*>>::iterator it;
            vector<string> ind;
            for (it = systems_aux.begin(); it != systems_aux.end(); it++) {
                if (!count(scn->processed_cars.begin(), scn->processed_cars.end(), it->first))
                    ind.push_back(it->first);
            }
            return ind;
        }
        void add_main_vars(SystemFactory& fac, vector<string> vars) {
            bool check = false;
            map<string, Car*>::iterator it;
            for (int k = 0; k < vars.size(); k++) {
                check = false;
                for (it = scn->cars.begin(); it != scn->cars.end(); it++) {
                    check = it->second->varExists(vars[k].c_str());
                    //cout << "searching " << vars[k].c_str() << " - " << check << " " << endl;
                    if (check) {
                        //cout << "var: " << vars[k].c_str() << endl;
                        fac.add_var(it->second->getVar(vars[k].c_str()));
                        //cout << vars[k].c_str() << " ";
                        break;
                    }
                }
            }
            //cout << endl;
        }
        bool pair_exists(vector<pair<string, string>> pairs, string s1, string s2) {
            for (pair<string, string> p : pairs)
                if ((p.first == s1 and p.second == s2) or (p.first == s2 and p.second == s1))
                    return true;
            return false;
        }

        int get_index(vector<string> vector, string s) {
            for (int i = 0; i < vector.size(); i++)
                if (vector[i] == s)
                    return i;
            return -1;
        }
        void print_matrix(vector<vector<string>> matrix) {
            /*for (vector<string> v : matrix) {
                for (string t : v)
                    cout << t << " ";
                cout << "--------------" << endl;
            }*/
        }
        void print_matrix(vector<vector<int>> matrix) {
            /*cout << "--------------" << endl;
            for (vector<int> v : matrix) {
                for (int t : v)
                    cout << t << " ";
                cout << endl;
            }
            cout << "--------------" << endl;*/
        }

        void print_3d(vector<vector<vector<int>>> d) {
            /*for (vector<vector<int>> v : d) {
                for (vector<int> v1 : v) {
                    for (int v2 : v1)
                        cout << v2 << " ";
                    cout << endl;
                }
                cout << endl;
            }*/
        }
        bool system_is_valid(System* sys) {
            System* s_aux = new System(*sys);
            CtcHC4 hc4(*s_aux);
            hc4.contract(s_aux->box);
            //if (!sys->box.is_empty()) {
            if (!s_aux->box.is_empty()) {
                //carregar o scenario
                return true;
            }
            else {
                //cout << "Invalid scenario!" << endl;
            }
        }
        void compile_crossings_everything() {

            //cout << "-------------------------- CROSSINGS --------------------------" << endl;
            vector<vector<int>> order;
            for (int i = 0; i < scn->cross_global_relations.size(); i++) {
                vector<int> aux;
                for (int j = 0; j < scn->cross_global_relations[i]->get_size(); j++) {
                    aux.push_back(j);
                }
                order.push_back(aux);
            }
            if (!order.size())
                return;
            vector<tuple<pair<string, int>, pair<string, int>, pair<float, float>, pair<float, float>>> cross_checks;
            vector<string> to_mix_cars;
            vector<string> cars_considered;
            Goal* goal = scn->goals[0];
            print_matrix(order);
            //cout << order.size() << " and " << order[0].size() << endl;
            vector<vector<int>> cart = cartesian(order);
            //cout << "cartesian" << endl;
            print_matrix(cart);
            
            string coll_var;
            bool collision = false;
            vector<ScenarioSolve*> to_replace_scenarios;
            vector<System*> to_mix;
            int index = 0;
            
            //cout << s_aux.size() << endl;
            for (ScenarioSolve* scenario : s_aux) {

                if (index > 100)
                    break;
                //cout << cart.size() << " is the size of all the cartesians" << endl;
                for (vector<int> ord : cart) {
                    index++;
                    if (index > 100)
                        break;
                    cout << "next:" << index << endl;
                    to_mix.clear();
                    to_mix.push_back(scenario->sys);
                    cross_checks.clear();
                    coll_var = scenario->coll_vars;
                    collision = scenario->isCollision;
                    for (int i = 0; i < ord.size(); i++) {
                        SystemFactory fac;
                        Crossing_Relation* r = scn->cross_global_relations[i]->final[ord[i]];
                        Car* c1 = scn->cars[r->car1];
                        Car* c2 = scn->cars[r->car2];

                        if (!count(scn->processed_cars.begin(), scn->processed_cars.end(), c1->getId())) {
                            scn->processed_cars.push_back(c1->getId());
                            if (!count(to_mix_cars.begin(), to_mix_cars.end(), c1->getId()))
                                to_mix_cars.push_back(c1->getId());
                        }
                        if (!count(scn->processed_cars.begin(), scn->processed_cars.end(), c2->getId())) {
                            scn->processed_cars.push_back(c2->getId());
                            if (!count(to_mix_cars.begin(), to_mix_cars.end(), c2->getId()))
                                to_mix_cars.push_back(c2->getId());
                        }

                        if (!count(cars_considered.begin(), cars_considered.end(), c1->getId()))
                            cars_considered.push_back(c1->getId());
                        if (!count(cars_considered.begin(), cars_considered.end(), c2->getId()))
                            cars_considered.push_back(c2->getId());

                        vector<string> mixed_vars = to_mix[0]->var_names();
                        for (string str : systems_aux[r->car1][0]->sys->var_names()) {
                            if (!count(mixed_vars.begin(), mixed_vars.end(), str))
                                mixed_vars.push_back(str);
                        }
                        for (string str : systems_aux[r->car2][0]->sys->var_names()) {
                            if (!count(mixed_vars.begin(), mixed_vars.end(), str))
                                mixed_vars.push_back(str);
                        }

                        float c2_ref = 0;
                        float c1_ref = 0;
                        Road* r1 = scn->roadNet->get_road((scn->get_road(c1)));
                        Road* r2 = scn->roadNet->get_road(scn->get_road(c2));
                        if (!scn->roadNet->get_road((scn->get_road(c1)))->dir) {
                            c1_ref = max(scn->roadNet->get_road((scn->get_road(c1)))->h_axis1 - scn->roadNet->get_road((scn->get_road(c1)))->h_axis0,
                                scn->roadNet->get_road((scn->get_road(c1)))->v_axis1 - scn->roadNet->get_road((scn->get_road(c1)))->v_axis0);
                        }
                        if (!scn->roadNet->get_road((scn->get_road(c2)))->dir) {
                            c2_ref = max(scn->roadNet->get_road((scn->get_road(c2)))->h_axis1 - scn->roadNet->get_road((scn->get_road(c2)))->h_axis0,
                                scn->roadNet->get_road((scn->get_road(c2)))->v_axis1 - scn->roadNet->get_road((scn->get_road(c2)))->v_axis0);
                        }

                        add_main_vars(fac, mixed_vars);
                        fac.add_var(r->get_first_car_tin());
                        fac.add_var(r->get_first_car_tout());
                        fac.add_var(r->get_second_car_tin());
                        fac.add_var(r->get_second_car_tout());

                        //(((-xcar2_0) - ((vcar2_0 * (tcar2_in0 - tcar2_0)) + (0.5 * ((((vcar2_1 - vcar2_0) / (tcar2_1 - tcar2_0)) * (tcar2_in0 - tcar2_0)) * (tcar2_in0 - tcar2_0))))) + 20) = 0
                        if (r1->dir) {
                            fac.add_ctr(r->cross[0].lb() - c1->x[r->trj_idx0_in].second = (c1->v[r->trj_idx0_in].second) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second) + 0.5 * ((c1->v[r->trj_idx0_in + 1].second - c1->v[r->trj_idx0_in].second) / (c1->t[r->trj_idx0_in + 1].second - c1->t[r->trj_idx0_in].second)) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second));
                            fac.add_ctr(r->cross[0].ub() - c1->x[r->trj_idx0_out].second + c1->getLength() = (c1->v[r->trj_idx0_out].second) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second) + 0.5 * ((c1->v[r->trj_idx0_out + 1].second - c1->v[r->trj_idx0_out].second) / (c1->t[r->trj_idx0_out + 1].second - c1->t[r->trj_idx0_out].second)) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second));
                        }
                        else {
                            fac.add_ctr(c1_ref - r->cross[0].ub() - c1->x[r->trj_idx0_in].second = (c1->v[r->trj_idx0_in].second) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second) + 0.5 * ((c1->v[r->trj_idx0_in + 1].second - c1->v[r->trj_idx0_in].second) / (c1->t[r->trj_idx0_in + 1].second - c1->t[r->trj_idx0_in].second)) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second) * (r->get_first_car_tin() - c1->t[r->trj_idx0_in].second));
                            fac.add_ctr(c1_ref - r->cross[0].lb() - c1->x[r->trj_idx0_out].second + c1->getLength() = (c1->v[r->trj_idx0_out].second) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second) + 0.5 * ((c1->v[r->trj_idx0_out + 1].second - c1->v[r->trj_idx0_out].second) / (c1->t[r->trj_idx0_out + 1].second - c1->t[r->trj_idx0_out].second)) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second) * (r->get_first_car_tout() - c1->t[r->trj_idx0_out].second));
                        }

                        if (r2->dir) {
                            fac.add_ctr(r->cross[1].lb() - c2->x[r->trj_idx1_in].second = (c2->v[r->trj_idx1_in].second) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second) + 0.5 * ((c2->v[r->trj_idx1_in + 1].second - c2->v[r->trj_idx1_in].second) / (c2->t[r->trj_idx1_in + 1].second - c2->t[r->trj_idx1_in].second)) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second));
                            fac.add_ctr(r->cross[1].ub() - c2->x[r->trj_idx1_out].second + c2->getLength() = (c2->v[r->trj_idx1_out].second) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second) + 0.5 * ((c2->v[r->trj_idx1_out + 1].second - c2->v[r->trj_idx1_out].second) / (c2->t[r->trj_idx1_out + 1].second - c2->t[r->trj_idx1_out].second)) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second));
                        }
                        else {
                            fac.add_ctr(c2_ref - r->cross[1].ub() - c2->x[r->trj_idx1_in].second = (c2->v[r->trj_idx1_in].second) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second) + 0.5 * ((c2->v[r->trj_idx1_in + 1].second - c2->v[r->trj_idx1_in].second) / (c2->t[r->trj_idx1_in + 1].second - c2->t[r->trj_idx1_in].second)) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second) * (r->get_second_car_tin() - c2->t[r->trj_idx1_in].second));
                            fac.add_ctr(c2_ref - r->cross[1].lb() - c2->x[r->trj_idx1_out].second + c2->getLength() = (c2->v[r->trj_idx1_out].second) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second) + 0.5 * ((c2->v[r->trj_idx1_out + 1].second - c2->v[r->trj_idx1_out].second) / (c2->t[r->trj_idx1_out + 1].second - c2->t[r->trj_idx1_out].second)) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second) * (r->get_second_car_tout() - c2->t[r->trj_idx1_out].second));
                        }
                        if (count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c1->getId()) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c2->getId())) {
                            fac.add_ctr(r->get_first_car_tin() <= r->get_second_car_tin());
                            fac.add_ctr(r->get_second_car_tin() <= r->get_first_car_tout());
                            coll_var = r->get_second_car_tin_name();
                            collision = true;
                        }
                        else {
                            fac.add_ctr(r->get_first_car_tout() < r->get_second_car_tin());
                        }

                        if (r1->dir and r2->dir) {
                            cross_checks.push_back(make_tuple(make_pair(c1->getId(), r->trj_idx0_out), make_pair(c2->getId(), r->trj_idx1_in), make_pair(r->cross[0].lb(), r->cross[0].ub()), make_pair(r->cross[1].lb(), r->cross[1].ub())));
                        }
                        else if(r1->dir and !r2->dir) {
                            cross_checks.push_back(make_tuple(make_pair(c1->getId(), r->trj_idx0_out), make_pair(c2->getId(), r->trj_idx1_in), make_pair(r->cross[0].lb(), r->cross[0].ub()), make_pair(r->cross[1].ub(), r->cross[1].lb())));
                        }
                        else if (!r1->dir and r2->dir) {
                            cross_checks.push_back(make_tuple(make_pair(c1->getId(), r->trj_idx0_out), make_pair(c2->getId(), r->trj_idx1_in), make_pair(r->cross[0].ub(), r->cross[0].lb()), make_pair(r->cross[1].lb(), r->cross[1].ub())));
                        }
                        else if (!r1->dir and !r2->dir) {
                            cross_checks.push_back(make_tuple(make_pair(c1->getId(), r->trj_idx0_out), make_pair(c2->getId(), r->trj_idx1_in), make_pair(r->cross[0].ub(), r->cross[0].lb()), make_pair(r->cross[1].ub(), r->cross[1].lb())));
                        }


                        System system(fac);


                        vector<string> v = system.var_names();
                        vector<string> c1v = systems_aux[r->car1][0]->sys->var_names();
                        vector<string> c2v = systems_aux[r->car2][0]->sys->var_names();
                        vector<string> s1 = to_mix[0]->var_names();
                        int index, i1, i2;
                        int i1c, i2c, is;
                        for (string str : v) {
                            i1c = get_index(c1v, str);
                            i2c = get_index(c2v, str);
                            is = get_index(s1, str);
                            index = get_index(v, str);
                            if (is != -1) {
                                system.box[index] = to_mix[0]->box[is];
                            }
                            else if (i1c != -1) {
                                system.box[index] = systems_aux[r->car1][0]->sys->box[i1c];
                            }
                            else if (i2c != -1) {
                                system.box[index] = systems_aux[r->car2][0]->sys->box[i2c];
                            }
                            else {
                                if (str == r->get_first_car_tin_name())
                                    system.box[index] = Interval(c1->trajectories[r->trj_idx0_in]->getT1(), c1->trajectories[r->trj_idx0_in + 1]->getT0());
                                if (str == r->get_first_car_tout_name())
                                    system.box[index] = Interval(c1->trajectories[r->trj_idx0_out]->getT1(), c1->trajectories[r->trj_idx0_out + 1]->getT0());
                                if (str == r->get_second_car_tin_name())
                                    system.box[index] = Interval(c2->trajectories[r->trj_idx1_in]->getT1(), c2->trajectories[r->trj_idx1_in + 1]->getT0());
                                if (str == r->get_second_car_tout_name())
                                    system.box[index] = Interval(c2->trajectories[r->trj_idx1_out]->getT1(), c2->trajectories[r->trj_idx1_out + 1]->getT0());
                            }
                        }


                        System mixed(*to_mix[0], system);
                        to_mix.clear();
                        to_mix.push_back(new System(mixed));

                    }

                    vector<System*> m;
                    m.push_back(to_mix[0]);
                    for (string unconsidered_car : to_mix_cars) {
                        System repl(*m[0], *systems_aux[unconsidered_car][0]->sys);
                        m.clear();
                        m.push_back(new System(repl));
                    }
                    //new ScenarioSolve(scn, m[0], scenario->coll_vars, scenario->isCollision, id++)
                    if (scenario->isCollision) {
                        to_replace_scenarios.push_back(new ScenarioSolve(scn, m[0], scenario->coll_vars, scenario->isCollision, scenario->relations_check, cross_checks));
                    }
                    else {
                        to_replace_scenarios.push_back(new ScenarioSolve(scn, m[0], coll_var, collision, scenario->relations_check, cross_checks));
                    }
               }
               
            }

            s_aux.clear();
            for (ScenarioSolve* scn : to_replace_scenarios) {
                if (system_is_valid(scn->sys)) {
                    s_aux.push_back(scn);
                    //cout << "Valid scenario!" << endl;
                }
                else {
                    //cout << "Invalid scenario!" << endl;
                }
            }

            //cout << "---------------- finish crossings ----------------" << endl;
        }
        bool exists_another_car_front(Car* c1, Car* c2,  int idx1, int idx2, vector<int> ord) {
            if (c1->get_lane(idx1) != c2->get_lane(idx2))
                return false;
            for (int i = 0; i < ord.size(); i++) {
                Relation* r = scn->global_relations[i]->final[ord[i]];
                if (r->get_first_car_name() != c1->getId())
                    continue;
                
                if (idx1 != r->trj_idx0)
                    continue;
                
                Car* c3 = scn->cars[r->get_second_car_name()];
                
                int idx3_alt = r->trj_idx1;

                if (c3->getId() == c2->getId() and r->trj_idx0 == idx1 and r->trj_idx1 == idx3_alt)
                    continue;

                Trajectory* trj_1 = c1->getTrajectory(idx1); 
                Trajectory* trj_3 = c3->getTrajectory(idx3_alt);

                if (trj_3->getLane() != c1->get_lane(idx1) and (!scn->changes_lane(c3, idx3_alt) and !scn->changes_lane(c1, idx1))) {
                    //cout << "continue here" << endl;
                    continue;
                }
                if (trj_3->getP0() > c1->getTrajectory(idx1)->getP1() and trj_3->getP1() < c2->getTrajectory(idx2)->getP0())
                    return true;
            }
            return false;
        }
        bool coll_relationship(Car* c1, Car* c2) {
            return count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c1->getId()) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c2->getId())
                or count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c2->getId()) and count(scn->goals[0]->cars.begin(), scn->goals[0]->cars.end(), c1->getId());
        }
        bool check_idxs(vector<int> ord){
            map<string, int> map;
            
            for (int i = 0; i < ord.size(); i++) {
                Relation* r = scn->global_relations[i]->final[ord[i]];
                if (map.find(r->get_first_car_name()) == map.end()) {
                    map[r->get_first_car_name()] = r->trj_idx0;
                }
                else if(map[r->get_first_car_name()] != r->trj_idx0)
                        return false;

                if (map.find(r->get_second_car_name()) == map.end()) {
                    map[r->get_second_car_name()] = r->trj_idx1;
                }
                else if (map[r->get_first_car_name()] != r->trj_idx0)
                    return false;

            }
        } 
        void compile_chasings_everything() {
            //cout << "-------------------------- CHASINGS --------------------------" << endl;
            vector<vector<int>> order;
            vector<pair<pair<string, int>, pair<string, int>>> relate_checks;

            for (int i = 0; i < scn->global_relations.size(); i++) {
                vector<int> aux;
                for (int j = 0; j < scn->global_relations[i]->get_size(); j++) {
                    aux.push_back(j);
                }
                order.push_back(aux);
            }
            if (!order.size()) {
                vector<System*> aux;
                string coll_var;
                bool collision;
                map<string, Car*>::iterator it;
                for (it = scn->cars.begin(); it != scn->cars.end(); it++) {
                    if (aux.size() == 0) {
                        aux.push_back(systems_aux[it->first][0]->sys);
                        continue;
                    }
                    System new_s(*aux[0], *systems_aux[it->first][0]->sys);
                    aux.clear();
                    aux.push_back(new System(new_s));
                }

                s_aux.push_back(new ScenarioSolve(scn, new System(*aux[0]), coll_var, collision));
                //cout << "there are no chasings here" << endl;
                return;
            }

            Goal* goal = scn->goals[0];
            print_matrix(order);
            cartesian(order);
            vector<vector<int>> cart = cartesian(order);
            print_matrix(cart);
            string coll_var;
            bool collision;
            bool put_constraints = true;
            vector<System*> final_systems;
            vector<System*> system;
            vector<string> cars_considered;
            vector<Relation*> relations;
            vector<string> all_cars;

            for (vector<int> ord : cart) {
                
                map<string, int> idx_ident;
                coll_var = "";
                collision = false;
                //cout << "next" << endl;
                relations.clear();
                relate_checks.clear();
                all_cars.clear();
                for (int i = 0; i < ord.size(); i++) {
                    put_constraints = true;
                    SystemFactory fac;
                    Relation* r = scn->global_relations[i]->final[ord[i]];

                    relations.push_back(r);

                    Car* c1 = scn->cars[r->get_first_car_name()];
                    Car* c2 = scn->cars[r->get_second_car_name()];

                    if(!count(all_cars.begin(), all_cars.end(), c1->getId()))
                        all_cars.push_back(c1->getId());
                    if(!count(all_cars.begin(), all_cars.end(), c2->getId()))
                        all_cars.push_back(c2->getId());

                    if (exists_another_car_front(c1, c2, r->trj_idx0, r->trj_idx1, ord)) {
                        //put_constraints = false;
                        continue;
                    }
                    if (c1->get_lane(r->trj_idx0) != c2->get_lane(r->trj_idx1) and !scn->changes_lane(c1, r->trj_idx0) and !scn->changes_lane(c2, r->trj_idx1)) {
                        put_constraints = false;
                    }

                    if (c1->trajectories[r->trj_idx0 + 1]->getP1() < c2->trajectories[r->trj_idx1]->getP0() ) {
                        put_constraints = false;
                    }

                    if (!count(scn->processed_cars.begin(), scn->processed_cars.end(), c1->getId()))
                        scn->processed_cars.push_back(c1->getId());
                    if(!count(scn->processed_cars.begin(), scn->processed_cars.end(), c2->getId()))
                        scn->processed_cars.push_back(c2->getId());
                    if (!count(cars_considered.begin(), cars_considered.end(), c1->getId()))
                        cars_considered.push_back(c1->getId());
                    if (!count(cars_considered.begin(), cars_considered.end(), c2->getId()))
                        cars_considered.push_back(c2->getId());
                    

                    add_main_vars(fac, systems_aux[r->get_first_car_name()][0]->sys->var_names());
                    add_main_vars(fac, systems_aux[r->get_second_car_name()][0]->sys->var_names());
                    if (put_constraints) {
                        fac.add_var(r->get_first_car());
                        fac.add_var(r->get_second_car());
                        //fac.add_var(r->get_time());
                        //fac.add_var(scn->coll_chasing_variable[0]);
                        if (scn->coll_chasing_variable.size() > 0)
                            fac.add_var(scn->coll_chasing_variable[0]);
                        else
                            fac.add_var(r->get_time());
                    }
                    
                    int idx0 = r->trj_idx0;
                    int idx1 = r->trj_idx1;

                    relate_checks.push_back(make_pair(make_pair(r->get_first_car_name(), idx0), make_pair(r->get_second_car_name(), idx1)));

                    if (scn->coll_chasing_variable.size() > 0 and put_constraints) {
                        fac.add_ctr(r->get_first_car() = c1->x[idx0].second + c1->v[idx0].second * (scn->coll_chasing_variable[0] - c1->t[idx0].second) + 0.5 * ((c1->v[idx0 + 1].second - c1->v[idx0].second) / (c1->t[idx0 + 1].second - c1->t[idx0].second)) * (scn->coll_chasing_variable[0] - c1->t[idx0].second) * (scn->coll_chasing_variable[0] - c1->t[idx0].second));
                        fac.add_ctr(r->get_second_car() = c2->x[idx1].second + c2->v[idx1].second * (scn->coll_chasing_variable[0] - c2->t[idx1].second) + 0.5 * ((c2->v[idx1 + 1].second - c2->v[idx1].second) / (c2->t[idx1 + 1].second - c2->t[idx1].second)) * (scn->coll_chasing_variable[0] - c2->t[idx1].second) * (scn->coll_chasing_variable[0] - c2->t[idx1].second));
                        fac.add_ctr(scn->coll_chasing_variable[0] >= c1->t[idx0].second);
                        fac.add_ctr(scn->coll_chasing_variable[0] <= c1->t[idx0 + 1].second);
                        fac.add_ctr(scn->coll_chasing_variable[0] >= c2->t[idx1].second);
                        fac.add_ctr(scn->coll_chasing_variable[0] <= c2->t[idx1 + 1].second);
                    }else{
                        if (put_constraints) {
                            fac.add_ctr(r->get_first_car() = c1->x[idx0].second + c1->v[idx0].second * (r->get_time() - c1->t[idx0].second) + 0.5 * ((c1->v[idx0 + 1].second - c1->v[idx0].second) / (c1->t[idx0 + 1].second - c1->t[idx0].second)) * (r->get_time() - c1->t[idx0].second) * (r->get_time() - c1->t[idx0].second));
                            fac.add_ctr(r->get_second_car() = c2->x[idx1].second + c2->v[idx1].second * (r->get_time() - c2->t[idx1].second) + 0.5 * ((c2->v[idx1 + 1].second - c2->v[idx1].second) / (c2->t[idx1 + 1].second - c2->t[idx1].second)) * (r->get_time() - c2->t[idx1].second) * (r->get_time() - c2->t[idx1].second));
                            fac.add_ctr(r->get_time() >= c1->t[idx0].second);
                            fac.add_ctr(r->get_time() <= c1->t[idx0 + 1].second);
                            fac.add_ctr(r->get_time() >= c2->t[idx1].second);
                            fac.add_ctr(r->get_time() <= c2->t[idx1 + 1].second);
                        }
                    }
                    

                    if (std::count(goal->cars.begin(), goal->cars.end(), c1->getId()) and std::count(goal->cars.begin(), goal->cars.end(), c2->getId()) and put_constraints) {
                        fac.add_ctr(r->get_first_car() = r->get_second_car() - c2->getLength());
                        collision = true;
                        coll_var = r->get_third_time();
                        coll_var = scn->collision_var;
                    }
                    else if(put_constraints) {
                        fac.add_ctr(r->get_first_car() < r->get_second_car() - c2->getLength() - 1);
                    }

                    System sys(fac);
                    vector<string> v = sys.var_names();
                    vector<string> v1 = systems_aux[r->get_first_car_name()][0]->sys->var_names();
                    vector<string> v2 = systems_aux[r->get_second_car_name()][0]->sys->var_names();
                    int index, i1, i2;
                    for (string str : v) {

                        index = get_index(v, str);
                        i1 = get_index(v1, str);
                        i2 = get_index(v2, str);

                        if (i1 != -1) {
                            sys.box[index] = systems_aux[r->get_first_car_name()][0]->sys->box[i1];
                        }
                        else if(i2 != -1) {
                            sys.box[index] = systems_aux[r->get_second_car_name()][0]->sys->box[i2];
                        }
                        else {
                                    
                            if (index < sys.box.size() and isinf(sys.box[index].diam())) {
                                sys.box[index] = Interval(c2->trajectories[idx1]->getP0(), c2->trajectories[idx1 + 1]->getP1());
                                //cout << v[i1] << " with interval " << sys.box[i1] << " and values " << c2->trajectories[vector[j]]->getP1() << ", " << c2->trajectories[vector[j] + 1]->getP0() << endl;
                            }
                            if (index + 1 < sys.box.size() and isinf(sys.box[index + 1].diam())) {
                                sys.box[index + 1] = Interval(c2->trajectories[idx1]->getP0(), c2->trajectories[idx1 + 1]->getP1());
                                //cout << v[i1 + 1] << " with interval " << sys.box[i1 + 1] << " and values " << c2->trajectories[vector[j]]->getP1()  << ", " << c2->trajectories[vector[j] + 1]->getP0() << endl;
                            } 
                            if (index + 2 < sys.box.size() and isinf(sys.box[index + 2].diam())) {
                                
                                //sys.box[index + 2] = Interval(c2->trajectories[idx1]->getT1(), c2->trajectories[idx1 + 1]->getT0());
                                //sys.box[index + 2] = Interval(0, scn->duration);
                                sys.box[index + 2] = Interval(0, scn->duration);
                                //cout << v[i1 + 2] << " with interval " << sys.box[i1 + 1] << " and values " << c2->trajectories[vector[j]]->getP1()  << ", " << c2->trajectories[vector[j] + 1]->getP0() << endl;
                            }
                        }

                    }
                    System sys1(sys, *systems_aux[r->get_first_car_name()][0]->sys);
                    System sys2(sys1, *systems_aux[r->get_second_car_name()][0]->sys);
                    if (system.size() == 0) {
                        system.push_back(new System(sys2));
                    }
                    else {
                        system[0] = new System(sys2, *system[0]);
                    }

                    //cout << *system[0] << endl;
                }
                for (string car : all_cars) {
                    //cout << "all car:" << car << endl;
                    if (!count(cars_considered.begin(), cars_considered.end(), car)) {
                        System new_s(*systems_aux[car][0]->sys, *system[0]);
                        system.clear();
                        system.push_back(new System(new_s));
                        //cout << "PASSA AQUIIIIIIIIIIIIIIII" << endl;
                    }
                }
                final_systems.push_back(system[0]);
                Car* c;
                //cout << "Cars considered: " << cars_considered.size() << endl;
                //cout << *system[0] << endl;
                // why do we need the for here, we just load the final scenario
                if (system_is_valid(system[0])) {
                    //cout << "------------------------------------------ FINAL SYS ------------------------------------------" << endl;
                    //cout << *system[0] << endl;
                    //cout << "Valid scenario!" << endl;
                    //cout << *system[0] << endl;
                    s_aux.push_back(new ScenarioSolve(scn, new System(*system[0]), coll_var, collision, relate_checks));
                }
                else {
                    //cout << "Invalid scenario!" << endl;
                }
                    
                system.clear();
                
            }
        }
        int get_car_index(string car) {
            for (int i = 0; i < scn->chasing_clusters.size(); i++)
                if (count(scn->chasing_clusters[i].begin(), scn->chasing_clusters[i].end(), car))
                    return i;
            return -1;
        }
        void defineSearchSpace(System* sys, Car* c) {
            //trajectories
            int i = 0;
            for (i = 0; i < c->trjInts.size(); i++) {
                sys->box[3 * i] = c->trjInts[i][0];
                sys->box[3 * i + 1] = c->trjInts[i][1];
                sys->box[3 * i + 2] = c->trjInts[i][2];
            }
        }
        void addCarConstraints(SystemFactory& fac, Car* c) {
            //general movement constraints
            for (int i = 0; i < c->t.size(); i++) {
                //cout << c->changing_lane.size() << " is the size of it" << endl;
                if (i >= 1) {
                    
                    //cout << i << ": " << c->changing_lane[i] << " of a size " << c->changing_lane.size() << endl;
                    if (c->changing_lane[i-1]) {
                        //this
                       // fac.add_ctr(cos(atan(scn->roadNet->get_lanediff_width(c->trajectories[i]->getRoad(), c->trajectories[i - 1]->getLane(), c->trajectories[i]->getLane()) / (c->x[i].second - c->x[i - 1].second))) * (c->v[i].second - c->v[i - 1].second) = (c->v[i].second - c->v[i - 1].second));
                        //fac.add_ctr(cos(atan(scn->roadNet->get_lanediff_width(c->trajectories[i]->getRoad(), c->trajectories[i - 1]->getLane(), c->trajectories[i]->getLane()) / (c->x[i].second - c->x[i - 1].second))) * (c->v[i].second - c->v[i - 1].second) = (c->v[i].second - c->v[i - 1].second));
                        fac.add_ctr(c->v[i - 1].second  =  ((c->x[i].second - c->x[i - 1].second) / (c->t[i].second - c->t[i - 1].second)) * cos(atan(scn->roadNet->get_lanediff_width(c->trajectories[i]->getRoad(), c->trajectories[i - 1]->getLane(), c->trajectories[i]->getLane()) / (c->x[i].second - c->x[i - 1].second))));
                        //*cos(atan(scn->roadNet->get_lanediff_width(c->trajectories[i]->getRoad(), c->trajectories[i - 1]->getLane(), c->trajectories[i]->getLane()) / (c->x[i].second - c->x[i - 1].second)))
                            
                    }
                    fac.add_ctr(c->x[i].second - c->x[i - 1].second = 0.5 * (c->v[i - 1].second + c->v[i].second) * (c->t[i].second - c->t[i - 1].second));
                    if (c->trajectories[i]->hasAcc) {
                        //cout << "car " << c->getId() << " has acceleration!" << endl;
                        fac.add_ctr(c->trajectories[i]->a * (c->t[i].second - c->t[i - 1].second) = (c->v[i].second - c->v[i - 1].second));
                    }

                }
            }
        }
        void addCarVars(SystemFactory& fac, Car* c) {
            
            for (int i = 0; i < c->t.size(); i++) {
                fac.add_var(c->t[i].second);
                fac.add_var(c->x[i].second);
                fac.add_var(c->v[i].second);
            }

            return;
        }
        string getSolution(ScenarioSolve* slv) {
            string fin = "";
            //cout << "write the solutions" << endl;
            slv->ords = slv->sys->var_names();
            Car* c;
            float angle = 0;
            for (int i = 0; i < slv->solutions.size(); i++) {
                string s = "{";
                int idx = 0;
                int k = 0;
                s += "\""  "roadNetwork"  "\": ";
                s += to_string(slv->scn->roadNet->id) + ", ";
                s += "\""  "duration"  "\": ";
                s += to_string(int(scn->duration));
                s += ", \""  "cars"  "\": [";

                vector<string>::reverse_iterator it;
                map<string, Car*>::reverse_iterator it1;
                for (it1 = scn->cars.rbegin(); it1 != scn->cars.rend(); it1++) {
                    //cout << it1->first << endl;
                    c = slv->scn->cars[it1->first];
                    s += "(";
                    s += "\"" + c->getId() + "\"";
                    s += ", ";
                    s += to_string(c->width);
                    s += ", ";
                    s += to_string(c->length);
                    s += ", ";
                    s += to_string(c->trajectories[0]->road);
                    s += ", ";
                    s += to_string(c->trajectories[0]->lane);
                    s += ", [";
                    for (int kaux = 0; kaux < c->t.size(); kaux++) {
                        //cout << "index: " << kaux << " of " << c->t.size() << endl;
                        angle = 0;
                        if (c->changing_lane[kaux] and kaux<c->t.size()-1) {
                            //angle = slv->getValue(c->angles[0].first, k);
                            angle = atan((scn->roadNet->get_lanediff_width(c->get_road(kaux), c->get_lane(kaux), c->get_lane(kaux + 1)))/ (slv->getValue(c->x[kaux + 1].first, k) - slv->getValue(c->x[kaux].first, k))) * 180 / 3.1415;
                        }
                        //cout << "ajajajajaj" << endl;
                        s += "(" + to_string(slv->getValue(c->t[kaux].first, k)) + ", " + to_string(slv->getValue(c->x[kaux].first, k)) + ", " + to_string(slv->getValue(c->v[kaux].first, k) * 3.6) + "," + to_string(c->trajectories[kaux]->road) + ","+ to_string(c->trajectories[kaux]->lane)+"," + to_string(angle)+")";
                        if (kaux != c->t.size() - 1)
                            s += ", ";
                    }
                    s += "])";
                    //cout << s << endl;
                    idx++;
                    if (idx != scn->cars.size())
                        s += ", ";
                }

                s += "]";
                //cout << "Collision time variable: " << slv->coll_vars << endl;
                //cout << "Time of collision: " << to_string(slv->getValue(slv->coll_vars, i)) << endl;
                //cout << "Number of variables: " << slv->ords.size() << endl;
                s += ", \"time\": " + to_string(slv->getValue(slv->coll_vars, i));
                fin += s + "}\n";
            }
            return fin;
        }

        bool copyFile(string SRC, string DEST)
        {
            std::ifstream src(SRC, std::ios::binary);
            std::ofstream dest(DEST, std::ios::binary);
            dest << src.rdbuf();
            return src && dest;
        }

        void save_use_case(string system, string solution, ScenarioSolve* slv) {
            string dir = "C:\\Users\\ASUS\\Documents\\Use cases\\";
            ofstream myfile;
            //system
            myfile.open(dir + "system.txt");
            myfile << system;
            myfile.close();
            //solution
            myfile.open(dir + "solution.txt");
            myfile << solution;
            myfile.close();
            //roadmap_file - transfer
            copyFile("C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\roadNetworks\\roadNetwork_"+ to_string(slv->scn->roadNet->id) + "withIntersections.json", "C:\\Users\\bermar3h\\Documents\\Use cases\\roadNetwork_"+ to_string(slv->scn->roadNet->id) +"withIntersections.json");
            //sample_json - transfer
            copyFile("C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\sample.json", "C:\\Users\\bermar3h\\Documents\\Use cases\\sample.json");

        }

        void saveSolutions(ScenarioSolve* slv) {

            ofstream myfile;
            string s;
            myfile.open("solutions.txt");
            s = getSolution(slv);
            myfile << s;
            myfile.close();
            string sys;
            std::stringstream buffer;
            buffer << *slv->sys << std::endl;
            sys = buffer.str();
            save_use_case(sys, s, slv);

        }
    };

    // queue elements are vectors so we need to compare those
    bool comparator(Trajectory* a, Trajectory* b)
    {
        return a->getT0() <= b->getT0() ;
    }

    vector<vector<Trajectory*>> processTrajectories(Json::Value car, Json::Value trajsV, Json::Value uniTrajsV) {
        vector<Trajectory*> trajs;
        std::vector<Trajectory*> uniTrajs;
        vector<vector<Trajectory*>> aux;
        int c = 0;
        int lane = -1;
        for (Json::Value::ArrayIndex i = 0; i != trajsV.size()-1; i++) {
            cout << "lane: " << lane << endl;
            if (lane == -1)
                lane = car["trajectories"][i]["lane"].asInt();

            if (lane != car["trajectories"][i+1]["lane"].asInt()) {
            }

            float t0_0 = car["trajectories"][i]["t0"].asFloat();
            float t0_1 = car["trajectories"][i]["t1"].asFloat();
            float t1_0 = car["trajectories"][i + 1]["t0"].asFloat();
            float t1_1 = car["trajectories"][i + 1]["t1"].asFloat();

            Trajectory* traj;
            
            if(car["trajectories"][i].isMember("a"))
                traj = new Trajectory(car["trajectories"][i]["id"].asInt(), car["trajectories"][i]["v0"].asFloat(), car["trajectories"][i]["v1"].asFloat(), car["trajectories"][i]["p0"].asFloat(),
                    car["trajectories"][i]["p1"].asFloat(), car["trajectories"][i]["t0"].asFloat(), car["trajectories"][i]["t1"].asFloat(), car["trajectories"][i]["a"].asFloat(), car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, true);
            else
                traj = new Trajectory(car["trajectories"][i]["id"].asInt(), car["trajectories"][i]["v0"].asFloat(), car["trajectories"][i]["v1"].asFloat(), car["trajectories"][i]["p0"].asFloat(),
                    car["trajectories"][i]["p1"].asFloat(), car["trajectories"][i]["t0"].asFloat(), car["trajectories"][i]["t1"].asFloat(), car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, false);

            trajs.push_back(traj);
            for (Json::Value::ArrayIndex j = 0; j < uniTrajsV.size(); j++) {
                if ((car["universal"][j]["t0"].asFloat() == 0 and car["universal"][j]["t1"].asFloat() == 0 and car["universal"][j]["v0"].asFloat() == 0 and car["universal"][j]["v1"].asFloat() == 0))
                    break;

                float uni_t0 = car["universal"][j]["t0"].asFloat();
                float uni_t1 = car["universal"][j]["t1"].asFloat();

                if (uni_t0 > t0_1 and uni_t0 < t1_0) {
                    if (car["trajectories"][i].isMember("a"))
                        trajs.push_back(new Trajectory(c, car["universal"][j]["v0"].asFloat(), car["universal"][j]["v1"].asFloat(), car["trajectories"][i]["p1"].asFloat(),
                            car["trajectories"][i+1]["p0"].asFloat(), uni_t0, uni_t0, car["trajectories"][i]["a"].asFloat(),
                            car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, true));
                    else
                        trajs.push_back(new Trajectory(c, car["universal"][j]["v0"].asFloat(), car["universal"][j]["v1"].asFloat(), car["trajectories"][i]["p1"].asFloat(),
                            car["trajectories"][i + 1]["p0"].asFloat(), uni_t0, uni_t0,
                            car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, false));
                    c++;
                }

                if (uni_t1 > t0_1 and uni_t1 < t1_0) {
                    if (car["trajectories"][i].isMember("a"))
                        trajs.push_back(new Trajectory(c, car["universal"][j]["v0"].asFloat(), car["universal"][j]["v1"].asFloat(), car["trajectories"][i]["p1"].asFloat(),
                            car["trajectories"][i + 1]["p0"].asFloat(), uni_t1, uni_t1, car["trajectories"][i]["a"].asFloat(),
                            car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, true));
                    else
                        trajs.push_back(new Trajectory(c, car["universal"][j]["v0"].asFloat(), car["universal"][j]["v1"].asFloat(), car["trajectories"][i]["p1"].asFloat(),
                            car["trajectories"][i + 1]["p0"].asFloat(), uni_t1, uni_t1,
                            car["trajectories"][i]["road"].asFloat(), car["trajectories"][i]["lane"].asFloat(), false, false));
                    c++;
                }
            }

            if (i == trajsV.size() - 2) {
                if (car["trajectories"][i+1].isMember("a"))
                    trajs.push_back(new Trajectory(c, car["trajectories"][i + 1]["v0"].asFloat(), car["trajectories"][i + 1]["v1"].asFloat(), car["trajectories"][i + 1]["p0"].asFloat(),
                        car["trajectories"][i + 1]["p1"].asFloat(), car["trajectories"][i + 1]["t0"].asFloat(), car["trajectories"][i + 1]["t1"].asFloat(), car["trajectories"][i + 1]["a"].asFloat(),
                        car["trajectories"][i + 1]["road"].asFloat(), car["trajectories"][i + 1]["lane"].asFloat(), false, true));
                else
                    trajs.push_back(new Trajectory(c, car["trajectories"][i + 1]["v0"].asFloat(), car["trajectories"][i + 1]["v1"].asFloat(), car["trajectories"][i + 1]["p0"].asFloat(),
                        car["trajectories"][i + 1]["p1"].asFloat(), car["trajectories"][i + 1]["t0"].asFloat(), car["trajectories"][i + 1]["t1"].asFloat(),
                        car["trajectories"][i + 1]["road"].asFloat(), car["trajectories"][i + 1]["lane"].asFloat(), false, false));
            }
        }

        sort(trajs.begin(), trajs.end(), comparator);
        for (Trajectory* t : trajs) {
            cout << car["id"].asString() << " ===> " << "p0: " << t->getP0() << ", p1: " << t->getP1() << ", v0: " << t->getV0() << ", v1: " << t->getV1() << ", t0: " << t->getT0() << ", t1: " << t->getT1()  << "hasAcc:" << t->hasAcc << endl;
        }
        cout << "-------------------------" << endl;
        for (Json::Value::ArrayIndex j = 0; j < uniTrajsV.size(); j++) {
            if ((car["universal"][j]["t0"].asFloat() == 0 and car["universal"][j]["t1"].asFloat() == 0 and car["universal"][j]["v0"].asFloat() == 0 and car["universal"][j]["v1"].asFloat() == 0))
                break;
            uniTrajs.push_back(new Trajectory(car["universal"][j]["id"].asInt(), car["universal"][j]["v0"].asFloat(), car["universal"][j]["v1"].asFloat(), car["universal"][j]["t0"].asFloat(), car["universal"][j]["t1"].asFloat(), true));
        }

        aux = { trajs, uniTrajs };
        return aux;

    }

    Scenario* processJson(string filename) {
        vector<Problem*> problems;
        bool alive = true;
        Json::Value root;
        Json::Reader reader;
        std::ifstream test(filename, std::ifstream::binary);
        bool parsingSuccessful = reader.parse(test, root, false);
        if (!parsingSuccessful) {
            //cout << "error! " << endl;
            //cout << filename << endl;
        }
        std::map<string, Car*> cars;
        //cout << root["cars"].size() << " is the size of all " << endl;
        for (Json::Value::ArrayIndex i = 0; i != root["cars"].size(); i++) {
            Json::Value car = root["cars"][i];
            std::vector<Trajectory*> trajs;
            std::vector<Trajectory*> uniTrajs;
            float a;
            vector<vector<Trajectory*>> aux = processTrajectories(car, car["trajectories"], car["universal"]);
            trajs = aux[0];
            uniTrajs = aux[1];

            cars.insert(make_pair(car["id"].asString(), new Car(car["id"].asString(), car["width"].asFloat(), car["length"].asFloat(), aux[0], aux[1])));
        }
        std::vector<Goal*> goals;
        for (Json::Value::ArrayIndex i = 0; i != root["goal"]["cars"].size(); i++) {
            std::vector<string> cars_ids;
            for (Json::Value::ArrayIndex j = 0; j != root["goal"]["cars"][i].size(); j++)
                cars_ids.push_back(root["goal"]["cars"][i][j].asString());
            goals.push_back(new Goal(root["goal"]["collides"].asBool(), cars_ids));
        }
        //read the road network
        createIntersections(root["networkID"].asInt());
        Json::Value root_alt;
        std::ifstream test1(FILE_PATH + root["networkID"].asString() + "withIntersections" + EXT, std::ifstream::binary);
        parsingSuccessful = reader.parse(test1, root_alt, false);
        if (!parsingSuccessful) { //cout << "error yeeeah!" << endl; 
        }

        std::vector<Road*> roads;
        for (Json::Value::ArrayIndex i = 0; i != root_alt["roads"].size(); i++) {
            //cout << root_alt["roads"][i] << endl;
            //cout << "i: " << i << " direction: " << root_alt["roads"][i]["direction"].asBool() << endl;
            roads.push_back(new Road(root_alt["roads"][i]["id"].asInt(), root_alt["roads"][i]["direction"].asBool(),
            root_alt["roads"][i]["h_axis0"].asFloat(), root_alt["roads"][i]["h_axis1"].asFloat(), root_alt["roads"][i]["v_axis0"].asFloat(), root_alt["roads"][i]["v_axis1"].asFloat(),
            root_alt["roads"][i]["n_lanes"].asInt()));
            //(int id, bool dir, float h_axis0, float h_axis1, float v_axis0, float v_axis1, int n_lanes)
        }
        std::vector<Intersection*> intersections;
        for (Json::Value::ArrayIndex i = 0; i != root_alt["intersections"].size(); i++) {
            intersections.push_back(new Intersection(root_alt["intersections"][i]["id"].asInt(), root_alt["intersections"][i]["road0"].asInt(), root_alt["intersections"][i]["lane0"].asInt(), root_alt["intersections"][i]["road1"].asInt(), root_alt["intersections"][i]["lane1"].asInt(), root_alt["intersections"][i]["x0"].asFloat(),
                root_alt["intersections"][i]["x1"].asFloat(), root_alt["intersections"][i]["y0"].asFloat(), root_alt["intersections"][i]["y1"].asFloat()));
        }

        return
            new Scenario(root["id"].asString(), root["duration"].asFloat(), cars, goals, new RoadNetwork(root["networkID"].asInt(), intersections,
                roads, root_alt["h_axis"].asFloat(), root_alt["v_axis"].asFloat()));
    }

    void print_matrix(vector<vector<int>> matrix) {
        /*cout << "--------------" << endl;
        for (vector<int> v : matrix) {
            for (int t : v)
                cout << t << " ";
            cout << endl;
        }
        cout << "--------------" << endl;*/
    }

    void run_all() {

        std::string path = "C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\test_generation\\simple_int_inc_cars_2_movements\\";
        for (int j = 0; j < 10; j++) {
            for (int i = 0; i < 5; i++) {
                try {
                    std::cout << "--------------------------- EXAMPLE" + to_string(j) + "_" + to_string(i) + ".json --------------------------------" << std::endl;
                    Scenario* scn = processJson("C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\test_generation\\simple_int_inc_cars_2_movements\\example" + to_string(j) + "_" + to_string(i) + ".json");

                    auto begin_building = std::chrono::high_resolution_clock::now();
                    Problem* p = new Problem(scn);
                    p->compile_movements();
                    p->compile_chasings_everything();
                    p->compile_crossings_everything();
                    auto end_building = std::chrono::high_resolution_clock::now();

                    bool found = false;
                    ScenarioSolve* front;
                    //cout << p->s_aux.size() << " is the number of final systems" << endl;

                    auto start_solving = std::chrono::high_resolution_clock::now();
                    auto solv_m_t_i = std::chrono::high_resolution_clock::now();
                    auto solv_m_t_f = std::chrono::high_resolution_clock::now();
                    std::queue<ScenarioSolve*> queue(std::deque<ScenarioSolve*>(p->s_aux.begin(),
                        p->s_aux.end()));
                    int invalid_models = 0;
                    int n_models = queue.size();

                    while (!queue.empty())
                    {
                        front = queue.front();
                        //cout << "-------------------------------------------- Scenario " << front->id << " being evaluated --------------------------------------------" << endl;
                        if (!front->check_feasibility() or front->coll_vars == "") {
                            //cout << "Not feasible!" << endl;
                            queue.pop();
                            invalid_models++;
                            continue;
                        }
                        solv_m_t_i = std::chrono::high_resolution_clock::now();
                        front->run();
                        front->load_moves(p->systems_aux, p->get_independent_cars());
                        front->showAll();
                        if (front->solutions.size() == 0 or front->solutions[0].is_empty() or front->timesRan > limitRun) {
                            queue.pop();
                            continue;
                        }
                        else if (!front->valid) {
                            //cout << "Recycle this one!" << endl;
                            queue.push(front);
                        }
                        else {
                            //cout << "Solution found, now go and save it!!" << endl;
                            p->saveSolutions(front);
                            found = true;
                            solv_m_t_f = std::chrono::high_resolution_clock::now();
                            break;
                        }

                        queue.pop();
                    }

                    auto finish_solving = std::chrono::high_resolution_clock::now();
                    duration<double> building_d = duration_cast<duration<double>>(end_building - begin_building);
                    duration<double> resolution_d = duration_cast<duration<double>>(finish_solving - start_solving);
                    /*cout << " ---------------------------- duration of the problem solving ---------------------------- " << endl;
                    cout << "building time: " << building_d.count() << endl;
                    cout << "solving time: " << resolution_d.count() << endl;
                    cout << "found a solution: " << found << endl;
                    if (found) {
                        cout << "--- Properties of the found system ---" << endl;
                        cout << "Times to run: " << front->timesRan << endl;
                        cout << "Ran out of time: " << front->ranOut << endl;
                        cout << "Time allowed to run: " << front->time << endl;
                    }*/
                    bool f = found;
                    //float b_time = building_d;
                    //float solving_time = resolution_d;
                    int n_cars = p->scn->cars.size();
                    int n_movements = p->scn->cars[0]->trajectories.size();
                    int n_variables = front->ords.size();
                    cout << "solution found: " << f << endl;
                    cout << "n_models: " << n_models << endl;
                    cout << "n_cars: " << n_cars << endl;
                    cout << "n_vars: " << n_variables << endl;
                    cout << "n_movements: " << endl;
                    cout << "eps: " << boxEps << endl;
                    cout << "numberSolutions: " << numberSolutions << endl;
                    cout << "limitRun: " << limitRun << endl;
                    cout << "limitTime: " << limitTime << endl;
                    cout << "building time: " << building_d.count() << endl;
cout << "timesRan: " << front->timesRan << endl;
cout << "invalid models: " << invalid_models << endl;
duration<double> f_t_s = duration_cast<duration<double>>(solv_m_t_f - solv_m_t_i);
cout << "solved model time to solve: " << f_t_s.count() << endl;
cout << "total solving time: " << resolution_d.count() << endl;

//invalid_models
//a solution was found or not
//number of times the solution model ran
//number of cars in the scenario
//number of movements
//number of variables

/* \textbf{n\char`_models\char`_gen} as the number of models generated by the ".dsl" file. yes
   \textbf{t\char`_build} as the time needed to build the models. yes
   \textbf{n\char`_inv\char`_models} as the number of invalid models that were generated.
   \textbf{n\char`_solutions} as the number of solutions found.
   \textbf{n\char`_times\char`_ran} as the number of times the solution model ran.
   \textbf{n\char`_cars} as the number of cars in the scenario.
   \textbf{n\char`_movements} as the number of movements for each car.
*/
                }
                catch (...) {
                    cout << "no results!" << endl;
                }
            }
        }

    }

    void save(string content, string path, int i, int j) {
        ofstream myfile;
        myfile.open(path + "example" + to_string(i) + "_" + to_string(j) + "_results.txt");
        myfile << content;
        myfile.close();
    }

    void showSolutions(vector<IntervalVector> iv) {
        IntervalVector box;
        cout << iv.size() << " solutions:" << endl;
        for (int i = 0; i < iv.size(); i++) {
            box = iv[i];
            cout << "   sol " << i << " -> " << box << " size: " << box.max_diam() << endl;
        }
    }

    bool equal_solutions(vector<IntervalVector> iv) {
        IntervalVector box;
        box = iv[0];
        for (int i = 0; i < iv.size(); i++) {
            for(int j = i+1 ; j<iv.size(); j++ )
            if (iv[i] == iv[j])
                return true;
        }
        return false;
    }

    bool solution_exists(IntervalVector v, vector<IntervalVector> iv) {
        if (iv.size() == 0)
            return false;
        for (int i = 0; i < iv.size(); i++) {
            if (iv[i] == v)
                return true;
        }
        return false;
    }

    void run_example(int i, int j) {
        /*cout << "eps: " << boxEps << endl;
        cout << "numberSolutions: " << numberSolutions << endl;
        cout << "limitRun: " << limitRun << endl;
        cout << "limitTime: " << limitTime << endl;*/

        std::string path = "C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\test_generation\\general_intersections\\";

        std::cout << "--------------------------- EXAMPLE" + to_string(i) + "_" + to_string(j) + ".json --------------------------------" << std::endl;
        
        Scenario* scn = processJson("C:\\MinGW\\msys\\1.0\\home\\ASUS\\Ibex\\ibex-2.8.9\\examples\\ConsoleApplication1\\ConsoleApplication1\\test_generation\\general_intersections\\example" + to_string(i) + "_" + to_string(j) + ".json");

        auto begin_building = std::chrono::high_resolution_clock::now();
        Problem* p = new Problem(scn);
        vector<IntervalVector> solution_set;
        solution_set.reserve(numberSolutions*2);
        cout << "start building!" << endl;
        p->compile_movements();
        cout << "start chasings" << endl;
        p->compile_chasings_everything();
        cout << "start crossings" << endl;
        p->compile_crossings_everything();
        auto end_building = std::chrono::high_resolution_clock::now();
        cout << "finished building!" << endl;
        bool found = false;
        ScenarioSolve* front;
        int n_solutions = 0;
        cout << p->s_aux.size() << " is the number of final systems" << endl;
        int n_models_processed = 0;
        auto start_solving = std::chrono::high_resolution_clock::now();
        auto solv_m_t_i = std::chrono::high_resolution_clock::now();
        auto solv_m_t_f = std::chrono::high_resolution_clock::now();
        std::queue<ScenarioSolve*> queue(std::deque<ScenarioSolve*>(p->s_aux.begin(),
            p->s_aux.end()));
        int invalid_models = 0;
        int n_models = queue.size();
        
        while (!queue.empty() && n_solutions != numberSolutions)
        {
            if (n_solutions >= numberSolutions)
                break;
            front = queue.front();
            cout << "-------------------------------------------- Scenario " << front->id << " being evaluated --------------------------------------------" << endl;
            cout << "times ran: " << front->timesRan << endl;
            n_models_processed=n_models_processed+1;
            cout << *front->sys << endl;
            if (!front->check_feasibility() or front->coll_vars == "") {
                queue.pop();
                invalid_models++;
                continue;
            }
            solv_m_t_i = std::chrono::high_resolution_clock::now();
            front->run();
            front->load_moves(p->systems_aux, p->get_independent_cars());
            //front->showAll();
            if ((front->solutions.size() > 0 and front->solutions[0].is_empty()) or front->timesRan >= limitRun) {
                cout << "pop it!" << endl;
                queue.pop();
                continue;
            }
            /*else if (!front->eps_valid())) {
                cout << "invalid!" << endl;
                queue.push(front);
            }*/
            else if (front->solutions.size() > 0) {
                cout << "Solution found, now go and save it!!" << endl;
                
                cout << "Number of solutions found: " << front->solutions.size();
                cout << "Total of " << front->solutions.size() << " found, " << numberSolutions - n_solutions << " to go!" << endl;
                p->saveSolutions(front);
                front->showAll();
                for (int i = 0; i < front->solutions.size(); i++) {
                    if (!solution_exists(front->solutions[i], solution_set)) {
                        solution_set.push_back(front->solutions[i]);
                        //n_solutions += front->solutions.size();
                        n_solutions += 1;
                    }
                }
                found = true;
                solv_m_t_f = std::chrono::high_resolution_clock::now();
            }
            
            if (front->last_eps > front->eps or n_solutions != numberSolutions) {
                cout << "Review!" << endl;
                queue.push(front);
            }
            //queue.pop();
            cout << "number of models to process: " << queue.size() << endl;
        }
        auto finish_solving = std::chrono::high_resolution_clock::now();
        duration<double> building_d = duration_cast<duration<double>>(end_building - begin_building);
        duration<double> resolution_d = duration_cast<duration<double>>(finish_solving - start_solving);
        /*cout << " ---------------------------- duration of the problem solving ---------------------------- " << endl;
        cout << "building time: " << building_d.count() << endl;
        cout << "solving time: " << resolution_d.count() << endl;
        cout << "found a solution: " << found << endl;
        if (found) {
            cout << "--- Properties of the found system ---" << endl;
            cout << "Times to run: " << front->timesRan << endl;
            cout << "Ran out of time: " << front->ranOut << endl;
            cout << "Time allowed to run: " << front->time << endl;
        }*/
        int n_cars = p->scn->cars.size();
        int n_movements = 2;//p->scn->cars[0]->trajectories.size();
        int n_variables = front->ords.size();
        cout << "total solutions: " << n_solutions << endl;
        cout << "solution found: " << to_string(found) << endl;
        /*cout << "n_models: " << to_string(n_models) << endl;
        cout << "n_cars: " << to_string(n_cars) << endl;
        cout << "n_vars: " << to_string(n_variables) << endl;
        cout << "n_movements: " << to_string(n_movements) << endl;
        cout << "eps: " << to_string(boxEps) << endl;
        cout << "numberSolutions: " << to_string(numberSolutions) << endl;
        cout << "limitRun: " << limitRun << endl;
        cout << "limitTime: " << limitTime << endl;*/
        cout << "building: " << to_string(building_d.count()) << endl;
        //cout << "timesRan: " << to_string(front->timesRan) << endl;
        //cout << "invalid models: " << to_string(invalid_models) << endl;
        duration<double> f_t_s = duration_cast<duration<double>>(solv_m_t_f - solv_m_t_i);
        //cout << "solved model time to solve: " << f_t_s.count() << endl;*/
        cout << "total solving time: " << resolution_d.count() << endl;
        //cout << "n_models_processed: " << to_string(n_models_processed) << endl;
        
        float t_time = building_d.count() + resolution_d.count();
        cout << "TOTAL TIMEEEEEEEEEEEE: " << t_time << endl;
        string content = "solution found: " + to_string(found) + "\n";
        /*content += "n_models: " + to_string(n_models) + "\n";
        content += "n_cars: " + to_string(n_cars) + "\n";
        content += "n_vars: " + to_string(n_variables) + "\n";
        content += "n_movements: " + to_string(n_movements) + "\n";
        content += "eps: " + to_string(boxEps) + "\n";
        */

        //content += "numberSolutions: " + to_string(numberSolutions) + "\n";
        //content += "limitRun: " + to_string(limitRun) + "\n";
        //content += "limitTime: " + to_string(limitTime) + "\n";
        content += "building time: " + to_string(building_d.count()) + "\n";
        //content += "timesRan: " + to_string(front->timesRan) + "\n";

        //content += "invalid models: " + to_string(invalid_models) + "\n";
        float p_duration = f_t_s.count();
        //content += "solved model time to solve: " + to_string(p_duration) + "\n";
        content += "total solving time: " + to_string(resolution_d.count()) + "\n";
        content += "TOTAL TIMEEEEEEEE: " + to_string(t_time) + "\n";
        showSolutions(solution_set);
        //if(found)
        save(content, path, i, j);
        
        //invalid_models
        //a solution was found or not
        //number of times the solution model ran
        //number of cars in the scenario
        //number of movements
        //number of variables

        /* 
        \textbf{n\char`_models\char`_gen} as the number of models generated by the ".dsl" file. yes
        \textbf{t\char`_build} as the time needed to build the models. yes
        \textbf{n\char`_inv\char`_models} as the number of invalid models that were generated.
        \textbf{n\char`_solutions} as the number of solutions found.
        \textbf{n\char`_times\char`_ran} as the number of times the solution model ran.
        \textbf{n\char`_cars} as the number of cars in the scenario.
        \textbf{n\char`_movements} as the number of movements for each car.
        */
        
    }

    void runalt() {
        System sys("test.txt");

        cout << sys << endl;

        DefaultSolver solver(sys,0.1);  
        //DefaultSolver solver(sys,0.01);
        solver.solve(sys.box);
        cout << "solved!" << endl;
        cout << solver.get_data() << endl;
        cout << "---------------------- data presented above ------------------------- " << endl;
    }

    void run() {

        string filename = "mainexample.txt";
        Scenarioo scn(filename);

        cout << "***************** " << scn.fileName << " *****************" << endl;
        cout << *scn.sys << endl;

        cout << "**************** Domains *****************" << endl;
        scn.showVariables();

        cout << "*************** Parameters ***************" << endl;
        // if needed redefine parameters eps and nBoxes:
        scn.setEps(0.0001);
        cout << "   eps: " << scn.eps << endl;			// default 1e-012
        scn.setNBoxes(1);
        cout << "   nBoxes: " << scn.nBoxes << endl;	// default 100

        cout << "**************** Solving ****************" << endl;
        scn.solve();
        cout << "*****************************************" << endl;
        scn.showPartition();
        scn.showSolutions();
        //scn.showAll();
        
    }
    
    int main(int argc, char** argv) {

        if (argc > 1 and argv[1] != "")
            boxEps = stof(argv[1]);

        if (argc > 2  and argv[2] != "")
            numberSolutions = atoi(argv[2]);

        if (argc > 3 and argv[3] != "")
            limitRun = atoi(argv[3]);

        if (argc > 4 and argv[4] != "")
            limitTime = stof(argv[4]);   

        //ConsoleApplication1 0.01 1 3 7

        //run_all();

        /*for (int i = 1; i <= 5; i++) {
            for (int j = 2; j <= 6; j++) {
                run_example(i ,j);

            }
        }*/

        run();
        return 0;

    }