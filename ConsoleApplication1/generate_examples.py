# -*- coding: utf-8 -*-
"""
Created on Thu Jul  7 17:40:14 2022

@author: bermar3h
"""
import random

sample = """{scenario(10):
    roadNet(1)
    car1:car(2,2)
    car2:car(2,2)
    car3:car(2,2)
    car1.drive_with:
        start(road:0, lane:0)
        movement(pos:[0,0], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    car2.drive_with:
        start(road:0, lane:0)
        movement(pos:[4,5], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    car3.drive_with:
        start(road:0, lane:0)
        movement(pos:[9,11], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    
    goal:collision(car1, car2)
    end
    }"""

duration = 10
scenario_id = "scenario"
dist = 100
roadNet = 1

scenario_prop = {}
scenario_prop["duration"] = duration
scenario_prop["roadNet"] = roadNet

n_roads = 3 #n_roads - goal is to put each car in each road available
n_lanes = 2 #n_lanes - put each car in each lane of each road
n_cars = random.randint(2,n_roads)
n_trajectories = 2

roads_taken = []
car_constructs = []
car_moves = {}
cars = []
goals_ids = []

for i in range(n_cars): #consider each car

    w = random.uniform(2, 3) #width
    l = random.uniform(w, 4.5) #length
    
    car_id = "car"+str(i)
    cars.append(car_id)
    car_constructs.append(car_id+":("+str(w)+","+str(l)+")")
    
    drive_with_construct = car_id+".drive_with:"
    trajectories = []
    car_moves[car_id] = []
    for n_trj in range(n_trajectories):
        
        #start(road:0, lane:0)
        road = min([i for i in range(n_roads)][i::])
        lane = int(random.randint(0, n_lanes))
        car_moves[car_id].append("start(road:"+str(road)+",lane:"+str(lane)+")")
        
        if(n_trj == 0):
            #initial ptv
            #movement(pos:[4,5], vel:[1,150], time:[0,0])
            pos0 = random.uniform(0, dist/3)
            pos1 = pos0 + random.uniform(0, dist/100)
            vel0 = random.uniform(20, 120)
            vel1 = random.uniform(vel0, 120)
            movement_quote = "movement(pos:["+str(pos0)+","+str(pos1)+"], vel:["+str(vel0)+","+str(vel1)+"], time:[0,0])"
        elif(n_trj == n_trj-1):
            #final ptv
            #movement(pos:[4,5], vel:[1,150], time:[10,10])
            pos0 = random.uniform(0, dist/3)
            pos1 = pos0 + random.uniform(0, dist/100)
            vel0 = random.uniform(20, 120)
            vel1 = random.uniform(vel0, 120)
            movement_quote = "movement(pos:["+str(pos0)+","+str(pos1)+"], vel:["+str(vel0)+","+str(vel1)+"], time:["+str(duration)+","+str(duration)+"])"
        
        car_moves[car_id].append(movement_quote)

goals_ids = [cars[i] for i in random.sample(range(0, n_cars), 2)]
    
    

print(car_moves)
print("====================")
print(car_constructs)       
print("====================")
print(goals_ids)
    

def write(scenario_prop, cars_constructs, car_movements, goals):
    
    scenario = """scenario("""
    scenario += str(duration) + "):\n"    
    scenario += "roadNet(" + str(scenario_prop["roadNet"]) + ")"
    
    for car_construct in car_constructs:
        scenario += car_construct + "\n"
        
    scenario += "\n"
    
    for key, val in car_movements.items():
        
        #print(key, val)
        scenario += key+".drive_with:\n"
        
        for move in val:
            scenario += move + "\n"
        
        scenario += "\n"
    
    return
    

write(scenario_prop, car_constructs, car_moves, goals_ids)
    
    
    
    
    


