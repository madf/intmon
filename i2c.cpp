#include "i2c.h"

using PortBase = I2C::PortBase;

void PortBase::init()
{
    disable();
    reset();
    setFreq();
    setTRise();
    setCCR();
    setConfig();
    setOwnAddress();
    enable();
}

bool PortBase::waitBusy()
{
    return waitBitOff(&m_regs->SR2, BIT(1)); // Wait while BUSY
}

bool PortBase::start()
{
    setBit(&m_regs->CR1, BIT(8)); // START
    return waitBitOn(&m_regs->SR1, BIT(0)); // Wait START
}

void PortBase::stop()
{
    setBit(&m_regs->CR1, BIT(9)); // STOP
    // STOP detection is only necessary in SLAVE mode
}

bool PortBase::writeAddress(uint8_t address, ReadWrite rw)
{
    if (rw == ReadWrite::READ)
        m_regs->DR = (address << 1) + 1;
    else
        m_regs->DR = address << 1;
    if (!waitBitOn(&m_regs->SR1, BIT(1))) // Wait ADDR
        return false;
    // Clear ADDR by reading SR1 and SR2
    volatile uint32_t temp = 0;
    temp = m_regs->SR1;
    temp = m_regs->SR2;
    (void) temp;
    return true;
}

bool PortBase::writeByte(uint8_t value)
{
    if (!waitBitOn(&m_regs->SR1, BIT(7))) // Wait TxE (maybe still transferring)
        return false;
    m_regs->DR = value;
    return waitBitOn(&m_regs->SR1, BIT(7)); // Wait TxE
}

std::pair<bool, uint8_t> PortBase::readByte(AckNack ack)
{
    if (ack == AckNack::ACK)
        setBit(&m_regs->CR1, BIT(10));
    else
        clearBit(&m_regs->CR1, BIT(10));
    const auto success = waitBitOn(&m_regs->SR1, BIT(6)); // Wait RxNE
    return {success, m_regs->DR};
}

void PortBase::disable()
{
    clearBit(&m_regs->CR1, BIT(0)); // Disable peripheral
}

void PortBase::enable()
{
    setBit(&m_regs->CR1, BIT(0)); // Disable peripheral
}

void PortBase::reset()
{
    setBit(&m_regs->CR1, BIT(15));
    clearBit(&m_regs->CR1, BIT(15));
}

void PortBase::setFreq()
{
    const auto val = static_cast<uint8_t>(m_PFreq);
    clearBit(&m_regs->CR2, 0x0000003F);
    setBit(&m_regs->CR2, val & 0x0000003F);
}

void PortBase::setTRise()
{
    // Master/Sm
    const auto val = static_cast<uint8_t>(m_PFreq) + 1;
    clearBit(&m_regs->TRISE, 0x0000003F);
    setBit(&m_regs->TRISE, val & 0x0000003F);
}

void PortBase::setCCR()
{
    // Master/Sm
    const auto val = static_cast<uint16_t>(m_PFreq * 1000000 / (m_speed * 2));
    clearBit(&m_regs->CCR, 0x00000FFF);
    setBit(&m_regs->CCR, val & 0x00000FFF);

    // Set Sm, reset Duty
    clearBit(&m_regs->CCR, 0x0000C000);
}

void PortBase::setConfig()
{
    // Disable NoStretch and GenericCall
    clearBit(&m_regs->CR1, 0x0000C000);
}

void PortBase::setOwnAddress()
{
    clearBit(&m_regs->OAR1, 0x00009000); // Set 7-bit addressing mode
    clearBit(&m_regs->OAR1, 0x00003FFF); // ADDR8-9, ADDR and ADDR0 are zero

    clearBit(&m_regs->OAR2, 0x000000FF); // No Dual mode, zero OAR2
}
