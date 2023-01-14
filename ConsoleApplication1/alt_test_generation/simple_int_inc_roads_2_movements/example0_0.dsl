scenario(10):

roadNet(0)
car0:car(2.728772847608504,3.710149118607087)
car1:car(2.329925707844276,3.4364268627800394)
car2:car(2.309293686597961,3.385884954722777)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[2.986233604387863,60.98623360438786], time:[0,0])
movement(pos:[100,130.8313675978798], vel:[2,103.70102432594464], time:[7.5,10])

car1.drive_with:
start(road:1,lane:0)
movement(pos:[2,3], vel:[2.986233604387863,43.98623360438786], time:[0,0])
movement(pos:[100,130.8313675978798], vel:[2,103.70102432594464], time:[7.5,10])

car2.drive_with:
start(road:2,lane:1)
movement(pos:[3,6], vel:[2.986233604387863,53.98623360438786], time:[0,0])
movement(pos:[100,130.8313675978798], vel:[2,103.70102432594464], time:[7.5,10])

goal:collision(car0,car1)end