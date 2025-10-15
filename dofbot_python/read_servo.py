"""Utility script to read and print each servo angle.

Loops through IDs 1-6, pings each servo, and reports the measured angle so
you can monitor joint feedback while troubleshooting.
"""

from Arm_Lib import Arm_Device
import time

from cli_args import parse_common_args


def poll_servos(arm: Arm_Device) -> None:
    """Continuously read all servos until interrupted."""
    while True:
        for servo_id in range(1, 7):
            # Check that the servo still responds on the bus.
            ping = arm.Arm_ping_servo(servo_id)
            arm.Arm_serial_set_torque(1)
            # Read the current angle so operators can track live feedback.
            angle = arm.Arm_serial_servo_read(servo_id)
            print(f"servo ID: {servo_id}, ping: {ping}, angle: {angle}")
            time.sleep(0.1)


def main() -> None:
    args = parse_common_args(__doc__)
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)
        try:
            poll_servos(arm)
        except KeyboardInterrupt:
            print("Program closed!")
    finally:
        del arm


if __name__ == "__main__":
    main()
