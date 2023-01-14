# -*- coding: utf-8 -*-
"""
Created on Thu Apr 28 16:09:07 2022

@author: bermar3h
"""

# -*- coding: utf-8 -*-
"""
Created on Thu Apr 28 11:08:33 2022

@author: bermar3h
"""

import pygame
from pygame.locals import *
import time as t
import math as m
import numpy as np
import ast
import json
import random
from pygame.locals import Rect, KEYDOWN, K_UP, K_DOWN, K_RIGHT, K_LEFT
from pygame.math import Vector2

BLACK = (0,0,0)
DARK_GREY = (169,169,169)
GREY = (211,211,211)
WHITE = (255,255,255)
GREEN = (0,128,0)
map_d = (100,100)


WIDTH, HEIGHT = 100, 100
r = 500/100
map_d = (100,100)
color_map = {}
roadlane_map = {}
FPS = 200
dir = "C:/MinGW/msys/1.0/home/ASUS/Ibex/ibex-2.8.9/examples/ConsoleApplication1/ConsoleApplication1/"

def centre_interval(interval, length):
    mid = (interval[0] + interval[1])/2
    side = length/2
    return [mid-side, mid+side]

def rand_col(min_v=0.5, max_v=1.0):
    hsv = np.concatenate([np.random.rand(2), np.random.uniform(min_v, max_v, size=1)])
    return to_hex(hsv_to_rgb(hdv))

def scale_distance(distances):
    return (distances[0]*WIDTH[0]/map_d[0], distances[1]*HEIGHT[1]/map_d[1])

def scale_point(point):
    return (point[0]*WIDTH/map_d[0], point[1]*HEIGHT[1]/map_d[1])

def scale_point(pos, orig, dest):
    return (pos[0]*dest[0]/orig[0], pos[1]*dest[1]/orig[1])

def scale_image(img, factor):
    size = round(img.get_width()*factor), round(img.get_height()*factor)
    return pygame.transform.scale(img, size)

def to_pygame(coords, height, obj_height):
    return (coords[0], height - coords[1] - obj_height)

def getTrajectory(car,Sol):
    for key in Sol.keys():
        if key==car:
            return Sol[car]
    return []

def getCarTrajectories(car_def,Sol):
    T = []
    for c in car_def:
        Tc = getTrajectory(c['car'],Sol)
        if Tc != []:
            T.append({c['car']:Tc})
    return T

def read_roads(file_name):
    f = open(file_name)
    data = json.load(f)
    roads = data["roads"]
    intersections = data["intersections"]
    return (roads, intersections)

def setTrajectories(trj):
    c = 3.6
    (time0, pos0, vel0, road0, lane0, angle0) = trj[0]
    vel0 = vel0 / c
    T = [(time0, pos0, vel0, road0, lane0, angle0)]
    for (time, pos, vel, road, lane, angle) in trj[1:]:
        if (vel == None):
            vel = vel0
        else:
            vel = vel / c
        if (pos == None):        
            dt = time-time0
            acc = (vel-vel0)/dt      
            pos = pos0 + vel0*dt + 0.5*acc*dt**2
        T.append((time, pos, vel, road, lane, angle))
        (time0, pos0, vel0) = (time, pos, vel)
    return T

def readDict(data):
    dic = ast.literal_eval(data)
    return dic

def readSolutions(f):
    #f = open("Presentation use cases/3 cars - 2 cars with acceleration change/solution.txt")
    S = []
    for line in f.readlines():
        S.append(readDict(line))
    f.close()
    return S
    
def compile_roads(roads, intersections):
    ROAD_MAP = []
    roads_vec = []
    for r in roads:
        road = []
        vertical = r["h_axis1"] - r["h_axis0"]<r["v_axis1"] - r["v_axis0"]
        
        x0 = r["h_axis0"]
        y0 = r["v_axis0"]
        
        color = GREY if r["direction"] else DARK_GREY
        lanes = []
        for l in range(0, int(r["n_lanes"])):
            lane = {}
            lane_m = {}
            if(vertical):
                step = (r["h_axis1"]-r["h_axis0"])/int(r["n_lanes"])
                ly = (r["v_axis1"]-r["v_axis0"])
                road.append([vertical, color, (x0 + step*l), y0, step, ly, r["direction"]])
                # direction, color, bX, tY, dimX, dimY, direction
                lane_m["x0"] = x0 + step*l
                lane_m["x1"] = (x0 + step*(l+1))
                lane_m["y0"] = y0
                lane_m["y1"] = y0+ly
                
            else:
                step = (r["v_axis1"]-r["v_axis0"])/int(r["n_lanes"])
                lx = (r["h_axis1"]-r["h_axis0"])
                road.append([vertical, color, x0, (y0+step*l), lx, step, r["direction"]])
                # direction, color, bX, tY, dimX, dimY, direction
                lane_m["x0"] = x0
                lane_m["x1"] = x0+lx
                lane_m["y0"] = y0 + step*l
                lane_m["y1"] = y0+step*(l+1)
            #lane[l] = lane_m
            lanes.append(lane_m)
        roadlane_map[r["id"]] = lanes
        ROAD_MAP.append(road)
    return ROAD_MAP
        
