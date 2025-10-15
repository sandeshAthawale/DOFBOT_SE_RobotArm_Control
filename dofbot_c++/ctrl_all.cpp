#include "Arm_Lib.h"
#include <iostream>
#include <thread>         // For std::this_thread::sleep_for
#include <chrono>         // For std::chrono::milliseconds
#include <stdexcept>
#include <csignal>        // For signal, SIGINT
#include <atomic>         // For std::atomic<bool>

// Global atomic boolean to handle Ctrl+C
std::atomic<bool> g_running(true);

// Signal handler function
void signal_handler(int signum) {
    if (signum == SIGINT) {
        g_running = false;
    }
}

/**
 * @brief Helper function to control all servos, equivalent to Python's ctrl_all_servo
 * @param arm An Arm_Device object (passed by reference)
 * @param angle The base angle to use
 * @param s_time The movement time in milliseconds
 */
void ctrl_all_servo(Arm_Device& arm, int angle, int s_time) {
    try {
        arm.Arm_serial_servo_write6(angle, 180 - angle, angle, angle, angle, angle, s_time);
        std::this_thread::sleep_for(std::chrono::milliseconds(s_time));
    } catch (const std::exception& e) {
        std::cerr << "Error in ctrl_all_servo: " << e.what() << std::endl;
    }
}

int main() {
    // Register the signal handler for SIGINT (Ctrl+C)
    signal(SIGINT, signal_handler);

    try {
        // --- Initialization ---
        // !! IMPORTANT: Change this to your serial port !!
        Arm_Device arm("/dev/ttyUSB0"); 
        
        // Sleep 0.1s, same as python
        std::this_thread::sleep_for(std::chrono::milliseconds(100)); 

        int dir_state = 1;
        int angle = 90;

        // --- Set initial position ---
        std::cout << "Setting initial position (90 degrees)..." << std::endl;
        arm.Arm_serial_servo_write6(90, 90, 90, 90, 90, 90, 500);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // --- Main Loop ---
        std::cout << "Starting main loop... Press Ctrl+C to stop." << std::endl;
        while (g_running) {
            if (dir_state == 1) {
                angle++;
                if (angle >= 180) {
                    dir_state = 0;
                }
            } else {
                angle--;
                if (angle <= 0) {
                    dir_state = 1;
                }
            }

            // std::cout is line-buffered, std::endl flushes the buffer
            std::cout << "Angle: " << angle << "   \r" << std::flush; 

            ctrl_all_servo(arm, angle, 10);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }

    } catch (const std::exception& e) {
        std::cerr << "A critical error occurred: " << e.what() << std::endl;
        std::cerr << "Please check your serial port name and permissions." << std::endl;
        return 1; // Exit with an error code
    }

    std::cout << "\nProgram closed!" << std::endl;
    return 0; // The Arm_Device destructor will be called automatically
}
