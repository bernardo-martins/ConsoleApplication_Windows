from textx import metamodel_from_file
import json
import os


data = {}
meta_model = metamodel_from_file('Grammar/grammar.tx')
model = meta_model.model_from_file('Grammar/example1.hello')

duration = model.scn.duration
data["duration"] = duration
data["id"] = "scn1"
data["networkID"] = model.rn.net_id
cars = {}
aux = []

cars_data = {}
aux_vector = []

for i in range(len(model.c)):
    car = model.c[i]
    aux_cars = {}
    aux_cars["width"] = car.w
    aux_cars["length"] = car.l
    cars_data[car.name] = aux_cars

for i in range(len(model.moves)):
    trs = {}
    c = {}
    uni_trs = {}
    trs_aux = []
    uni_aux = []
    road = -1
    lane = -1
    print(model.moves[i].car_id.name)
    print(model.moves[i])
    for j in range(len(model.moves[i].lanes)):
        #print("=================================")
        print("lane:",model.moves[i].lanes[j])
    for j in range(len(model.moves[i].lbvel)):
        trs = {}
        trs["id"] = j
        trs["road"] = model.moves[i].id_road[j]
        trs["lane"] = model.moves[i].id_lane[j]
        trs["p0"] = model.moves[i].lbpos[j]
        trs["p1"] = model.moves[i].ubpos[j]
        trs["t0"] = model.moves[i].lbtime[j]
        trs["t1"] = model.moves[i].ubtime[j]
        trs["v0"] = model.moves[i].lbvel[j]
        trs["v1"] = model.moves[i].ubvel[j]
        print(trs)
        trs_aux.append(trs)
    for j in range(len(model.moves[i].lbvel2)):
        unis = {}
        unis["t0"] = model.moves[i].lbtime2[j]
        unis["t1"] = model.moves[i].ubtime2[j]
        unis["v0"] = model.moves[i].lbvel2[j]
        unis["v1"] = model.moves[i].ubvel2[j]
        print(trs)
        uni_aux.append(unis)
    
    unis = {}
    if(len(uni_aux)==0):
        unis = None
    else:
        unis = uni_aux
    c["id"] = model.moves[i].car_id.name
    c["width"] = cars_data[model.moves[i].car_id.name]["width"]
    c["length"] = cars_data[model.moves[i].car_id.name]["length"]
    c["universal"] = unis
    c["trajectories"] = trs_aux
    aux.append(c)
    print("----------")

"""all_cars = [model.moves[i].car_id.name for i in range(len(model.c))]
for car in all_cars:
    c = {}
    c["id"] = car
    c["width"] = cars_data[car]["width"]
    c["length"] = cars_data[car]["length"]
    trs = {}
    trs_aux = []
    print(model.moves[i].ubtime2)
    print(model.moves[i].lbtime2)
    for j in range(len(model.moves[i].id_road)):
        trs = {}
        trs["id"] = j
        trs["road"] = model.moves[i].id_road[j]
        trs["lane"] = model.moves[i].id_lane[j]
        trs["p0"] = model.moves[i].lbpos[j]
        trs["p1"] = model.moves[i].ubpos[j]
        trs["t0"] = model.moves[i].lbtime[j]
        trs["t1"] = model.moves[i].ubtime[j]
        trs["v0"] = model.moves[i].lbvel[j]
        trs["v1"] = model.moves[i].ubvel[j]
        trs_aux.append(trs)
    print("----------------")
    unis = {}
    unis = None
    c["universal"] = unis
    c["trajectories"] = trs_aux
    aux.append(c)
"""
data["cars"] = aux
g = {}
g["cars"] = [[model.g.car1_id.name, model.g.car2_id.name]]
g["collides"] = 1 if(g["cars"]!=None) else 0
data["goal"] = g

json_object = json.dumps(data, indent = 4)
with open("sample.json", "w") as outfile:
    outfile.write(json_object)
"""dir = "C:/MinGW/msys/1.0/home/bermar3h/Ibex/ibex-lib-ibex-2.8.8/examples/ConsoleApplication1/ConsoleApplication1/"
print(os.startfile(dir+"ConsoleApplication1.exe"))
os.startfile(dir+"ConsoleApplication1.exe")
exec(open(dir+"solution.py").read())"""


