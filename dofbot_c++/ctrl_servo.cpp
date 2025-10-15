/**
 * @file ctrl_servo.cpp
 * @brief Command a single servo or a full six-joint pose from the CLI.
 */

#include "Arm_Lib.h"
#include "cli_args.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <exception>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>
#include <vector>

namespace {
    const char* kUsageSuffix =
        "\nAdditional parameters:\n"
        "  --servo-id ID       Servo ID to control (1-6). Required when --angles is absent.\n"
        "  --angle DEG         Target angle for the selected servo.\n"
        "  --move-time MS      Movement time in milliseconds (default: 800).\n"
        "  --angles S1 S2 S3 S4 S5 S6\n"
        "                      Command all six servos simultaneously.";

    const std::array<const char*, 6> kServoNames = {"S1", "S2", "S3", "S4", "S5", "S6"};

    const std::string& expect_value(const std::vector<std::string>& tokens, size_t& index) {
        if (index + 1 >= tokens.size()) {
            throw std::runtime_error("Missing value for argument: " + tokens[index]);
        }
        return tokens[++index];
    }
    void validate_single(int servo_id, int angle) {
        if (servo_id < 1 || servo_id > 6) {
            throw std::runtime_error("Servo ID must be between 1 and 6.");
        }
        const int max_angle = (servo_id == 5) ? 270 : 180;
        if (angle < 0 || angle > max_angle) {
            std::ostringstream oss;
            oss << "Servo " << servo_id << " angle must be between 0 and " << max_angle << '.';
            throw std::runtime_error(oss.str());
        }
    }

    void validate_group(const std::array<int, 6>& angles) {
        const std::array<int, 6> max_angles = {180, 180, 180, 180, 270, 180};
        for (size_t idx = 0; idx < angles.size(); ++idx) {
            if (angles[idx] < 0 || angles[idx] > max_angles[idx]) {
                std::ostringstream oss;
                oss << "S" << (idx + 1) << " angle must be between 0 and " << max_angles[idx] << '.';
                throw std::runtime_error(oss.str());
            }
        }
    }
}

int main(int argc, char* argv[]) {
    const std::string description =
        "Move one servo or the entire arm to specified angles." + std::string(kUsageSuffix);

    try {
        std::vector<std::string> extra_args;
        const CommonArgs common = parse_common_args(argc, argv, description, &extra_args);

        int servo_id = -1;
        int angle = -1;
        int move_time = 800;
        std::array<int, 6> target_angles = {90, 90, 90, 90, 90, 90};
        bool has_group_command = false;

        for (size_t i = 0; i < extra_args.size(); ++i) {
            const std::string& token = extra_args[i];

            if (token == "--servo-id") {
                servo_id = std::stoi(expect_value(extra_args, i));
            } else if (token.rfind("--servo-id=", 0) == 0) {
                servo_id = std::stoi(token.substr(11));
            } else if (token == "--angle") {
                angle = std::stoi(expect_value(extra_args, i));
            } else if (token.rfind("--angle=", 0) == 0) {
                angle = std::stoi(token.substr(8));
            } else if (token == "--move-time") {
                move_time = std::stoi(expect_value(extra_args, i));
            } else if (token.rfind("--move-time=", 0) == 0) {
                move_time = std::stoi(token.substr(12));
            } else if (token == "--angles") {
                if (i + 6 >= extra_args.size()) {
                    throw std::runtime_error("--angles expects six values.");
                }
                for (int j = 0; j < 6; ++j) {
                    target_angles[j] = std::stoi(extra_args[++i]);
                }
                has_group_command = true;
            } else {
                std::cerr << "Unrecognized argument: " << token << '\n';
                return 1;
            }
        }

        if (move_time < 0) {
            throw std::runtime_error("--move-time must be zero or positive.");
        }

        if (has_group_command) {
            if (servo_id != -1 || angle != -1) {
                throw std::runtime_error("--angles cannot be combined with --servo-id/--angle.");
            }
        } else {
            if (servo_id == -1 || angle == -1) {
                throw std::runtime_error("Provide --servo-id and --angle, or use --angles S1 S2 S3 S4 S5 S6.");
            }
        }

        Arm_Device arm(common.port);
        std::this_thread::sleep_for(std::chrono::duration<double>(common.init_delay));

        const double wait_seconds = std::max(move_time > 0 ? move_time / 1000.0 : 0.0, 0.1);

        if (has_group_command) {
            validate_group(target_angles);
            arm.Arm_serial_set_torque(1);
            arm.Arm_serial_servo_write6(
                target_angles[0],
                target_angles[1],
                target_angles[2],
                target_angles[3],
                target_angles[4],
                target_angles[5],
                move_time
            );
            std::this_thread::sleep_for(std::chrono::duration<double>(wait_seconds));

            std::array<int, 6> feedback{};
            for (int id = 1; id <= 6; ++id) {
                feedback[id - 1] = arm.Arm_serial_servo_read(id);
            }

            std::cout << "Commanded angles: ";
            for (size_t idx = 0; idx < target_angles.size(); ++idx) {
                std::cout << kServoNames[idx] << '=' << target_angles[idx] << (idx + 1 < target_angles.size() ? ", " : "");
            }
            std::cout << "\nFeedback angles: ";
            for (size_t idx = 0; idx < feedback.size(); ++idx) {
                std::cout << kServoNames[idx] << '=' << feedback[idx] << (idx + 1 < feedback.size() ? ", " : "");
            }
            std::cout << std::endl;

        } else {
            validate_single(servo_id, angle);
            const int ping_response = arm.Arm_ping_servo(servo_id);
            std::cout << "Ping response for servo " << servo_id << ": " << ping_response << std::endl;

            arm.Arm_serial_set_torque(1);
            arm.Arm_serial_servo_write(servo_id, angle, move_time);
            std::this_thread::sleep_for(std::chrono::duration<double>(wait_seconds));

            const int feedback = arm.Arm_serial_servo_read(servo_id);
            std::cout << "Servo " << servo_id << " reports angle: " << feedback << "°" << std::endl;
        }

    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    return 0;
}
