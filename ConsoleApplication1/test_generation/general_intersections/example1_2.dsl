scenario(10):

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


goal:collision(car0,car1)end