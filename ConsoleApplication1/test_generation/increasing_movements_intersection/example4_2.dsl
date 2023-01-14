scenario(10):

roadNet(2)
car0:car(2.860428415805897,3.4311093094068923)
car1:car(2.8874802328346223,3.570143583654405)

car0.drive_with:
start(road:0,lane:2)
movement(pos:[3.794550599612887,6.614442534108524], vel:[4.696904546859857,94.696904546859855], time:[0,0])
movement(pos:[10,60], vel:[1,300],time:[1,2])
movement(pos:[61,80], vel:[1,300],time:[2.1,4])
movement(pos:[81,120], vel:[1,300],time:[4.1,6])
movement(pos:[122,194], vel:[1,300],time:[6.1,7.1])
movement(pos:[195,234], vel:[1,300],time:[7.2,8.4])
movement(pos:[234.51404089270386,334.51404089270386], vel:[2,125.2665921538223], time:[8.5,10])

car1.drive_with:
start(road:2,lane:0)
movement(pos:[2.6460924139928617,4.9514621722196654], vel:[2.5642642037499743,92.56426420374997], time:[0,0])
movement(pos:[10,60], vel:[1,300],time:[1,2])
movement(pos:[61,80], vel:[1,300],time:[2.1,4])
movement(pos:[81,120], vel:[1,300],time:[4.1,6])
movement(pos:[122,194], vel:[1,300],time:[6.1,7.1])
movement(pos:[195,234], vel:[1,300],time:[7.2,8.4])
movement(pos:[238.97700413493544,338.97700413493544], vel:[2,125.96419584328899], time:[8.5,10])

goal:collision(car0,car1)end