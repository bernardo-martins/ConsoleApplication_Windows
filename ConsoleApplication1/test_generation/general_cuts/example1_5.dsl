scenario(10):

roadNet(2)
car0:car(2.860428415805897,3.4311093094068923)
car1:car(2.8874802328346223,3.570143583654405)

car0.drive_with:
start(road:0,lane:2)
movement(pos:[0.494550599612887,0.614442534108524], vel:[4.696904546859857,94.696904546859855], time:[0,0])
movement(pos:[1,90], vel:[1,300],time:[1,3])
movement(pos:[91,150], vel:[1,300],time:[3.1,4.9])
movement(pos:[151,234], vel:[1,300],time:[5,8.2])
movement(pos:[234.51404089270386,334.51404089270386], vel:[2,125.2665921538223], time:[8.5,10])

car1.drive_with:
start(road:0,lane:2)
movement(pos:[12.6460924139928617,14.9514621722196654], vel:[2.5642642037499743,92.56426420374997], time:[0,0])
movement(pos:[238.97700413493544,338.97700413493544], vel:[2,125.96419584328899], time:[8.5,10])

goal:collision(car0,car1)end