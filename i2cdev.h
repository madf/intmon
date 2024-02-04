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
        Device(Port&& port, uint8_t address)
            : m_port(std::move(port)),
              m_address(address)
        {
            static_assert(isPort_v<Port>, "Port must be an I2C port");
        }

        bool readRegs(uint8_t regNum, void* buf, size_t size)
        {
            // Send preamble
            if (!preamble(regNum))
                return false;

            // Request data
            if (!m_port.start())
                return false;
            if (!m_port.writeAddress(m_address, ReadWrite::READ))
                return false;

            // Read data
            size_t i = 0;
            for (;i < size - 1; ++i)
                if (!readByte(buf, i, AckNack::ACK))
                    return false;
            // Last byte with NACK
            if (!readByte(buf, i, AckNack::NACK))
                return false;

            m_port.stop();
            return true;
        }

        bool writeRegs(uint8_t regNum, const void* data, size_t size)
        {
            // Send preamble
            if (!preamble(regNum))
                return false;

            // Send data
            for (size_t i = 0; i < size; ++i)
            {
                const auto byte = static_cast<const uint8_t*>(data)[i];
                if (!m_port.writeByte(byte))
                    return false;
            }

            m_port.stop();
            return true;
        }

        bool readReg(uint8_t regNum, uint8_t& value) { return readRegs(regNum, &value, 1); }
        bool writeReg(uint8_t regNum, uint8_t value) { return writeRegs(regNum, &value, 1); }

    private:
        I2C::PortBase m_port;
        uint8_t m_address;

        bool preamble(uint8_t regNum)
        {
            if (!m_port.start())
                return false;
            if (!m_port.writeAddress(m_address, ReadWrite::WRITE))
                return false;
            if (!m_port.writeByte(regNum))
                return false;
            return true;
        }

        bool readByte(void* buf, size_t i, AckNack ack)
        {
            auto* bytePtr = static_cast<uint8_t*>(buf) + i;
            const auto res = m_port.readByte(ack);
            if (!std::get<0>(res))
                return false;
            *bytePtr = std::get<1>(res);
            return true;
        }
};

}
