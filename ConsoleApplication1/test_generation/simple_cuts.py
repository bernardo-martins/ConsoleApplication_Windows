# -*- coding: utf-8 -*-
"""
Created on Tue Jul 26 14:39:25 2022

@author: bermar3h
"""

# -*- coding: utf-8 -*-
"""
Created on Tue Jul 12 13:48:42 2022

@author: bermar3h
"""

import random
import json
#next step: generate the roads and ally this to the roads
#MAINLY CUT OUT SCENARIOS, TRY CUT IN

n_trajectories = 4

def write(dir, scenario_prop, cars_constructs, car_movements, goals, index, road_id):
    
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

    
    
    f = open("./"+dir+"/example"+str(index)+"_"+ str(int(road_id))+".dsl", "w")
    f.write(scenario)
    f.close()
    print(scenario)
    print("================================================")
    
    return
    

def simple_cuts_inc_roads_n_movements(dir, index, road_i, n_roads, n_lanes_per_road):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1
    
    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet
    
    n_roads = n_roads #n_roads - goal is to put each car in each road available
    n_lanes = n_lanes_per_road #n_lanes - put each car in each lane of each road
    n_cars_per_chasing = 2
    n_chasings = n_roads
    
    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []
    
    #print("number of cars per chasing:",n_chasings)
    #print("number of roads:", n_roads)
    #print("number of cars:", n_cars_per_chasing)
    
    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        #print("--------------------------------")
        #start(road:0, lane:0)
        r = -1
        l = 0
        while r!=road or l!=lane:
            r = min([chasing for chasing in range(n_roads)][chasing::])
            l = int(random.randint(0, n_lanes))
            if(road == r):
                r = r + 1
            road = r
            lane = l
        #print("road:", road)
        #print("lane:", lane)
        lastpos = 0
        lastinitpos = 0
        for aux in range(n_cars_per_chasing): #consider each car
            
            w = random.uniform(2, 3) #width
            le = random.uniform(w, 4.5) #length
            
            car_id = "car"+str(i)
            cars.append(car_id)
            car_constructs.append(car_id+":("+str(w)+","+str(le)+")")
            car_id = cars[i]
            i = i+1
            drive_with_construct = car_id+".drive_with:"
            trajectories = []
            car_moves[car_id] = []
            
            #print( "YAYAYAYAYAYAYAYAYA:",car_id)
            
            prev_lane = lane
            if(aux == n_cars_per_chasing-1):
                #prev_lane = lane

                if(max(0,lane - 1) == lane):
             #       print("enter here")
                    lane = min(n_lanes_per_road-1, lane+1)
                else:
               #     print("enter here instead")
                    lane = max(0,lane - 1)
            #print("NEW LANE:", lane)
            #print("PREV LANE:", prev_lane)

            
            for n_trj in range(n_trajectories):

                #start(road:0, lane:0)
                #road = min([i for i in range(n_roads)][i::])
                #lane = int(random.randint(0, n_lanes))
                if(n_trj == 0):
                    car_moves[car_id].append("start(road:"+str(road)+",lane:"+str(prev_lane)+")")

                if(n_trj == 0):
                    #initial ptv
                    #movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastinitpos, dist/1.5)
                    pos1 = pos0 + random.uniform(2,3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    lastinitpos = pos1
                    print("this is the lastpos: ",lastpos)
                    movement_quote = "movement(pos:["+str(pos0)+","+str(pos1)+"], vel:["+str(vel0)+","+str(vel1)+"], time:[0,0])"
                else:
                        if(aux == n_cars_per_chasing-1):
                            movement_quote = "change_lane(from:"+ str(prev_lane) +",to:"+str(lane)+")"
                            car_moves[car_id].append(movement_quote)

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


def simple_cuts_inc_cars_n_movements(dir, index, road_i, n_roads, n_lanes_per_road):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    n_roads = n_roads  # n_roads - goal is to put each car in each road available
    n_lanes = n_lanes_per_road  # n_lanes - put each car in each lane of each road
    n_cars_per_chasing = random.randint(2, index+2)
    n_chasings = 1

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []

    # print("number of cars per chasing:",n_chasings)
    # print("number of roads:", n_roads)
    # print("number of cars:", n_cars_per_chasing)

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # print("--------------------------------")
        # start(road:0, lane:0)
        r = -1
        l = 0
        while r != road or l != lane:
            r = min([chasing for chasing in range(n_roads)][chasing::])
            l = int(random.randint(0, n_lanes))
            if (road == r):
                r = r + 1
            road = r
            lane = l
        # print("road:", road)
        # print("lane:", lane)
        lastpos = 0
        lastinitpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            le = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            cars.append(car_id)
            car_constructs.append(car_id + ":(" + str(w) + "," + str(le) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            # print( "YAYAYAYAYAYAYAYAYA:",car_id)

            prev_lane = lane
            if (aux == n_cars_per_chasing - 1):
                # prev_lane = lane

                if (max(0, lane - 1) == lane):
                    #       print("enter here")
                    lane = min(n_lanes_per_road - 1, lane + 1)
                else:
                    #     print("enter here instead")
                    lane = max(0, lane - 1)
            # print("NEW LANE:", lane)
            # print("PREV LANE:", prev_lane)

            for n_trj in range(n_trajectories):

                # start(road:0, lane:0)
                # road = min([i for i in range(n_roads)][i::])
                # lane = int(random.randint(0, n_lanes))
                if (n_trj == 0):
                    car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(prev_lane) + ")")

                if (n_trj == 0):
                    # initial ptv
                    # movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastinitpos, dist / 1.5)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    lastinitpos = pos1
                    print("this is the lastpos: ", lastpos)
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[0,0])"
                else:
                    if (aux == n_cars_per_chasing - 1):
                        movement_quote = "change_lane(from:" + str(prev_lane) + ",to:" + str(lane) + ")"
                        car_moves[car_id].append(movement_quote)

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


