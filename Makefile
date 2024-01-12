DEVICE = stm32f401cdu6
OPTIMIZE = -Os -ggdb3

HARDWARE_FLAGS = -mcpu=cortex-m4 -mthumb -mfloat-abi=hard -mfpu=fpv4-sp-d16

WARNING_FLAGS  = -W -Wall -Wextra -Wundef -Wshadow -Wdouble-promotion \
		 -Wformat-truncation -Wconversion

COMMON_FLAGS   = $(WARNING_FLAGS) -fno-common -ffunction-sections -fdata-sections \
		 $(HARDWARE_FLAGS) $(OPTIMIZE)

CFLAGS        ?= $(COMMON_FLAGS)

CXXFLAGS      ?= $(COMMON_FLAGS) -std=c++23 -fno-exceptions

LDFLAGS       ?= -T$(DEVICE).ld -nostartfiles -nostdlib --specs nano.specs -lc -lgcc -Wl,--gc-sections -Wl,-Map=$@.map

ASFLAGS       ?= -x assembler-with-cpp -Wall -fmessage-length=0 $(HARDWARE_FLAGS) $(OPTIMIZE)

HOST = arm-none-eabi

AS = $(HOST)-as
CC = $(HOST)-gcc
CXX = $(HOST)-g++
OBJCOPY = $(HOST)-objcopy
OBJDUMP = $(HOST)-objdump
STRIP = $(HOST)-strip
SIZE = $(HOST)-size

SOURCES = vector_table.S startup.S main.cpp timer.cpp systick.cpp utils.cpp

SANITIZED_SOURCES = $(patsubst %.S,,$(SOURCES))

RM = rm -f

PROG = firmware

.PHONY: all clean check scan size flash

all: $(PROG).bin test_clocks test_clocks.elf test_bits test_bits.elf

test_clocks: test_clocks.cpp clocks.h
	g++ -std=c++23 -ggdb3 $(WARNING_FLAGS) test_clocks.cpp -o $@

test_clocks.elf: test_clocks.cpp clocks.h
	$(CXX) $(CXXFLAGS) test_clocks.cpp $(LDFLAGS) -o $@

test_bits: test_bits.cpp utils.h
	g++ -std=c++23 -ggdb3 $(WARNING_FLAGS) test_bits.cpp -o $@

test_bits.elf: test_bits.cpp utils.h
	$(CXX) $(CXXFLAGS) test_bits.cpp $(LDFLAGS) -o $@

$(PROG).elf: $(subst .S,.o,$(subst .c,.o,$(subst .cpp,.o,$(SOURCES))))
	$(CXX) $(CXXFLAGS) $^ $(LDFLAGS) -o $@

$(PROG).bin: $(PROG).elf
	$(OBJCOPY) -O binary $< $@

flash: $(PROG).bin
	st-flash --reset write $< 0x8000000

clean:
	$(RM) *.o *.d $(PROG).*

check:
	cppcheck --enable=all --std=c++03 --language=c++ --suppress=*:picojson.h $(DEFS) -q *.h *.cpp *.c

scan:
	scan-build -analyze-headers --view make $(MAKEOPTS)

ifneq ($(MAKECMDGOALS),distclean)
ifneq ($(MAKECMDGOALS),clean)
ifneq ($(MAKECMDGOALS),check)
-include $(subst .c,.d,$(subst .cpp,.d,$(SANITIZED_SOURCES)))
endif
endif
endif

%.d: %.c
	@$(CC) -MM $(CFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$

%.d: %.cpp
	@$(CXX) -MM $(CXXFLAGS) $< > $@.$$$$; \
	sed 's,\($*\).o[ :]*,\1.o $@ : ,g' < $@.$$$$ > $@; \
	rm -f $@.$$$$
