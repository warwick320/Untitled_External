#include <windows.h>
#include <SDK/Cache.h>

#include <Cheats/Players.h>
#include <Cheats/Visuals.h>
#include <ImGui/imgui_impl_win32.h>
#include <SDK/Classes/AddressMananger.h>
#include "SharedMemoryManager.h"
#include "Cheats/Misc.h"

#include <Cheats/Object.h>

RBX::Instance g_Team;

std::unique_ptr<NamedPipeServer> sharedMem;

struct CurlResponse {
    std::string data;
};

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, CurlResponse* response) {
    size_t totalSize = size * nmemb;
    response->data.append((char*)contents, totalSize);
    return totalSize;
}
void ChangeOffsets() {
    CURL* curl;
    CURLcode res;
    CurlResponse response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();

    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://offsets.ntgetwritewatch.workers.dev/offsets.json");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        res = curl_easy_perform(curl);

        if (res == CURLE_OK) {
            try {
                auto j = nlohmann::json::parse(response.data);

                if (j.contains("Children")) {
                    Offsets::Childern = std::stoull(j["Children"].get<std::string>(), 0, 16);
                }

                if (j.contains("Name")) {
                    Offsets::Name = std::stoull(j["Name"].get<std::string>(), 0, 16);
                }

                if (j.contains("ClassDescriptor")) {
                    Offsets::ClassDescriptor = std::stoull(j["ClassDescriptor"].get<std::string>(), 0, 16);
                }

                if (j.contains("Dimensions")) {
                    Offsets::Dimensions = std::stoull(j["Dimensions"].get<std::string>(), 0, 16);
                }

                if (j.contains("viewmatrix")) {
                    Offsets::ViewMatrix = std::stoull(j["viewmatrix"].get<std::string>(), 0, 16);
                }

                if (j.contains("FakeDataModelPointer")) {
                    Offsets::FakeDataModel = std::stoull(j["FakeDataModelPointer"].get<std::string>(), 0, 16);
                }

                if (j.contains("FakeDataModelToDataModel")) {
                    Offsets::RealDataModel = std::stoull(j["FakeDataModelToDataModel"].get<std::string>(), 0, 16);
                }

                if (j.contains("VisualEnginePointer")) {
                    Offsets::VisualEngine = std::stoull(j["VisualEnginePointer"].get<std::string>(), 0, 16);
                }

                if (j.contains("LocalPlayer")) {
                    Offsets::LocalPlayer = std::stoull(j["LocalPlayer"].get<std::string>(), 0, 16);
                }

                if (j.contains("ModelInstance")) {
                    Offsets::ModelInstance = std::stoull(j["ModelInstance"].get<std::string>(), 0, 16);
                }

                if (j.contains("Primitive")) {
                    Offsets::Primitive = std::stoull(j["Primitive"].get<std::string>(), 0, 16);
                }

                if (j.contains("PartSize")) {
                    Offsets::PartSize = std::stoull(j["PartSize"].get<std::string>(), 0, 16);
                }

                if (j.contains("Position")) {
                    Offsets::PartPosition = std::stoull(j["Position"].get<std::string>(), 0, 16);
                }

                if (j.contains("Team")) {
                    Offsets::Team = std::stoull(j["Team"].get<std::string>(), 0, 16);
                }

                if (j.contains("CanCollide")) {
                    Offsets::CanCollide = std::stoull(j["CanCollide"].get<std::string>(), 0, 16);
                }

                if (j.contains("CFrame")) {
                    Offsets::CFrame = std::stoull(j["CFrame"].get<std::string>(), 0, 16);
                }

                if (j.contains("WalkSpeed")) {
                    Offsets::speedAddress = std::stoull(j["WalkSpeed"].get<std::string>(), 0, 16);
                }

                if (j.contains("WalkSpeedCheck")) {
                    Offsets::WalkSpeedCheck = std::stoull(j["WalkSpeedCheck"].get<std::string>(), 0, 16);
                }

                if (j.contains("JumpPower")) {
                    Offsets::JumpPowerAddress = std::stoull(j["JumpPower"].get<std::string>(), 0, 16);
                }
                debug_print("Successfully updated offsets from web", 0);
            }
            catch (const std::exception& e) {
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "JSON parsing error: %s", e.what());
                debug_print(errorMsg, 1);
            }
        }
        else {
            char errorMsg[256];
            snprintf(errorMsg, sizeof(errorMsg), "curl_easy_perform() failed: %s", curl_easy_strerror(res));
            debug_print(errorMsg, 1);
        }

        curl_easy_cleanup(curl);
    }
    else {
        debug_print("Failed to initialize curl for offset update", 1);
    }

    curl_global_cleanup();
}
void inject() {
    printf("Sizeof Data %d \n", sizeof(SharedData));
    debug_print("injecting", 0);
    if (!comms->is_connected()) {
        debug_print("Failed connect to Ntdll", 1);
        sleep_ms(5000);
        exit(0);
    }
    debug_print("Connected to Ntdll successfully", 0);
    debug_print("Waiting for roblox", 0);
	debug_print("Build Number 0", 0);
    while (comms->find_process("RobloxPlayerBeta.exe") == 0) sleep_ms(4000);
    printf("[+] PID: %d\n", comms->process_id);

    debug_print("Attaching to process wait a sec", 0);
    sleep_ms(2000);
    if (!comms->v_attach(comms->process_id)) {
        debug_print("Failed to attach to process", 1);
        sleep_ms(5000);
        exit(0);
    }
    debug_print("Successfully attached to process", 0);

    if (comms->find_image() == 0) {
        debug_print("Failed to get the base address", 1);
        sleep_ms(5000);
        exit(0);
    }

    printf("Found Roblox with base address: %llx\n", comms->image_address);

    fakeDataModel = comms->read<u64>(comms->image_address + Offsets::FakeDataModel);
    dataModel = std::make_unique<RBX::DataModel>(comms->read<u64>(fakeDataModel + Offsets::RealDataModel));

    printf("First DataModel -> 0x%llx\n", dataModel->getAddress());

    visualEngine = std::make_unique<RBX::VisualEngine>(comms->read<u64>(comms->image_address + Offsets::VisualEngine));
    printf("First VisEngine -> 0x%llx\n", visualEngine->getAddress());
}
bool MemShared = false;
void SharedMemoryUpdateThread() {
    while (MemShared && runningThread) {
        sleep_ms(1);

        if (sharedMem) {
            try {
                sharedMem->ReadFromShared();
                sharedMem->UpdateToShared();
            }
            catch (...) {
                std::cout << "[ERROR] Exception in SharedMemoryUpdateThread!" << std::endl;
                break;
            }
        }
        else {
            std::cout << "[ERROR] SharedMem is null!" << std::endl;
            break;
        }
    }
    debug_print("SharedMemoryUpdateThread exiting", 0);
}
HANDLE g_qmlProcessHandle = nullptr;

