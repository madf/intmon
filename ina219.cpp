#include "ina219.h"

void INA219::init()
{
}

bool INA219::readData(uint16_t& v, uint16_t& vs, uint16_t& c, uint16_t& p)
{
    return readMSB(0x01, vs)
        && readMSB(0x02, v)
        && readMSB(0x03, p)
        && readMSB(0x04, c);
}

bool INA219::readMSB(uint8_t regNum, uint16_t& value)
{
    std::array<uint8_t, 2> data;
    if (!m_dev.readRegs(regNum, data.data(), data.size()))
        return false;
    value = (data[0] << 8) + data[1];
    return true;
}
