#pragma once

#include "field.h"

#include <cstdint>

struct RCC
{
    volatile uint32_t CR;           // Clock control
    volatile uint32_t PLLCFGR;      // PLL configuration
    volatile uint32_t CFGR;         // Clock configuration
    volatile uint32_t CIR;          // Clock interrupt
    volatile uint32_t AHB1RSTR;     // AHB1 peripheral reset
    volatile uint32_t AHB2RSTR;     // AHB2 peripheral reset
    volatile uint32_t AHB3RSTR;     // AHB3 peripheral reset
    volatile uint32_t RESERVED0;
    volatile uint32_t APB1RSTR;     // APB1 peripheral reset
    volatile uint32_t APB2RSTR;     // APB2 peripheral reset
    volatile uint32_t RESERVED1[2];
    volatile uint32_t AHB1ENR;      // AHB1 peripheral clock enable
    volatile uint32_t AHB2ENR;      // AHB2 peripheral clock enable
    volatile uint32_t AHB3ENR;      // AHB3 peripheral clock enable
    volatile uint32_t RESERVED2;
    volatile uint32_t APB1ENR;      // APB1 peripheral clock enable
    volatile uint32_t APB2ENR;      // APB2 peripheral clock enable
    volatile uint32_t RESERVED3[2];
    volatile uint32_t AHB1LPENR;    // AHB1 peripheral clock enable in low power mode
    volatile uint32_t AHB2LPENR;    // AHB2 peripheral clock enable in low power mode
    volatile uint32_t AHB3LPENR;    // AHB3 peripheral clock enable in low power mode
    volatile uint32_t RESERVED4;
    volatile uint32_t APB1LPENR;    // APB1 peripheral clock enable in low power mode
    volatile uint32_t APB2LPENR;    // APB2 peripheral clock enable in low power mode
    volatile uint32_t RESERVED5[2];
    volatile uint32_t BDCR;         // Backup domain control
    volatile uint32_t CSR;          // Clock control & status
    volatile uint32_t RESERVED6[2];
    volatile uint32_t SSCGR;        // Spread spectrum clock generation
    volatile uint32_t PLLI2SCFGR;   // PLLI2S configuration
    volatile uint32_t PLLSAICFGR;   // PLLSAI configuration
    volatile uint32_t DCKCFGR;      // Dedicated clocks configuration

    static RCC* get() { return reinterpret_cast<RCC*>(0x40023800); }

    using HSION     = Field<&RCC::CR, 0, 1>;
    using HSIRDY    = Field<&RCC::CR, HSION::End, 1>;
    using RES0      = Reserved<HSIRDY::End, 1>;
    using HSITRIM   = Field<&RCC::CR, RES0::End, 5>;
    using HSICAL    = Field<&RCC::CR, HSITRIM::End, 8>;
    using HSEON     = Field<&RCC::CR, HSICAL::End, 1>;
    using HSERDY    = Field<&RCC::CR, HSEON::End, 1>;
    using HSEBYP    = Field<&RCC::CR, HSERDY::End, 1>;
    using CSSON     = Field<&RCC::CR, HSEBYP::End, 1>;
    using RES1      = Reserved<CSSON::End, 4>;
    using PLLON     = Field<&RCC::CR, RES1::End, 1>;
    using PLLRDY    = Field<&RCC::CR, PLLON::End, 1>;
    using PLLI2SON  = Field<&RCC::CR, PLLRDY::End, 1>;
    using PLLI2SRDY = Field<&RCC::CR, PLLI2SON::End, 1>;
    using RES2      = Reserved<PLLI2SRDY::End, 4>;
    static_assert(RES2::End == 32, "RCC::CR layout does not cover exactly 32 bits");

    using PLLM   = Field<&RCC::PLLCFGR, 0, 6>;
    using PLLN   = Field<&RCC::PLLCFGR, PLLM::End, 9>;
    using RES3   = Reserved<PLLN::End, 1>;
    using PLLP   = Field<&RCC::PLLCFGR, RES3::End, 2>;
    using RES4   = Reserved<PLLP::End, 4>;
    using PLLSRC = Field<&RCC::PLLCFGR, RES4::End, 1>;
    using RES5   = Reserved<PLLSRC::End, 1>;
    using PLLQ   = Field<&RCC::PLLCFGR, RES5::End, 4>;
    using RES6   = Reserved<PLLQ::End, 4>;
    static_assert(RES6::End == 32, "RCC::PLLCFGR layout does not cover exactly 32 bits");

