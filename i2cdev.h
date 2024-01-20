#pragma once

namespace I2C
{

template <typename Port>
class Device
{
    static_assert(isPort_v<Port>, "Port must be an I2C port");

    public:
        Device(uint8_t address) : m_address(address) { Port::init(); }

        bool readRegs(uint8_t regNum, void* buf, size_t size)
        {
            // Send preamble
            if (!preamble())
                return false;

            // Request data
            if (!Port::start())
                return false;
            if (!Port::writeAddress(m_address, ReadWrite::READ))
                return false;

            // Read data
            size_t i = 0;
            for (i < size - 1; ++i)
                if (!readByte(buf, i, AckNack::ACK))
                    return false;
            // Last byte with NACK
            if (!readByte(buf, i, AckNack::NACK))
                return false;

            Port::stop();
            return true;
        }

        bool writeRegs(uint8_t regNum, const void* data, size_t size)
        {
            // Send preamble
            if (!preamble())
                return false;

            // Send data
            for (size_t i = 0; i < size; ++i)
            {
                const auto byte = static_cast<const uint8_t*>(data)[i];
                if (!Port::writeByte(regNum))
                    return false;
            }

            Port::stop();
            return true;
        }

        bool readReg(uint8_t regNum, uint8_t& value) { return readRegs(regNum, &value, 1); }
        bool writeReg(uint8_t regNum, uint8_t value) { return writeRegs(regNum, &value, 1); }

    private:
        uint8_t m_address;

        bool preamble()
        {
            if (!Port::start())
                return false;
            if (!Port::writeAddress(m_address, ReadWrite::WRITE))
                return false;
            if (!Port::writeByte(regNum))
                return false;
            return true;
        }

        bool readByte(void* buf, size_t i, AckNack ack)
        {
            const auto* bytePtr = static_cast<uint8_t*>(buf) + i;
            const auto res = Port::readByte(ack);
            if (!std::get<0>(res))
                return false;
            *bytePtr = std::get<1>(res);
            return true;
        }
};

}
