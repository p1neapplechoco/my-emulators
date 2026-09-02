PYTHON = python3

.PHONY: chip8 run-chip8 list

chip8:
	$(PYTHON) launcher.py chip8

run-chip8:
	cd CHIP-8 && ./test.exe

list:
	$(PYTHON) launcher.py --list
