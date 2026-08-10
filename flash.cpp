#include "flash.h"

using Interface = Flash::Interface;

auto Interface::getBORLevel() -> BORLevel
{
    return static_cast<BORLevel>(Flash::BORLEV::read());
}

void Interface::setBORLevel(BORLevel level)
{
}
