import random as r


def test_motion(vel0, time0, pos0, trj):
    T = [(time0, pos0, vel0)]
    for (time, pos, vel) in trj[1:]:
        if (vel == None):
            vel = vel0
        else:
            vel = vel
        if (pos == None):
            dt = time - time0
            acc = (vel - vel0) / dt
            pos = pos0 + vel0 * dt + 0.5 * acc * dt ** 2
        T.append((time, pos, vel))
        (time0, pos0, vel0) = (time, pos, vel)
    return T

def generate(m0, n_trajs, prev_ts, prev_vls, isub):

    trjs = []
    ts = []
    vs = []
    (vel0,time0,pos0) = m0
    trjs.append((vel0,time0,pos0))
    
    if(not isub):
        ts.append(time0)
        vs.append(vel0)
    for i in range(1, n_trajs):
        dt = r.uniform(1, 3.5)
        if(not isub):
            vel = ((r.uniform(40, 120)))
            time = time0 + dt
            print("time: ",time)
        else:
            vel = (prev_vls[i] + r.uniform(30,60))
            time = prev_ts[i] + dt

            print("timex: ", time)
            print("prev timex: ",prev_ts[i])

        acc = (vel - vel0) / dt
        pos = pos0 + vel0 * dt + 0.5 * acc * dt ** 2
        trjs.append((vel,time,pos))
        ts.append(time)
        vs.append(vel)

    return trjs, ts, vs

def generate_alternative(m0, n_trajs):

    trjs = []

    (vel0_0,time0_0,pos0_0), (vel0_1,time0_1,pos0_1) = m0


    (mvel0,mtime0,mpos0) = ((vel0_0 + vel0_1)/2,(time0_0 + time0_1)/2,(pos0_0 + pos0_1)/2)


    #trjs.append(((vel0_0, time0_0, pos0_0), (vel0_1, time0_1, pos0_1)))

    for i in range(1,n_trajs):

        dt = r.uniform(1, 10 / (n_trajs + 2))
        vel = r.uniform(20, 80)
        if (n_trajs - 1 != i):
            time = min(mtime0 + dt, 10)
        else:
            time = 10
            dt = 10 - mtime0

        acc = (vel/3.6 - mvel0/3.6) / dt
        pos = mpos0 + vel/3.6 * dt + 0.5 * (acc) * (dt) ** 2
        (mvel0, mtime0, mpos0) = (vel, time, pos)

        (v0,t0,p0),(v1,t1,p1) = ((max(10, mvel0 - 50), max(0, mtime0 - 2), max(0, mpos0 - 50)), (mvel0+50,min(mtime0+2,10),mpos0+50))

        trjs.append(((max(10, mvel0 - 50), max(0, mtime0 - 2), max(0, mpos0 - 50)), (mvel0+50,min(mtime0+2,10),mpos0+50)))

        assert(v1>=v0)
        assert(v1>=0)
        assert(v0>=0)

        assert(t1>=t0)
        assert(t0>=0)
        assert(t1>=0)

        assert (p1 >= p0)
        assert (p1 >= 0)
        assert (p0 >= 0)


    return trjs


def generate_movements(m0, m1, n_trajs):
    lbs,ts,vs = generate(m0,n_trajs,[],[], False)
    ubs,ts,vs= generate(m1,n_trajs,ts,vs,True)
    print(lbs)
    #print(ts)
    #print(vs)
    print("----------------------")
    print(ubs)

def main():

    #(time0, pos0, vel0)
    print(generate_alternative(((110.80207837476922, 0, 3.8963750296789144), (150.80207837476922, 0, 6.261631066922922)),4))
    #90 kmh = 2.5 mps 2.5 m por 10 segundos da 25 metros


    #m0 = (90,0,0)

    #time = 10
    #vel = 90/3.6
    #dt = 10
    #acc = (70/3.6 - vel)/dt
    #pos = m0[0] + vel * dt + 0.5 * (acc) * (dt) ** 2

    #m1 = (70, 10, pos)

    #print(m0)
    print("---------")
    #print(m1)

main()