void monitorQMLProcess() {
    if (g_qmlProcessHandle == nullptr) {
        debug_print("No QML process to monitor", 1);
        return;
    }
    
    debug_print("Starting QML process monitor", 0);
    DWORD waitResult = WaitForSingleObject(g_qmlProcessHandle, INFINITE);
    
    if (waitResult == WAIT_OBJECT_0) {
        debug_print("QML process has terminated - Shutting down main process", 0);
        runningThread = false;
        MemShared = false;
        
        if (render) {
            render->isRunning = false;
        }
        TerminateProcess(GetCurrentProcess(), 0);
    } else {
        debug_print("Failed to wait for QML process", 1);
    }
}

void startQML() {
    debug_print("Starting QML UI", 0);
    char fullPath[MAX_PATH];
    GetCurrentDirectoryA(MAX_PATH, fullPath);

    snprintf(fullPath, sizeof(fullPath), "%s\\ui\\EX.exe", fullPath);
    if (GetFileAttributesA(fullPath) == INVALID_FILE_ATTRIBUTES) {
        debug_print("EX.exe file does not exist", 1);
        return;
    }
    HANDLE hJob = CreateJobObjectA(NULL, NULL);
    if (hJob == NULL) {
        debug_print("Failed to create job object", 1);
        return;
    }

    JOBOBJECT_EXTENDED_LIMIT_INFORMATION jeli = { 0 };
    jeli.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
    if (!SetInformationJobObject(hJob, JobObjectExtendedLimitInformation, &jeli, sizeof(jeli))) {
        debug_print("Failed to set job object information", 1);
        CloseHandle(hJob);
        return;
    }

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (CreateProcessA(
        fullPath,       // application name
        NULL,           // command line
        NULL,           // process attributes
        NULL,           // thread attributes
        FALSE,          // inherit handles
        CREATE_SUSPENDED, // creation flags
        NULL,           // environment
        NULL,           // current directory
        &si,            // startup info
        &pi             // process info
    )) {
        if (AssignProcessToJobObject(hJob, pi.hProcess)) {
            g_qmlProcessHandle = pi.hProcess;
            
            ResumeThread(pi.hThread);
            debug_print("QML UI started successfully with CreateProcess and assigned to job", 0);
            
            std::thread(monitorQMLProcess).detach();
        }
        else {
            debug_print("Failed to assign process to job object", 1);
            TerminateProcess(pi.hProcess, 0);
            CloseHandle(pi.hProcess);
        }

        CloseHandle(pi.hThread);
    }
    else {
        DWORD error = GetLastError();
        char errorMsg[256];
        snprintf(errorMsg, sizeof(errorMsg), "CreateProcess failed - Error code: %lu", error);
        debug_print(errorMsg, 1);
        CloseHandle(hJob);
    }
}


str CurrentVersion;

