#include <Render/Render.h>
#include <dwmapi.h>

#include <ImGui/imgui_internal.h>
bool ToggleSwitch(const char* id, bool* v, ImVec2 size) {
    ImGuiIO& io = ImGui::GetIO();
    ImGuiWindow* window = ImGui::GetCurrentWindow();
    if (!window) return false;
    ImDrawList* dl = ImGui::GetWindowDrawList();

    ImVec2 pos = ImGui::GetCursorScreenPos();
    ImVec2 p1 = pos;
    ImVec2 p2 = ImVec2(pos.x + size.x, pos.y + size.y);
    float radius = size.y * 0.5f;

    ImGuiID gid = ImGui::GetID(id);
    ImGuiStorage* store = ImGui::GetStateStorage();
    float anim = store->GetFloat(gid, *v ? 1.0f : 0.0f);
    float target = *v ? 1.0f : 0.0f;
    float speed = 9.0f;
    // 애니메이션 값 갱신
    anim = anim + (target - anim) * ImSaturate(io.DeltaTime * speed);
    store->SetFloat(gid, anim);

    ImVec4 colOff = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    ImVec4 colOn = ImVec4(0.46f, 0.12f, 0.12f, 1.0f);
    ImVec4 bg = ImLerp(colOff, colOn, anim);
    ImU32 uBg = ImGui::GetColorU32(bg);

    dl->AddRectFilled(p1, p2, ImGui::GetColorU32(ImVec4(0, 0, 0, 0.15f)), radius);
    dl->AddRectFilled(p1, p2, uBg, radius);

    float knobR = radius - 3.0f;
    ImVec2 knobCenter = ImVec2(ImLerp(p1.x + radius, p2.x - radius, anim), (p1.y + p2.y) * 0.5f);
    dl->AddCircleFilled(knobCenter, knobR, ImGui::GetColorU32(ImVec4(0.86f, 0.12f, 0.12f, 1.0f)));

    ImGui::InvisibleButton(id, size);
    if (ImGui::IsItemClicked()) {
        *v = !*v;
    }
    ImGui::SameLine(0, 0);
    ImGui::Dummy(size);
    ImGui::NewLine();

    return *v;
}
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK windowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam)) {
        return true;
    }
    switch (msg) {
    case WM_SYSCOMMAND:
        if ((wParam & 0xfff0) == SC_KEYMENU) {
            return 0;
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CLOSE:
        return 0;
        break;
    }
    return DefWindowProcW(hwnd, msg, wParam, lParam);
}

bool Render::createDevice() {
    DXGI_SWAP_CHAIN_DESC swapChainDescription;
    ZeroMemory(&swapChainDescription, sizeof(swapChainDescription));

    swapChainDescription.BufferCount = 2;
    swapChainDescription.BufferDesc.Width = GetSystemMetrics(SM_CXSCREEN);
    swapChainDescription.BufferDesc.Height = GetSystemMetrics(SM_CYSCREEN);
    swapChainDescription.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    swapChainDescription.BufferDesc.RefreshRate.Numerator = 60;
    swapChainDescription.BufferDesc.RefreshRate.Denominator = 1;

    swapChainDescription.OutputWindow = overlaywindow;
    swapChainDescription.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
    swapChainDescription.Windowed = TRUE;
    swapChainDescription.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

    swapChainDescription.SampleDesc.Count = 1;
    swapChainDescription.SampleDesc.Quality = 0;
    swapChainDescription.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

    D3D_FEATURE_LEVEL featureLevel;
    D3D_FEATURE_LEVEL featureLevelList[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0 };

    HRESULT result = D3D11CreateDeviceAndSwapChain(
        nullptr,
        D3D_DRIVER_TYPE_HARDWARE,
        nullptr,
        0,
        featureLevelList,
        2,
        D3D11_SDK_VERSION,
        &swapChainDescription,
        &swapChain,
        &device,
        &featureLevel,
        &deviceContext
    );

    if (result == DXGI_ERROR_UNSUPPORTED) {
        result = D3D11CreateDeviceAndSwapChain(
            nullptr,
            D3D_DRIVER_TYPE_WARP,
            nullptr,
            0,
            featureLevelList,
            2,
            D3D11_SDK_VERSION,
            &swapChainDescription,
            &swapChain,
            &device,
            &featureLevel,
            &deviceContext
        );
    }

    if (FAILED(result)) {
        MessageBoxA(nullptr, "Error: Unable to initialize DirectX 11 device.\nCheck GPU drivers or DirectX installation.", "Critical Problem", MB_ICONERROR | MB_OK);
        return false;
    }

    ID3D11Texture2D* backBuffer = nullptr;
    if (SUCCEEDED(swapChain->GetBuffer(0, IID_PPV_ARGS(&backBuffer)))) {
        device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
        backBuffer->Release();
        return true;
    }
    return false;
}

