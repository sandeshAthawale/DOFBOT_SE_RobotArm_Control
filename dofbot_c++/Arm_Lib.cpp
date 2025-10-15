#include "Arm_Lib.h"
#include <iostream>
#include <stdexcept>
#include <cstring>      // For bzero, strerror
#include <unistd.h>     // For open, close, write, usleep
#include <fcntl.h>      // For O_RDWR, O_NOCTTY
#include <termios.h>    // For termios, tcgetattr, tcsetattr
#include <cerrno>       // For errno
#include <sys/select.h> // For select
#include <sys/time.h>
#include <cmath>

// Constructor: Opens and configures the serial port
Arm_Device::Arm_Device(const std::string& com) : port_name(com), ser_fd(-1) {
    // Open the serial port
    // O_RDWR: Read/Write
    // O_NOCTTY: Not the controlling terminal
    // O_NDELAY: Non-blocking (we'll set timeouts later)
    ser_fd = open(port_name.c_str(), O_RDWR | O_NOCTTY);
    if (ser_fd == -1) {
        throw std::runtime_error("Failed to open serial port: " + port_name + " - " + strerror(errno));
    }

    // Configure the serial port
    struct termios tty;
    std::memset(&tty, 0, sizeof(tty));

    if (tcgetattr(ser_fd, &tty) != 0) {
        close(ser_fd);
        throw std::runtime_error("Failed to get serial attributes: " + std::string(strerror(errno)));
    }

    // Set Baud Rate (115200)
    cfsetospeed(&tty, B115200);
    cfsetispeed(&tty, B115200);

    // Set Port Settings
    tty.c_cflag &= ~PARENB;        // No parity
    tty.c_cflag &= ~CSTOPB;        // 1 stop bit
    tty.c_cflag &= ~CSIZE;         // Clear data size bits
    tty.c_cflag |= CS8;            // 8 data bits
    tty.c_cflag &= ~CRTSCTS;       // No hardware flow control
    tty.c_cflag |= CREAD | CLOCAL; // Enable receiver, ignore modem control lines

    // Local flags (non-canonical mode)
    tty.c_lflag &= ~ICANON; // Disable canonical mode
    tty.c_lflag &= ~ECHO;   // Disable echo
    tty.c_lflag &= ~ECHOE;  // Disable erasure
    tty.c_lflag &= ~ECHONL; // Disable newline echo
    tty.c_lflag &= ~ISIG;   // Disable interpretation of INTR, QUIT and SUSP

    // Input flags (ignore parity)
    tty.c_iflag &= ~(IXON | IXOFF | IXANY); // Disable software flow control
    tty.c_iflag &= ~(IGNBRK|BRKINT|PARMRK|ISTRIP|INLCR|IGNCR|ICRNL); // Disable special handling
    tty.c_iflag |= IGNPAR;

    // Output flags (raw output)
    tty.c_oflag = 0;

    // Set Timeouts (VMIN = 0, VTIME = 2)
    // This matches the Python 'timeout=.2' (2 deciseconds = 0.2s)
    tty.c_cc[VMIN] = 0;
    tty.c_cc[VTIME] = 2;

    // Apply the settings
    if (tcsetattr(ser_fd, TCSANOW, &tty) != 0) {
        close(ser_fd);
        throw std::runtime_error("Failed to set serial attributes: " + std::string(strerror(errno)));
    }

    // Sleep for 0.2s to let the port initialize, just like the Python code
    usleep(200000); 
    std::cout << "Serial port " << port_name << " opened successfully." << std::endl;
}

// Destructor: Closes the serial port
Arm_Device::~Arm_Device() {
    if (ser_fd != -1) {
        close(ser_fd);
        std::cout << "\nSerial port " << port_name << " closed." << std::endl;
    }
}