    using SW      = Field<&RCC::CFGR, 0, 2>;
    using SWS     = Field<&RCC::CFGR, SW::End, 2>;
    using HPRE    = Field<&RCC::CFGR, SWS::End, 4>;
    using RES7    = Reserved<HPRE::End, 2>;
    using PPRE1   = Field<&RCC::CFGR, RES7::End, 3>;
    using PPRE2   = Field<&RCC::CFGR, PPRE1::End, 3>;
    using RTCPRE  = Field<&RCC::CFGR, PPRE2::End, 5>;
    using MCO1    = Field<&RCC::CFGR, RTCPRE::End, 2>;
    using I2SSRC  = Field<&RCC::CFGR, MCO1::End, 1>;
    using MCO1PRE = Field<&RCC::CFGR, I2SSRC::End, 3>;
    using MCO2PRE = Field<&RCC::CFGR, MCO1PRE::End, 3>;
    using MCO2    = Field<&RCC::CFGR, MCO2PRE::End, 2>;
    static_assert(MCO2::End == 32, "RCC::CFGR layout does not cover exactly 32 bits");

    using LSIRDYF     = Field<&RCC::CIR, 0, 1>;
    using LSERDYF     = Field<&RCC::CIR, LSIRDYF::End, 1>;
    using HSIRDYF     = Field<&RCC::CIR, LSERDYF::End, 1>;
    using HSERDYF     = Field<&RCC::CIR, HSIRDYF::End, 1>;
    using PLLRDYF     = Field<&RCC::CIR, HSERDYF::End, 1>;
    using PLLI2SRDYF  = Field<&RCC::CIR, PLLRDYF::End, 1>;
    using RES8        = Reserved<PLLI2SRDYF::End, 1>;
    using CSSF        = Field<&RCC::CIR, RES8::End, 1>;
    using LSIRDYIE    = Field<&RCC::CIR, CSSF::End, 1>;
    using LSERDYIE    = Field<&RCC::CIR, LSIRDYIE::End, 1>;
    using HSIRDYIE    = Field<&RCC::CIR, LSERDYIE::End, 1>;
    using HSERDYIE    = Field<&RCC::CIR, HSIRDYIE::End, 1>;
    using PLLRDYIE    = Field<&RCC::CIR, HSERDYIE::End, 1>;
    using PLLI2SRDYIE = Field<&RCC::CIR, PLLRDYIE::End, 1>;
    using RES9        = Reserved<PLLI2SRDYIE::End, 2>;
    using LSIRDYC     = Field<&RCC::CIR, RES9::End, 1>;
    using LSERDYC     = Field<&RCC::CIR, LSIRDYC::End, 1>;
    using HSIRDYC     = Field<&RCC::CIR, LSERDYC::End, 1>;
    using HSERDYC     = Field<&RCC::CIR, HSIRDYC::End, 1>;
    using PLLRDYC     = Field<&RCC::CIR, HSERDYC::End, 1>;
    using PLLI2SRDYC  = Field<&RCC::CIR, PLLRDYC::End, 1>;
    using RES10       = Reserved<PLLI2SRDYC::End, 1>;
    using CSSC        = Field<&RCC::CIR, RES10::End, 1>;
    using RES11       = Reserved<CSSC::End, 8>;
    static_assert(RES11::End == 32, "RCC::CIR layout does not cover exactly 32 bits");

    using GPIOARST = Field<&RCC::AHB1RSTR, 0, 1>;
    using GPIOBRST = Field<&RCC::AHB1RSTR, GPIOARST::End, 1>;
    using GPIOCRST = Field<&RCC::AHB1RSTR, GPIOBRST::End, 1>;
    using GPIODRST = Field<&RCC::AHB1RSTR, GPIOCRST::End, 1>;
    using GPIOERST = Field<&RCC::AHB1RSTR, GPIODRST::End, 1>;
    using RES12    = Reserved<GPIOERST::End, 2>;
    using GPIOHRST = Field<&RCC::AHB1RSTR, RES12::End, 1>;
    using RES13    = Reserved<GPIOHRST::End, 4>;
    using CRCRST   = Field<&RCC::AHB1RSTR, RES13::End, 1>;
    using RES14    = Reserved<CRCRST::End, 8>;
    using DMA1RST  = Field<&RCC::AHB1RSTR, RES14::End, 1>;
    using DMA2RST  = Field<&RCC::AHB1RSTR, DMA1RST::End, 1>;
    using RES15    = Reserved<DMA2RST::End, 9>;
    static_assert(RES15::End == 32, "RCC::AHB1RSTR layout does not cover exactly 32 bits");

