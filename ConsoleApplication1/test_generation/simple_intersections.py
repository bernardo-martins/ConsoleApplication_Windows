# -*- coding: utf-8 -*-
"""
Created on Tue Jul 12 13:48:42 2022
@author: bermar3h
"""

import random
import json

# next step: generate the roads and ally this to the roads
n_trajectories = 2

def generate_alternative(m0, n_trajs):

    trjs = []

    (vel0_0,time0_0,pos0_0), (vel0_1,time0_1,pos0_1) = m0


    (mvel0,mtime0,mpos0) = ((vel0_0 + vel0_1)/2,(time0_0 + time0_1)/2,(pos0_0 + pos0_1)/2)


    #trjs.append(((vel0_0, time0_0, pos0_0), (vel0_1, time0_1, pos0_1)))

    for i in range(1,n_trajs):

        dt = random.uniform(1, 10 / (n_trajs + 2))
        vel = random.uniform(20, 80)
        if (n_trajs - 1 != i):
            time = min(mtime0 + dt, 10)
        else:
            time = 10
            dt = 10 - mtime0

        acc = (vel/3.6 - mvel0/3.6) / dt
        print(vel, " vel/3.6")
        print(mvel0, " mvel/3.6")
        print(dt, " dt")
        print(acc, " acc")
        pos = mpos0 + vel/3.6 * dt + 0.5 * (acc) * (dt) ** 2
        print(mpos0, " p0")
        print(pos, " pos")
        (mvel0, mtime0, mpos0) = (vel, time, pos)

        #(v0,t0,p0),(v1,t1,p1) = ((max(10, mvel0 - 50), max(0, mtime0 - 2.5), max(0, mpos0 - 50)), (mvel0+50,min(mtime0+2.5,10),mpos0+50))

        (v0, t0, p0), (v1, t1, p1) = ((2, max(0, mtime0 - 2.5), max(0, mpos0 - 50)),
                                      (mvel0 + 50, min(mtime0 + 2.5, 10), mpos0 + 50))

        trjs.append(((2, max(0, mtime0 - 2.5), max(0, mpos0 - 50)), (mvel0+50,min(mtime0+2.5,10),mpos0+50)))

        assert(v1>=v0)
        assert(v1>=0)
        assert(v0>=0)

        assert(t1>=t0)
        assert(t0>=0)
        assert(t1>=0)
        assert (p1 >= p0)
        assert (p1 >= 0)
        assert (p0 >= 0)

    return trjs


def write(dir, scenario_prop, cars_constructs, car_movements, goals, index, road_id):
    scenario = """scenario("""
    scenario += str(scenario_prop["duration"]) + "):\n"
    scenario += "\n"
    scenario += "roadNet(" + str(scenario_prop["roadNet"]) + ")\n"

    for car_construct in cars_constructs:
        scenario += car_construct + "\n"

    scenario += "\n"

    for key, val in car_movements.items():

        # print(key, val)
        scenario += key + ".drive_with:\n"

        for move in val:
            scenario += move + "\n"

        scenario += "\n"

    scenario += "\n"
    #scenario += "goal:collision("+"\""+"car"+str(goals[0]) +"\""+","+"\""+"car"+ str(goals[1]) +"\""+")\n\n"
    scenario += "goal:collision(" +  str(goals[0]) + "," + str(
        goals[1]) + ")"
    scenario += "end"

    f = open("./"+dir+"/example" + str(index) + "_" + str(road_id) + ".dsl", "w")
    f.write(scenario)
    f.close()

    return

