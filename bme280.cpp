#include "bme280.h"

#include "timer.h"

namespace
{

enum Registers : uint8_t {
    DIG_T1 = 0x88,
    DIG_T2 = 0x8A,
    DIG_T3 = 0x8C,

    DIG_P1 = 0x8E,
    DIG_P2 = 0x90,
    DIG_P3 = 0x92,
    DIG_P4 = 0x94,
    DIG_P5 = 0x96,
    DIG_P6 = 0x98,
    DIG_P7 = 0x9A,
    DIG_P8 = 0x9C,
    DIG_P9 = 0x9E,

    DIG_H1 = 0xA1,
    DIG_H2 = 0xE1,
    DIG_H3 = 0xE3,
    DIG_H4 = 0xE4,
    DIG_H5 = 0xE5,
    DIG_H6 = 0xE7,

    CHIPID = 0xD0,
    VERSION = 0xD1,
    SOFTRESET = 0xE0,

    CAL26 = 0xE1, // R calibration stored in 0xE1-0xF0

    CONTROLHUMID = 0xF2,
    STATUS = 0xF3,
    CONTROL = 0xF4,
    CONFIG = 0xF5,
    PRESSUREDATA = 0xF7,
    TEMPDATA = 0xFA,
    HUMIDDATA = 0xFD
};

template <typename T>
T fromThree(T h, T l, T xl)
{
    return (h << 12) | (l << 4) | (xl >> 4);
}

uint32_t fromTwo(uint32_t h, uint32_t l)
{
    return (h << 8) | l;
}

}

auto BME280::init(Mode mode,
                  Sampling st,
                  Sampling sp,
                  Sampling sh,
                  Filter filter,
                  Standby dur) noexcept -> Status
{
    uint8_t id = 0;
    if (!readId(id))
        return Status::READING_ID_FAILURE;
    //if (id != 0x60)
    //    return Status::BAD_ID;

    if (!m_dev.writeReg(Registers::SOFTRESET, 0xB6))
        return Status::SOFT_RESET_FAILURE;

    for (bool res = true; res;)
    {
        Timer::wait(std::chrono::milliseconds(10));
        if (!isReadingCalibration(res))
            return Status::READING_CALIBRATION_FAILURE;
    }

    if (!readCoefficients())
        return Status::READING_COEFFICIENTS_FAILURE;

    if (!setSampling(mode, st, sp, sh, filter, dur))
        return Status::SETTING_SAMPLING_FAILURE;
    Timer::wait(std::chrono::milliseconds(100));

    return Status::OK;
}

auto BME280::init() noexcept -> Status
{
    return init(Mode::NORMAL,
                Sampling::X16,
                Sampling::X16,
                Sampling::X16,
                Filter::OFF,
                Standby::MS_0_5);
}

bool BME280::isReadingCalibration(bool& res)
{
    uint8_t status = 0;
    if (!readStatus(status))
        return false;
    res = (status & 0x01) != 0;
    return true;
}

bool BME280::readCoefficients()
{
    Calibration calib;
    uint8_t h4 = 0;
    uint16_t h5 = 0;
    bool res =  read(Registers::DIG_T1, calib.digT1)
             && read(Registers::DIG_T2, calib.digT2)
             && read(Registers::DIG_T3, calib.digT3)

             && read(Registers::DIG_P1, calib.digP1)
             && read(Registers::DIG_P2, calib.digP2)
             && read(Registers::DIG_P3, calib.digP3)
             && read(Registers::DIG_P4, calib.digP4)
             && read(Registers::DIG_P5, calib.digP5)
             && read(Registers::DIG_P6, calib.digP6)
             && read(Registers::DIG_P7, calib.digP7)
             && read(Registers::DIG_P8, calib.digP8)
             && read(Registers::DIG_P9, calib.digP9)

             && read(Registers::DIG_H1, calib.digH1)
             && read(Registers::DIG_H2, calib.digH2)
             && read(Registers::DIG_H3, calib.digH3)
             && read(Registers::DIG_H4, h4)
             && read(Registers::DIG_H5, h5)
             && read(Registers::DIG_H6, calib.digH6);
    if (!res)
        return false;

    calib.digH4 = (h4 << 4) + (h5 & 0x000F);
    calib.digH5 = (h5 >> 4);

    calib.tFine = 0;

    m_calib = calib;
    return true;
}

