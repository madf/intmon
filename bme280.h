#pragma once

#include "i2cdev.h"

#include <string>
#include <cstdint>

class BME280
{
    public:
        enum class Sampling : uint8_t
        {
            SKIP = 0b000,
            X1   = 0b001,
            X2   = 0b010,
            X4   = 0b011,
            X8   = 0b100,
            X16  = 0b101
        };

        enum class Mode : uint8_t
        {
            SLEEP  = 0b00,
            FORCED = 0b01,
            NORMAL = 0b11
        };

        enum class Filter : uint8_t
        {
            OFF = 0b000,
            X2  = 0b001,
            X4  = 0b010,
            X8  = 0b011,
            X16 = 0b100
        };

        enum class Standby : uint8_t
        {
            MS_0_5  = 0b000,
            MS_10   = 0b110,
            MS_20   = 0b111,
            MS_62_5 = 0b001,
            MS_125  = 0b010,
            MS_250  = 0b011,
            MS_500  = 0b100,
            MS_1000 = 0b101
        };

        enum class Status
        {
            OK,
            NOT_READY,
            READING_ID_FAILURE,
            BAD_ID,
            SOFT_RESET_FAILURE,
            READING_CALIBRATION_FAILURE,
            READING_COEFFICIENTS_FAILURE,
            SETTING_SAMPLING_FAILURE
        };

        template <typename P>
        BME280(P& port, uint8_t address)
            : m_dev(port, address)
        {
        }

        Status init(Mode mode,
                    Sampling st,
                    Sampling sp,
                    Sampling sh,
                    Filter filter,
                    Standby dur) noexcept;
        Status init() noexcept;

        bool readId(uint8_t& res) noexcept;
        bool readStatus(uint8_t& res) noexcept;
        bool readData(uint32_t& h, uint32_t& p, int32_t& t) noexcept;

        bool readRaw(void* data) noexcept;

    private:
        I2C::Device m_dev;

        struct Calibration
        {
            // Temperature compensation values
            uint16_t digT1;
            int16_t  digT2;
            int16_t  digT3;

            // Pressure compensation values
            uint16_t digP1;
            int16_t  digP2;
            int16_t  digP3;
            int16_t  digP4;
            int16_t  digP5;
            int16_t  digP6;
            int16_t  digP7;
            int16_t  digP8;
            int16_t  digP9;

            // Humidity compensation values
            uint8_t  digH1;
            int16_t  digH2;
            uint8_t  digH3;
            int16_t  digH4;
            int16_t  digH5;
            int8_t   digH6;

            // Intermediate temperature coefficient
            int32_t tFine;
        };

        Calibration m_calib;

        bool isReadingCalibration(bool& res);
        bool readCoefficients();
        bool setSampling(Mode mode, Sampling st, Sampling sp, Sampling sh, Filter filter, Standby dur);

        template <typename T>
        bool read(uint8_t regNum, T& res)
        {
            return m_dev.readRegs(regNum, &res, sizeof(res));
        }

        int32_t compT(int32_t v);
        uint32_t compP(uint32_t v);
        uint32_t compH(uint32_t v);
};

std::string toString(BME280::Status s) noexcept;