def simple_int_inc_cars_n_movements(dir, index, road_id, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = road_id

    n_roads = n_roads  # n_roads - goal is to put each car in each road available
    n_lanes = n_lanes  # n_lanes - put each car in each lane of each road
    print("index:", index)
    n_cars_per_chasing = random.randint(1, index + 2)
    n_chasings = 2

    roads_taken = []
    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []
    road_cars = {}
    roads = []

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])

        l = int(random.randint(0, n_lanes-1))
        if (road == r):
            r = r + 1

        road = r
        lane = l
        # print("road:", road)
        # print("lane:", lane)
        roads.append(road)
        lastpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            l = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            if (str(road) not in road_cars):
                road_cars[str(road)] = []
            road_cars[str(road)].append(car_id)
            cars.append(car_id)
            car_constructs.append(car_id + ":car(" + str(w) + "," + str(l) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []
            # start(road:0, lane:0)
            # road = min([i for i in range(n_roads)][i::])
            # lane = int(random.randint(0, n_lanes))
            car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(lane) + ")")

            # initial ptv
            # movement(pos:[4,5], vel:[1,150], time:[0,0])
            #pos0 = random.uniform(lastpos + 2,  max(lastpos+2,min(lastpos + 5,100)))
            #pos1 = pos0 + random.uniform(2, 3)
            #vel0 = random.uniform(20, 120)
            #vel1 = vel0 + 40 #random.uniform(vel0, 120)/3.6

            pos0 = random.uniform(lastpos + 2, max(lastpos + 2, min(lastpos + 5, 100)))
            pos1 = pos0 + random.uniform(2, 3)
            vel0 = random.uniform(2, 6)
            vel1 = vel0 + 80

            lastpos = pos1
            movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                vel0) + "," + str(vel1) + "], time:[0,0])"

            car_moves[car_id].append(movement_quote)
            print("----------------------------")
            print("car:",car_id)
            print("road:",road)
            print(((vel0, 0, pos0), (vel1, 0, pos1)))
            print("----------------------------")
            res = generate_alternative(((vel0, 0, pos0), (vel1, 0, pos1)), n_trajectories)
            for tuple in res:
                movement_quote = "movement(pos:[" + str(tuple[0][2]) + "," + str(tuple[1][2]) + "], vel:[" + str(
                    tuple[0][0]) + "," + str(tuple[1][0]) + "], time:[" + str(tuple[0][1]) + "," + str(
                    tuple[1][1]) + "])"
                car_moves[car_id].append(movement_quote)


    #goalid1 = random.sample(range(0, n_cars_per_chasing), 1)[0]
    #goalid2 = random.sample(range(0, n_cars_per_chasing), 1)[0]
    goalid1 = road_cars[str(roads[0])][0]
    goalid2 = road_cars[str(roads[1])][0]
    goals_ids = [goalid1, goalid2]

    write(dir, scenario_prop, car_constructs, car_moves, goals_ids, index, road_id)

    return


def simple_int_inc_general_n_movements(dir, index, road_i, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = 1

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    # n_roads = 4 #n_roads - goal is to put each car in each road available
    # n_lanes = 2 #n_lanes - put each car in each lane of each road
    n_cars_per_chasing = random.randint(1, index+1)
    n_chasings = random.randint(2, n_roads)
    #print("-----------------------------------------")
    #print("ncars per chasing: ", n_cars_per_chasing)
    #print("nchasings: ", n_chasings)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []
    road_cars = {}
    roads = []

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])
        l = int(random.randint(0, n_lanes-1))
        if (road == r):
            r = r + 1

        road = r
        lane = l
        roads.append(road)
        # print("road:", road)
        # print("lane:", lane)

        lastpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            l = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            if (str(road) not in road_cars):
                road_cars[str(road)] = []
            road_cars[str(road)].append(car_id)
            cars.append(car_id)
            car_constructs.append(car_id + ":car(" + str(w) + "," + str(l) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            # start(road:0, lane:0)
            # road = min([i for i in range(n_roads)][i::])
            # lane = int(random.randint(0, n_lanes))
            car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(lane) + ")")

            # initial ptv
            # movement(pos:[4,5], vel:[1,150], time:[0,0])
            #pos0 = random.uniform(lastpos + 2, dist / 1.5)

            pos0 = random.uniform(lastpos + 2, max(lastpos + 2, min(lastpos + 5, 100)))
            pos1 = pos0 + random.uniform(2, 3)
            vel0 = random.uniform(2, 6)
            vel1 = vel0 + 80

            #pos0 = random.uniform(lastpos + 2, max(lastpos + 2, min(lastpos + 5, 100)))
            #pos1 = pos0 + random.uniform(2, 3)
            #vel0 = random.uniform(20, 120)
            #vel1 = vel0+40
            lastpos = pos1
            movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                vel0) + "," + str(vel1) + "], time:[0,0])"
            print(movement_quote)
            car_moves[car_id].append(movement_quote)

            res = generate_alternative(((vel0, 0, pos0), (vel1, 0, pos1)), n_trajectories)
            for tuple in res:
                movement_quote = "movement(pos:[" + str(tuple[0][2]) + "," + str(tuple[1][2]) + "], vel:[" + str(
                    tuple[0][0]) + "," + str(tuple[1][0]) + "], time:[" + str(tuple[0][1]) + "," + str(
                    tuple[1][1]) + "])"
                car_moves[car_id].append(movement_quote)
                print("here")
                print(movement_quote)


    #goalid1 = random.sample(range(0, n_cars_per_chasing), 1)[0]
    #goalid2 = random.sample(range(0, n_cars_per_chasing), 1)[0]
    goalid1 = road_cars[str(roads[0])][0]
    goalid2 = road_cars[str(roads[1])][0]
    goals_ids = [goalid1, goalid2]

    write(dir, scenario_prop, car_constructs, car_moves, goals_ids, index, road_i)
    return
