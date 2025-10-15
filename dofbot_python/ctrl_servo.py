"""Move one servo or a full six-joint pose from the command line.

You can supply either a single servo ID with its target angle or provide six
angles (S1–S6) to command the whole arm simultaneously.
"""

from __future__ import annotations

from Arm_Lib import Arm_Device
import time

from cli_args import build_common_parser


def parse_args():
    parser = build_common_parser(__doc__)
    parser.add_argument(
        "--servo-id",
        type=int,
        help="ID of the servo to control (1-6). Required when --angles is not used.",
    )
    parser.add_argument(
        "--angle",
        type=int,
        help="Target angle for the selected servo. Required when --angles is not used.",
    )
    parser.add_argument(
        "--move-time",
        type=int,
        default=800,
        help="Time in milliseconds for the movement (default: 800).",
    )
    parser.add_argument(
        "--angles",
        metavar=("S1", "S2", "S3", "S4", "S5", "S6"),
        type=int,
        nargs=6,
        help="If provided, move all six servos simultaneously using these angles.",
    )

    args = parser.parse_args()

    if args.move_time < 0:
        parser.error("--move-time must be zero or positive.")

    if args.angles is not None:
        if args.servo_id is not None or args.angle is not None:
            parser.error("--angles cannot be combined with --servo-id or --angle.")
    else:
        if args.servo_id is None or args.angle is None:
            parser.error("Provide --servo-id and --angle when --angles is not used.")

    return args


def validate_single(servo_id: int, angle: int) -> None:
    if servo_id < 1 or servo_id > 6:
        raise ValueError("Servo ID must be between 1 and 6.")
    if servo_id == 5:
        if not 0 <= angle <= 270:
            raise ValueError("Servo 5 angle must be between 0 and 270.")
    else:
        if not 0 <= angle <= 180:
            raise ValueError("Servo angle must be between 0 and 180.")


def validate_group(angles: list[int]) -> None:
    ranges = [(0, 180), (0, 180), (0, 180), (0, 180), (0, 270), (0, 180)]
    for idx, (value, (low, high)) in enumerate(zip(angles, ranges), start=1):
        if not low <= value <= high:
            raise ValueError(f"S{idx} must be between {low} and {high}.")


def command_single_servo(arm: Arm_Device, servo_id: int, angle: int, move_time: int) -> None:
    """Ping, move, and read back a single servo."""
    validate_single(servo_id, angle)
    # Confirm the servo responds before commanding motion.
    ping = arm.Arm_ping_servo(servo_id)
    print(f"Ping response for servo {servo_id}: {ping}")

    arm.Arm_serial_set_torque(1)
    arm.Arm_serial_servo_write(servo_id, angle, move_time)
    time.sleep(max(move_time / 1000.0, 0.1))

    current = arm.Arm_serial_servo_read(servo_id)
    print(f"Servo {servo_id} reports angle {current}°")


def command_all_servos(arm: Arm_Device, angles: list[int], move_time: int) -> None:
    """Drive all six servos to the supplied angles."""
    validate_group(angles)
    arm.Arm_serial_set_torque(1)
    arm.Arm_serial_servo_write6_array(angles, move_time)
    time.sleep(max(move_time / 1000.0, 0.1))

    feedback = []
    for servo_id in range(1, 7):
        feedback.append(arm.Arm_serial_servo_read(servo_id))
    print(
        "Commanded angles (S1-S6): "
        f"{angles} -> feedback {feedback}"
    )


def main() -> None:
    args = parse_args()
    arm = Arm_Device(args.port)
    try:
        time.sleep(args.init_delay)

        try:
            if args.angles is not None:
                command_all_servos(arm, list(args.angles), args.move_time)
            else:
                command_single_servo(arm, args.servo_id, args.angle, args.move_time)
        except ValueError as exc:
            print(f"Input error: {exc}")
    finally:
        del arm


if __name__ == "__main__":
    main()
