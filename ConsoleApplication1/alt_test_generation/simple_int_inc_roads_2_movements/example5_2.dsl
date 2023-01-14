scenario(10):

roadNet(2)
car0:car(2.113865549501698,2.428796482566809)
car1:car(2.4887044833193155,3.856062056902754)
car2:car(2.8401797675882445,2.928999731782178)
car3:car(2.8708601705500487,3.7198744033386646)

car0.drive_with:
start(road:0,lane:0)
movement(pos:[3.4726333912047003,5.680220062017157], vel:[2.3538448066106024,42.3538448066106], time:[0,0])
movement(pos:[39.53915155494661,139.5391515549466], vel:[2,77.84233556100409], time:[7.5,10])

car1.drive_with:
start(road:1,lane:0)
movement(pos:[2.829378521537099,5.768629564114099], vel:[3.0367248465304773,43.036724846530475], time:[0,0])
movement(pos:[199.08235241094556,299.08235241094553], vel:[2,116.42691189052562], time:[7.5,10])

car2.drive_with:
start(road:2,lane:2)
movement(pos:[3.3404412289778422,5.919590506401482], vel:[4.436471452990919,44.436471452990915], time:[0,0])
movement(pos:[17.45133293520354,117.45133293520354], vel:[2,73.22260658053364], time:[7.5,10])

car3.drive_with:
start(road:3,lane:2)
movement(pos:[2.485205196318997,5.169833432853649], vel:[3.0297105527726824,43.029710552772684], time:[0,0])
movement(pos:[18.720224035136425,118.72022403513643], vel:[2,73.25081931718958], time:[7.5,10])


goal:collision(car0,car1)end