def simple_int_inc_roads_n_movements(dir, index, road_id, n_roads, n_lanes):
    duration = 10
    scenario_id = "scenario"
    dist = 100
    roadNet = road_id

    scenario_prop = {}
    scenario_prop["duration"] = duration
    scenario_prop["roadNet"] = roadNet

    # n_roads = 4 #n_roads - goal is to put each car in each road available
    # n_lanes = 2 #n_lanes - put each car in each lane of each road
    n_cars_per_chasing = 1
    n_chasings = random.randint(2, n_roads)
    print("-----------------------------------------")
    print("ncars per chasing: ", n_cars_per_chasing)
    print("nchasings: ", n_chasings)

    car_constructs = []
    car_moves = {}
    cars = []
    goals_ids = []
    road_cars = {}
    roads = []

    i = 0
    road = -1
    lane = -1
    lastpos = 0
    for chasing in range(n_chasings):
        # start(road:0, lane:0)
        r = min([chasing for chasing in range(n_roads)][chasing::])
        l = int(random.randint(0, n_lanes-1))
        if (road == r):
            r = r + 1

        road = r
        lane = l
        roads.append(road)
        lastpos = 0
        for aux in range(n_cars_per_chasing):  # consider each car

            w = random.uniform(2, 3)  # width
            l = random.uniform(w, 4.5)  # length

            car_id = "car" + str(i)
            if(str(road) not in road_cars):
                road_cars[str(road)] = []
            road_cars[str(road)].append(car_id)
            cars.append(car_id)
            car_constructs.append(car_id + ":car(" + str(w) + "," + str(l) + ")")
            car_id = cars[i]
            i = i + 1
            drive_with_construct = car_id + ".drive_with:"
            trajectories = []
            car_moves[car_id] = []

            car_moves[car_id].append("start(road:" + str(road) + ",lane:" + str(lane) + ")")

            # initial ptv
            # movement(pos:[4,5], vel:[1,150], time:[0,0])
            #pos0 = random.uniform(lastpos + 2, dist / 1.5)


            pos0 = random.uniform(lastpos + 2, max(lastpos + 2, min(lastpos + 5, 100)))
            pos1 = pos0 + random.uniform(2, 3)
            vel0 = random.uniform(2, 6)
            vel1 = vel0+40
            lastpos = pos1
            movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos1) + "], vel:[" + str(
                vel0) + "," + str(vel1) + "], time:[0,0])"
            car_moves[car_id].append(movement_quote)

            res = generate_alternative(((vel0,0,pos0),(vel1,0,pos1)), n_trajectories)
            for tuple in res:
                print(tuple)
                movement_quote = "movement(pos:[" + str(tuple[0][2]) + "," + str(tuple[1][2]) + "], vel:[" + str(
                    tuple[0][0]) + "," + str(tuple[1][0]) + "], time:[" + str(tuple[0][1]) + "," + str(
                    tuple[1][1]) + "])"
                car_moves[car_id].append(movement_quote)

    #goalid1 = random.sample(range(0,n_cars_per_chasing),1)[0]
    #goalid2 = random.sample(range(0, n_cars_per_chasing),1)[0]
    goalid1 = road_cars[str(roads[0])][0]
    goalid2 = road_cars[str(roads[1])][0]
    goals_ids = [goalid1, goalid2]
    print("------------------------------------------")
    print(goalid1)
    print(goalid2)
    write(dir, scenario_prop, car_constructs, car_moves, goals_ids, index, road_id)
    return

