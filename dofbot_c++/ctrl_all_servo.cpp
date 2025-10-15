/**
 * @file ctrl_all_servo.cpp
 * @brief Sweeps all servos across their range, mirroring the Python demo.
 */

#include "Arm_Lib.h"
#include "cli_args.h"

#include <atomic>
#include <chrono>
#include <csignal>
#include <exception>
#include <iostream>
#include <thread>

namespace {
    std::atomic<bool> g_running(true);

    void handle_signal(int signum) {
        if (signum == SIGINT) {
            g_running = false;
        }
    }

    void ctrl_all_servo(Arm_Device& arm, int angle, int move_time_ms) {
        arm.Arm_serial_servo_write6(angle, 180 - angle, angle, angle, angle, angle, move_time_ms);
        if (!g_running) {
            return;
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(move_time_ms));
    }
}

int main(int argc, char* argv[]) {
    const std::string description =
        "Continuously sweep every joint back and forth to verify coordinated motion.";

    std::signal(SIGINT, handle_signal);

    try {
        const CommonArgs args = parse_common_args(argc, argv, description);
        Arm_Device arm(args.port);

        std::this_thread::sleep_for(std::chrono::duration<double>(args.init_delay));

        int angle = 90;
        int dir_state = 1;

        std::cout << "Initializing pose..." << std::endl;
        arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Sweeping servos. Press Ctrl+C to stop." << std::endl;
        while (g_running) {
            if (dir_state == 1) {
                ++angle;
                if (angle >= 180) {
                    dir_state = 0;
                }
            } else {
                --angle;
                if (angle <= 0) {
                    dir_state = 1;
                }
            }

            ctrl_all_servo(arm, angle, 10);
            if (!g_running) {
                break;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

        std::cout << "\nProgram closed." << std::endl;
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
