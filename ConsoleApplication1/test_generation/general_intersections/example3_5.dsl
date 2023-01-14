scenario(10):

roadNet(2)
car0:car(2,3)
car1:car(2,3)
car2:car(2,3)
car3:car(2,3)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[0,0], vel:[55.0,85.0], time:[0,0])
movement(pos:[32.29,67.71000000000001], vel:[55.0,85.0], time:[2.0,3.0])
movement(pos:[82.28999999999999,117.71000000000001], vel:[55.0,85.0], time:[4.5,5.5])
movement(pos:[132.29,167.71], vel:[55.0,85.0], time:[7.0,8.0])
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


goal:collision(car0,car1)end