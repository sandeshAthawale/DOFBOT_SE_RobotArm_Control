#ifndef ARM_LIB_H
#define ARM_LIB_H

#include <string>
#include <vector>
#include <cstdint> // For uint8_t, uint16_t

class Arm_Device {
public:
    /**
     * @brief Constructs the Arm_Device and opens the serial port.
     * @param com The serial port path (e.g., "/dev/tty.usbserial-2130").
     * @throws std::runtime_error if the port cannot be opened or configured.
     */
    explicit Arm_Device(const std::string& com);

    /**
     * @brief Destructor. Closes the serial port.
     */
    ~Arm_Device();

    /**
     * @brief Sets the angles of all 6 servos simultaneously.
     * @param s1 Angle for servo 1 (0-180)
     * @param s2 Angle for servo 2 (0-180)
     * @param s3 Angle for servo 3 (0-180)
     * @param s4 Angle for servo 4 (0-180)
     * @param s5 Angle for servo 5 (0-270)
     * @param s6 Angle for servo 6 (0-180)
     * @param time Movement time in milliseconds.
     * @throws std::out_of_range if any angle is outside its valid range.
     * @throws std::runtime_error if there is a serial write error.
     */
    void Arm_serial_servo_write6(int s1, int s2, int s3, int s4, int s5, int s6, int time);

    /**
     * @brief Command a single servo to move to the desired angle.
     */
    void Arm_serial_servo_write(int id, int angle, int time);

    /**
     * @brief Enable or disable torque on all servos.
     */
    void Arm_serial_set_torque(int onoff);

    /**
     * @brief Ping a servo ID to confirm it is responding.
     * @return Response byte (typically 0xDA) or 0 if no data was received.
     */
    int Arm_ping_servo(int id);

    /**
     * @brief Read the current angle for a given servo.
     * @return Angle in degrees, or -1 if the read failed.
     */
    int Arm_serial_servo_read(int id);

    /**
     * @brief Turn the buzzer on for the requested duration (0 keeps it on).
     */
    void Arm_Buzzer_On(int delay = 0xFF);

    /**
     * @brief Switch the buzzer off.
     */
    void Arm_Buzzer_Off();

private:
    int ser_fd; // Serial port file descriptor
    std::string port_name;

    // Protocol constants
    static const uint8_t __HEAD = 0xFF;
    static const uint8_t __DEVICE_ID = 0xFC;
    // 257 - 252 = 5. Used as the starting value for the checksum.
    static const uint8_t __COMPLEMENT = 5;

    /**
     * @brief Maps a value from one range to another (like Arduino's map()).
     */
    uint16_t map_angle_to_pos(int angle, int in_min, int in_max, int out_min, int out_max);

    /**
     * @brief Calculates the checksum for a command packet.
     */
    uint8_t calculate_checksum(const std::vector<uint8_t>& cmd);

    /**
     * @brief Writes a byte vector to the serial port.
     */
    void write_serial(const std::vector<uint8_t>& data);

    /**
     * @brief Read a single byte from the serial port with a timeout.
     */
    bool read_byte(uint8_t& byte, unsigned int timeout_ms);

    /**
     * @brief Read a protocol frame and return the payload without the checksum.
     */
    bool read_response(uint8_t& ext_type, std::vector<uint8_t>& payload, unsigned int timeout_ms = 200);
};

#endif // ARM_LIB_H
