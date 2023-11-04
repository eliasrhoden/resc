

import numpy as np
import dataclasses
from scipy import integrate
import matplotlib.pyplot as plt
import c_functions
from c_functions import RescTasks
from tqdm import tqdm
import datetime

@dataclasses.dataclass
class Motor:
    R:float # resistance LQ-domain
    L:float # inductance LQ-domain
    N:int # no pole pairs
    lam:float # motor constant
    J:float # inertia
    d:float # damping

    def current_torque(self,Id, Iq):
        return 3/2*self.N*Iq*self.lam

    def currents(self,Ud,Uq,omega):

        Id = 1/self.R*Ud
        Iq = 1/self.R*(Uq - self.N*omega*self.lam)

        return Id,Iq

    def ode_f(self,Ud, Uq, omega):
        # https://www.mathworks.com/help/sps/ref/pmsm.html

        Id,Iq = self.currents(Ud, Uq, omega)

        T = self.current_torque(Id, Iq)

        dot_omega = 1/self.J*(T - self.d*omega)

        return dot_omega


def park_trafo(U,V,W,theta):
    # Power invariant trafo
    # https://www.mathworks.com/help/sps/ref/parktransform.html?s_tid=doc_ta
    pi23 = np.pi*2/3
    X = np.array([U,V,W])
    P = np.array([[np.sin(theta), np.sin(theta - pi23), np.sin(theta + pi23)],
                    [np.cos(theta), np.cos(theta - pi23), np.cos(theta + pi23)]])

    Y = np.sqrt(2/3)*P@X

    return Y


def inv_park_trafo(d,q,theta):
    # power invariant inverse trafo, U to Q-axis
    # https://www.mathworks.com/help/sps/ref/inverseparktransform.html
    pi23 = np.pi*2/3
    X = np.array([d,q])
    P = np.array([[np.sin(theta), np.sin(theta - pi23), np.sin(theta + pi23)],
                [np.cos(theta), np.cos(theta - pi23), np.cos(theta + pi23)]]).T

    Y = np.sqrt(2/3)*P@X

    return Y

def wrap_angle(x):
    n = int(x/(np.pi*2))
    return x - n*2*np.pi

def sim(mtr:Motor, ts, N_sc, N_tg, Tf,resc:RescTasks):

    N = int(Tf/ts)
    signal_log = []

    DC_LINK = 12.0

    # inital state
    Y = np.zeros(4)
    Y[3] = 4.1337 # Inital angle

    # I/O
    traj_in = c_functions.TrajGen_IN(0)
    traj_out = c_functions.TrajGen_OUT(0.0,0.0,0.0)

    servo_in = c_functions.ServoCtrl_IN(0.0,0.0,0.0,0.0,0)
    servo_out = c_functions.ServoCtrl_OUT(0.0,0.0,0.0,0.0)

    curr_in = c_functions.CurrCtrl_IN(0.0,0.0,0.0,0.0,0)
    curr_out = c_functions.CurrCtrl_OUT(0.0,0.0,0.0,0.0)

    curr_in.current_ctrl_cmd = 0

    for i in tqdm(range(N)):
        ti = ts*i

        if i == 5:
            curr_in.current_ctrl_cmd = 1 # Enable DQ-offset search

        # Unpack "true states"
        Id,Iq,omega,theta = Y
        theta_elec = theta*mtr.N
        IU,IV,IW = inv_park_trafo(Id,Iq, theta_elec)

        # If trajectory generation run this step
        if i % N_tg == 0:
            resc.traj_gen(traj_in, traj_out)

        # If servo ctrl run this step
        if i % N_sc == 0:
            servo_in.ref_acc = traj_out.ref_acc
            servo_in.ref_vel = traj_out.ref_vel
            servo_in.ref_pos = traj_out.ref_pos
            servo_in.encoder_ang = wrap_angle(theta + np.deg2rad(13.37))
            resc.servo_ctrl(servo_in, servo_out)

        if curr_in.current_ctrl_cmd == 2:
            servo_out.ref_Id = 0
            servo_out.ref_Iq = 0.1

        curr_in.rotor_ang = servo_out.rotor_ang
        curr_in.rotor_vel = servo_out.rotor_vel

        curr_in.Iu = IU
        curr_in.Iv = IV
        curr_in.Iw = IW

        curr_in.ref_Id = servo_out.ref_Id
        curr_in.ref_Iq = servo_out.ref_Iq

        resc.current_ctrl(curr_in, curr_out)

        UU,UV,UW = curr_out.phase_U,curr_out.phase_V,curr_out.phase_W

        #UU,UV = UV,UU

        #UU -= 0.3
        #UV -= 0.3
        #UW -= 0.3

        UU *= DC_LINK
        UV *= DC_LINK
        UW *= DC_LINK

        # Mtr mdl
        Ud,Uq = park_trafo(UU, UV, UW, theta_elec)
        T = mtr.current_torque(Id, Iq)

        signal_log.append([ti, Id, Iq, omega,T,UU,UV,UW,theta,IU,IV,IW,
                        curr_out.debug0,curr_out.debug1,curr_out.debug2,curr_out.debug3,
                        servo_out.debug0,servo_out.debug1,servo_out.debug2,servo_out.debug3,servo_out.debug4])

        # ode-scipy
        def ode_f(t,y):

            omega,theta = y
            dot_omega = mtr.ode_f(Ud,Uq, omega)

            return (dot_omega,omega)

        Yivp = Y[2:]
        sol = integrate.solve_ivp(ode_f, [ti,ti+ts], Yivp,max_step=ts/3)
        idp,iqp = mtr.currents(Ud, Uq, omega)
        Y = np.r_[idp,iqp,sol.y[:,-1]]

    signals = np.array(signal_log)
    return signals


