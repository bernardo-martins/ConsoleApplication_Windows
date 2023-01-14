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
movement(pos:[0,0], vel:[60.0,80.0], time:[0,0])
movement(pos:[61.10666666666667,72.22666666666667], vel:[60.0,80.0], time:[3.0833333333333335,3.5833333333333335])
movement(pos:[127.77333333333334,138.89333333333335], vel:[60.0,80.0], time:[6.416666666666667,6.916666666666667])
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

car5.drive_with:
start(road:0,lane:0)
movement(pos:[40,40], vel:[60.0,80.0], time:[0,0])
movement(pos:[240.0,240.0], vel:[60.0,80.0], time:[10,10])


goal:collision(car0,car1)end