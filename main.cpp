#pragma once

#include <chrono>
#include <thread>
#include <windows.h>

#include "main.h"
#include "cpu.h"
#include "gpu.h"
#include "memory.h"

using namespace NoxGB;
using namespace std::chrono;
using namespace std::this_thread;

#define UNICODE

static unsigned char* frameBuffer = nullptr;
static bool running = false;

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

#define FPS 60

void cpuProcess(HWND hwnd) {
	Memory memory;
	memory.initiate();

	GPU gpu;
	gpu.memory = &memory;
	memory.initiate();

	CPU cpu;
	cpu.memory = &memory;
	cpu.gpu = &gpu;
	gpu.initiate();
	frameBuffer = gpu.buffer;

	int frames = 0;
	time_point<system_clock> clock = system_clock::now();
	while (running) {
		frames++;
		if (frames % FPS == 0)
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

		InvalidateRect(hwnd, NULL, true);
		// timer to keep the emulation with screen framerate
		int lapsedSeconds = frames / FPS;
		int lapsedParts = frames % FPS;
		sleep_until(clock + seconds(lapsedSeconds) + microseconds(1000000 * lapsedParts / FPS));
	}
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class.
	const char CLASS_NAME[] = "GBEmu";

	WNDCLASS wc = {};

	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	// Create the window.

	HWND hwnd = CreateWindowEx(
		0,                              // Optional window styles.
		CLASS_NAME,                     // Window class
		"GBEmu",    // Window text
		WS_OVERLAPPEDWINDOW,            // Window style

										// Size and position
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 400,

		NULL,       // Parent window    
		NULL,       // Menu
		hInstance,  // Instance handle
		NULL        // Additional application data
	);

	if (hwnd == NULL)
	{
		return 0;
	}

	ShowWindow(hwnd, nCmdShow);
	running = true;
	std::thread cpuThread(&cpuProcess, hwnd);
	cpuThread.detach();

	MSG msg = {};
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	running = false;
}

int main() {
	return wWinMain(GetModuleHandle(NULL), NULL, NULL, SW_SHOWNORMAL);
}

void printBuffer(HDC hdc, unsigned char* buffer, size_t width, size_t height) {
	if (buffer == nullptr || hdc == nullptr) {
		return;
	}

	HBITMAP map = CreateBitmap(width, height, 1, 8 * 4, (void*)buffer);
	HDC src = CreateCompatibleDC(hdc);
	SelectObject(src, map);
	StretchBlt(hdc, 10, 10, width * 2, height * 2, src, 0, 0, width, height, SRCCOPY);

	DeleteObject(map);
	DeleteDC(src);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		printBuffer(hdc, frameBuffer, 160, 144);
		EndPaint(hwnd, &ps);
		return 0;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