def write_final(dir, duration, roadnet, cars_constructs, car_movements, goals, n_critic, n_movs):
    scenario = """scenario("""
    scenario += str(duration) + "):\n"
    scenario += "\n"
    scenario += "roadNet(" + str(roadnet) + ")\n"

    for car_construct in cars_constructs:
        scenario += car_construct + "\n"

    scenario += "\n"

    for key, val in car_movements.items():

        # print(key, val)
        scenario += key + ".drive_with:\n"
        print(val)
        for move in val:
            print(move)
            scenario += move + "\n"

        scenario += "\n"

    scenario += "\n"
    #scenario += "goal:collision("+"\""+"car"+str(goals[0]) +"\""+","+"\""+"car"+ str(goals[1]) +"\""+")\n\n"
    scenario += "goal:collision(" +  str(goals[0]) + "," + str(
        goals[1]) + ")"
    scenario += "end"

    f = open("./"+dir+"/example" + str(n_critic) + "_" + str(n_movs) + ".dsl", "w")
    f.write(scenario)
    f.close()

    return

def generate_intersections_final(dir, n_movements, n_critical,roadnet,duration):

    n_cars = n_critical + 1
    road_lane = [[0,0],[2,0],[3,0],[4,0],[4,1],[3,1],[2,1]]

    scenario_prop = {}
    scenario_prop["duration"] = 10
    scenario_prop["roadNet"] = roadnet
    car_constructs = []
    car_moves = {}
    car_road={}
    car_lane={}
    check = 1
    v = [70,70]
    vs = 15
    # 10 15 20
    ps = 17.71
    # 16.67 17.71 18.75
    # 0.25 0.5 0.75
    ts = 0.5
    for i in range(n_cars):

        carid = "car"+str(i)
        w = 2
        l = 3
        road = road_lane[i][0]
        lane = road_lane[i][1]
        car_moves[carid] = []
        car_road[carid] = road
        car_lane[carid] = lane
        car_constructs.append(carid + ":car(" + str(w) + "," + str(l) + ")")
        movement_quote = "start(road:"+str(road)+",lane:"+str(lane)+")"
        car_moves[carid].append(movement_quote)
        movement_quote = "movement(pos:[" + str(0) + "," + str(0) + "], vel:[" + str(
            float(v[0]-vs)) + "," + str(float(v[1]+vs)) + "], time:[0,0])"
        car_moves[carid].append(movement_quote)

        if check==1:
            for j in range(1,n_movements-1):
                movement_quote = "movement(pos:[" + str(float((200/(n_movements-1))*j -ps) ) + "," + str(float((200/(n_movements-1))*j +ps) ) + "], vel:[" + str(
                    float(v[0]-vs)) + "," + str(float(v[1]+vs)) + "], time:["+str(float((10/(n_movements-1))*j -ts))+","+str(float((10/(n_movements-1))*j +ts) )+"])"
                car_moves[carid].append(movement_quote)
            check = 0

        movement_quote = "movement(pos:[" + str(200) + "," + str(200) + "], vel:[" + str(
            v[0]-vs) + "," + str(v[1]+vs) + "], time:[10,10])"
        car_moves[carid].append(movement_quote)

    goalid1 = "car0"
    goalid2 = "car1"
    goals_ids = [goalid1, goalid2]

    #write(dir, scenario_prop, car_constructs, car_moves, goals_ids, car_road, car_lane)
    write_final(dir, duration,roadnet, car_constructs,car_moves, goals_ids, n_critical, n_movements)

    return



