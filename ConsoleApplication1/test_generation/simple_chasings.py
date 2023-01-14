# -*- coding: utf-8 -*-
"""
Created on Tue Jul 12 13:48:42 2022

@author: bermar3h
"""

import random
import json
#next step: generate the roads and ally this to the roads

n_trajectories = 4

def write(dir,scenario_prop, cars_constructs, car_movements, goals, index, road_i):
    
    scenario = """scenario("""
    scenario += str(scenario_prop["duration"]) + "):\n"
    scenario += "\n" 
    scenario += "roadNet(" + str(scenario_prop["roadNet"]) + ")\n"
    
    for car_construct in cars_constructs:
        scenario += car_construct + "\n"
        
    scenario += "\n"
    
    for key, val in car_movements.items():
        
        #print(key, val)
        scenario += key+".drive_with:\n"
        
        for move in val:
            scenario += move + "\n"
        
        scenario += "\n"

    scenario += "\n"
    scenario += "collision(car"+str(goals[0]) + ",car" + str(goals[1]) +")\n\n"
    scenario += "end"
    
    #f = open("example_chasing_"+str(index)+".dsl", "w")
    f = open("./"+dir+"/example"+str(index)+"_"+str(road_i)+".dsl", "w")
    f.write(scenario)
    f.close()

    return
    
def simple_chase_inc_general_n_movements(dir, index, road_i, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    # n_roads = 4 #n_roads - goal is to put each car in each road available
    # n_lanes = 2 #n_lanes - put each car in each lane of each road
    n_cars_per_chasing = random.randint(2, index+2)
    n_chasings = random.randint(2, n_roads)
    n_trajectories = 2
    print("-----------------------------------------")
    print("ncars per chasing: ", n_cars_per_chasing)
    print("nchasings: ", n_chasings)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])
        l = int(random.randint(0, n_lanes))
        if (road == r):
            r = r + 1

        road = r
        lane = l
        # print("road:", road)
        # print("lane:", lane)

        lastpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            l = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            cars.append(car_id)
            car_constructs.append(car_id + ":(" + str(w) + "," + str(l) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            for n_trj in range(n_trajectories):

                # start(road:0, lane:0)
                # road = min([i for i in range(n_roads)][i::])
                # lane = int(random.randint(0, n_lanes))
                if (n_trj == 0):
                    car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(lane) + ")")

                if (n_trj == 0):
                    # initial ptv
                    # movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastpos + 2, dist / 1.5)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[0,0])"
                else:
                    # final ptv
                    # movement(pos:[4,5], vel:[1,150], time:[10,10])
                    pos0 = random.uniform(lastpos, dist / 3)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(10, 120)
                    vel1 = random.uniform(vel0, 120)
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[" + str(duration) + "," + str(duration) + "])"

                car_moves[car_id].append(movement_quote)

    goals_ids = [cars[i] for i in random.sample(range(0, n_cars_per_chasing), 2)]
    write(dir,scenario_prop, car_constructs, car_moves, goals_ids, index, road_i)
    return

