#include <stdafx.h>
#include "Renderer.h"

Renderer* Renderer::m_instance = nullptr;

Renderer::Renderer()  : m_windowWidth(0),m_windowHeight(0), m_windowHandle(nullptr),m_windowDC(nullptr),
m_bmpInfo(nullptr), m_bitBuffer(nullptr), m_bufferBmp(nullptr), m_bufferDC(nullptr), m_bitmapHandle(nullptr){
	m_instance = this;
}

Renderer::~Renderer() {
	m_instance = nullptr;
}

//static windows message handle callback function
static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

int Renderer::Initialise(HWND a_consoleWindow, unsigned int a_width, unsigned int a_height) {
	m_windowWidth = a_width;
	m_windowHeight = a_height;

	//register a windows class with this console application to get a device context
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.hInstance = (HINSTANCE)GetModuleHandle(NULL);
	wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wndClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wndClass.lpszMenuName = nullptr;
	wndClass.lpszClassName = "RayCaster Framework";

	if (!RegisterClassEx(&wndClass))
	{
		return 1;
	}

	//set up window for the application 
	//this window will use device independent bitmaps for rendering
	LONG x = 0; LONG y = 0;
	RECT consoleRect = { NULL };
	if (GetWindowRect(a_consoleWindow, &consoleRect)) {
		x = consoleRect.right;
		y = consoleRect.top;
	}

	RECT windowRect = { x,y,x + (LONG)m_windowWidth, y + (LONG)m_windowHeight };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);
	//create window based of window decription 
	m_windowHandle = CreateWindowA("RayCaster Framework", "Main Scene",
		WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, (HINSTANCE)GetModuleHandle(NULL), nullptr);
	ShowWindow(m_windowHandle, SW_SHOW);
	MoveWindow(m_windowHandle, x, y, m_windowWidth, m_windowHeight, true);
	//create a back buffer render target
	//device context
	m_windowDC = GetDC(m_windowHandle);
	if (m_windowDC == nullptr) {
		return 1;
	}

	//create a bite size array that will be large enough to hold bitmap data
	char* data = (char*)malloc(sizeof(BITMAPINFOHEADER) + 256 * sizeof(RGBQUAD));
	if (data != nullptr) {
		m_bmpInfo = (BITMAPINFO*)data;
		m_bmpInfo->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
		m_bmpInfo->bmiHeader.biWidth = (LONG)m_windowWidth;
		m_bmpInfo->bmiHeader.biHeight = -(LONG)m_windowHeight;
		m_bmpInfo->bmiHeader.biBitCount = 32;
		m_bmpInfo->bmiHeader.biPlanes = 1;
		m_bmpInfo->bmiHeader.biCompression = BI_RGB;
		m_bmpInfo->bmiHeader.biSizeImage = 0;
		m_bmpInfo->bmiHeader.biXPelsPerMeter = 0;
		m_bmpInfo->bmiHeader.biYPelsPerMeter = 0;
		m_bmpInfo->bmiHeader.biClrUsed = 256;
		m_bmpInfo->bmiHeader.biClrImportant = 256;

		m_bufferBmp = CreateDIBSection(m_windowDC, m_bmpInfo, DIB_RGB_COLORS, &m_bitBuffer, NULL, 0);
		if (m_bufferBmp == nullptr) {
			free(data);
			return 1;
		}
		//get buffer device context
		m_bufferDC = CreateCompatibleDC(m_windowDC);
		if (m_bufferDC == nullptr) {
			free(data);
			return 1;
		}
		m_bitmapHandle = (HBITMAP)SelectObject(m_bufferDC, m_bufferBmp);
		if (m_bitmapHandle == nullptr) {
			free(data);
			return 1;
		}


		free(data);
		return 0;
	}

	return 1;
}

void Renderer::ClearRenderBuffer(){
	//clear out window background
	//FillRect(m_bufferDC, &clRect, (HBRUSH)(0x0000) + 2);
	
	//for (int i = 0; i < 5000; ++i) {
	//	unsigned int xPos = rand() % m_windowWidth;
	//	unsigned int yPos = rand() % m_windowHeight;
	//	//calculate the position in the screen buffer
	//	unsigned int index = (xPos + (yPos * m_windowWidth)) * 4;
	//
	//	if (xPos + (yPos * m_windowWidth) > m_windowWidth * m_windowHeight) {
	//
	//	}
	//	else {
	//		//0x00_BB_GG_RR
	//		unsigned int colour = (rand() % 256) << 16 | (rand() % 256) << 8 | (rand() % 256);
	//		//copy the colour value into bitmap buffer memory
	//		memcpy(&(((char*)(m_bitBuffer))[index]), &colour, 3);
	//
	//	}
	//}

}


#define BYTES_PER_PIXEL 4

void Renderer::FillRenderBuffer(const unsigned int a_x, const unsigned int a_y, const unsigned int a_width, const unsigned int a_height, const void* a_data) {
	unsigned int* backBuffer = (unsigned int*)m_bitBuffer;
	unsigned int* imageData = (unsigned int*)a_data;
	unsigned int index = a_x + (a_y * m_windowWidth);
	if (index < (m_windowWidth * m_windowHeight)) {
		//copy out each row of data to the bitmap buffer
		for (unsigned int y = 0; y < a_height; ++y) {
			if (a_y + y <m_windowHeight) {
				unsigned int bytestToCopy = BYTES_PER_PIXEL * a_width;
				if (a_x + a_width > m_windowWidth) {
					bytestToCopy = (a_width - ((a_x + a_width) - m_windowWidth)) * BYTES_PER_PIXEL;
				}
				index = a_x + ((a_y + y) * m_windowWidth);
				memcpy(&backBuffer[index], &imageData[y * a_width], bytestToCopy);
			}
		}
	}
}

void Renderer::Draw() {
	RedrawWindow(m_windowHandle, nullptr, nullptr, RDW_INVALIDATE);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam) {
	UNREFERENCED_PARAMETER(wParam);
	UNREFERENCED_PARAMETER(lParam);
	PAINTSTRUCT paintStruct;
	HDC hDC;
	Renderer* renderer = Renderer::GetInstance();
	if (hwnd == renderer->GetWindowHandle()) {
		switch (message) {
		case WM_PAINT: { //called when the window content is invalidated
			hDC = BeginPaint(hwnd, &paintStruct);

			RECT clRect;
			GetClientRect(hwnd, &clRect);
			BitBlt(hDC, clRect.left, clRect.top, (clRect.right - clRect.left) + 1,
				(clRect.bottom - clRect.top) + 1, renderer->GetBufferContext(), 0, 0, SRCCOPY);

			EndPaint(hwnd, &paintStruct);
			break;
		}
		case WM_DESTROY: { //called when the window needs to close
			PostQuitMessage(0);
			break;

		}
		default: {
			return DefWindowProc(hwnd, message, wParam, lParam);
		};
		}

	}
	return DefWindowProc(hwnd, message, wParam, lParam);
}