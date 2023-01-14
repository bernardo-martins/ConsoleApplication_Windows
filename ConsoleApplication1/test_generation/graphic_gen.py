import matplotlib
matplotlib.use('TkAgg')
import matplotlib.pyplot as plt
import numpy as np
import seaborn as sb
import math as m


def building_time():
    plt.style.use('_mpl-gallery')

    # make data
    x = [1,2,3,4,5]

    #solutions_searched: [1,2,3,4,5]
    #solutions found: [1,2,3,4,4]
    #building_time: [0.468,0.296,0.303,0.286,0.381]
    #solving_time: [0.540,8.121,43.041,37.190,236.576]
    #models_processed: [1,1,3,4,10]

    print(x)

    txt="I need the caption to be present a little below X-axis"

    # make some synthetic data
    fig = plt.figure()
    ax1 = fig.add_axes((0.1, 0.2, 0.8, 0.7))

    ax1.set_title("Building time")
    ax1.set_xlabel('Run number')
    ax1.set_ylabel('Time in seconds')

    # make the edge colors match the facecolors
    y_ints = [0.368,0.296,0.303,0.286,0.381]
    y_chas = [0.139000,0.094978,0.097040,0.118997,0.118998]
    y_cuts = [0.087996,0.078998,0.128991,0.109002,0.107726]
    ax1.scatter(x,y_ints, c='r', edgecolors='face')
    ax1.scatter(x,y_chas, c='g', edgecolors='face')
    ax1.scatter(x,y_cuts, c='b', edgecolors='face')
    # center text
    # fig.text(.5, .05, txt, ha='center')

    # use OO interface
    ax1.set_xlim([0, 5.5])
    ax1.set_ylim([0, 0.5])

    # resize the figure to match the aspect ratio of the Axes
    fig.set_size_inches(7, 8, forward=True)

    plt.show()

    return

def solving_time():
    plt.style.use('_mpl-gallery')

    # make data
    x = [1,2,3,4,5]

    #solutions_searched: [1,2,3,4,5]
    #solutions found: [1,2,3,4,4]
    #building_time: [0.468,0.296,0.303,0.286,0.381]
    #solving_time: [0.540,8.121,43.041,37.190,236.576]
    #models_processed: [1,1,3,4,10]

    print(x)

    txt="I need the caption to be present a little below X-axis"

    # make some synthetic data
    fig = plt.figure()
    ax1 = fig.add_axes((0.1, 0.2, 0.8, 0.7))

    ax1.set_title("Solving time")
    ax1.set_xlabel('Number of solutions searched')
    ax1.set_ylabel('Time in seconds')

    # make the edge colors match the facecolors
    y_ints = [0.2148,4.8898,37.190,43.1802,100]
    y_chas = [1.7335,7.4004,38.4192,100,100]
    y_cuts = [1.0139,14.5956,100,100,100]

    ax1.scatter(x,y_chas, c='g', edgecolors='face', label = "chasing")
    ax1.scatter(x,y_cuts, c='b', edgecolors='face', label = "cut-in/cut-out")
    ax1.scatter(x, y_ints, c='r', edgecolors='face', label="intersection")
    # center text
    # fig.text(.5, .05, txt, ha='center')

    # use OO interface
    ax1.set_xlim([0, 5.5])
    ax1.set_ylim([0, 84])

    ax1.legend()

    # resize the figure to match the aspect ratio of the Axes
    fig.set_size_inches(7, 8, forward=True)

    plt.show()

    return

def range_with_floats(start, stop, step):
    while stop > start:
        yield start
        start += step

def results():

    err_r1 = 0.753*1
    err_r2 = 2.061*1
    err_r3 = 1.002*1
    err_r4 = 1.781*1

    r1 = 0.03
    r2 = 0.08244
    r3 = 0.04
    r4 = 0.071

    print("R1 in ", [r1 - err_r1, r1 + err_r1],";")
    print("R2 in ", [r2 - err_r2, r2 + err_r2],";")
    print("R3 in ", [r3 - err_r3, r3 + err_r3],";")
    print("R4 in ", [r4 - err_r4, r4 + err_r4],";")

    #[0.5, 1] ; [0.928511, 1] ; [-0.18197, 0.276808] ; [0.5, 1] ; [-1, -0.5] ; [0.0850573, 1] ; [0.806531, 0.828339] ; [2.03132, 2.29848] ; [0.842, 0.907198] ; [2.0208, 2.065]

    for A in range_with_floats(-1,-1,0.01):
        for B in range_with_floats(-1,1,0.01):
            for C in range_with_floats(-1,1,0.01):
                for D in range_with_floats(-1,1,0.01):
                    for E in range_with_floats(-1,1,0.01):
                        for F in range_with_floats(-1,1,0.01):
                            if(0.834 + (-0.02567 * A + (-0.01672) * B + (-0.01444) * C + (-0.01866) * D + (-0.02326) * E + (-0.11003) * F + (0.001737) * B * C + (0.0012) * E * F)/2 <= r1+err_r1 and 0.834 + (-0.02567 * A + (-0.01672) * B + (-0.01444) * C + (-0.01866) * D + (-0.02326) * E + (-0.11003) * F + (0.001737) * B * C + (0.0012) * E * F)/2 >= r1-err_r1):
                                if(2.8455 + (0.0422 * A + (-1.48053) * B + (-0.32674) * C + (-0.14259) * E + (-0.12016) * F + (0.160356) * B * C)/2>=err_r2-r2 and 2.8455 + (0.0422 * A + (-1.48053) * B + (-0.32674) * C + (-0.14259) * E + (-0.12016) * F + (0.160356) * B * C)/2<=err_r2+r2 ):
                                    if(0.9118 + (0.043822 * A + (-0.2551) * B + (-0.14823) * C + (0.017753) * D + (-0.1187) * E + (-0.13116) * F + (0.045659) * B * C + (0.009722) * B * E + (0.023441) * B * F)/2 >= err_r3-r3 and 0.9118 + (0.043822 * A + (-0.2551) * B + (-0.14823) * C + (0.017753) * D + (-0.1187) * E + (-0.13116) * F + (0.045659) * B * C + (0.009722) * B * E + (0.023441) * B * F)/2 <= err_r3+r3):
                                        if(2.7341 + (-0.02363 * A + (-1.23657) * B + (-0.19269) * C + (-0.01557) * D + (-0.04062) * E + (-0.09662) * F + (0.114653) * B * C)/2 >= err_r4-r4 and 2.7341 + (-0.02363 * A + (-1.23657) * B + (-0.19269) * C + (-0.01557) * D + (-0.04062) * E + (-0.09662) * F + (0.114653) * B * C)/2<=err_r4+r4):
                                            print("yes")
                                            print(A)
                                            print(B)
                                            print(C)
                                            print(D)
                                            print(E)
                                            print(F)
                                            return

def main():
    #building_time()
    solving_time()

main()