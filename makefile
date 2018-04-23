# make all libs clean cleanall install picocom tags print-<variable>

################################################################################

TARGET := sensor

ASRCS := $(sort $(shell find . -name '*.s' -printf '%f '))
AOBJS := $(addprefix objs/,$(ASRCS:.s=.o))

CSRCS := $(sort $(shell find src -path src/u8g2 -prune -o -name '*.c' -printf '%f '))
CDEPS := $(patsubst %.c,deps/%.d,$(CSRCS))
COBJS := $(addprefix objs/,$(CSRCS:.c=.o))

U8G2SRCS := $(sort $(shell find src/u8g2 -name '*.c' -printf '%f '))
U8G2DEPS := $(patsubst %.c,deps/%.d,$(U8G2SRCS))
U8G2OBJS := $(addprefix objs/,$(U8G2SRCS:.c=.o))

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I inc/u8g2 -I /usr/arm-none-eabi/include
CFLAGS := -march=armv6-m -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fsigned-char -fdata-sections -ffunction-sections -O1 -Wall -Werror
LDFLAGS := -nostdlib -nostartfiles -nodefaultlibs -Llibs -L/usr/arm-none-eabi/lib/armv6-m -L/usr/lib/gcc/arm-none-eabi/7.3.0/armv6-m -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,--cref -Wl,--gc-sections
LDLIBS := -lc_nano -lnosys -lm -lu8g2 -lgcc

ifeq ($(DEBUG),1)
   CFLAGS+=-g
endif
ifeq ($(DEBUG),2)
   CFLAGS+=-ggdb3
endif

vpath %.h inc:inc/u8g2
vpath %.s src
vpath %.c src:src/u8g2

$(shell if [ ! -d deps ]; then mkdir -p deps; fi)
$(shell if [ ! -d objs ]; then mkdir -p objs; fi)
$(shell if [ ! -d libs ]; then mkdir -p libs; fi)

################################################################################

$(TARGET).elf : $(AOBJS) $(COBJS)
	$(info Linking $(TARGET))
	arm-none-eabi-gcc $^ $(LDFLAGS) $(LDLIBS) -o $@
	arm-none-eabi-objdump -d -S -z -w $@ > $(TARGET).lst
	arm-none-eabi-objcopy -O ihex $@ $(TARGET).hex
	arm-none-eabi-objcopy -O binary $@ $(TARGET).bin
	arm-none-eabi-size --format=sysv --common -d $(TARGET).elf

objs/%.o : %.s
	arm-none-eabi-gcc $< -c $(CFLAGS) $(ASFLAGS) -o $@

objs/%.o : %.c deps/%.d
	arm-none-eabi-gcc $< -c $(CPPFLAGS) $(CFLAGS) -o $@
	arm-none-eabi-gcc $< -E $(CPPFLAGS) -MM -MP -MT $@ -MF deps/$*.d && touch $@

deps/%.d : ;

.PRECIOUS : deps/%.d

################################################################################

.PHONY : all libs clean cleanall install picocom tags print-%

all : $(TARGET).elf

libs : libs/libu8g2.a

libs/libu8g2.a : $(U8G2OBJS)
	arm-none-eabi-ar -rcsDv --target=elf32-littlearm $@ $^

clean :
	rm -rf *.o *.elf *.bin *.hex *.map *.lst cscope* tags deps objs

cleanall : clean
	rm -rf libs

install : all
	~/bin/lpc21isp $(TARGET).hex /dev/ttyUSB0 115200 12000

picocom :
	picocom -b 38400 --echo /dev/ttyUSB0

tags :
	ctags -R --extra=+f *
	find . -name '*.[csh]' > cscope.files
	cscope -q -R -b -i cscope.files

print-% :
	@echo $* = $($*)

################################################################################

-include $(CDEPS)
-include $(U8G2DEPS)
