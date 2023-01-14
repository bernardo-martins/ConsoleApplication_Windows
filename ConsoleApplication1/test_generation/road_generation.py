# -*- coding: utf-8 -*-
"""
Created on Wed Jul 13 13:56:13 2022

@author: bermar3h
"""

import random
import json

def generate(identifier, n_roads_h, n_roads_v):
    h_axis = 100
    v_axis = 100
    
    road_lengths_hypo = [4, 6, 6, 9, 12]
    n_lanes_hypo = [2, 3, 2, 3, 4]
    combo_idx = random.randrange(len(road_lengths_hypo))
    
    # possible intersections must grow with the increase of the number of roads
    n_horizontal = n_roads_h #random.randint(2,5)
    n_vertical = n_roads_v #random.randint(2,5)
    n_roads = n_horizontal + n_vertical
    n_intersections = n_roads*n_roads
    
    roads = []
    ident = 0
    l_vnumber = 0
    for i_h in range(n_vertical):
        road = {}
        road["id"] = ident 
        road["direction"] = random.randint(0,1)
        road["v_axis0"] = 0
        road["v_axis1"] = 100
        road["h_axis0"] = random.uniform(l_vnumber, l_vnumber + 10) + random.uniform(5, 10) 
        road["h_axis1"] = road["h_axis0"] + road_lengths_hypo[combo_idx]
        road["n_lanes"] = n_lanes_hypo[combo_idx]
        l_vnumber = road["h_axis1"]
        ident = ident + 1
        roads.append(road)
    
    l_hnumber = 0
    for i_h in range(n_horizontal):
        road = {}
        road["id"] = ident
        road["direction"] = random.randint(0,1)
        road["h_axis0"] = 0
        road["h_axis1"] = 100
        road["v_axis0"] = random.uniform(l_hnumber, l_hnumber + 10) + random.uniform(5, 10) 
        road["v_axis1"] = road["v_axis0"] + road_lengths_hypo[combo_idx]
        road["n_lanes"] = n_lanes_hypo[combo_idx]
        l_hnumber = road["v_axis1"]
        ident = ident + 1
        roads.append(road)
    
    road_map = {}
    road_map["id"] = identifier
    road_map["h_axis"] = h_axis
    road_map["v_axis"] = v_axis
    road_map["intersections"] = []
    road_map["roads"] = roads
    
    json_object = json.dumps(road_map, indent = 4)
    with open("./testing_roads/road"+str(i)+".json", "w") as outfile:
        outfile.write(json_object)
        print(identifier)
        outfile.close()
    return road_map, n_roads, n_intersections

roads = [(2,1),(2,3),(3,4),(4,5),(5,5)]

for i in range(5):
    road_map, n_roads, n_intersections = generate(i, roads[i][0], roads[i][1])

print("done!")

    
