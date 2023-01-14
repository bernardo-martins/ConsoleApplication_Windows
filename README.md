
# ConsoleApplication1

Project is based on a DSL for the generation of critical scenarios for ADAS/AV. The project is based in the continous constraint programming paradigm and was the study goal for a Master's thesis of Bernardo Martins between February and July.


## Run locally

To run the project:
```bash
  cd ConsoleApplication1/ConsoleApplication1
  run.bat
```

Inside the .bat file are both commands to run the DSL part and the solver part. The latter can be edited to run with the desired configuration.


## Run locally in parts

To run the DSL generation script:
```bash
  cd ConsoleApplication1/ConsoleApplication1
  py language_gen.py
```

To run the solver:
```bash
  cd ConsoleApplication1/ConsoleApplication1
  make ConsoleApplication1
  ConsoleApplication1 eps nSets limitRun limitTimeRun
```

The main application has four arguments to be presented and, if not given as an input, defaults will be assumed:
1. eps: maximum size of each variable box (e.g. if we have a variable with the interval [1,4], size 3 (4-1), the solver will not take it as a final answer. The solver will accept minimal boxes with size under eps; default: 0.01)
2. nBoxes: number of sets of boxes the solver to search for a fix solution
3. limitTimesRun: limit of times a system can run (default: 4)
4. limitTimeRun: amount of time a system can run searching for a solution (default: 7 seconds)


## FAQ

#### How do I get the final solution, the final instantiation of each variable?

The final solution is generated after running the solver and it can be found in the file "solution.txt".

#### How can I see the boxes of values outputed by the solver?

The box of values is outputed in the command line together with the final solution and is saved in the variable "partitions" in the "ScenarioSolve" class.

#### How can I see the constraint system that resulted from the generation?

The systems are saved in the variable "s_aux" saved in the class "Problem". From here, each of the "ScenarioSolve" object have a variable called 
sys" that can be accessed.

### Grammar rules, how do they work?

A scenario must start with the following grammar:

scenario(duration):
    roadNet(networkId)
    car1:car(w_1,l_n)
    car2:car(w_2,l_2)
    ...
    carn:car(w_n,l_n)

    car1.drive_with:
        start(road:road_car1, lane:lane_car1)
        movement(pos:[0,0], vel:[1,150], time:[0,0])
        fix_acceleration(acc:acceleration_value)
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    car2.drive_with:
        start(road:1, lane:0)
        movement(pos:[4,5], vel:[1,150], time:[0,0])
        change_lane(from:prev_lane, to:next_lane)
        ...
        movement(vel:[40, 89], time:[2,7])
        ...
        movement(pos:[100,100], vel:[1,150], time:[10,10])
    ...
    carn.drive_with:
        start(road:road_carn, lane:lane_carn)
        movement(pos:[0,0], vel:[1,150], time:[0,0])
        movement(pos:[100,100], vel:[1,150], time:[10,10])

    goal:collision(car1, car2)
end


A car driving properties, specified as movements, must have an initial instant in the scenario and a final instant (here, t=0 and t=10).

Example:

The following scenario specifies an intersection between car1 and car3.

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


## Documentation to the solver used, IBEX

[IBEX](http://www.ibex-lib.org/doc/)

## Documentation to the library jsoncpp

[jsoncpp](https://github.com/open-source-parsers/jsoncpp)


## 🔗 Links
[![portfolio](https://img.shields.io/badge/my_portfolio-000?style=for-the-badge&logo=ko-fi&logoColor=white)](https://github.com/bernardo-martins/)
[![linkedin](https://img.shields.io/badge/linkedin-0A66C2?style=for-the-badge&logo=linkedin&logoColor=white)](https://www.linkedin.com/in/bernardo-martins24/)