bool Render::createWindow(cstr windowname) {
    overlayClass.cbSize = sizeof(overlayClass);
    overlayClass.style = CS_CLASSDC;
    overlayClass.lpszClassName = "Untitiled_External";
    overlayClass.hInstance = GetModuleHandleA(0);
    overlayClass.lpfnWndProc = windowProcedure;

    RegisterClassEx(&overlayClass);

    overlaywindow = CreateWindowEx(
        WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW,
        overlayClass.lpszClassName,
        "Untitiled_External",
        WS_POPUP,
        0,
        0,
        GetSystemMetrics(SM_CXSCREEN),
        GetSystemMetrics(SM_CYSCREEN),
        0,
        0,
        overlayClass.hInstance,
        0
    );
    if (!overlaywindow) {
        MessageBoxA(nullptr, "Failed to create overlay window", "Error", MB_ICONWARNING | MB_OK);
        return false;
    }

    SetLayeredWindowAttributes(overlaywindow, RGB(0, 0, 0), BYTE(255), LWA_ALPHA);

    RECT clientArea{}, windowArea{};
    GetClientRect(overlaywindow, &clientArea);
    GetClientRect(overlaywindow, &windowArea);

    POINT diff{};
    ClientToScreen(overlaywindow, &diff);

    MARGINS margins{
        windowArea.left + (diff.x - windowArea.left),
        windowArea.top + (diff.y - windowArea.top),
        windowArea.right,
        windowArea.bottom,
    };
    DwmExtendFrameIntoClientArea(overlaywindow, &margins);

    ShowWindow(overlaywindow, SW_SHOW);
    UpdateWindow(overlaywindow);
    return true;
}
std::string GetExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    std::string::size_type pos = std::string(buffer).find_last_of("\\/");
    return std::string(buffer).substr(0, pos);
}
bool Render::createImGui() {
    using namespace ImGui;
    CreateContext();



    StyleColorsDark();

    if (!ImGui_ImplWin32_Init(overlaywindow)) {
        return false;
    }
    if (!ImGui_ImplDX11_Init(device, deviceContext)) {
        return false;
    }
    return true;
}

void Render::destroyDevice() {
    if (device) {
        device->Release();
        deviceContext->Release();
        swapChain->Release();
        renderTargetView->Release();
    }
}

void Render::destroyWindow() {
    DestroyWindow(overlaywindow);
    UnregisterClassA(overlayClass.lpszClassName, overlayClass.hInstance);
}

void Render::destoryImGui() {
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}

Render::~Render() {
    destroyDevice();
    destroyWindow();
    destoryImGui();
}

void Render::setupOverlay(cstr windowName) {
    createWindow(windowName);
    createDevice();
    createImGui();
}

