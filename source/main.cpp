#include <stdafx.h>
#include "Renderer.h"

#define CONSOLE_WIDTH 120
#define CONSOLE_HEIGHT 40

//initalise global variables
HWND g_windowHandle = nullptr;
HDC g_windowDC = nullptr;
void* g_bitBuffer			 = nullptr;
BITMAPINFO* g_bmpInfo	   	 = nullptr;
HBITMAP g_bufferBmp			 = nullptr;
HDC g_buffDevContext		 = nullptr;
HBITMAP g_defBmp			 = nullptr;

//static windows message handle callback function
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int main(int argv, char* argc[])
{
	UNREFERENCED_PARAMETER(argv);
	UNREFERENCED_PARAMETER(argc);
	//create console buffer 
	wchar_t* screen = new wchar_t[120 * 40];
	memset(screen, ' ', CONSOLE_WIDTH * CONSOLE_HEIGHT);
	//windows api code for console buffer 
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);

	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, 20, 180, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	//Create a render context window/bitmap render window
	unsigned int windowWidth = 640;
	unsigned int windowHeight = 480;
	Renderer mainWindow;
	if (!mainWindow.Initialise(consoleWindow, windowWidth, windowHeight)) {


		//seed random
		srand((unsigned int)time(nullptr));

		MSG msg = { 0 };
		//Create timer with current time and delta time between frames.
		auto currentTime = std::chrono::high_resolution_clock::now();
		auto previousTime = currentTime;

		//delta time variable
		std::chrono::duration<double> elapsedTime;

		unsigned int frame = 0;

		while (msg.message != WM_QUIT) {
			//RECT clRect;
			//GetClientRect(mainWindow.GetWindowHandle(), &clRect);

			if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
			else {
				previousTime = currentTime;
				currentTime = std::chrono::high_resolution_clock::now();
				elapsedTime = currentTime - previousTime;

				double fElapsedTime = elapsedTime.count();

				mainWindow.ClearRenderBuffer();

				for (int i = 0; i < 5000; ++i) {
					unsigned int xPos = rand() % 640;
					unsigned int yPos = rand() % 480;
					//colour format is AA_BB_GG_RR
					unsigned int colour = (rand() % 256) << 16 | (rand() % 256) << 8 | (rand() % 256);
					mainWindow.FillRenderBuffer(xPos, yPos, 1, 1, &colour);
				}

				mainWindow.Draw();
				if (frame % 30 == 0) {
					swprintf_s(screen, 16, L"FPS=%4.2f ", 1.0f / fElapsedTime);
					screen[(CONSOLE_WIDTH * CONSOLE_HEIGHT) - 1] = '\0';
					DWORD dwBytesWritten = 0;
					WriteConsoleOutputCharacter(hConsole, (LPCSTR)screen, 22, { 0,0 }, &dwBytesWritten);
				}
				++frame;
			}
		}

	}

	return 0;
}


