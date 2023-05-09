
import ctypes as c
import os
import typing


# These should math the structure in CurrentCtrl.h
class CurrCtrl_IN(c.Structure):
    _fields_ = [('ref_Id',c.c_float), ('ref_Iq', c.c_float),
                ('rotor_ang',c.c_float), ('rotor_vel', c.c_float),
                ('current_ctrl_cmd',c.c_int),
                ('Iu',c.c_float),('Iv',c.c_float),('Iw',c.c_float)]

class CurrCtrl_OUT(c.Structure):
    _fields_ = [('phase_U',c.c_float), ('phase_V', c.c_float), ('phase_W', c.c_float), 
                ('debug0', c.c_float),('debug1', c.c_float),('debug2', c.c_float),('debug3', c.c_float) ]

# ServoCtrl.h
class ServoCtrl_IN(c.Structure):
    _fields_ = [('rotor_ang',c.c_float), ('ref_acc', c.c_float),
                ('ref_vel',c.c_float), ('ref_pos', c.c_float),
                ('servo_ctrl_cmd',c.c_int)]

class ServoCtrl_OUT(c.Structure):
    _fields_ = [('ref_Id',c.c_float), ('ref_Iq', c.c_float), ('rotor_vel', c.c_float) ]

# TrajGen.h
class TrajGen_IN(c.Structure):
    _fields_ = [('traj_cmd',c.c_int)]

class TrajGen_OUT(c.Structure):
    _fields_ = [('ref_acc',c.c_float), ('ref_vel', c.c_float), ('ref_pos', c.c_float) ]


class RescTasks(typing.Protocol):
    def current_ctrl(self,inp:CurrCtrl_IN, out:CurrCtrl_OUT):
        pass
    
    def servo_ctrl(self, inp:ServoCtrl_IN, out:ServoCtrl_OUT):
        pass

    def traj_gen(self, inp:TrajGen_IN, out:TrajGen_OUT):
        pass


class C_Functions:

    def __init__(self):
        self._compile_c_code()
        self.dll = c.CDLL("sim_out/resc_ctrl.so")

    def current_ctrl(self,inp:CurrCtrl_IN, out:CurrCtrl_OUT):
        self.dll.current_ctrl(c.byref(inp),c.byref(out))
    
    def servo_ctrl(self, inp:ServoCtrl_IN, out:ServoCtrl_OUT):
        self.dll.servo_ctrl(c.byref(inp), c.byref(out))

    def traj_gen(self, inp:TrajGen_IN, out:TrajGen_OUT):
        self.dll.traj_gen(c.byref(inp), c.byref(out))

    def _compile_c_code(self):
        build_res = os.system('cmd /c "build.bat"')
        if build_res != 0:
            raise Exception("Build failed!")

def test_current():

    inp_t = [float(i+10) for i in range(4)]
    inp_t.append(int(len(inp_t)))


    inp = CurrCtrl_IN(*inp_t)

    out = CurrCtrl_OUT(0.0,0.0,0.0)

    print('', out.phase_U, out.phase_V, out.phase_W)

    cc = C_Functions()

    cc.current_ctrl(inp,out)

    print('', out.phase_U, out.phase_V, out.phase_W)

def test_servo():

    inp_t = [float(i+10) for i in range(3)]
    inp_t.append(int(len(inp_t)))


    inp = ServoCtrl_IN(*inp_t)

    out = ServoCtrl_OUT(0.0,0.0,0.0)

    cc = C_Functions()

    cc.servo_ctrl(inp, out)


def test_traj():


    inp = TrajGen_IN(0)

    out = CurrCtrl_OUT(0.0,0.0,0.0)

    cc = C_Functions()

    cc.traj_gen(inp, out)



if __name__ == '__main__':
    test_current()
    test_servo()
    test_traj()