def simple_cuts_increase_general_n_movements(dir, index, road_i, n_roads, n_lanes_per_road):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    n_roads = n_roads  # n_roads - goal is to put each car in each road available
    n_lanes = n_lanes_per_road  # n_lanes - put each car in each lane of each road
    n_cars_per_chasing = random.randint(2, index+2)
    n_chasings = random.randint(2, n_roads)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []

    # print("number of cars per chasing:",n_chasings)
    # print("number of roads:", n_roads)
    # print("number of cars:", n_cars_per_chasing)

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # print("--------------------------------")
        # start(road:0, lane:0)
        r = -1
        l = 0
        while r != road or l != lane:
            r = min([chasing for chasing in range(n_roads)][chasing::])
            l = int(random.randint(0, n_lanes))
            if (road == r):
                r = r + 1
            road = r
            lane = l
        # print("road:", road)
        # print("lane:", lane)
        lastpos = 0
        lastinitpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            le = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            cars.append(car_id)
            car_constructs.append(car_id + ":(" + str(w) + "," + str(le) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            # print( "YAYAYAYAYAYAYAYAYA:",car_id)

            prev_lane = lane
            if (aux == n_cars_per_chasing - 1):
                # prev_lane = lane

                if (max(0, lane - 1) == lane):
                    #       print("enter here")
                    lane = min(n_lanes_per_road - 1, lane + 1)
                else:
                    #     print("enter here instead")
                    lane = max(0, lane - 1)
            # print("NEW LANE:", lane)
            # print("PREV LANE:", prev_lane)

            for n_trj in range(n_trajectories):

                # start(road:0, lane:0)
                # road = min([i for i in range(n_roads)][i::])
                # lane = int(random.randint(0, n_lanes))
                if (n_trj == 0):
                    car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(prev_lane) + ")")

                if (n_trj == 0):
                    # initial ptv
                    # movement(pos:[4,5], vel:[1,150], time:[0,0])
                    pos0 = random.uniform(lastinitpos, dist / 1.5)
                    pos1 = pos0 + random.uniform(2, 3)
                    vel0 = random.uniform(20, 120)
                    vel1 = random.uniform(vel0, 120)
                    lastpos = pos1
                    lastinitpos = pos1
                    print("this is the lastpos: ", lastpos)
                    movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                        vel0) + "," + str(vel1) + "], time:[0,0])"
                else:
                    if (aux == n_cars_per_chasing - 1):
                        movement_quote = "change_lane(from:" + str(prev_lane) + ",to:" + str(lane) + ")"
                        car_moves[car_id].append(movement_quote)

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

def generate_alternative(m0, n_trajs):

    trjs = []

    (vel0_0,time0_0,pos0_0), (vel0_1,time0_1,pos0_1) = m0
    trjs.append(((vel0_0, time0_0, pos0_0), (vel0_1, time0_1, pos0_1)))

    for i in range(1,n_trajs):

        dt0 = random.uniform(1, 10/(n_trajs+2))
        vel0 = ((random.uniform(40, 120)))
        time0 = time0_1 + dt0

        dt1 = random.uniform(1, 2)
        vel1 = (vel0 + random.uniform(30, 60))
        time1 = time0 + dt1

        vel1 = vel1/3.6
        vel0 = vel0/3.6

        acc1 = (vel1 - vel0_1) / dt1
        pos1 = pos0_1 + vel1 * dt1 + 0.5 * acc1 * dt1 ** 2

        acc0 = (vel0 - vel0_0) / dt0
        pos0 = pos0_0 + vel0 * dt0 + 0.5 * acc0 * dt0 ** 2

        (vel0_0, time0_0, pos0_0) = (vel0,time0,pos0)
        (vel0_1, time0_1, pos0_1) = (vel1,time1,pos1)

        if(n_trajs-1 == i):
            trjs.append(((vel0_0, 10, pos0_0), (vel0_1, 10, pos0_1)))
        else:
            trjs.append(((vel0_0, time0_0, pos0_0),(vel0_1, time0_1, pos0_1)))

    return trjs

def main():
    
    for i in range(5):
        
        f = open("./testing_roads/road"+str(i)+".json")
        data = json.load(f)

        n_roads = len([i for i in range(len(data["roads"]))])
        n_lanes_per_road = [road["n_lanes"] for road in data["roads"]][0]
        print(n_roads," thisssss")
        print(n_lanes_per_road, " thisssss")
        for j in range(10):
            simple_cuts_inc_roads_n_movements("simple_cuts_inc_roads_"+str(n_trajectories)+"_movements",j, i, n_roads, n_lanes_per_road)
            simple_cuts_inc_cars_n_movements("simple_cuts_inc_cars_"+str(n_trajectories)+"_movements", j, i, n_roads, n_lanes_per_road)
            simple_cuts_increase_general_n_movements("simple_cuts_inc_general_"+str(n_trajectories)+"_movements", j, i, n_roads, n_lanes_per_road)
            
main()


