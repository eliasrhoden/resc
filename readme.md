# RESC

Inspired by: https://vesc-project.com/

## Hardware
Running on a STM-Nucleo F446RE ([st.com/en/evaluation-tools/nucleo-f446re](https://www.st.com/en/evaluation-tools/nucleo-f446re.html))

Electrical components are listed in `hardware.md`.

## Repo structure
* 3dprint - Files for 3D-printing
* adc sampling - Calculations regarding ADC sampling and timing
* docs - Figures for documentation
* logger - Stuff related to logging
* MCU Manuals - STM-manuals
* notebook - Python Jupyter notebook for concept, development and analytics
* notes - Stuff to remember
* sim - Python simulation
* sim_out - Binary outputs for simulation
* stm32_cube_worksace - Workspace for STM32CubeIDE
    - Everything in `stm32_cube_workspace\resc\resc` is build and run when running the simulation.

