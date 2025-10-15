/**
 * @file left_right.cpp
 * @brief Sweeps the arm left and right to validate shoulder joints.
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

    void reset_pose(Arm_Device& arm) {
        arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500);
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

int main(int argc, char* argv[]) {
    const std::string description =
        "Continuous left/right sweeping routine to observe base and elbow behaviour.";

    std::signal(SIGINT, handle_signal);

    try {
        const CommonArgs args = parse_common_args(argc, argv, description);
        Arm_Device arm(args.port);

        std::this_thread::sleep_for(std::chrono::duration<double>(args.init_delay));
        reset_pose(arm);

        std::cout << "Running sweep. Press Ctrl+C to stop." << std::endl;
        while (g_running) {
            arm.Arm_serial_servo_write(3, 0, 1000);
            if (!g_running) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 180, 1000);
            if (!g_running) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!g_running) break;

            arm.Arm_serial_servo_write(1, 180, 500);
            if (!g_running) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(500));
            if (!g_running) break;
            arm.Arm_serial_servo_write(1, 0, 1000);
            if (!g_running) break;
            std::this_thread::sleep_for(std::chrono::seconds(1));
            if (!g_running) break;

            arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 1000);
            if (!g_running) break;
            std::this_thread::sleep_for(std::chrono::milliseconds(1500));
        }

        std::cout << "\nProgram closed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
