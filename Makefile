
compile:
	cmake -S . -B build -DCMAKE_BUILD_TYPE=Debug
	cmake --build build

run: compile
	@./build/app/venturi

clean:
	@rm -rf build
	@mkdir build