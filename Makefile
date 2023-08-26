.PHONY: flash
flash:
	cd firmware && pio run --target upload -e bluepill_f103c8

debug:
	cd firmware; pio debug
	~/code/nixpkgs/result/bin/openocd -f firmware/openocd.cfg -f interface/stlink.cfg -f board/stm32f103c8_blue_pill.cfg
gdb:
	gdb -ix firmware/.gdbinit
