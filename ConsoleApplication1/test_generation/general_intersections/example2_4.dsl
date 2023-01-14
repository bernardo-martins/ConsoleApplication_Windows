scenario(10):

roadNet(2)
car0:car(2,3)
car1:car(2,3)
car2:car(2,3)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[48.95666666666667,84.37666666666667], vel:[55.0,85.0], time:[2.8333333333333335,3.8333333333333335])
movement(pos:[115.62333333333333,151.04333333333335], vel:[55.0,85.0], time:[6.166666666666667,7.166666666666667])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car1.drive_with:
start(road:2,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])

car2.drive_with:
start(road:3,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[200,200], vel:[55,85], time:[10,10])


goal:collision(car0,car1)end