// Maps an angle to a servo position value
uint16_t Arm_Device::map_angle_to_pos(int angle, int in_min, int in_max, int out_min, int out_max) {
    long result = (long)(angle - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    return static_cast<uint16_t>(result);
}

// Calculates the checksum
uint8_t Arm_Device::calculate_checksum(const std::vector<uint8_t>& cmd) {
    // Start with the complement value (5), just like the Python code `sum(cmd, 5)`
    uint16_t sum = __COMPLEMENT; 
    for (uint8_t byte : cmd) {
        sum += byte;
    }
    return static_cast<uint8_t>(sum & 0xFF); // Return the low byte
}

// Writes data to the serial port
void Arm_Device::write_serial(const std::vector<uint8_t>& data) {
    if (ser_fd == -1) {
        throw std::runtime_error("Serial port is not open.");
    }
    ssize_t n = write(ser_fd, data.data(), data.size());
    if (n < 0) {
        throw std::runtime_error("Serial write error: " + std::string(strerror(errno)));
    }
    if (n < static_cast<ssize_t>(data.size())) {
         std::cerr << "Warning: Only wrote " << n << " of " << data.size() << " bytes." << std::endl;
    }
}

bool Arm_Device::read_byte(uint8_t& byte, unsigned int timeout_ms) {
    if (ser_fd == -1) {
        return false;
    }

    fd_set readfds;
    FD_ZERO(&readfds);
    FD_SET(ser_fd, &readfds);

    struct timeval tv;
    tv.tv_sec = timeout_ms / 1000;
    tv.tv_usec = (timeout_ms % 1000) * 1000;

    int result = select(ser_fd + 1, &readfds, nullptr, nullptr, &tv);
    if (result <= 0) {
        return false;
    }

    ssize_t n = ::read(ser_fd, &byte, 1);
    if (n <= 0) {
        return false;
    }

    return true;
}

bool Arm_Device::read_response(uint8_t& ext_type, std::vector<uint8_t>& payload, unsigned int timeout_ms) {
    uint8_t head = 0;
    while (read_byte(head, timeout_ms)) {
        if (head != __HEAD) {
            continue;
        }

        uint8_t device_id_minus_one = 0;
        if (!read_byte(device_id_minus_one, timeout_ms)) {
            return false;
        }
        if (device_id_minus_one != (__DEVICE_ID - 1)) {
            continue;
        }

        uint8_t ext_len = 0;
        if (!read_byte(ext_len, timeout_ms)) {
            return false;
        }
        if (!read_byte(ext_type, timeout_ms)) {
            return false;
        }

        int data_len = static_cast<int>(ext_len) - 2;
        if (data_len <= 0) {
            return false;
        }

        std::vector<uint8_t> buffer;
        buffer.reserve(data_len > 0 ? data_len - 1 : 0);
        uint16_t check_sum = ext_len + ext_type;
        uint8_t rx_check_num = 0;

        for (int i = 0; i < data_len; ++i) {
            uint8_t value = 0;
            if (!read_byte(value, timeout_ms)) {
                return false;
            }
            if (i == data_len - 1) {
                rx_check_num = value;
            } else {
                check_sum += value;
                buffer.push_back(value);
            }
        }

        if ((check_sum % 256) != rx_check_num) {
            std::cerr << "Checksum mismatch while reading response." << std::endl;
            continue;
        }

        payload = std::move(buffer);
        return true;
    }
    return false;
}

// The main function to control all 6 servos
void Arm_Device::Arm_serial_servo_write6(int s1, int s2, int s3, int s4, int s5, int s6, int time) {
    // Check angle ranges
    if (s1 > 180 || s2 > 180 || s3 > 180 || s4 > 180 || s5 > 270 || s6 > 180 ||
        s1 < 0 || s2 < 0 || s3 < 0 || s4 < 0 || s5 < 0 || s6 < 0) {
        throw std::out_of_range("Angle parameter is out of range.");
    }

    // Invert angles for servos 2, 3, 4 as in the Python code
    s2 = 180 - s2;
    s3 = 180 - s3;
    s4 = 180 - s4;

    // Map angles to position values
    uint16_t pos1 = map_angle_to_pos(s1, 0, 180, 900, 3100);
    uint16_t pos2 = map_angle_to_pos(s2, 0, 180, 900, 3100);
    uint16_t pos3 = map_angle_to_pos(s3, 0, 180, 900, 3100);
    uint16_t pos4 = map_angle_to_pos(s4, 0, 180, 900, 3100);
    uint16_t pos5 = map_angle_to_pos(s5, 0, 270, 380, 3700);
    uint16_t pos6 = map_angle_to_pos(s6, 0, 180, 900, 3100);

    // Split time into high and low bytes
    uint8_t time_H = (time >> 8) & 0xFF;
    uint8_t time_L = time & 0xFF;

    // Build the command packet
    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x11, // Length (17 bytes)
        0x1d, // Command ID
        static_cast<uint8_t>((pos1 >> 8) & 0xFF), // s1 High Byte
        static_cast<uint8_t>(pos1 & 0xFF),       // s1 Low Byte
        static_cast<uint8_t>((pos2 >> 8) & 0xFF), // s2 High Byte
        static_cast<uint8_t>(pos2 & 0xFF),       // s2 Low Byte
        static_cast<uint8_t>((pos3 >> 8) & 0xFF), // s3 High Byte
        static_cast<uint8_t>(pos3 & 0xFF),       // s3 Low Byte
        static_cast<uint8_t>((pos4 >> 8) & 0xFF), // s4 High Byte
        static_cast<uint8_t>(pos4 & 0xFF),       // s4 Low Byte
        static_cast<uint8_t>((pos5 >> 8) & 0xFF), // s5 High Byte
        static_cast<uint8_t>(pos5 & 0xFF),       // s5 Low Byte
        static_cast<uint8_t>((pos6 >> 8) & 0xFF), // s6 High Byte
        static_cast<uint8_t>(pos6 & 0xFF),       // s6 Low Byte
        time_H,
        time_L
    };

    // Calculate and append the checksum
    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    // Send the command
    try {
        write_serial(cmd);
    } catch (const std::exception& e) {
        std::cerr << "Arm_serial_servo_write6 serial error: " << e.what() << std::endl;
    }
}

