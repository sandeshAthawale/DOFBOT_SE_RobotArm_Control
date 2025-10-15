#include "cli_args.h"

#include <cstdlib>
#include <iostream>
#include <sstream>
#include <vector>

namespace {
    bool matches_prefix(const std::string& arg, const std::string& prefix, std::string& value) {
        if (arg.rfind(prefix, 0) == 0 && arg.size() > prefix.size()) {
            value = arg.substr(prefix.size());
            return true;
        }
        return false;
    }

    void print_usage(const std::string& program, const std::string& description) {
        if (!description.empty()) {
            std::cout << description << "\n\n";
        }
        std::cout << "Usage: " << program << " [--port PATH] [--init-delay SECONDS]\n";
        std::cout << "  --port         Serial device path (default: /dev/tty.usbserial-2130)\n";
        std::cout << "  --init-delay   Seconds to wait after connecting before commands (default: 0.1)\n";
        std::cout << "  --help         Show this message and exit\n";
    }
}

CommonArgs parse_common_args(int argc, char* argv[], const std::string& description,
                             std::vector<std::string>* remaining_args) {
    CommonArgs args;

    const std::string program = (argc > 0) ? argv[0] : "dofbot_demo";
    for (int i = 1; i < argc; ++i) {
        std::string current = argv[i];
        if (current == "--help" || current == "-h") {
            print_usage(program, description);
            std::exit(0);
        } else if (current == "--port" && i + 1 < argc) {
            args.port = argv[++i];
        } else if (current == "--init-delay" && i + 1 < argc) {
            args.init_delay = std::stod(argv[++i]);
        } else {
            std::string value;
            if (matches_prefix(current, "--port=", value)) {
                args.port = value;
            } else if (matches_prefix(current, "--init-delay=", value)) {
                args.init_delay = std::stod(value);
            } else {
                if (remaining_args) {
                    remaining_args->push_back(current);
                } else {
                    std::cerr << "Unrecognized argument: " << current << "\n";
                    print_usage(program, description);
                    std::exit(1);
                }
            }
        }
    }

    return args;
}