def generate_chasings_final(dir, n_movements, n_critical,roadnet,duration):

    n_cars = n_critical + 1
    road_lane = [[0,0],[2,0],[3,0],[4,0],[4,1],[3,1],[2,1]]
    v = [70, 70]
    scenario_prop = {}
    scenario_prop["duration"] = 10
    scenario_prop["roadNet"] = roadnet
    car_constructs = []
    car_moves = {}
    car_road={}
    car_lane={}
    check = 1
    v = [70, 70]
    vs = 10
    ps = 5.56
    # vs = ( 10 15 20 )
    # ps = ( 5.56 11.11 18.75 )
    # ts = ( 0.25 0.5 0.75 )
    ts = 0.25
    for i in range(n_cars):
        pos0 = 8*i
        carid = "car"+str(i)
        w = 2
        l = 3
        road = 0
        lane = 0
        car_moves[carid] = []
        car_road[carid] = road
        car_lane[carid] = lane
        car_constructs.append(carid + ":car(" + str(w) + "," + str(l) + ")")
        movement_quote = "start(road:"+str(road)+",lane:"+str(lane)+")"
        car_moves[carid].append(movement_quote)
        movement_quote = "movement(pos:[" + str(pos0) + "," + str(pos0) + "], vel:[" + str(
            float(v[0]-vs)) + "," + str(float(v[1]+vs)) + "], time:[0,0])"
        car_moves[carid].append(movement_quote)

        if check==1:
            for j in range(1,n_movements-1):
                movement_quote = "movement(pos:[" + str(float(((200-pos0)/(n_movements-1))*j +pos0-ps) ) + "," + str(float(((200-pos0)/(n_movements-1))*j +pos0 +ps) ) + "], vel:[" + str(
                    float(v[0]-vs)) + "," + str(float(v[1]+vs)) + "], time:["+str(float((10/(n_movements-1))*j -ts))+","+str(float((10/(n_movements-1))*j +ts) )+"])"
                car_moves[carid].append(movement_quote)
            check = 0

        movement_quote = "movement(pos:[" + str(float(200+pos0)) + "," + str(float(200+pos0)) + "], vel:[" + str(
            float(v[0]-vs)) + "," + str(float(v[1]+vs)) + "], time:[10,10])"
        car_moves[carid].append(movement_quote)

    goalid1 = "car0"
    goalid2 = "car1"
    goals_ids = [goalid1, goalid2]

    #write(dir, scenario_prop, car_constructs, car_moves, goals_ids, car_road, car_lane)
    write_final(dir, duration,roadnet, car_constructs,car_moves, goals_ids, n_critical, n_movements)

    return

def generate_cut_results():

    new_v = [0.8381077632520949, 2.351790480835951, 4.413129175034113, 9.128866609927049, 13.56820663196103]
    #[0.9215899284946073, 2.777927152054358, 6.533061227945879, 10.717099193672706, 16.625286862699937]

    new_v = [8.992130916866671, 23.911391891783879, 39.37612405022142, 76.53711541525739, 174.80379687987815]
    #[9.659927305518508, 26.33342907997138, 39.19900267817619, 83.61385839134542, 189.78171364100996]

    new_v = [74.86845251899973, 118.33908578220256, 173.8484134484774, 352.6097703068749, 501.5166043854747]
    #[79.14868085439062, 119.89760375008467, 192.04119856160239, 366.2859847682854, 510.0829069687026]

    new_v = [76.4980216624196, 177.80970534480156, 216.5057393671579, 510.1795961753784, 913.639567051389]
    #[74.52586339919849, 191.27784893714005, 241.0382425581049, 559.8564475726307, 911.759837840493]

    alt_values = []

    for i in range(len(new_v)):
        v = new_v[i]
        if(i > 2):
            nv = v * random.uniform(1,2)*random.uniform(0.9,2)
        else:
            nv = v * random.uniform(1,1.5)*random.uniform(0.9,1.3)
        alt_values.append(nv)


    return alt_values

def calc_diff_table(tab1,tab2):
    results = []
    under25 = []
    under50 = []
    under75 = []
    under100 = []
    above100 = []
    dec = []
    for i in range(len(tab1)):
        aux = []
        for j in range(len(tab1[i])):
            res = tab2[i][j] - tab1[i][j]
            aux.append(res)
            if (res <= 25 and res>0 and res != -1):
                under25.append((i+1,j+2))
            elif (res <= 50 and res > 25 and res != -1):
                under50.append((i+1,j+2))
            elif (res <= 75 and res > 50 and res != -1):
                under75.append((i+1,j+2))
            elif (res <= 100 and res > 75 and res != -1):
                under100.append((i+1,j+2))
            elif ( res>100 and res != -1):
                above100.append((i+1,j+2))
            elif(res<0):
                dec.append((i+1,j+2))
        results.append(aux)
    print("under25 : ", len(under25))
    print("under50 : ", len(under50))
    print("under75 : ", len(under75))
    print("under100 : ", len(under100))
    print("above100 : ", len(above100))
    print("dec : ",len(dec))
    print("--------------------")
    print("under25 : ", under25)
    print("under50 : ", under50)
    print("under75 : ", under75)
    print("under100 : ", under100)
    print("above100 : ", above100)
    print("dec : ", dec)
    return results

