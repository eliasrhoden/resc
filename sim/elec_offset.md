
# Encoder angle offset

* Rotor angle $\theta_R$
* Encoder angle $\theta_E$
* Offset $\theta_0$


Assume the measured angle contains an offset

$$
\theta_E = \theta_R + \theta_0
$$

The angle used by the *true* park transforms $\theta_P$, depend on the rotor angle and the no of polepairs $n_p$

$$
\theta_P = n_p \theta_R
$$


## Method

For finding the offset, the motor is run in open-loop mode. Where the phase voltages are applied with the park angle $\theta_P^*$ and the offset (and nr of polepairs).

$$
\theta_P^* = \beta_0 + \beta_1 \theta_E
$$


$$
n_p \theta_R = \beta_0 + \beta_1 \theta_E
$$

$$
n_p (\theta_E - \theta_0) = \beta_0 + \beta_1 \theta_E
$$

$$
n_p = \beta_1
$$

$$
\theta_0 = \dfrac{\beta_0}{n_p}
$$


# NOTE
This has been confirmed to work in SIM when the direction matches, i.e. a positive electrical velocity, yields a positive encoder velocity. Flip two phases if the motor turns in the wrong dir.



