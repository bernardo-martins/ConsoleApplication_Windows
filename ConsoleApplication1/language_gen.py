# -*- coding: utf-8 -*-
"""
Created on Tue Jul  5 17:38:45 2022

@author: bermar3h
"""

from lark import Lark, Transformer, v_args
import json
import os
import random
from random import seed
from random import gauss

data = {}
cars  = {}
car_ids = []
uni_trajs = {}
exi_trajs = {}
roads = {}
lanes = {}
goal = {}
accs = {}

exp_grammar = """
    start : "scenario(" num "):" r_spec cs_spec moves* goal_spec end
    cs_spec: c_spec+
    c_spec: variable ":car(" width "," length")"
    r_spec: "roadNet(" id_road ")"
    moves: variable ".drive_with:" move*
    move: change_lane | fix_acc | start_lane | universal_inst | existencial_inst
    
    fix_acc: "fix_acceleration(" "ac" ":" num ")"
    instantiation: universal_inst | existencial_inst_acc | existencial_inst
    existencial_inst: "movement(" "pos" ":" interval "," "vel:" interval "," "time:" interval ")"
    existencial_inst_acc: "movement(" "pos" ":" interval "," "vel:" interval "," "time:" interval "," "ac:" num ")"
    universal_inst: "movement(" "vel" ":" interval "," "time:" interval ")"
    change_lane: "change_lane(" "from" ":" lane_from "," "to" ":" lane_to ")"
    start_lane: "start(" "road" ":" road "," "lane" ":" lane ")"
    goal_spec: "goal" ":" "collision(" id "," id ")"
    interval : "["num","num"]"
    end: "end"    
        
    variable: STRING
    num: NUMBER
    id_road: NUMBER
    id: STRING
    width: NUMBER
    length: NUMBER
    road:NUMBER
    lane:NUMBER
    lane_from: NUMBER
    lane_to: NUMBER
    
    OPERATOR: ("+"|"-"|"*"|"/")
    COMPARATOR: (">="|"<="|"=")
    STRING: /[a-zA-Z_]([a-zA-Z0-9_]*|$)/
    SPACE: " "
    
    %import common.NUMBER
    %import common.WS_INLINE
    %import common.WS
    %ignore /\s+/
    %ignore WS
    %ignore WS_INLINE
    %ignore SPACE
"""

exp_parser = Lark(exp_grammar, regex=True, ambiguity="explicit")
exp = exp_parser.parse

def run_instant(t):
    #print(t.data)
    if t.data == "num":
        #print("duration")
        number = t.children[0]
        data["duration"] = float(number)
        data["id"] = "scn1"
    elif t.data == "r_spec":
        #print("r_spec")
        roadId = t.children[0].children[0]
        data["networkID"] = int(roadId)
    elif t.data == "cs_spec":
        #print("cs_spec")
        for t_aux in t.children:
            run_instant(t_aux)
    elif t.data == "moves":
        exi = {}
        uni = {}
        #print("---------------------------")
        #print("name:", t.children[0].children[0])
        if(str(t.children[0].children[0]) not in car_ids):
            raise SyntaxError('Unknown variable: %s' % str(t.children[0].children[0]))
        for t_aux in t.children[1::]:
            if(t_aux.children[0].data == "start_lane"):
                roads[str(t.children[0].children[0])] = int(t_aux.children[0].children[0].children[0])
                lanes[str(t.children[0].children[0])] = int(t_aux.children[0].children[1].children[0])
            if(t_aux.children[0].data == "change_lane"):
                #print("prev:", lanes[str(t.children[0].children[0])])
                #print("change lane to: ", int(t_aux.children[0].children[1].children[0]))
                lanes[str(t.children[0].children[0])] = int(t_aux.children[0].children[1].children[0])
                #print("after:", lanes[str(t.children[0].children[0])])
            if(t_aux.children[0].data == "existencial_inst"):
                exi["road"] = int(roads[str(t.children[0].children[0])])
                exi["lane"] = int(lanes[str(t.children[0].children[0])])
                exi["p0"] = float(t_aux.children[0].children[0].children[0].children[0])
                exi["p1"] = float(t_aux.children[0].children[0].children[1].children[0])
                exi["t0"] = float(t_aux.children[0].children[2].children[0].children[0])
                exi["t1"] = float(t_aux.children[0].children[2].children[1].children[0])
                exi["v0"] = float(t_aux.children[0].children[1].children[0].children[0])
                exi["v1"] = float(t_aux.children[0].children[1].children[1].children[0])
                exi_trajs[t.children[0].children[0]].append(exi)
                #print(exi)
                exi = {}
            if(t_aux.children[0].data == "universal_inst"):
                uni["road"] = int(roads[str(t.children[0].children[0])])
                uni["lane"] = int(lanes[str(t.children[0].children[0])])
                uni["v0"] = float(t_aux.children[0].children[0].children[0].children[0])
                uni["v1"] = float(t_aux.children[0].children[0].children[1].children[0])
                uni["t0"] = float(t_aux.children[0].children[1].children[0].children[0])
                uni["t1"] = float(t_aux.children[0].children[1].children[1].children[0])
                uni_trajs[t.children[0].children[0]].append(uni)
                #print(uni)
                uni={}
            if(t_aux.children[0].data == "fix_acc"):
                
                #accs["car"] = float(t_aux.children[0].children[3].children[0])
                """exi ={}
                exi["road"] = int(roads[str(t.children[0].children[0])])
                exi["lane"] = int(lanes[str(t.children[0].children[0])])
                exi["p0"] = float(t_aux.children[0].children[0].children[0].children[0])
                exi["p1"] = float(t_aux.children[0].children[0].children[1].children[0])
                exi["t0"] = float(t_aux.children[0].children[2].children[0].children[0])
                exi["t1"] = float(t_aux.children[0].children[2].children[1].children[0])
                exi["v0"] = float(t_aux.children[0].children[1].children[0].children[0])
                exi["v1"] = float(t_aux.children[0].children[1].children[1].children[0])
                exi["a"] = float(t_aux.children[0].children[3].children[0])
                exi_trajs[t.children[0].children[0]].append(exi)"""
                #print("here is where we specify acceleration")
                #print(t.children[0].children[0])
                exi["a"] = float(t_aux.children[0].children[0].children[0])
                #print(t_aux.children[0].children[0].children[0])
                print("---------")
    elif t.data == "c_spec":
        car ={}
        #car["id"] = str(t.children[0].children[0])
        car["width"] = float(t.children[1].children[0])
        car["length"] = float(t.children[2].children[0])
        car_ids.append(str(t.children[0].children[0]))
        cars[str(t.children[0].children[0])] = car
        uni_trajs[str(t.children[0].children[0])] = []
        exi_trajs[str(t.children[0].children[0])] = []
    elif t.data == "goal_spec":
        #print(t.children[0].children[0])
        #print(t.children[1].children[0])
        """
        "goal": {
        "cars": [
            [
                "car2",
                "car1"
            ]
        ],
        "collides": 1
        }
        """
        goal["cars"] = [[str(t.children[0].children[0]),str(t.children[1].children[0])]]
        goal["collides"] = 1
    elif t.data == "end":
        print("end")
    else:
        raise SyntaxError('Unknown instruction: %s' % t.data)
    