bool BME280::setSampling(Mode mode, Sampling st, Sampling sp, Sampling sh, Filter filter, Standby dur)
{
    return m_dev.writeReg(Registers::CONTROL,      static_cast<uint8_t>(Mode::SLEEP))
        && m_dev.writeReg(Registers::CONTROLHUMID, static_cast<uint8_t>(sh))
        && m_dev.writeReg(Registers::CONFIG,       (static_cast<uint8_t>(dur) << 5) + (static_cast<uint8_t>(filter) << 2))
        && m_dev.writeReg(Registers::CONTROL,      (static_cast<uint8_t>(st) << 5)  + (static_cast<uint8_t>(sp) << 2) + static_cast<uint8_t>(mode));
}

bool BME280::readRaw(void* data)
{
    return m_dev.readRegs(Registers::PRESSUREDATA, data, 8);
}

bool BME280::readId(uint8_t& res)
{
    return m_dev.readReg(Registers::CHIPID, res);
}

bool BME280::readStatus(uint8_t& res)
{
    return m_dev.readReg(Registers::STATUS, res);
}

bool BME280::readData(uint32_t& h, uint32_t& p, int32_t& t)
{
    uint8_t regs[8];
    if (!m_dev.readRegs(Registers::PRESSUREDATA, regs, sizeof(regs)))
        return false;
    t = compT(fromThree<int32_t>(regs[3], regs[4], regs[5])); // This should go first
    p = compP(fromThree<uint32_t>(regs[0], regs[1], regs[2]));
    h = compH(fromTwo(regs[6], regs[7]));
    return true;
}

int32_t BME280::compT(int32_t v)
{
    const int32_t var1 = ((((v / 8) - ((int32_t)m_calib.digT1 * 2))) * ((int32_t)m_calib.digT2)) >> 11;
    const int32_t var2 = (((((v / 16) - ((int32_t)m_calib.digT1)) * ((v / 16) - ((int32_t)m_calib.digT1))) >> 12) * ((int32_t)m_calib.digT3)) >> 14;
    m_calib.tFine = var1 + var2;
    return (m_calib.tFine * 5 + 128) / 256;
}

uint32_t BME280::compP(uint32_t v)
{
    int64_t var1 = ((int64_t)m_calib.tFine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)m_calib.digP6;
    var2 = var2 + ((var1*(int64_t)m_calib.digP5)<<17);
    var2 = var2 + (((int64_t)m_calib.digP4)<<35);
    var1 = ((var1 * var1 * (int64_t)m_calib.digP3)>>8) + ((var1 * (int64_t)m_calib.digP2)<<12);
    var1 = (((((int64_t)1)<<47)+var1))*((int64_t)m_calib.digP1)>>33;
    if(var1 == 0)
        return 0; // avoid exception caused by division by zero
    int64_t p = 1048576 - v;
    p = (((p<<31) - var2)*3125)/var1;
    var1 = (((int64_t)m_calib.digP9) * (p>>13) * (p>>13)) >> 25;
    var2 =(((int64_t)m_calib.digP8) * p) >> 19;
    return ((p + var1 + var2) >> 8) + (((int64_t)m_calib.digP7)<<4);
}

uint32_t BME280::compH(uint32_t v)
{
    int32_t v_x1_u32r = (m_calib.tFine - ((int32_t)76800));
    v_x1_u32r = (((((v << 14) - (((int32_t)m_calib.digH4) << 20) - (((int32_t)m_calib.digH5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)m_calib.digH6)) >> 10) * (((v_x1_u32r * ((int32_t)m_calib.digH3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)m_calib.digH2) + 8192) >> 14));
    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)m_calib.digH1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400? 419430400: v_x1_u32r);
    return (uint32_t)(v_x1_u32r>>12);
}

std::string toString(BME280::Status s) noexcept
{
    switch (s)
    {
        case BME280::Status::NOT_READY: return "Not ready";
        case BME280::Status::READING_ID_FAILURE: return "Failed to read device id";
        case BME280::Status::BAD_ID: return "Bad device id";
        case BME280::Status::SOFT_RESET_FAILURE: return "Failed to perform soft reset";
        case BME280::Status::READING_CALIBRATION_FAILURE: return "Failed to read calibration";
        case BME280::Status::READING_COEFFICIENTS_FAILURE: return "Failed to read coefficients";
        case BME280::Status::SETTING_SAMPLING_FAILURE: return "Failed to set sampling";
        case BME280::Status::OK: return "Ok";
        default: return "<unknown>";
    }
}
