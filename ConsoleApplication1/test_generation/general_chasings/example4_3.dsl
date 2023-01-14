scenario(10):

roadNet(2)
car0:car(2,3)
car1:car(2,3)
car2:car(2,3)
car3:car(2,3)
car4:car(2,3)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[60.0,80.0], time:[0,0])
movement(pos:[94.44,105.56], vel:[60.0,80.0], time:[4.75,5.25])
movement(pos:[200.0,200.0], vel:[60.0,80.0], time:[10,10])

car1.drive_with:
start(road:0,lane:0)
movement(pos:[8,8], vel:[60.0,80.0], time:[0,0])
movement(pos:[208.0,208.0], vel:[60.0,80.0], time:[10,10])

car2.drive_with:
start(road:0,lane:0)
movement(pos:[16,16], vel:[60.0,80.0], time:[0,0])
movement(pos:[216.0,216.0], vel:[60.0,80.0], time:[10,10])

car3.drive_with:
start(road:0,lane:0)
movement(pos:[24,24], vel:[60.0,80.0], time:[0,0])
movement(pos:[224.0,224.0], vel:[60.0,80.0], time:[10,10])

car4.drive_with:
start(road:0,lane:0)
movement(pos:[32,32], vel:[60.0,80.0], time:[0,0])
movement(pos:[232.0,232.0], vel:[60.0,80.0], time:[10,10])


goal:collision(car0,car1)end