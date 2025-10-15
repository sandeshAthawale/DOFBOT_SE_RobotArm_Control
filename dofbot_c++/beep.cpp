/**
 * @file beep.cpp
 * @brief Demonstrates the buzzer control commands.
 */

#include "Arm_Lib.h"
#include "cli_args.h"

#include <chrono>
#include <exception>
#include <iostream>
#include <thread>

int main(int argc, char* argv[]) {
    const std::string description =
        "Buzzer demo for the Dofbot arm. Plays a few tones to confirm communication.";

    try {
        const CommonArgs args = parse_common_args(argc, argv, description);
        Arm_Device arm(args.port);

        std::this_thread::sleep_for(std::chrono::duration<double>(args.init_delay));

        std::cout << "Short beep..." << std::endl;
        arm.Arm_Buzzer_On(1);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Longer beep..." << std::endl;
        arm.Arm_Buzzer_On(3);
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Continuous tone..." << std::endl;
        arm.Arm_Buzzer_On();
        std::this_thread::sleep_for(std::chrono::seconds(1));

        std::cout << "Silencing buzzer." << std::endl;
        arm.Arm_Buzzer_Off();
        std::this_thread::sleep_for(std::chrono::seconds(1));

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
