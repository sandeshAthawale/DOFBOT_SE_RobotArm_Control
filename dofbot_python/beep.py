"""Buzzer demo for the Dofbot arm.

Runs a short sequence that exercises the onboard buzzer with different
durations so team members can quickly confirm the arm is responsive.
"""

from Arm_Lib import Arm_Device
import time

from cli_args import parse_common_args


def main() -> None:
    args = parse_common_args(__doc__)
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)

        # Brief beep to verify hardware responds to commands.
        arm.Arm_Buzzer_On(1)
        time.sleep(1)

        # Slightly longer chirp for comparison.
        arm.Arm_Buzzer_On(3)
        time.sleep(1)

        # Hold the buzzer on until we explicitly switch it off.
        arm.Arm_Buzzer_On()
        time.sleep(1)

        # Silence the buzzer before exiting so the arm is quiet.
        arm.Arm_Buzzer_Off()
        time.sleep(1)
    finally:
        del arm


if __name__ == "__main__":
    main()
