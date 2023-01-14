scenario(10):

roadNet(2)
car0:car(2.195127654259853,4.153579447248769)
car1:car(2.005658348350722,2.984013467048357)
car2:car(2.789353669709988,4.465035793962904)
car3:car(2.1723902162532003,2.60840099292996)
car4:car(2.732383828446835,3.703005935914962)

car0.drive_with:
start(road:0,lane:1)
movement(pos:[2.6632445513605307,4.667328308686155], vel:[2.9830452918178905,42.98304529181789], time:[0,0])
movement(pos:[163.1843577474212,263.1843577474212], vel:[2,107.94559221344812], time:[7.5,10])

car1.drive_with:
start(road:1,lane:0)
movement(pos:[2.849684393258306,5.248121489023232], vel:[3.328397834029911,43.328397834029914], time:[0,0])
movement(pos:[72.50669519120697,172.50669519120697], vel:[2,86.2060027513592], time:[7.5,10])

car2.drive_with:
start(road:2,lane:2)
movement(pos:[3.8156924399547942,5.8832288202351695], vel:[3.219441324547289,43.21944132454729], time:[0,0])
movement(pos:[157.386996780064,257.386996780064], vel:[2,106.34882245084165], time:[7.5,10])

car3.drive_with:
start(road:3,lane:2)
movement(pos:[2.0823692887949683,4.441615709482386], vel:[4.4250745365374655,44.425074536537466], time:[0,0])
movement(pos:[119.17599706298549,219.1759970629855], vel:[2,97.9610526075024], time:[7.5,10])

car4.drive_with:
start(road:4,lane:0)
movement(pos:[2.8685490360998873,5.231337642031556], vel:[3.682976026623012,43.68297602662301], time:[0,0])
movement(pos:[198.23700644226537,298.2370064422654], vel:[2,116.49922048697559], time:[7.5,10])


goal:collision(car0,car1)end