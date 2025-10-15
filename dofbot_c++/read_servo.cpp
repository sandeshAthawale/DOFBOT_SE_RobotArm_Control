/**
 * @file read_servo.cpp
 * @brief Continuously reads and prints all servo angles.
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
}

int main(int argc, char* argv[]) {
    const std::string description =
        "Poll servos 1-6, printing their ping response and reported angles.";

    std::signal(SIGINT, handle_signal);

    try {
        const CommonArgs args = parse_common_args(argc, argv, description);
        Arm_Device arm(args.port);

        std::this_thread::sleep_for(std::chrono::duration<double>(args.init_delay));

        while (g_running) {
            for (int id = 1; id <= 6 && g_running; ++id) {
                const int ping = arm.Arm_ping_servo(id);
                arm.Arm_serial_set_torque(1);
                const int angle = arm.Arm_serial_servo_read(id);
                std::cout << "Servo " << id << " ping: " << ping << ", angle: " << angle << "°" << std::endl;
                std::this_thread::sleep_for(std::chrono::milliseconds(100));
            }
        }

        std::cout << "\nProgram closed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