    using RES16    = Reserved<0, 7>;
    using OTGFSRST = Field<&RCC::AHB2RSTR, RES16::End, 1>;
    using RES17    = Reserved<OTGFSRST::End, 24>;
    static_assert(RES17::End == 32, "RCC::AHB2RSTR layout does not cover exactly 32 bits");

    using TIM2RST   = Field<&RCC::APB1RSTR, 0, 1>;
    using TIM3RST   = Field<&RCC::APB1RSTR, TIM2RST::End, 1>;
    using TIM4RST   = Field<&RCC::APB1RSTR, TIM3RST::End, 1>;
    using TIM5RST   = Field<&RCC::APB1RSTR, TIM4RST::End, 1>;
    using RES18     = Reserved<TIM5RST::End, 7>;
    using WWDGRST   = Field<&RCC::APB1RSTR, RES18::End, 1>;
    using RES19     = Reserved<WWDGRST::End, 2>;
    using SPI2RST   = Field<&RCC::APB1RSTR, RES19::End, 1>;
    using SPI3RST   = Field<&RCC::APB1RSTR, SPI2RST::End, 1>;
    using RES20     = Reserved<SPI3RST::End, 1>;
    using USART2RST = Field<&RCC::APB1RSTR, RES20::End, 1>;
    using RES21     = Reserved<USART2RST::End, 3>;
    using I2C1RST   = Field<&RCC::APB1RSTR, RES21::End, 1>;
    using I2C2RST   = Field<&RCC::APB1RSTR, I2C1RST::End, 1>;
    using I2C3RST   = Field<&RCC::APB1RSTR, I2C2RST::End, 1>;
    using RES22     = Reserved<I2C3RST::End, 4>;
    using PWRRST    = Field<&RCC::APB1RSTR, RES22::End, 1>;
    using RES23     = Reserved<PWRRST::End, 3>;
    static_assert(RES23::End == 32, "RCC::APB1RSTR layout does not cover exactly 32 bits");

    using TIM1RST   = Field<&RCC::APB2RSTR, 0, 1>;
    using RES24     = Reserved<TIM1RST::End, 3>;
    using USART1RST = Field<&RCC::APB2RSTR, RES24::End, 1>;
    using USART6RST = Field<&RCC::APB2RSTR, USART1RST::End, 1>;
    using RES25     = Reserved<USART6RST::End, 2>;
    using ADC1RST   = Field<&RCC::APB2RSTR, RES25::End, 1>;
    using RES26     = Reserved<ADC1RST::End, 2>;
    using SDIORST   = Field<&RCC::APB2RSTR, RES26::End, 1>;
    using SPI1RST   = Field<&RCC::APB2RSTR, SDIORST::End, 1>;
    using SPI4RST   = Field<&RCC::APB2RSTR, SPI1RST::End, 1>;
    using SYSCFGRST = Field<&RCC::APB2RSTR, SPI4RST::End, 1>;
    using RES27     = Reserved<SYSCFGRST::End, 1>;
    using TIM9RST   = Field<&RCC::APB2RSTR, RES27::End, 1>;
    using TIM10RST  = Field<&RCC::APB2RSTR, TIM9RST::End, 1>;
    using TIM11RST  = Field<&RCC::APB2RSTR, TIM10RST::End, 1>;
    using RES28     = Reserved<TIM11RST::End, 13>;
    static_assert(RES28::End == 32, "RCC::APB2RSTR layout does not cover exactly 32 bits");

