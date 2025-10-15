"""Choreographed demo routine for the Dofbot arm.

Loops through a collection of joint motions that showcase complex movements,
useful for demos and confirming every servo responds as expected.
"""

from Arm_Lib import Arm_Device
import time

from cli_args import parse_common_args

TIME_1 = 500
TIME_2 = 1000
TIME_SLEEP = 0.5
STEP_DELAY = 0.001


def run_routine(arm: Arm_Device) -> None:
    """Execute the looping dance routine until interrupted."""
    arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
    time.sleep(1)

    while True:
        # Arm sway forward.
        arm.Arm_serial_servo_write(2, 180 - 120, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 120, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 60, TIME_1)
        time.sleep(TIME_SLEEP)

        # Elbow/shoulder lift for a “wave” motion.
        arm.Arm_serial_servo_write(2, 180 - 135, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 135, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 45, TIME_1)
        time.sleep(TIME_SLEEP)

        # Return to neutral swing.
        arm.Arm_serial_servo_write(2, 180 - 120, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 120, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 60, TIME_1)
        time.sleep(TIME_SLEEP)

        # Pause briefly at center.
        arm.Arm_serial_servo_write(2, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 90, TIME_1)
        time.sleep(TIME_SLEEP)

        # Step through intermediate poses for variety.
        arm.Arm_serial_servo_write(2, 180 - 80, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 80, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 80, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(2, 180 - 60, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 60, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 60, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(2, 180 - 45, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 45, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 45, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(2, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(3, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 90, TIME_1)
        time.sleep(STEP_DELAY)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(4, 20, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(6, 150, TIME_1)
        time.sleep(STEP_DELAY)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(4, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(6, 90, TIME_1)
        time.sleep(TIME_SLEEP)

        # Open and close the gripper to finish the flourish.
        arm.Arm_serial_servo_write(4, 20, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(6, 150, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(4, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(6, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(1, 0, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(5, 0, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(3, 180, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 0, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(6, 180, TIME_1)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(6, 0, TIME_2)
        time.sleep(TIME_SLEEP)

        arm.Arm_serial_servo_write(6, 90, TIME_2)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(1, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(5, 90, TIME_1)
        time.sleep(TIME_SLEEP)

        # Reset to neutral before the loop restarts.
        arm.Arm_serial_servo_write(3, 90, TIME_1)
        time.sleep(STEP_DELAY)
        arm.Arm_serial_servo_write(4, 90, TIME_1)
        time.sleep(TIME_SLEEP)

        print("END OF LINE!")


def main() -> None:
    args = parse_common_args(__doc__)
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)
        try:
            run_routine(arm)
        except KeyboardInterrupt:
            print("Program closed!")
    finally:
        del arm


if __name__ == "__main__":
    main()
