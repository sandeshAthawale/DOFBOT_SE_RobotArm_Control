"""
Shared command-line argument utilities for Dofbot helper scripts.

All scripts can import `parse_common_args` to reuse the same definitions
for the serial port and initial delay, so team members only need to adjust
defaults in this single place.
"""

from __future__ import annotations

import argparse
from typing import Iterable, Optional

DEFAULT_PORT = "/dev/tty.usbserial-2130"
DEFAULT_INIT_DELAY = 0.1


def build_common_parser(description: Optional[str] = None) -> argparse.ArgumentParser:
    """Create an ArgumentParser populated with the shared options."""
    parser = argparse.ArgumentParser(description=description)
    parser.add_argument(
        "--port",
        default=DEFAULT_PORT,
        help="Serial device name used to communicate with the arm "
        f"(default: {DEFAULT_PORT})",
    )
    parser.add_argument(
        "--init-delay",
        type=float,
        default=DEFAULT_INIT_DELAY,
        help="Seconds to wait after creating the Arm_Device connection before sending commands "
        f"(default: {DEFAULT_INIT_DELAY})",
    )
    return parser


def parse_common_args(
    description: Optional[str] = None, argv: Optional[Iterable[str]] = None
) -> argparse.Namespace:
    """Parse the shared arguments and return the populated namespace."""
    parser = build_common_parser(description=description)
    return parser.parse_args(args=argv)
