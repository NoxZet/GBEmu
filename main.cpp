#pragma once

#include <chrono>
#include <thread>

#include "main.h"
#include "cpu.h"
#include "gpu.h"
#include "memory.h"

using namespace NoxGB;
using namespace std::chrono;
using namespace std::this_thread;

int main() {
	Memory memory;
	memory.initiate();

	GPU gpu;
	gpu.memory = &memory;
	memory.initiate();

	CPU cpu;
	cpu.memory = &memory;
	cpu.gpu = &gpu;
	gpu.initiate();

	int frames = 0;
	time_point<system_clock> clock = system_clock::now();
	while (true) {
		frames++;
		if (frames % 60 == 0)
		printf("Frame: %d\n", frames);
		int cycles = 0;
		int refreshes = gpu.refreshes;
		bool wasDisabled = !gpu.enable;

		// Keep cycling till next frame is rendered
		while (
			(!wasDisabled && refreshes == gpu.refreshes) ||
			(wasDisabled && cycles < CYCLES_PER_LINE * 144)
		) {
			cycles += cpu.runInstruction() + 1;
			if (!gpu.enable) {
				wasDisabled = true;
			}
		}

		// print gpu.buffer
		// timer to keep the emulation with screen framerate
		int lapsedSeconds = frames / 60;
		int lapsedParts = frames % 60;
		sleep_until(clock + seconds(lapsedSeconds) + microseconds(1000000 * lapsedParts / 60));
	}
}
