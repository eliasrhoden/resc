




class PythonResc:

    def __init__(self):
        self.old_theta = 0

    def current_ctrl(self,inp:CurrCtrl_IN, out:CurrCtrl_OUT):
        pass
    
    def servo_ctrl(self, inp:ServoCtrl_IN, out:ServoCtrl_OUT):
        Ts = 0.0006*5
        d_theta = (inp.rotor_ang - self.old_theta)*1/0.006

        pass

    def traj_gen(self, inp:TrajGen_IN, out:TrajGen_OUT):
        pass


