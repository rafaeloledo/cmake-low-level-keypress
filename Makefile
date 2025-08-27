.PHONY: build

build:
	cd out && cmake --build . --config Release && cd ..
run:
	.\out\cmake-low-level-keypress\Release\KeyPress.exe