def simple_chase_inc_cars_n_movements(dir, index, road_i, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1
    
    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    n_roads = n_roads #- goal is to put each car in each road available
    n_lanes = n_lanes # - put each car in each lane of each road
    n_cars_per_chasing = random.randint(2, index+3)
    n_chasings = 1
    n_trajectories = 2
    print("-----------------------------------------")
    print("ncars per chasing: ", n_cars_per_chasing)
    print("nchasings: ", n_chasings)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []
    
    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        #start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])
        l = int(random.randint(0, n_lanes))
        if(road==r):
            r = r + 1
        
        road = r
        lane = l
        #print("road:", road)
        #print("lane:", lane)
        
        lastpos = 0
        for aux in range(n_cars_per_chasing): #consider each car
            
            w = random.uniform(2, 3) #width
            l = random.uniform(w, 4.5) #length
            
            car_id = "car"+str(i)
            cars.append(car_id)
            car_constructs.append(car_id+":("+str(w)+","+str(l)+")")
            car_id = cars[i]
            i = i+1
            drive_with_construct = car_id+".drive_with:"
            trajectories = []
            car_moves[car_id] = []
            
            
            for n_trj in range(n_trajectories):
                
                #start(road:0, lane:0)
                #road = min([i for i in range(n_roads)][i::])
                #lane = int(random.randint(0, n_lanes))
                if(n_trj == 0):
                    car_moves[car_id].append("start(road:"+str(road)+",lane:"+str(lane)+")")
                
                if(n_trj == 0):
                    #initial ptv
                    #movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastpos + 2, dist/1.5)
                    pos1 = pos0 + random.uniform(2,3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    movement_quote = "movement(pos:["+str(pos0)+","+str(pos1)+"], vel:["+str(vel0)+","+str(vel1)+"], time:[0,0])"
                else:
                    #final ptv
                    #movement(pos:[4,5], vel:[1,150], time:[10,10])
                    pos0 = random.uniform(lastpos, dist/3)
                    pos1 = pos0 + random.uniform(2,3)   
                    vel0 = random.uniform(10, 120)
                    vel1 = random.uniform(vel0, 120)
                    movement_quote = "movement(pos:["+str(pos0)+","+str(pos1)+"], vel:["+str(vel0)+","+str(vel1)+"], time:["+str(duration)+","+str(duration)+"])"
                
                car_moves[car_id].append(movement_quote)
        
            
    goals_ids = [cars[i] for i in random.sample(range(0, n_cars_per_chasing), 2)]
    write(dir,scenario_prop, car_constructs, car_moves, goals_ids, index, road_i)

    return

def simple_chase_inc_roads_n_movements(dir, index, road_i, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    # n_roads = 4 #n_roads - goal is to put each car in each road available
    # n_lanes = 2 #n_lanes - put each car in each lane of each road
    n_cars_per_chasing = 2
    n_chasings = n_roads
    n_trajectories = 2
    print("-----------------------------------------")
    print("ncars per chasing: ", n_cars_per_chasing)
    print("nchasings: ", n_chasings)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])
        l = int(random.randint(0, n_lanes))
        if (road == r):
            r = r + 1

        road = r
        lane = l
        # print("road:", road)
        # print("lane:", lane)

        lastpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            l = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            cars.append(car_id)
            car_constructs.append(car_id + ":(" + str(w) + "," + str(l) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            for n_trj in range(n_trajectories):

                # start(road:0, lane:0)
                # road = min([i for i in range(n_roads)][i::])
                # lane = int(random.randint(0, n_lanes))
                if (n_trj == 0):
                    car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(lane) + ")")

                if (n_trj == 0):
                    # initial ptv
                    # movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastpos + 2, dist / 1.5)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[0,0])"
                else:
                    # final ptv
                    # movement(pos:[4,5], vel:[1,150], time:[10,10])
                    pos0 = random.uniform(lastpos, dist / 3)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(10, 120)
                    vel1 = random.uniform(vel0, 120)
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[" + str(duration) + "," + str(duration) + "])"

                car_moves[car_id].append(movement_quote)

    goals_ids = [cars[i] for i in random.sample(range(0, n_cars_per_chasing), 2)]
    write(dir,scenario_prop, car_constructs, car_moves, goals_ids, index, road_i)

    return


def main():

    for i in range(5):
        
        f = open("./testing_roads/road"+str(i)+".json")
        data = json.load(f)

        n_roads = len([i for i in range(len(data["roads"]))])
        n_lanes_per_road = [road["n_lanes"] for road in data["roads"]][0]
        for j in range(10):
            simple_chase_inc_general_n_movements("simple_chase_inc_roads_"+str(n_trajectories)+"_movements",j, i, n_roads, n_lanes_per_road)
            simple_chase_inc_cars_n_movements("simple_chase_inc_cars_"+str(n_trajectories)+"_movements",j, i, n_roads, n_lanes_per_road)
            simple_chase_inc_roads_n_movements("simple_chase_inc_roads_"+str(n_trajectories)+"_movements",j, i, n_roads, n_lanes_per_road)

main()


