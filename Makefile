.PHONY: flash
flash:
	cd firmware && pio run --target upload