def run(program):
    for inst in program.children:
        run_instant(inst)


def test(dir,file):
    
    f = open(dir+file+".dsl")
    print("------------------")
    print(dir+file+".dsl")
    contents = f.read()

    aux = exp(str(contents))
    run(aux)
    car_vec = []
    for i in range(len(car_ids)):
        car_json = {}
        car_json["id"] = car_ids[i]

        car_json["width"] = cars[car_ids[i]]["width"]
        car_json["length"] = cars[car_ids[i]]["length"]
        if(len(uni_trajs)==0):
            car_json["universal"] = None
        else:
            car_json["universal"] = uni_trajs[car_ids[i]]
        
        exi_trajs_aux =[]
        for j in range(len(exi_trajs[car_ids[i]])):
            exi_trajs[car_ids[i]][j]["id"] = j
            exi_trajs_aux.append(exi_trajs[car_ids[i]][j])
            
        car_json["trajectories"] = exi_trajs_aux

        car_vec.append(car_json)
    data["cars"] = car_vec
    data["goal"] = goal
    json_object = json.dumps(data, indent= 4)
    f = open(dir+file+".json", "w")
    f.write(json_object)
    print(json_object)
    f.close()


def random():
    for _ in range(10):
        value = gauss(0, 10)
        print(value)

def generate_vars():

    v1 = 0.01
    v2 = 0.02
    v3 = 0.03
    v4 = 0.04

    err1,err2,err3,err4 = 0.03

    lb1 = v1-err1
    ub1 = v1+err1
    lb2 = v2 - err2
    ub2 = v2 + err2
    lb3 = v3 - err3
    ub3 = v3 + err3
    lb4 = v4 - err4
    ub4 = v4 + err4

    print("V1 in [",lb1,",",ub1,"]")
    print("V2 in [", lb2, ",", ub2, "]")
    print("V3 in [", lb3, ",", ub3, "]")
    print("V4 in [", lb4, ",", ub4, "]")


if __name__ == '__main__':
    file = "C:/MinGW/msys/1.0/home/ASUS/Ibex/ibex-2.8.9/examples/ConsoleApplication1/ConsoleApplication1/example.dsl"
    dir = "C:/MinGW/msys/1.0/home/ASUS/Ibex/ibex-2.8.9/examples/ConsoleApplication1/ConsoleApplication1/test_generation/general_intersections/"
    file_spec = "C:/MinGW/msys/1.0/home/ASUS/Ibex/ibex-2.8.9/examples/ConsoleApplication1/ConsoleApplication1/test_generation/general_chasings/example0_0.dsl"
    for filename in os.listdir(dir):
        data = {}
        cars = {}
        car_ids = []
        uni_trajs = {}
        exi_trajs = {}
        roads = {}
        lanes = {}
        goal = {}
        accs = {}
        if("json" in filename or "results" in filename):
            continue
        print(filename[:-4])
        test(dir,filename[:-4])

        #[50,100] -> [60,90] -> [70,80]

        """scenario(10):

roadNet(3)
car0:car(2.1,4)
car1:car(2.2,4.9)

car0.drive_with:
start(road:1,lane:1)
movement(pos:[0,0], vel:[30,50.0], time:[0,0])
movement(pos:[100,130], vel:[55,85], time:[9,10])

car1.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[0,200], vel:[64,120], time:[4,6])
movement(pos:[200,200], vel:[55,85], time:[12,13])


goal:collision(car0,car1)end"""