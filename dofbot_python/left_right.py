"""Left-right sweeping routine for the Dofbot arm.

Continuously swings the arm across its workspace to ensure the shoulder and
elbow servos respond correctly.
"""

from Arm_Lib import Arm_Device
import time

from cli_args import parse_common_args


def run_sweep(arm: Arm_Device) -> None:
    """Send the left-right pattern until interrupted."""
    arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500)
    time.sleep(1)

    while True:
        # Push the wrist forward to prepare for the swing.
        arm.Arm_serial_servo_write(3, 0, 1000)
        time.sleep(0.001)
        arm.Arm_serial_servo_write(4, 180, 1000)
        time.sleep(1)

        # Sweep the base left and right.
        arm.Arm_serial_servo_write(1, 180, 500)
        time.sleep(0.5)
        arm.Arm_serial_servo_write(1, 0, 1000)
        time.sleep(1)

        # Return every joint to neutral before the next loop.
        arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 1000)
        time.sleep(1.5)


def main() -> None:
    args = parse_common_args(__doc__)
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)
        try:
            run_sweep(arm)
        except KeyboardInterrupt:
            print("Program closed!")
    finally:
        del arm


if __name__ == "__main__":
    main()