void Arm_Device::Arm_serial_servo_write(int id, int angle, int time) {
    if (id == 0) {
        Arm_serial_servo_write6(angle, angle, angle, angle, angle, angle, time);
        return;
    }

    if (id < 1 || id > 6) {
        throw std::out_of_range("Servo ID must be between 1 and 6.");
    }

    if ((id == 2 || id == 3 || id == 4) && (angle >= 0 && angle <= 180)) {
        angle = 180 - angle;
    }

    uint16_t pos = 0;
    if (id == 5) {
        if (angle < 0 || angle > 270) {
            throw std::out_of_range("Servo 5 angle must be between 0 and 270.");
        }
        pos = map_angle_to_pos(angle, 0, 270, 380, 3700);
    } else {
        if (angle < 0 || angle > 180) {
            throw std::out_of_range("Servo angle must be between 0 and 180.");
        }
        pos = map_angle_to_pos(angle, 0, 180, 900, 3100);
    }

    uint8_t value_H = static_cast<uint8_t>((pos >> 8) & 0xFF);
    uint8_t value_L = static_cast<uint8_t>(pos & 0xFF);
    uint8_t time_H = static_cast<uint8_t>((time >> 8) & 0xFF);
    uint8_t time_L = static_cast<uint8_t>(time & 0xFF);

    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x07,
        static_cast<uint8_t>(0x10 + id),
        value_H,
        value_L,
        time_H,
        time_L
    };

    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    try {
        write_serial(cmd);
    } catch (const std::exception& e) {
        std::cerr << "Arm_serial_servo_write serial error: " << e.what() << std::endl;
    }
}

void Arm_Device::Arm_serial_set_torque(int onoff) {
    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x04,
        0x1A,
        static_cast<uint8_t>(onoff ? 0x01 : 0x00)
    };

    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    try {
        write_serial(cmd);
    } catch (const std::exception& e) {
        std::cerr << "Arm_serial_set_torque serial error: " << e.what() << std::endl;
    }
}

int Arm_Device::Arm_ping_servo(int id) {
    if (id <= 0 || id > 250) {
        throw std::out_of_range("Servo ID must be between 1 and 250.");
    }

    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x04,
        0x38,
        static_cast<uint8_t>(id)
    };

    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    try {
        write_serial(cmd);
    } catch (const std::exception& e) {
        std::cerr << "Arm_ping_servo serial error: " << e.what() << std::endl;
        return 0;
    }

    uint8_t ext_type = 0;
    std::vector<uint8_t> payload;
    if (read_response(ext_type, payload)) {
        if (!payload.empty()) {
            return payload[0];
        }
    }
    return 0;
}

int Arm_Device::Arm_serial_servo_read(int id) {
    if (id < 1 || id > 6) {
        throw std::out_of_range("Servo ID must be between 1 and 6.");
    }

    uint8_t command_id = static_cast<uint8_t>(id + 0x30);
    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x03,
        command_id
    };

    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    uint8_t ext_type = 0;
    std::vector<uint8_t> payload;

    bool received = false;
    for (int attempt = 0; attempt < 2 && !received; ++attempt) {
        try {
            write_serial(cmd);
        } catch (const std::exception& e) {
            std::cerr << "Arm_serial_servo_read serial error: " << e.what() << std::endl;
            return -1;
        }

        if (read_response(ext_type, payload)) {
            received = true;
        }
    }

    if (!received) {
        return -1;
    }

    if (ext_type != 0x0A || payload.size() < 3) {
        return -1;
    }

    uint16_t pos = (static_cast<uint16_t>(payload[0]) << 8) | payload[1];

    int angle = -1;
    if (id == 5) {
        angle = static_cast<int>(std::round((270.0 * (pos - 380)) / (3700 - 380)));
        if (angle < 0 || angle > 270) {
            return -1;
        }
    } else {
        angle = static_cast<int>(std::round((180.0 * (pos - 900)) / (3100 - 900)));
        if (angle < 0 || angle > 180) {
            return -1;
        }
        if (id == 2 || id == 3 || id == 4) {
            angle = 180 - angle;
        }
    }

    return angle;
}

void Arm_Device::Arm_Buzzer_On(int delay) {
    std::vector<uint8_t> cmd = {
        __HEAD,
        __DEVICE_ID,
        0x04,
        0x06,
        static_cast<uint8_t>(delay & 0xFF)
    };

    uint8_t checksum = calculate_checksum(cmd);
    cmd.push_back(checksum);

    try {
        write_serial(cmd);
    } catch (const std::exception& e) {
        std::cerr << "Arm_Buzzer_On serial error: " << e.what() << std::endl;
    }
}

void Arm_Device::Arm_Buzzer_Off() {
    Arm_Buzzer_On(0x00);
}