def draw_bck(WIN):
    pygame.draw.rect(WIN, GREEN, Rect(0, 0, 500, 500))
    return

def draw_road(ROAD_MAP, intersections,  WIN):
    for r in ROAD_MAP:
        r_map = {}
        for lr in r:
            lane_map = {}
            lr4 = (float(lr[4]))*5.0
            lr3 = (float(lr[3]))*5.0
            lr2 = (float(lr[2]))*5.0
            lr5 = (float(lr[5]))*5.0
            #print("lr1:",lr[1], " lr2:", float(lr[2]), " lr3", float(lr[3]), "lr4", float(lr[4]), "lr5", float(lr[5]))
            if(lr[0]):
                pygame.draw.rect(WIN, lr[1], Rect(lr2, lr3, lr4, lr5))
                pygame.draw.rect(WIN, WHITE, Rect(lr2, lr3, 1, lr5))
            else:
                pygame.draw.rect(WIN, lr[1], Rect(lr2, lr3, lr4, lr5))
                pygame.draw.rect(WIN, WHITE, Rect(lr2, lr3, lr4, 1))
        #print("=========================")
    for x in intersections:
        #pygame.draw.rect(WIN, (255,0,0), Rect(x["x0"]"*WIDTH/map_d[0]", x["y0"]"*HEIGHT/map_d[0]", (x["x1"]-x["x0"])"*WIDTH/map_d[0]", (x["y1"]-x["y0"])"*WIDTH/map_d[0]"))
        pygame.draw.rect(WIN, (255,0,0), Rect(x["x0"]*5, x["y0"]*5, (x["x1"]-x["x0"])*5, (x["y1"]-x["y0"])*5))

    return

def getPosition(self, trj,t, position, initial_pos):
    
    (time0, pos0, vel0, road0, lane0, angle0) = trj[0]
    if(self.horizontal):
        ypos = position[1]
    else:
        ypos = position[0]
    if (t==time0):
        return (pos0, ypos, lane0)
    i = 1
    (time1, pos1, vel1, road1, lane1, angle1) = trj[i]
    while (i<len(trj) and time1 < t):
        (time0, pos0, vel0, road0, lane0, angle0) = trj[i]
        i = i + 1
        #print("time:")
        #print(t)
        (time1, pos1, vel1, road1, lane1, angle1) = trj[i]
    #print(i-1, " - ", i)   
    if (t==time1):
        return (pos1, ypos, lane0)
     
    #dt = t-time0
    dt = t - time0
    acc = (vel1-vel0)/(time1-time0)       
    pos = pos0 + vel0*dt + 0.5*acc*dt**2
    print(t)
    print(self.identifier," -> pos[0]:",position[0], ", pos[1]:", position[1])
    if(angle0>0):
        
        print(lane0, lane1)
        if(self.horizontal):
            if(lane1 - lane0 > 0):
                ypos = initial_pos[1] + (pos-pos0)*abs(m.tan(angle0*(3.1415/180)))
            else:
                ypos = initial_pos[1] - (pos-pos0)*abs(m.tan(angle0*(3.1415/180)))
        else:
            if(lane1 - lane0 > 0):
                print(initial_pos[0], angle0,"aquiiii")
                ypos = initial_pos[0] + (pos-pos0)*abs(m.tan(angle0*(3.1415/180)))
                #print(self.identifier)
                #print("(",pos,",",ypos,")")
            else:
                ypos = initial_pos[0] - (pos-pos0)*abs(m.tan(angle0*(3.1415/180)))
        #ypos = ypos + (pos1-pos0)*abs(m.tan(30))
        #pos = (pos0 + vel0*dt + 0.5*acc*dt**2)*m.sin(45)"""
    return [pos, ypos, lane0]