def plot_sim_res(Y):
    t = Y[:,0]
    Id = Y[:,1]
    Iq = Y[:,2]
    omega = Y[:,3]
    T = Y[:,4]
    UU = Y[:,5]
    UV  = Y[:,6]
    UW  = Y[:,7]
    theta  = Y[:,8]
    IU = Y[:,9]
    IV = Y[:,10]
    IW = Y[:,11]

    plt.figure(1)
    plt.plot(t,Id,label='Id')
    plt.plot(t,Iq,label='Iq')
    plt.plot(t,T,label='Torque')
    plt.legend()
    #plt.ylabel('Current')

    plt.figure(2)

    plt.subplot(2,1,1)
    plt.plot(t,omega,label='Velocity')
    plt.ylabel('Velocity')

    plt.subplot(2,1,2)
    plt.plot(t,theta,label='Position')
    plt.ylabel('Position')

    plt.figure(3)
    plt.plot(t,UU,label='U')
    plt.plot(t,UV,label='V')
    plt.plot(t,UW,label='W')
    plt.ylabel('Phase voltage')
    plt.legend()

    plt.figure(4)
    plt.plot(t,IU,label='U')
    plt.plot(t,IV,label='V')
    plt.plot(t,IW,label='W')
    plt.ylabel('Phase current')
    plt.legend()

    plt.figure()
    plt.title("Current ctrl - Debug floats")
    plt.plot(t,Y[:,12],label='Debug0')
    plt.plot(t,Y[:,13],label='Debug1')
    plt.plot(t,Y[:,14],label='Debug2')
    plt.plot(t,Y[:,15],label='Debug3')
    plt.legend()

    plt.figure()
    plt.title("Servo ctrl - Debug floats")
    plt.plot(t,Y[:,16],label='Debug0')
    plt.plot(t,Y[:,17],label='Debug1')
    plt.plot(t,Y[:,18],label='Debug2')
    plt.plot(t,Y[:,19],label='Debug3')
    plt.plot(t,Y[:,20],label='Debug3')
    plt.legend()

def main():

    print("Building")
    resc = c_functions.C_Functions()

    J = 1e-3
    d = 0.01
    mtr = Motor(0.9, 1e-4, 7, 0.009, J, d)

    TS = 0.00066
    print("Running sim")
    Y = sim(mtr, TS, N_sc=5,N_tg=10,Tf=10,resc=resc)
    print(Y.shape)

    plot_sim_res((Y))

    if False:
        time_str = datetime.datetime.now().strftime('%y%m%d_%H%M')
        fname = r'logger\logs\sim_log_' + time_str + '.npy'
        print('Saving as: ' + fname)
        np.save(fname,Y)

    plt.show()



if __name__ == '__main__':
    main()




