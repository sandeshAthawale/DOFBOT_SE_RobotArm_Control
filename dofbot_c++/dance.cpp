/**
 * @file dance.cpp
 * @brief Choreographed routine showcasing compound arm motions.
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
    constexpr int TIME_1 = 500;
    constexpr int TIME_2 = 1000;
    constexpr double TIME_SLEEP = 0.5;
    constexpr double STEP_DELAY = 0.001;

    std::atomic<bool> g_running(true);

    void handle_signal(int signum) {
        if (signum == SIGINT) {
            g_running = false;
        }
    }

    void sleep_seconds(double seconds) {
        std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
    }

    void run_routine(Arm_Device& arm) {
        arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500);
        sleep_seconds(1.0);

        while (g_running) {
            arm.Arm_serial_servo_write(2, 180 - 120, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 120, TIME_1);
            if (!g_running) break;
            sleep_seconds(STEP_DELAY);
            arm.Arm_serial_servo_write(4, 60, TIME_1);
            if (!g_running) break;
            sleep_seconds(TIME_SLEEP);

            arm.Arm_serial_servo_write(2, 180 - 135, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 135, TIME_1);
            if (!g_running) break;
            sleep_seconds(STEP_DELAY);
            arm.Arm_serial_servo_write(4, 45, TIME_1);
            if (!g_running) break;
            sleep_seconds(TIME_SLEEP);

            arm.Arm_serial_servo_write(2, 180 - 120, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 120, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 60, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(2, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 90, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(2, 180 - 80, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 80, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 80, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(2, 180 - 60, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 60, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 60, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(2, 180 - 45, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 45, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 45, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(2, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(3, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(4, 20, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(6, 150, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(4, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(6, 90, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(4, 20, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(6, 150, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(4, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(6, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(1, 0, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(5, 0, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(3, 180, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 0, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(6, 180, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(6, 0, TIME_2);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(6, 90, TIME_2);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(1, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(5, 90, TIME_1);
            sleep_seconds(TIME_SLEEP);
            if (!g_running) break;

            arm.Arm_serial_servo_write(3, 90, TIME_1);
            sleep_seconds(STEP_DELAY);
            if (!g_running) break;
            arm.Arm_serial_servo_write(4, 90, TIME_1);
            sleep_seconds(TIME_SLEEP);

            std::cout << "END OF LINE!" << std::endl;
        }
    }
}

int main(int argc, char* argv[]) {
    const std::string description =
        "Looping dance routine that moves multiple joints for demonstration purposes.";

    std::signal(SIGINT, handle_signal);

    try {
        const CommonArgs args = parse_common_args(argc, argv, description);
        Arm_Device arm(args.port);

        std::this_thread::sleep_for(std::chrono::duration<double>(args.init_delay));
        run_routine(arm);

        std::cout << "Program closed." << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