def readCars(S, road_map):
    cars = []
    for c in S["cars"]:
        car = Car(road_map[c[3]][c[4]][2], road_map[c[3]][c[4]][3], str(c[0]), c[1], c[2], c[3], c[4], c[5])
        print(c[4])
        print(c[5])
        print("-------------")
        car.check()
        #cars.append(Car(road_map[c[3]][c[4]][2], road_map[c[3]][c[4]][3], str(c[0]), c[1], c[2], c[3], c[4], c[5]))
        cars.append(car)
    return cars

class Car:
    def __init__(self, x, y, identifier, width, lenght, road, lane, trjs):
        self.x = x
        self.y = y
        self.x0 = x
        self.y0 = y
        self.length = lenght
        self.identifier = identifier
        self.width = width
        self.road = road
        self.lane = lane
        self.trjs = trjs
        self.position = Vector2(x, y)
        self.color = "#"+''.join([random.choice('ABCDEF0123456789') for i in range(6)])

    def check(self):
        width = roadlane_map[self.road][self.lane]["x1"]-roadlane_map[self.road][self.lane]["x0"]
        length = roadlane_map[self.road][self.lane]["y1"]-roadlane_map[self.road][self.lane]["y0"]
        if(width > length):
            assert(self.width<=length)
        else:
            assert(self.width<=width)
        
    def allocate(self, road_map):
        self.horizontal = road_map[self.road][self.lane][4] > road_map[self.road][self.lane][5] #horizontal length bigger than vertical
        self.direction = road_map[self.road][self.lane][6]
        if(not self.horizontal):
            #self.width = self.width "* WIDTH/map_d[0]
            #self.length = self.length" * HEIGHT/map_d[0]
            self.width = self.width
            self.length = self.length
            #self.y0 = self.y*HEIGHT/map_d[0] + self.width
            if(self.direction):
                self.y0 = self.y - self.width
            else:
                self.y0 = map_d[1] + self.y - self.width
            self.x0 = self.x
        else:
            alt = self.width
            #self.width= self.length * HEIGHT/map_d[0]
            self.width= self.length
            #self.length = alt * WIDTH/map_d[0]
            self.length = alt
            #self.x0 = self.x*WIDTH/map_d[0] + self.width
            if(self.direction):
                self.x0 = (self.x - self.width)
            else:
                self.x0 = map_d[1] + self.y - self.width
            self.y0 = self.y
        self.trajectory = setTrajectories(self.trjs)
        print("write the trajectories")
        print(self.trajectory)
        return
    
    def draw(self, road_map, WIN):
        if(self.horizontal):
            pygame.draw.rect(WIN, self.color, Rect((self.position[0]-self.width)*r, self.position[1]*r, self.width*r, self.length*r))
            #print(self.identifier, self.position[1] + self.length)
        else:
            #print(self.identifier, self.position[0])
            #print("-------------------")
            #print([roadlane_map[self.road][self.lane]["x0"], roadlane_map[self.road][self.lane]["x1"]])
            #print("middle",(roadlane_map[self.road][self.lane]["x1"]+roadlane_map[self.road][self.lane]["x0"])/2)
            #print(self.width)
            #print(self.identifier, centre_interval([roadlane_map[self.road][self.lane]["x0"], roadlane_map[self.road][self.lane]["x1"]], self.width))
            pygame.draw.rect(WIN, self.color, Rect(self.position[0]*r,(self.position[1]-self.length)*r, self.width*r, self.length*r))
            #interv = centre_interval([roadlane_map[self.road][self.lane]["x0"], roadlane_map[self.road][self.lane]["x1"]], self.width)
            #pygame.draw.rect(WIN, self.color, Rect(interv[0]*r,(self.position[1]-self.length)*r, self.width*r, self.length*r))
    
    def update(self, time):
        
        pos = getPosition(self, self.trajectory,time, self.position, [self.x0, self.y0])
        #if(self.horizontal):    
            #self.y0 = roadlane_map[self.road][pos[2]]["y0"]
        if(self.horizontal and self.y0 != roadlane_map[self.road][pos[2]]["y0"]):
            print(self.identifier)
            self.y0 = roadlane_map[self.road][pos[2]]["y0"]
        elif(not self.horizontal and self.x0!=roadlane_map[self.road][pos[2]]["x0"]):
                self.x0 = roadlane_map[self.road][pos[2]]["x0"]
        #print(self.identifier,"->",self.x0)
        #print(roadlane_map)
        #print(pos)
        if (self.horizontal):
            if (self.direction):
                #self.position[0] = xt + self.width"*WIDTH/map_d[0]
                self.position[0] = pos[0]
                self.position[1] = pos[1]
            else:
                #self.position[0] = xt + self.width"*WIDTH/map_d[0]#*WIDTH/map_d[0]
                self.position[0] =map_d[0] - pos[0] + self.width
                self.position[1] = pos[1]
        else:
            if (self.direction):
                #self.position[1] = xt + self.width"*HEIGHT/map_d[0]#*HEIGHT/map_d[0]
                self.position[1] = pos[0]
                self.position[0] = pos[1]
            else:
                #self.position[1] = xt + self.width"*HEIGHT/map_d[0]#*HEIGHT/map_d[0]
                self.position[1] = map_d[1] - pos[0] + self.width
                self.position[0] = pos[1]
                
    def drawTrajectory(self, ROAD_MAP, WIN):
        for (time, pos, vel, road, lane, angles) in self.trajectory:
            if (self.horizontal):
                if (self.direction):
                    #z_rect = Rect(self.x0 + pos"*WIDTH/map_d[0]", self.y0, 1, ROAD_MAP[self.road][self.lane][5])
                    z_rect = Rect(pos*r, self.y0*r, 1*r, ROAD_MAP[self.road][self.lane][5]*r)
                else:
                    #z_rect = Rect(self.x0 + pos"*WIDTH/map_d[0]", self.y0, 1, ROAD_MAP[self.road][self.lane][5])
                    z_rect = Rect((map_d[1]-pos-self.width+1)*r, self.y0*r, 1*r, ROAD_MAP[self.road][self.lane][5]*r)
            else:
                if (self.direction):
                    #z_rect = Rect(self.x0, self.y0 + pos"*HEIGHT/map_d[0]", ROAD_MAP[self.road][self.lane][4], 1)
                    z_rect = Rect(self.x0*r, (pos)*r, ROAD_MAP[self.road][self.lane][4]*r, 1*r)
                else:
                    #print("trajectory square:", (self.x0, (map_d[1]-pos), ROAD_MAP[self.road][self.lane][4], 1))
                    #z_rect = Rect(self.x0, self.y0 - pos"*HEIGHT/map_d[0]", ROAD_MAP[self.road][self.lane][4], 1)
                    z_rect = Rect(self.x0*r, (map_d[1]-pos-self.width+1)*r, ROAD_MAP[self.road][self.lane][4]*r, 1*r)
            pygame.draw.rect(WIN, self.color, z_rect)
    

