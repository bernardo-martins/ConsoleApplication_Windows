scenario(10):
    roadNet(1)
    car1:car(2,2)
    car2:car(2,2)
    car3:car(2,2)

    car1.drive_with:
        start(road:0, lane:0)
        movement(pos:[0,0], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    car2.drive_with:
        start(road:1, lane:0)
        movement(pos:[4,5], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    car3.drive_with:
        start(road:2, lane:0)
        movement(pos:[0,0], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])

    goal:collision(car1, car3)
end