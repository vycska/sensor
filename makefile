# make all libs clean cleanall install picocom tags board_images print-<variable>

################################################################################

TARGET := sensor

DEBUG := 0
OPTIM := O1

ASRCS := $(sort $(shell find . -path ./src/u8g2 -prune -o -name '*.s' -printf '%f '))
AOBJS := $(addprefix objs/,$(ASRCS:.s=.o))

CSRCS := $(sort $(shell find src -path src/u8g2 -prune -o -name '*.c' -printf '%f '))
CDEPS := $(patsubst %.c,deps/%.d,$(CSRCS))
COBJS := $(addprefix objs/,$(CSRCS:.c=.o))

U8G2SRCS := $(sort $(shell find src/u8g2/csrc -name '*.c' -printf '%f '))
U8G2DEPS := $(patsubst %.c,deps/%.d,$(U8G2SRCS))
U8G2OBJS := $(addprefix objs/,$(U8G2SRCS:.c=.o))

ASFLAGS := -Wa,--warn -Wa,--fatal-warnings
CPPFLAGS := -I inc -I src/u8g2/csrc -I /usr/arm-none-eabi/include -I /usr/lib/gcc/arm-none-eabi/9.2.0/include
CFLAGS := -march=armv6-m -mcpu=cortex-m0plus -mthumb -mfloat-abi=soft -mlittle-endian -ffreestanding -fsigned-char -fdata-sections -ffunction-sections -Wall -Werror -$(OPTIM)
LDFLAGS := -nostdlib -nostartfiles -nodefaultlibs -Llibs -L/usr/arm-none-eabi/lib/thumb/v6-m/nofp -L/usr/lib/gcc/arm-none-eabi/9.2.0/thumb/v6-m/nofp -T $(TARGET).ld -Wl,-Map=$(TARGET).map -Wl,--cref -Wl,--gc-sections -Wl,--print-memory-usage -Wl,--stats -Wl,--print-output-form
LDLIBS := -lu8g2 -lm -lgcc -lc_nano -lnosys

ifeq ($(DEBUG),1)
   CFLAGS += -g
endif

vpath %.h inc:src/u8g2/csrc
vpath %.s src
vpath %.c src:src/u8g2/csrc

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

.PHONY : all libs clean cleanall install picocom tags board_images print-%

all : $(TARGET).elf

libs : libs/libu8g2.a

libs/libu8g2.a : $(U8G2OBJS)
	arm-none-eabi-ar -rcsDv --target=elf32-littlearm $@ $^

clean :
	rm -rf *.d *.o *.bin *.hex *.elf *.lst *.map *.png *cscope* *tags* deps objs

cleanall : clean
	rm -rf libs

install : all
	~/bin/lpc21isp -donotstart -verify -bin $(TARGET).bin /dev/ttyUSB0 115200 12000

picocom :
	picocom -b 38400 --echo /dev/ttyUSB0

tags :
	find . -path './src/u8g2' -prune -o -name '*.[csh]' -printf '%h/%f ' > ctags.files
	find . -path './src/u8g2' -prune -o -name '*.cpp' -printf '%h/%f  ' >> ctags.files
	find . -path './src/u8g2' -prune -o -name '*.ld' -printf '%h/%f ' >> ctags.files
	find . -path './src/u8g2' -prune -o -name 'makefile' -printf '%h/%f ' >> ctags.files
	find ./src/u8g2/csrc -name '*' -a -type 'f' -printf '%h/%f ' >> ctags.files
	ctags --extras=+f `cat ctags.files`
	cat ctags.files | tr ' ' '\n' > cscope.files
	cscope -q -R -b -i cscope.files

board_images : board_front.png board_back.png

board_front.png : board_front.r
	R --vanilla -q -f board_front.r

board_back.png : board_back.r
	R --vanilla -q -f board_back.r

print-% :
	@echo $* = $($*)

################################################################################

-include $(CDEPS)
-include $(U8G2DEPS)
