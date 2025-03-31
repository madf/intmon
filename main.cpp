#include "gpio.h"
#include "mco.h"
#include "led.h"
#include "keyboard.h"
#include "screen.h"
#include "i2c.h"
#include "display.h"
#include "rtc.h"
#include "adc.h"
#include "bme280.h"
#include "ina219.h"
#include "systick.h"
#include "timer.h"
#include "clocks.h"
#include "utils.h"

#include <chrono>

extern "C"
void SystemInit()
{
}

extern "C" void NMI_Handler() { while (true); }
extern "C" void HardFault_Handler() { while (true); }
extern "C" void MemManage_Handler() { while (true); }
extern "C" void UsageFault_Handler() { while (true); }
extern "C" void SVC_Handler() { while (true); }
extern "C" void DebugMon_Handler() { while (true); }
extern "C" void PendSV_Handler() { while (true); }
// extern "C" void SysTick_Handler() { while (true); }
extern "C" void WWDG_IRQHandler() { while (true); }
extern "C" void PVD_IRQHandler() { while (true); }
extern "C" void TAMP_STAMP_IRQHandler() { while (true); }
extern "C" void RTC_WKUP_IRQHandler() { while (true); }
extern "C" void FLASH_IRQHandler() { while (true); }
extern "C" void RCC_IRQHandler() { while (true); }
extern "C" void EXTI0_IRQHandler() { while (true); }
extern "C" void EXTI1_IRQHandler() { while (true); }
extern "C" void EXTI2_IRQHandler() { while (true); }
extern "C" void EXTI3_IRQHandler() { while (true); }
extern "C" void EXTI4_IRQHandler() { while (true); }
extern "C" void DMA1_Stream0_IRQHandler() { while (true); }
extern "C" void DMA1_Stream1_IRQHandler() { while (true); }
extern "C" void DMA1_Stream2_IRQHandler() { while (true); }
extern "C" void DMA1_Stream3_IRQHandler() { while (true); }
extern "C" void DMA1_Stream4_IRQHandler() { while (true); }
extern "C" void DMA1_Stream5_IRQHandler() { while (true); }
extern "C" void DMA1_Stream6_IRQHandler() { while (true); }
extern "C" void ADC_IRQHandler() { while (true); }
extern "C" void EXTI9_5_IRQHandler() { while (true); }
extern "C" void TIM1_BRK_TIM9_IRQHandler() { while (true); }
extern "C" void TIM1_UP_TIM10_IRQHandler() { while (true); }
extern "C" void TIM1_TRG_COM_TIM11_IRQHandler() { while (true); }
extern "C" void TIM1_CC_IRQHandler() { while (true); }
extern "C" void TIM2_IRQHandler() { while (true); }
extern "C" void TIM3_IRQHandler() { while (true); }
extern "C" void TIM4_IRQHandler() { while (true); }
extern "C" void I2C1_EV_IRQHandler() { while (true); }
extern "C" void I2C1_ER_IRQHandler() { while (true); }
extern "C" void I2C2_EV_IRQHandler() { while (true); }
extern "C" void I2C2_ER_IRQHandler() { while (true); }
extern "C" void SPI1_IRQHandler() { while (true); }
extern "C" void SPI2_IRQHandler() { while (true); }
extern "C" void USART1_IRQHandler() { while (true); }
extern "C" void USART2_IRQHandler() { while (true); }
extern "C" void EXTI15_10_IRQHandler() { while (true); }
extern "C" void RTC_Alarm_IRQHandler() { while (true); }
extern "C" void OTG_FS_WKUP_IRQHandler() { while (true); }
extern "C" void DMA1_Stream7_IRQHandler() { while (true); }
extern "C" void SDIO_IRQHandler() { while (true); }
extern "C" void TIM5_IRQHandler() { while (true); }
extern "C" void SPI3_IRQHandler() { while (true); }
extern "C" void DMA2_Stream0_IRQHandler() { while (true); }
extern "C" void DMA2_Stream1_IRQHandler() { while (true); }
extern "C" void DMA2_Stream2_IRQHandler() { while (true); }
extern "C" void DMA2_Stream3_IRQHandler() { while (true); }
extern "C" void DMA2_Stream4_IRQHandler() { while (true); }
extern "C" void OTG_FS_IRQHandler() { while (true); }
extern "C" void DMA2_Stream5_IRQHandler() { while (true); }
extern "C" void DMA2_Stream6_IRQHandler() { while (true); }
extern "C" void DMA2_Stream7_IRQHandler() { while (true); }
extern "C" void USART6_IRQHandler() { while (true); }
extern "C" void I2C3_EV_IRQHandler() { while (true); }
extern "C" void I2C3_ER_IRQHandler() { while (true); }
extern "C" void FPU_IRQHandler() { while (true); }
extern "C" void SPI4_IRQHandler() { while (true); }

using MCO1 = MCO::Port<1>;
using HSE = Clocks::HSE<25.0>;
using LSE = Clocks::LSE<32.768>;
using PLL = Clocks::PLL<HSE, 25, 336, 4, 7>;
using SysClock = Clocks::SysClock<PLL, Clocks::HPRE::DIV2, Clocks::PPRE::DIV2, Clocks::PPRE::DIV1>;
using ADCInput = GPIO::Pin<'A', 0>;

int main()
{
    LSE::enable();
    SysClock::enable();
    SysTick::init(SysClock::AHBFreq * 1000); // MHz to ms

    MCO1::enable(MCO1::Source::HSE, MCO::PRE::DIV5);

    ADC::init(ADC::PRE::DIV1);
    ADC::setIntChannel(ADC::IntChannel::TSVREF);

    ADCInput::enable();
    ADCInput::setMode(GPIO::Mode::ANALOG);
    ADCInput::setPull(GPIO::Pull::NO);

    RTC::Device::init();

    Screen screen(SysClock::APB1Freq);

    screen.run();
    return 0;
}