    using GPIOAEN = Field<&RCC::AHB1ENR, 0, 1>;
    using GPIOBEN = Field<&RCC::AHB1ENR, GPIOAEN::End, 1>;
    using GPIOCEN = Field<&RCC::AHB1ENR, GPIOBEN::End, 1>;
    using GPIODEN = Field<&RCC::AHB1ENR, GPIOCEN::End, 1>;
    using GPIOEEN = Field<&RCC::AHB1ENR, GPIODEN::End, 1>;
    using RES29   = Reserved<GPIOEEN::End, 2>;
    using GPIOHEN = Field<&RCC::AHB1ENR, RES29::End, 1>;
    using RES30   = Reserved<GPIOHEN::End, 4>;
    using CRCEN   = Field<&RCC::AHB1ENR, RES30::End, 1>;
    using RES31   = Reserved<CRCEN::End, 8>;
    using DMA1EN  = Field<&RCC::AHB1ENR, RES31::End, 1>;
    using DMA2EN  = Field<&RCC::AHB1ENR, DMA1EN::End, 1>;
    using RES32   = Reserved<DMA2EN::End, 9>;
    static_assert(RES32::End == 32, "RCC::AHB1ENR layout does not cover exactly 32 bits");

    using RES33   = Reserved<0, 7>;
    using OTGFSEN = Field<&RCC::AHB2ENR, RES33::End, 1>;
    using RES34   = Reserved<OTGFSEN::End, 24>;
    static_assert(RES34::End == 32, "RCC::AHB2ENR layout does not cover exactly 32 bits");

    using TIM2EN   = Field<&RCC::APB1ENR, 0, 1>;
    using TIM3EN   = Field<&RCC::APB1ENR, TIM2EN::End, 1>;
    using TIM4EN   = Field<&RCC::APB1ENR, TIM3EN::End, 1>;
    using TIM5EN   = Field<&RCC::APB1ENR, TIM4EN::End, 1>;
    using RES35    = Reserved<TIM5EN::End, 7>;
    using WWDGEN   = Field<&RCC::APB1ENR, RES35::End, 1>;
    using RES36    = Reserved<WWDGEN::End, 2>;
    using SPI2EN   = Field<&RCC::APB1ENR, RES36::End, 1>;
    using SPI3EN   = Field<&RCC::APB1ENR, SPI2EN::End, 1>;
    using RES37    = Reserved<SPI3EN::End, 1>;
    using USART2EN = Field<&RCC::APB1ENR, RES37::End, 1>;
    using RES38    = Reserved<USART2EN::End, 3>;
    using I2C1EN   = Field<&RCC::APB1ENR, RES38::End, 1>;
    using I2C2EN   = Field<&RCC::APB1ENR, I2C1EN::End, 1>;
    using I2C3EN   = Field<&RCC::APB1ENR, I2C2EN::End, 1>;
    using RES39    = Reserved<I2C3EN::End, 4>;
    using PWREN    = Field<&RCC::APB1ENR, RES39::End, 1>;
    using RES40    = Reserved<PWREN::End, 3>;
    static_assert(RES40::End == 32, "RCC::APB1ENR layout does not cover exactly 32 bits");

    using TIM1EN   = Field<&RCC::APB2ENR, 0, 1>;
    using RES41    = Reserved<TIM1EN::End, 3>;
    using USART1EN = Field<&RCC::APB2ENR, RES41::End, 1>;
    using USART6EN = Field<&RCC::APB2ENR, USART1EN::End, 1>;
    using RES42    = Reserved<USART6EN::End, 2>;
    using ADC1EN   = Field<&RCC::APB2ENR, RES42::End, 1>;
    using RES43    = Reserved<ADC1EN::End, 2>;
    using SDIOEN   = Field<&RCC::APB2ENR, RES43::End, 1>;
    using SPI1EN   = Field<&RCC::APB2ENR, SDIOEN::End, 1>;
    using SPI4EN   = Field<&RCC::APB2ENR, SPI1EN::End, 1>;
    using SYSCFGEN = Field<&RCC::APB2ENR, SPI4EN::End, 1>;
    using RES44    = Reserved<SYSCFGEN::End, 1>;
    using TIM9EN   = Field<&RCC::APB2ENR, RES44::End, 1>;
    using TIM10EN  = Field<&RCC::APB2ENR, TIM9EN::End, 1>;
    using TIM11EN  = Field<&RCC::APB2ENR, TIM10EN::End, 1>;
    using RES45    = Reserved<TIM11EN::End, 13>;
    static_assert(RES45::End == 32, "RCC::APB2ENR layout does not cover exactly 32 bits");

