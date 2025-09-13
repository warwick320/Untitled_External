#pragma once

#include <d3d11.h>
#include <dxgi.h>
#define IMGUI_DEFINE_MATH_OPERATORS
#include <ImGui/imgui.h>
#include <ImGui/imgui_impl_dx11.h>
#include <ImGui/imgui_impl_win32.h>

class Render {
private:
	HWND overlaywindow;
	WNDCLASSEX overlayClass;

	ID3D11Device* device;
	ID3D11DeviceContext* deviceContext;
	ID3D11RenderTargetView* renderTargetView;
	IDXGISwapChain* swapChain;

	bool createDevice();
	bool createWindow(cstr windowname);
	bool createImGui();

	void destroyDevice();
	void destroyWindow();
	void destoryImGui();
	bool lastDeleteKeyState = false; // Ãß°¡
public:
	Render() = default;
	~Render();
	bool isVisible = false; //should show
	void setupOverlay(cstr windowName);
	bool isRunning = true; // should Run
	void startRender();
	void renderMenu();
	void endRender();

};