scenario(10):

roadNet(2)
car0:car(2,3)
car1:car(2,3)
car2:car(2,3)
car3:car(2,3)
car4:car(2,3)
car5:car(2,3)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[22.29,57.71], vel:[55.0,85.0], time:[1.5,2.5])
movement(pos:[62.29,97.71000000000001], vel:[55.0,85.0], time:[3.5,4.5])
movement(pos:[102.28999999999999,137.71], vel:[55.0,85.0], time:[5.5,6.5])
movement(pos:[142.29,177.71], vel:[55.0,85.0], time:[7.5,8.5])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car1.drive_with:
start(road:2,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car2.drive_with:
start(road:3,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car3.drive_with:
start(road:4,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car4.drive_with:
start(road:4,lane:1)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car5.drive_with:
start(road:3,lane:1)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])


goal:collision(car0,car1)end