    using GPIOALPEN = Field<&RCC::AHB1LPENR, 0, 1>;
    using GPIOBLPEN = Field<&RCC::AHB1LPENR, GPIOALPEN::End, 1>;
    using GPIOCLPEN = Field<&RCC::AHB1LPENR, GPIOBLPEN::End, 1>;
    using GPIODLPEN = Field<&RCC::AHB1LPENR, GPIOCLPEN::End, 1>;
    using GPIOELPEN = Field<&RCC::AHB1LPENR, GPIODLPEN::End, 1>;
    using RES46     = Reserved<GPIOELPEN::End, 2>;
    using GPIOHLPEN = Field<&RCC::AHB1LPENR, RES46::End, 1>;
    using RES47     = Reserved<GPIOHLPEN::End, 4>;
    using CRCLPEN   = Field<&RCC::AHB1LPENR, RES47::End, 1>;
    using RES48     = Reserved<CRCLPEN::End, 2>;
    using FLITFLPEN = Field<&RCC::AHB1LPENR, RES48::End, 1>;
    using SRAM1LPEN = Field<&RCC::AHB1LPENR, FLITFLPEN::End, 1>;
    using RES49     = Reserved<SRAM1LPEN::End, 4>;
    using DMA1LPEN  = Field<&RCC::AHB1LPENR, RES49::End, 1>;
    using DMA2LPEN  = Field<&RCC::AHB1LPENR, DMA1LPEN::End, 1>;
    using RES50     = Reserved<DMA2LPEN::End, 9>;
    static_assert(RES50::End == 32, "RCC::AHB1LPENR layout does not cover exactly 32 bits");

    using RES51     = Reserved<0, 7>;
    using OTGFSLPEN = Field<&RCC::AHB2LPENR, RES51::End, 1>;
    using RES52     = Reserved<OTGFSLPEN::End, 24>;
    static_assert(RES52::End == 32, "RCC::AHB2LPENR layout does not cover exactly 32 bits");

    using TIM2LPEN   = Field<&RCC::APB1LPENR, 0, 1>;
    using TIM3LPEN   = Field<&RCC::APB1LPENR, TIM2LPEN::End, 1>;
    using TIM4LPEN   = Field<&RCC::APB1LPENR, TIM3LPEN::End, 1>;
    using TIM5LPEN   = Field<&RCC::APB1LPENR, TIM4LPEN::End, 1>;
    using RES53      = Reserved<TIM5LPEN::End, 7>;
    using WWDGLPEN   = Field<&RCC::APB1LPENR, RES53::End, 1>;
    using RES54      = Reserved<WWDGLPEN::End, 2>;
    using SPI2LPEN   = Field<&RCC::APB1LPENR, RES54::End, 1>;
    using SPI3LPEN   = Field<&RCC::APB1LPENR, SPI2LPEN::End, 1>;
    using RES55      = Reserved<SPI3LPEN::End, 1>;
    using USART2LPEN = Field<&RCC::APB1LPENR, RES55::End, 1>;
    using RES56      = Reserved<USART2LPEN::End, 3>;
    using I2C1LPEN   = Field<&RCC::APB1LPENR, RES56::End, 1>;
    using I2C2LPEN   = Field<&RCC::APB1LPENR, I2C1LPEN::End, 1>;
    using I2C3LPEN   = Field<&RCC::APB1LPENR, I2C2LPEN::End, 1>;
    using RES57      = Reserved<I2C3LPEN::End, 4>;
    using PWRLPEN    = Field<&RCC::APB1LPENR, RES57::End, 1>;
    using RES58      = Reserved<PWRLPEN::End, 3>;
    static_assert(RES58::End == 32, "RCC::APB1LPENR layout does not cover exactly 32 bits");

