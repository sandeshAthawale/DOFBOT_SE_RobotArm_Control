"""Sweep all servos through their full range.

Moves every joint back and forth so you can observe coordinated motion and
verify wiring/power without editing the script manually.
"""

from Arm_Lib import Arm_Device
import time

from cli_args import parse_common_args


def ctrl_all_servo(arm: Arm_Device, angle: int, s_time: int = 500) -> None:
    """Command every servo to the supplied angle with a shared duration."""
    arm.Arm_serial_servo_write6(
        angle, 180 - angle, angle, angle, angle, angle, s_time
    )
    # Give the hardware time to complete the motion before the next step.
    time.sleep(s_time / 1000)


def run_sequence(arm: Arm_Device) -> None:
    """Continuously sweep the servos until interrupted."""
    dir_state = 1
    angle = 90
    arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
    time.sleep(1)

    while True:
        if dir_state == 1:
            angle += 1
            if angle >= 180:
                dir_state = 0
        else:
            angle -= 1
            if angle <= 0:
                dir_state = 1

        # Move all servos in small increments to create a smooth sweep.
        ctrl_all_servo(arm, angle, 10)
        time.sleep(10 / 1000)


def main() -> None:
    args = parse_common_args(__doc__)
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)
        try:
            run_sequence(arm)
        except KeyboardInterrupt:
            print("Program closed!")
    finally:
        del arm


if __name__ == "__main__":
    main()