void Render::startRender() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    //bool currentDeleteKeyState = (GetAsyncKeyState(VK_DELETE) & 0x8000) != 0;
    //if (currentDeleteKeyState && !lastDeleteKeyState) {
    //    isVisible = !isVisible;
    //}
    //lastDeleteKeyState = currentDeleteKeyState;

    if (isVisible) {
        SetWindowLong(overlaywindow, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TOPMOST | WS_EX_LAYERED);
    }
    else {
        SetWindowLong(overlaywindow, GWL_EXSTYLE, WS_EX_TOOLWINDOW | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_LAYERED);
    }
}

void Render::endRender() {
    ImGui::Render();
    float clearColor[4]{ 0, 0, 0, 0 };
    deviceContext->OMSetRenderTargets(1, &renderTargetView, nullptr);
    deviceContext->ClearRenderTargetView(renderTargetView, clearColor);
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
    swapChain->Present(1, 0);
}


void ApplyModernStyle() {
    static bool done = false;
    if (done) return;
    done = true;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowRounding = 12.0f;
    style.FrameRounding = 8.0f;
    style.GrabRounding = 6.0f;
    style.WindowPadding = ImVec2(16, 16);
    style.FramePadding = ImVec2(10, 6);
    style.ItemSpacing = ImVec2(10, 10);
    style.ItemInnerSpacing = ImVec2(8, 6);
    style.TabRounding = 6.0f;
    style.ScrollbarRounding = 6.0f;

    ImVec4* colors = style.Colors;
    colors[ImGuiCol_WindowBg] = ImVec4(0.10f, 0.10f, 0.10f, 0.98f);
    colors[ImGuiCol_TitleBg] = ImVec4(0.26f, 0.05f, 0.05f, 1.0f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.9f, 0.14f, 0.14f, 1.0f);
    colors[ImGuiCol_Header] = ImVec4(0.18f, 0.05f, 0.05f, 1.0f);
    colors[ImGuiCol_Button] = ImVec4(0.28f, 0.08f, 0.08f, 1.0f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.6f, 0.14f, 0.14f, 1.0f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.85f, 0.16f, 0.16f, 1.0f);
    colors[ImGuiCol_Tab] = ImVec4(0.14f, 0.04f, 0.04f, 1.0f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.5f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_TabActive] = ImVec4(0.88f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.86f, 0.12f, 0.12f, 1.0f);
    colors[ImGuiCol_FrameBg] = ImVec4(0.18f, 0.18f, 0.18f, 1.0f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.86f, 0.12f, 0.12f, 1.0f); // 
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.9f, 0.14f, 0.14f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.86f, 0.12f, 0.12f, 0.35f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.86f, 0.12f, 0.12f, 0.80f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.86f, 0.12f, 0.12f, 0.60f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.9f, 0.14f, 0.14f, 1.0f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.86f, 0.12f, 0.12f, 0.60f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.9f, 0.14f, 0.14f, 1.0f);
}
void Render::renderMenu() {
    ApplyModernStyle();
    using namespace ImGui;

    SetNextWindowSize(ImVec2(300, 200), ImGuiCond_FirstUseEver);
    SetNextWindowPos(ImVec2(120, 120), ImGuiCond_FirstUseEver);

    if (!Begin("untitled_external @warwick.320", &isRunning,
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse)) {
        End();
        return;
    }
    //TextColored(ImVec4(0.66f, 0.12f, 0.12f, 1.0f), "Esp");
    Text("Esp");
    ToggleSwitch("esp_toggle", &Esp_Enabled, ImVec2(68, 30));



    if (Esp_Enabled) {
        Checkbox("Show Player Names", &esp_show_names);
        Checkbox("Show Box", &esp_show_box);
        Checkbox("Show Bones", &esp_show_bones);
        Checkbox("Show Distance", &esp_show_distance);
    }
    //TextColored(ImVec4(0.66f, 0.12f, 0.12f, 1.0f), "AimBot");
    Text("Aimbot");
    ToggleSwitch("aimbot_toggle", &Aimbot_Enabled, ImVec2(68, 30));

    if (Aimbot_Enabled) {
        SliderFloat("Fov Size", &fov_size, 10.0f, 400.0f, "%.2f");
    }

    End();
}