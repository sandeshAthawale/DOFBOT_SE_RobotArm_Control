/**
 * @file cli_args.h
 * @brief Shared command-line argument utilities for Dofbot demos.
 */

#ifndef DOFBOT_CLI_ARGS_H
#define DOFBOT_CLI_ARGS_H

#include <string>
#include <vector>

struct CommonArgs {
    std::string port = "/dev/tty.usbserial-2130";
    double init_delay = 0.1;
};

/**
 * @brief Parse shared CLI parameters (--port, --init-delay).
 *        Prints usage and exits when --help is provided.
 */
CommonArgs parse_common_args(int argc, char* argv[], const std::string& description,
                             std::vector<std::string>* remaining_args = nullptr);

#endif // DOFBOT_CLI_ARGS_H