    using TIM1LPEN   = Field<&RCC::APB2LPENR, 0, 1>;
    using RES59      = Reserved<TIM1LPEN::End, 3>;
    using USART1LPEN = Field<&RCC::APB2LPENR, RES59::End, 1>;
    using USART6LPEN = Field<&RCC::APB2LPENR, USART1LPEN::End, 1>;
    using RES60      = Reserved<USART6LPEN::End, 2>;
    using ADC1LPEN   = Field<&RCC::APB2LPENR, RES60::End, 1>;
    using RES61      = Reserved<ADC1LPEN::End, 2>;
    using SDIOLPEN   = Field<&RCC::APB2LPENR, RES61::End, 1>;
    using SPI1LPEN   = Field<&RCC::APB2LPENR, SDIOLPEN::End, 1>;
    using SPI4LPEN   = Field<&RCC::APB2LPENR, SPI1LPEN::End, 1>;
    using SYSCFGLPEN = Field<&RCC::APB2LPENR, SPI4LPEN::End, 1>;
    using RES62      = Reserved<SYSCFGLPEN::End, 1>;
    using TIM9LPEN   = Field<&RCC::APB2LPENR, RES62::End, 1>;
    using TIM10LPEN  = Field<&RCC::APB2LPENR, TIM9LPEN::End, 1>;
    using TIM11LPEN  = Field<&RCC::APB2LPENR, TIM10LPEN::End, 1>;
    using RES63      = Reserved<TIM11LPEN::End, 13>;
    static_assert(RES63::End == 32, "RCC::APB2LPENR layout does not cover exactly 32 bits");

    using LSEON  = Field<&RCC::BDCR, 0, 1>;
    using LSERDY = Field<&RCC::BDCR, LSEON::End, 1>;
    using LSEBYP = Field<&RCC::BDCR, LSERDY::End, 1>;
    using RES64  = Reserved<LSEBYP::End, 5>;
    using RTCSEL = Field<&RCC::BDCR, RES64::End, 2>;
    using RES65  = Reserved<RTCSEL::End, 5>;
    using RTCEN  = Field<&RCC::BDCR, RES65::End, 1>;
    using BDRST  = Field<&RCC::BDCR, RTCEN::End, 1>;
    using RES66  = Reserved<BDRST::End, 15>;
    static_assert(RES66::End == 32, "RCC::BDCR layout does not cover exactly 32 bits");

    using LSION = Field<&RCC::CSR, 0, 1>;
    using LSIRDY = Field<&RCC::CSR, LSION::End, 1>;
    using RES67  = Reserved<LSIRDY::End, 22>;
    using RMVF = Field<&RCC::CSR, RES67::End, 1>;
    using BORRSTF = Field<&RCC::CSR, RMVF::End, 1>;
    using PINRSTF = Field<&RCC::CSR, BORRSTF::End, 1>;
    using PORRSTF = Field<&RCC::CSR, PINRSTF::End, 1>;
    using SFTRSTF = Field<&RCC::CSR, PORRSTF::End, 1>;
    using IWDGRSTF = Field<&RCC::CSR, SFTRSTF::End, 1>;
    using WWDGRSTF = Field<&RCC::CSR, IWDGRSTF::End, 1>;
    using LPWRRSTF = Field<&RCC::CSR, WWDGRSTF::End, 1>;
    static_assert(LPWRRSTF::End == 32, "RCC::CSR layout does not cover exactly 32 bits");

    using MODPER    = Field<&RCC::SSCGR, 0, 13>;
    using INCSTEP   = Field<&RCC::SSCGR, MODPER::End, 15>;
    using RES68     = Reserved<INCSTEP::End, 2>;
    using SPREADSEL = Field<&RCC::SSCGR, RES68::End, 1>;
    using SSCGEN    = Field<&RCC::SSCGR, SPREADSEL::End, 1>;
    static_assert(SSCGEN::End == 32, "RCC::SSCGR layout does not cover exactly 32 bits");

    using RES69   = Reserved<0, 6>;
    using PLLI2SN = Field<&RCC::PLLI2SCFGR, RES69::End, 9>;
    using RES70   = Reserved<PLLI2SN::End, 13>;
    using PLLI2SR = Field<&RCC::PLLI2SCFGR, RES70::End, 3>;
    using RES71   = Reserved<PLLI2SR::End, 1>;
    static_assert(RES71::End == 32, "RCC::PLLI2SCFGR layout does not cover exactly 32 bits");

    using RES72  = Reserved<0, 24>;
    using TIMPRE = Field<&RCC::DCKCFGR, RES72::End, 1>;
    using RES73  = Reserved<TIMPRE::End, 7>;
    static_assert(RES73::End == 32, "RCC::DCKCFGR layout does not cover exactly 32 bits");
};
