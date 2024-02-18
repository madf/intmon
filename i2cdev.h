#pragma once

#include "i2c.h"

#include <tuple>
#include <cstdint>

namespace I2C
{

class Device
{

    public:
        template <typename Port>
        Device(Port& port, uint8_t address)
            : m_port(port),
              m_address(address)
        {
            static_assert(isPort_v<Port>, "Port must be an I2C port");
        }

        bool readRegs(uint8_t regNum, void* buf, size_t size);
        bool writeRegs(uint8_t regNum, const void* data, size_t size);

        bool readReg(uint8_t regNum, uint8_t& value) { return readRegs(regNum, &value, 1); }
        bool writeReg(uint8_t regNum, uint8_t value) { return writeRegs(regNum, &value, 1); }

    private:
        I2C::PortBase m_port;
        uint8_t m_address;

        bool preamble(uint8_t regNum);
        bool readByte(void* buf, size_t i, AckNack ack);
};

}
