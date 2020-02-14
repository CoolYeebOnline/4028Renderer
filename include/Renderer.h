#pragma once
#ifndef __RENDERER_H__
#define __RENDERER_H__

#include <stdafx.h>

class Renderer {
public:
	//constructor destructor 
	Renderer();
	~Renderer();
	//get a valid instance of the class
	static Renderer* GetInstance() { return m_instance; }
	//initialise function
	int Initialise(HWND a_consoleWindow, unsigned int a_width, unsigned int a_height);

	void ClearRenderBuffer();
	void Draw();

	void FillRenderBuffer(const unsigned int a_x, const unsigned int a_y, const unsigned int a_width, const unsigned int a_height, const void* a_data);
	HWND GetWindowHandle()const { return m_windowHandle; }
	HDC GetBufferContext()const { return m_windowDC; }
private:
	static Renderer* m_instance;

	unsigned int	 m_windowWidth;
	unsigned int	 m_windowHeight;

	HWND			 m_windowHandle;
	HDC				 m_windowDC;
	void*			 m_bitBuffer;
	BITMAPINFO*		 m_bmpInfo;
	HBITMAP			 m_bufferBmp;
	HDC				 m_bufferDC;
	HBITMAP			 m_bitmapHandle;
}; 



#endif //__RENDERER_H__