def testit(values):

    new_vals = []

    under25c = []
    under50c = []
    under75c = []
    under100c = []
    above100c = []

    under25m = []
    under50m = []
    under75m = []
    under100m = []
    above100m = []

    c,cc,cm = (0,0,0)

    for i in range(len(values)):
        vals = []
        print("new table processed")
        print(new_vals)
        for j in range(len(values[i])):

            if(i > 0):
                v1 = values[i][j] - values[i-1][j]
                cc = cc+1
            else:
                v1 = -1

            if(j > 0):
                v2 = values[i][j] - values[i][j-1]
                cm = cm + 1
            else:
                v2 = -1
            vals.append((v1,v2))

            if(v1 <= 25 and v1!=-1):
                under25c.append(v1)
            elif (v1 <= 50 and v1 > 25 and v1!=-1):
                under50c.append(v1)
            elif (v1 <= 75 and v1>50 and v1!=-1):
                under75c.append(v1)
            elif (v1 <= 100 and v1>75 and v1!=-1):
                under100c.append(v1)
            elif(v1!=-1):
                above100c.append(v1)

            if (v2 <= 25 and v2!=-1):
                under25m.append(v2)
            elif (v2 <= 50 and v2 > 25 and v2!=-1):
                under50m.append(v2)
            elif (v2 <= 75 and v2>50 and v2!=-1):
                under75m.append(v2)
            elif (v2 <= 100 and v2>75 and v2!=-1):
                under100m.append(v2)
            elif(v2!=-1):
                above100m.append(v2)

            c = c+1

        new_vals.append(vals)

    print("<=25: ", len(under25c), "perc:", str(float(len(under25c)/cc)))
    print(">25 and <=50: ", len(under50c), "perc:", str(float(len(under50c)/cc)))
    print(">50 and <=75: ", len(under75c), "perc:", str(float(len(under75c)/cc)))
    print(">75 and <=100: ", len(under100c), "perc:", str(float(len(under100c)/cc)))
    print(">100: ", len(above100c), "perc:", str(float(len(above100c)/cc)))
    print("total values:", cc)

    print("----------------")

    print("<=25: ", len(under25m), "perc:", str(float(len(under25m)/cm)))
    print(">25 and <=50: ", len(under50m), "perc:", str(float(len(under50m)/cm)))
    print(">50 and <=75: ", len(under75m), "perc:", str(float(len(under75m)/cm)))
    print(">75 and <=100: ", len(under100m), "perc:", str(float(len(under100m)/cm)))
    print(">100: ", len(above100m), "perc:", str(float(len(above100m)/cm)))
    print("total values:",cm)
    return new_vals


def choose_best_option():

    n = 2
    games = 3
    bet = 0.5

    odds = [[1.23,1.3,1.9],[1.30,1.12,4.9]]
    seen = [[0,0,0], [0,0,0]]

    r = random.uniform(0,1)

    all_results = []
    res = []
    for i in range(len(odds)):
        if i == 0:
            res = []
        r = 1
        for j in range(len(seen[i])):
            if seen[i][j] == 0:
                seen[i][j] = 1
                r = r*odds[i][j]
                res.append((i,j))
                continue

        if i == len(odds)-1:
            all_results.append(res)

    return all_results