def main():
    ##(roadID, duration, cars) = readSolutions()
    ##readSolutions()
    f = open(dir+"solutions.txt",'r')
    S = readSolutions(f)
    i = 0
    for s in S:
        road_file = dir+"roadNetworks/roadNetwork_%swithIntersections.json" % str(s["roadNetwork"])
        (roads, intersections) = read_roads(road_file)
        ROAD_MAP = compile_roads(roads, intersections)
        cars = readCars(s, ROAD_MAP)
        
        for c in cars:
            c.allocate(ROAD_MAP)
        clock = pygame.time.Clock()
        run = True    
        time = 0
        time_stop = s["time"]
        dtt = 0
        WIN = pygame.display.set_mode((500,500))
        #screen = pygame.display.set_mode((500,500))
        i = i+ 1
        pygame.display.set_caption("Simulation "+ str(i))
        while run:
                time += dtt/FPS
                
                if(time <= time_stop):
                    clock.tick(FPS)
                    draw_bck(WIN) 
                    draw_road(ROAD_MAP, intersections, WIN)
                    for c in cars:
                        c.drawTrajectory(ROAD_MAP, WIN)
                        c.update(time)
                        c.draw(ROAD_MAP, WIN)
                    pygame.display.update()
                    for event in pygame.event.get():
                        if event.type == pygame.QUIT:
                            print("exiting...")
                            return
                        if event.type == KEYDOWN:
                            if event.key == 32 or event.key == K_UP: # Space -> toggle movement
                                if dtt == 0:
                                    dtt = 1
                                else:
                                    dtt = 0
                            if event.key == K_LEFT:
                                time = 0
                                dtt = 0
                            if event.key == K_RIGHT: # Delete -> quit
                                run = False
                else:
                     dtt = 0
                     #run = False
                     t.sleep(5)
                     run = False
        
    pygame.quit()

main()