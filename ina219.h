#include "i2cdev.h"

class INA219
{
    public:
        template <typename Port>
        INA219(Port& port, uint8_t address)
            : m_dev(port, address)
        {
        }

        void init();

        bool readData(uint16_t& v, uint16_t& vs, uint16_t& c, uint16_t& p);
    private:
        I2C::Device m_dev;

        bool readMSB(uint8_t regNum, uint16_t& v);
};
