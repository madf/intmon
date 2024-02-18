#include "i2cdev.h"

using Device = I2C::Device;

bool Device::readRegs(uint8_t regNum, void* buf, size_t size)
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

bool Device::writeRegs(uint8_t regNum, const void* data, size_t size)
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

bool Device::preamble(uint8_t regNum)
{
    if (!m_port.waitBusy())
        return false;
    if (!m_port.start())
        return false;
    if (!m_port.writeAddress(m_address, ReadWrite::WRITE))
        return false;
    if (!m_port.writeByte(regNum))
        return false;
    return true;
}

bool Device::readByte(void* buf, size_t i, AckNack ack)
{
    auto* bytePtr = static_cast<uint8_t*>(buf) + i;
    const auto res = m_port.readByte(ack);
    if (!std::get<0>(res))
        return false;
    *bytePtr = std::get<1>(res);
    return true;
}