void currentVersion() {
    CURL* curl;
    CURLcode res;
    CurlResponse response;

    curl_global_init(CURL_GLOBAL_DEFAULT);
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, "https://offsets.ntgetwritewatch.workers.dev/version");
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            debug_print(("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res))).c_str(), 1);
            CurrentVersion = "fetch_failed";
        }
        else {
            std::string trimmed_response = response.data;
            size_t start = trimmed_response.find_first_not_of(" \t\r\n");
            size_t end = trimmed_response.find_last_not_of(" \t\r\n");

            if (start != std::string::npos && end != std::string::npos) {
                CurrentVersion = trimmed_response.substr(start, end - start + 1);
                debug_print(("Successfully fetched version: " + CurrentVersion).c_str(), 0);
            }
            else {
                debug_print("Empty or invalid response received", 1);
                CurrentVersion = "invalid_response";
            }
        }

        curl_easy_cleanup(curl);
    }
    else {
        debug_print("Failed to initialize curl", 1);
        CurrentVersion = "curl_init_failed";
    }

    curl_global_cleanup();
}

void versionChecker() {
    currentVersion();
    char currentDir[MAX_PATH];
    if (!GetCurrentDirectoryA(MAX_PATH, currentDir)) {
        debug_print("Failed to get current directory", 1);
        return;
    }
    char versionFilePath[MAX_PATH];
    snprintf(versionFilePath, sizeof(versionFilePath), "%s\\version.cfg", currentDir);

    FILE* file = nullptr;
    errno_t err = fopen_s(&file, versionFilePath, "r");
    if (err != 0 || !file) {
        debug_print("Failed to open version.cfg file", 1);
        return;
    }
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), file) == nullptr) {
        debug_print("Failed to read version from file", 1);
        fclose(file);
        return;
    }
    fclose(file);
    buffer[strcspn(buffer, "\r\n")] = '\0';

    printf("Offsets Version: %s\n", buffer);
    printf("Current Version: %s\n", CurrentVersion.c_str());
    if(strcmp(buffer, CurrentVersion.c_str()) == 0) {
        debug_print("You are using the latest version.", 0);
    } else if (CurrentVersion == "fetch_failed" || CurrentVersion == "curl_init_failed" || CurrentVersion == "invalid_response") {
        debug_print("Could not fetch the latest version. Please check your internet connection.", 1);
    }
    else {
        ChangeOffsets();
       
    }
}

auto main(int argc, char** argv) -> int {
    versionChecker();
    inject();
    sharedMem = std::make_unique<NamedPipeServer>();
    if (sharedMem->Initialize()) {
        debug_print("Ready for UI communication", 0);
        sharedMem->PrintDebugInfo();
    }
    else {
        debug_print("Failed to initialize shared memory!", 1);
        return -1;
    }

    render = std::make_unique<Render>();
    render->setupOverlay("Untitled_External");

    std::thread playerThread(updatePlayers);
    std::thread addressThread(CheckAddressesLoop);
    std::thread miscThread(miscLoop);
    std::thread workspaceThread(WorkspaceObjPos);
    std::thread sharedMemThread(SharedMemoryUpdateThread);
    std::thread triggerThread(triggerBotThread);

    bool firstTime = false;

    while (render->isRunning) {
        //Fly();
        if (!firstTime) {
            firstTime = true;
            MemShared = true;
            sleep_ms(100);
            std::thread(startQML).detach();
        }

        if (!runningThread) {
            debug_print("Restart signal received - Terminating all threads", 0);

            MemShared = false;
            render->isRunning = false;


            debug_print("Waiting for threads to finish...", 0);
            sleep_ms(500);


            debug_print("Force terminating threads...", 0);


            char currentPath[MAX_PATH];
            GetModuleFileNameA(NULL, currentPath, MAX_PATH);

            debug_print("Restarting program...", 0);


            STARTUPINFOA si = { sizeof(si) };
            PROCESS_INFORMATION pi;

            if (CreateProcessA(
                currentPath,
                NULL,
                NULL,
                NULL,
                FALSE,
                0,
                NULL,
                NULL,
                &si,
                &pi
            )) {
                debug_print("Program restart initiated successfully", 0);
                CloseHandle(pi.hProcess);
                CloseHandle(pi.hThread);


                TerminateProcess(GetCurrentProcess(), 0);
            }
            else {
                DWORD error = GetLastError();
                char errorMsg[256];
                snprintf(errorMsg, sizeof(errorMsg), "Failed to restart program - Error code: %lu", error);
                debug_print(errorMsg, 1);

                TerminateProcess(GetCurrentProcess(), 1);
            }
        }

        render->startRender();
        if (Esp_Enabled) espLoop();
        if (Aimbot_Enabled) aimbotLoop();
        render->endRender();
    }

    runningThread = false;
    MemShared = false;


    sleep_ms(200);

    try {
        if (triggerThread.joinable()) triggerThread.join();
        if (playerThread.joinable()) playerThread.join();
        if (sharedMemThread.joinable()) sharedMemThread.join();
        if (addressThread.joinable()) addressThread.join();
        if (miscThread.joinable()) miscThread.join();
        if (workspaceThread.joinable()) workspaceThread.join();
    }
    catch (...) {
        debug_print("Exception during thread cleanup", 1);
    }

    return 0;
}