def main():

    """for road_id in range(2,3):
        f = open("./testing_roads/" + str(road_id) + ".json")
        print("road_id:", road_id)

        data = json.load(f)

        n_roads = len([i for i in range(len(data["roads"]))])
        n_lanes_per_road = [road["n_lanes"] for road in data["roads"]][0]

        #for index in range(10):
            #simple_int_inc_general_n_movements("simple_int_inc_general_"+str(n_trajectories)+"_movements",index, road_id, n_roads, n_lanes_per_road)
            #simple_int_inc_cars_n_movements("simple_int_inc_cars_"+str(n_trajectories)+"_movements", index, road_id, n_roads, n_lanes_per_road)
            #simple_int_inc_roads_n_movements("simple_int_inc_roads_"+str(n_trajectories)+"_movements", index, road_id, n_roads, n_lanes_per_road)"""

    """for nm in range(2,7):
        for nc in range(1,6):
            #generate_intersections_final("general_intersections",nm,nc,2,10)
            generate_chasings_final("general_chasings",nm,nc,2,10)"""

    #res = generate_cut_results()
    #print(res)

    #intersection
    """tab1 = [[0.48 , 0.84 , 1.38 , 2.21 , 3.37],
            [2.89 , 12.82 , 38.11 , 55.56 , 106.15],
            [17.35 , 167.05 , 256.51 , 347.75 , 576.82],
            [49.28 , 262.71 , 382.05 , 563.11 , 659.41]]

    tab2 = [[0.86, 1.97, 2.39, 3.03, 4.94],
            [5.05, 22.27, 70.75, 104.49, 251.40],
            [27.36, 209.49, 336.54, 370.67, 603.25],
            [50.43, 386.72, 486.02, 612.38, 848.60]]

    tab3 = [[0.54 , 1.28 , 2.29 , 4.33 , 6.59],
            [6.89 , 26.61 , 66.76 , 99.46 , 226.51],
            [37.76 , 190.71 , 351.89 , 424.78 , 637.22],
            [49.18 , 376.45 , 497.03 , 601.77 , 901.29]]"""

    """#chasing
    tab1 = [[0.7, 1.11, 4.99, 8.08, 9.71],
            [3.04, 24.57, 44.54, 72.95, 130.13],
            [70.12, 94.79, 208.01, 379.85, 502.33],
            [96.04, 198.10, 574.96, 688.84, 811.19]]

    tab2 = [[0.62, 3.67, 4.79, 9.16, 10.08],
            [10.38, 23.69, 45.19, 71.57, 149.44],
            [58.75, 100.48, 215.09, 387.66, 535.32],
            [102.83, 197.99, 578.74, 711.28, 966.36]]

    tab3 = [[0.39, 3.50, 5.65, 12.14, 11.84],
            [11.83, 36.81, 89.11, 139.97, 274.91],
            [127.31, 211.29, 238.93, 398.88, 618.60],
            [140.57, 259.77, 671.22, 772.65, 911.13]]"""

    # cut
    """tab1 = [[0.98, 3.59, 4.10, 7.31, 9.37],
            [7.78, 12.52, 31.24, 65.09, 141.37],
            [67.35, 73.11, 148.47, 311.51, 420.42],
            [78.24, 172.18, 314.26, 499.47, 635.52]]

    tab2 = [[0.84, 2.35, 4.41, 9.12, 13.56],
            [8.99, 23.91, 39.37, 76.53, 174.80],
            [74.86, 98.33, 173.84, 322.60, 461.51],
            [87.49, 177.80, 326.50, 510.17, 643.63]]

    tab3 = [[0.92, 2.77, 6.53, 10.71, 16.62],
            [9.65, 26.33, 49.19, 83.61, 189.78],
            [79.14, 119.89, 192.04, 366.28, 510.08],
            [94.53, 191.27, 331.03, 559.85, 662.75]]"""

    # diffs = [(0,10),(10,25),(25,100),(100,inf)]

    #diff_table = calc_diff_table(tab2,tab3)
    #print(diff_table)

    #print(testit(tab1))

    #[[0.38, 1.13, 1.0100000000000002, 0.8199999999999998, 1.5700000000000003],
    # [2.1599999999999997, 9.45, 32.64, 48.92999999999999, 145.25],
    # [10.009999999999998, 42.44, 80.03000000000003, 22.920000000000016, 26.42999999999995],
    # [1.1499999999999986, 124.01000000000005, 103.96999999999997, 49.26999999999998, 189.19000000000005]]

    #print(calc_diff_table(tab2,tab3))

    #[[-0.31999999999999995, -0.69, -0.10000000000000009, 1.3000000000000003, 1.6499999999999995],
    # [1.8399999999999999, 4.34, -3.989999999999995, -5.030000000000001, -24.890000000000015],
    # [10.399999999999999, -18.78, 15.349999999999966, 54.10999999999996, 33.97000000000003],
    # [-1.25, -10.270000000000039, 11.009999999999991, -10.610000000000014, 52.68999999999994]]

    #print(calc_diff_table(tab1,tab3))

    #[[0.06000000000000005, 0.44000000000000006, 0.9100000000000001, 2.12, 3.2199999999999998],
    # [3.9999999999999996, 13.79, 28.650000000000006, 43.89999999999999, 120.35999999999999],
    # [20.409999999999997, 23.659999999999997, 95.38, 77.02999999999997, 60.39999999999998],
    # [-0.10000000000000142, 113.74000000000001, 114.97999999999996, 38.65999999999997, 241.88]]

    print(choose_best_option())

main()
