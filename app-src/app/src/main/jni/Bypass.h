#include <jni.h>
#include <string>
#include <list>
#include <vector>
#include <string.h>
#include <pthread.h>
#include <vector>
#include <thread>
#include <jni.h>
#include <fstream>
#include <unistd.h>
#include <sys/mman.h>
#include <unwind.h>
#include <dlfcn.h>
#include <libgen.h>
#include <stdint.h>
#include <cstring>
#include <functional>
#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <ctime>
#include <chrono> 
#include <fcntl.h>
#include <sys/stat.h>
#include <cstddef>
#include <semaphore.h>
#include <stdint.h>
#include <sstream>
#include <stdarg.h>
#include <stdio.h>
#include <curl/curl.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <stdlib.h>
#include <cstdint>
#include <cstdio>
#include <stdbool.h>
#include "Main/Tools.h" 
#include "Main/Logger.h"
#include "Main/oxorany.h"
#include "Main/obfuscate.h"
#include "Main/Utils.h"
#include "Main/KittyMemory/MemoryPatch.h"
#include <random> //
#include "Main/Dobby/dobby.h"
#include "Main/Macros.h"

//CRASHFIXER 
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>

class _BYTE;
class _QWORD;
class _DWORD;
class _WORD;
#define _QWORD long
#define _DWORD long
#define _BYTE long
#define _WORD long
//#define HOOK_LIB
#define targetLibName oxorany("libanogs.so")
#define targetLibName oxorany("libUE4.so")
#define targetLibName oxorany("libTBlueData.so")
#define targetLibName oxorany("libhdmpve.so")
#include <unordered_map>
using namespace std;
#define __int8 char
#define __int16 short
#define __int32 int
#define __int64 long long
#define _BYTE  uint8_t
#define _WORD  uint16_t
#define _DWORD uint32_t
#define _QWORD uint64_t
#define _OWORD uint64_t
#define _BOOL8 uint64_t
char *Offset;
#define ret_zero
#define j_j__free
#define log_suspicious_activity
#define apply_cheat_penalty
#define check_memory_integrity
#define __OFSUB__
#define AnoSDKIoctlOld_0
#define HIWORD
#define JUMPOUT
#define byte_4
#define ARM64_SYSREG
#include <random>
#define _ReadStatusReg
#define BYTE5
#define BYTE4
#define HIBYTE
#define BYTE6
#define sub_95A8204
#define IsMemoryReadable
#define BYTE1
#define BYTE3
#define MEMORY_BASIC_INFORMATION mbi
#define BYTE2
#define _WriteStatusReg
typedef long long int64; 
typedef short int16;     
uintptr_t ANOGS;
uintptr_t UE5;
DWORD EGLBase = 0;
DWORD EGLSize = 0;
DWORD EGLAlloc = 0;
DWORD libcBase = 0;
DWORD libcSize = 0;
DWORD libcAlloc = 0;
//DWORD libUE4Base = 0;
DWORD UE4Base = 0;
//DWORD libanogsBase = 0;
DWORD libanortBase = 0;
DWORD libanogsAlloc = 0;
DWORD libUE4Alloc = 0;
unsigned int libanogsSize  = 0;
unsigned int libUE4Size  = 0;
DWORD NewBase = 0;
uintptr_t anogs;

static uintptr_t libanogsBase = 0;
static uintptr_t libUE4Base = 0;







///PRIVATE HOOKS
#define THUNDER(RET, NAME, ARGS) \
    typedef RET (*NAME##_t) ARGS; \
    static NAME##_t o##NAME = nullptr; \
    static RET h##NAME ARGS

static void *ResolveSymbol(const char *lib, const char *sym) {
    void *handle = dlopen(lib, RTLD_NOW);
    return handle ? dlsym(handle, sym) : nullptr;
}

//MADE BY @THUNDEROWNERX
static bool WritePatch(void *target, const void *patch, size_t size) {
    if (!target || !patch || size == 0) {
        return false;
    }

    const long pageSize = sysconf(_SC_PAGESIZE);
    const uintptr_t addr = reinterpret_cast<uintptr_t>(target);
    const uintptr_t pageStart = addr & ~(static_cast<uintptr_t>(pageSize) - 1);
    const uintptr_t pageEnd = (addr + size + pageSize - 1) & ~(static_cast<uintptr_t>(pageSize) - 1);
    const size_t len = pageEnd - pageStart;

    if (mprotect(reinterpret_cast<void *>(pageStart), len, PROT_READ | PROT_WRITE | PROT_EXEC) != 0) {
        return false;
    }

    std::memcpy(target, patch, size);
    __builtin___clear_cache(reinterpret_cast<char *>(target), reinterpret_cast<char *>(target) + size);
    mprotect(reinterpret_cast<void *>(pageStart), len, PROT_READ | PROT_EXEC);
    return true;
}


//NOP OR RET0 DEFINED HERE 
static const uint8_t kNop[] = {0x1F, 0x20, 0x03, 0xD5};
  static const uint8_t kRet0[] = {0x00, 0x00, 0x80, 0xD2, 0xC0, 0x03, 0x5F, 0xD6};
  
  
//ADD HOOK DEFINE HERE 

size_t hook_strlen(const char *s) {
if (s != nullptr) {
if (strstr(s, "BATTLEGROUNDS MOBILE INDIA is not a real-world")) {
const char *SidMsg = "Bypass Activated... \n after 3 Game Restart\n Play Safe Always\n Send Feedback to Owner";
memset(const_cast<char *>(s), 0, strlen(s)); 
strcpy(const_cast<char *>(s), SidMsg);
}}
return strlen(s);
}        


#include "Main/encrypt_protect.h"
#include "LoginKey.h"

// LOGIN STATE
bool isLoggedIn = false;
bool loginAttempted = false;
std::string loginMessage = "Paste Key in Clipboard";
std::string loginStatus = "";

// Draw Login Screen using Eagle_GUI (called from game render thread)
void DrawLoginScreen() {
    if (!tslFontUI) return;
    
    float screenW = 800;
    float screenH = 600;
    
    // Dark overlay background
    DrawFilledRect(Canvas, FVector2D{0, 0}, screenW, screenH, FLinearColor(0, 0, 0, 0.85f));
    
    // Login box
    float boxW = 400;
    float boxH = 300;
    float boxX = (screenW - boxW) / 2;
    float boxY = (screenH - boxH) / 2;
    
    // Box background
    DrawFilledRect(Canvas, FVector2D{boxX, boxY}, boxW, boxH, FLinearColor(0.10f, 0.10f, 0.15f, 0.95f));
    
    // Box border
    DrawRectangle(Canvas, FVector2D{boxX, boxY}, boxW, boxH, 2.0f, FLinearColor(0.33f, 0.30f, 0.90f, 1.0f));
    
    // Title bar
    DrawFilledRect(Canvas, FVector2D{boxX, boxY}, boxW, 40.0f, FLinearColor(0.15f, 0.20f, 0.89f, 1.0f));
    
    // Title text
    DrawText(Canvas, FString("KEY LOGIN"), FVector2D{boxX + boxW/2 - 50, boxY + 12}, FLinearColor(1,1,1,1), FLinearColor(0,0,0,1), 16, true);
    
    // Instruction text
    DrawText(Canvas, FString("Paste your license key in clipboard"), FVector2D{boxX + 20, boxY + 60}, FLinearColor(0.8f,0.8f,0.8f,1), FLinearColor(0,0,0,1), 12, false);
    DrawText(Canvas, FString("then tap the START button below"), FVector2D{boxX + 20, boxY + 80}, FLinearColor(0.8f,0.8f,0.8f,1), FLinearColor(0,0,0,1), 12, false);
    
    // Key display area (shows clipboard content preview)
    DrawFilledRect(Canvas, FVector2D{boxX + 20, boxY + 110}, boxW - 40, 40.0f, FLinearColor(0.05f, 0.05f, 0.1f, 1.0f));
    DrawRectangle(Canvas, FVector2D{boxX + 20, boxY + 110}, boxW - 40, 40.0f, 1.0f, FLinearColor(1.0f, 0.4f, 0, 1.0f));
    
    std::string clipPreview = getClipboardText();
    if (clipPreview.length() > 30) clipPreview = clipPreview.substr(0, 30) + "...";
    if (clipPreview.empty()) clipPreview = "(empty - paste key here)";
    DrawText(Canvas, FString(clipPreview.c_str()), FVector2D{boxX + 30, boxY + 122}, FLinearColor(1,1,1,1), FLinearColor(0,0,0,1), 11, false);
    
    // START button
    float btnW = 200;
    float btnH = 45;
    float btnX = boxX + (boxW - btnW) / 2;
    float btnY = boxY + 170;
    
    bool btnHovered = MouseInZone(FVector2D{btnX, btnY}, FVector2D{btnW, btnH});
    FLinearColor btnColor = btnHovered ? FLinearColor(0.20f, 0.25f, 0.94f, 1.0f) : FLinearColor(0.33f, 0.30f, 0.90f, 1.0f);
    DrawFilledRect(Canvas, FVector2D{btnX, btnY}, btnW, btnH, btnColor);
    DrawText(Canvas, FString("START"), FVector2D{btnX + btnW/2 - 30, btnY + 14}, FLinearColor(1,1,1,1), FLinearColor(0,0,0,1), 16, true);
    
    // Handle button click
    if (btnHovered && MouseDown) {
        if (!loginAttempted) {
            loginAttempted = true;
            loginMessage = "Validating...";
        }
    }
    
    // Status message
    if (!loginStatus.empty()) {
        FLinearColor statusColor = (loginStatus.find("OK") != std::string::npos) ? 
            FLinearColor(0, 1, 0, 1) : FLinearColor(1, 0.3f, 0.3f, 1);
        DrawText(Canvas, FString(loginStatus.c_str()), FVector2D{boxX + 20, boxY + 240}, statusColor, FLinearColor(0,0,0,1), 13, false);
    }
    
    // Footer
    DrawText(Canvas, FString("Contact admin for key"), FVector2D{boxX + boxW/2 - 70, boxY + boxH - 25}, FLinearColor(0.5f,0.5f,0.5f,1), FLinearColor(0,0,0,1), 10, false);
}
// Hook install (run at init)
 
void *THUNDER1_thread(void *) {
        UE5 = Tools::GetBaseAddress("libUE4.so");
        while (!UE5) {
        UE5 = Tools::GetBaseAddress("libUE4.so");
            sleep(1);
    }
    
    ANOGS = Tools::GetBaseAddress("libanogs.so");
        while (!ANOGS) {
        ANOGS = Tools::GetBaseAddress("libanogs.so");
            sleep(1);
    }
    do {
        sleep(1);
    } while (!isLibraryLoaded(targetLibName));
    libanogsBase = findLibrary("libanogs.so");
    libUE4Base   = findLibrary("libUE4.so");
    
    libanogsAlloc = (DWORD)malloc(libanogsSize);
    libUE4Alloc   = (DWORD)malloc(libUE4Size);
    memcpy((void *)libanogsAlloc, (void *)libanogsBase, libanogsSize);
    memcpy((void *)libUE4Alloc, (void *)libUE4Base, libUE4Size);
    LOGI(" THUNDER BYPASS LOADED");
    LOGI("UE4 size : 0x%04X | ANOGS size : 0x%04X",libUE4Size,libanogsSize);

    // ==================== LOGIN FLOW ====================
    LOGI("Waiting for key validation...");
    showToast("Paste your license key in clipboard");
    
    // Wait for Eagle_GUI to be ready (fonts loaded by game)
    int waitCount = 0;
    while (!tslFontUI && waitCount < 120) {
        sleep(1);
        waitCount++;
    }
    
    if (!tslFontUI) {
        LOGI("Font not available, using clipboard auto-detect mode");
        // Fallback: clipboard-based login without GUI
        showToast("Waiting for key in clipboard...");
        while (!isLoggedIn) {
            std::string key = getClipboardText();
            if (!key.empty() && key.length() >= 5) {
                loginAttempted = true;
                showToast("Validating key...");
                std::string result = Login(key.c_str());
                if (result == "OK") {
                    isLoggedIn = true;
                    showToast("Key verified! Loading...");
                    LOGI("Login successful");
                } else {
                    loginStatus = "Invalid: " + result;
                    LOGI("Login failed: %s", result.c_str());
                    showToast(("Key invalid: " + result).c_str());
                    loginAttempted = false;
                    sleep(5);
                }
            }
            if (!isLoggedIn) sleep(3);
        }
    } else {
        // GUI-based login: wait for user to paste key and tap Start
        showToast("Login screen active - paste key & tap Start");
        while (!isLoggedIn) {
            if (loginAttempted) {
                std::string key = getClipboardText();
                if (key.empty()) {
                    loginStatus = "Clipboard empty!";
                    showToast("Paste key in clipboard first!");
                    loginAttempted = false;
                } else {
                    loginStatus = "Checking key...";
                    std::string result = Login(key.c_str());
                    if (result == "OK") {
                        isLoggedIn = true;
                        loginStatus = "Login successful!";
                        showToast("Key verified! Loading bypass...");
                        LOGI("Login successful");
                    } else {
                        loginStatus = "Failed: " + result;
                        LOGI("Login failed: %s", result.c_str());
                        showToast(("Key failed: " + result).c_str());
                        loginAttempted = false;
                    }
                }
            }
            sleep(1);
        }
    }
    // ==================== LOGIN DONE ====================

   #if defined(__aarch64__)
 LOGI("====================================================");
 LOGI("======< LIBRARY LOADED BY @COLOR_RED >=====");
 LOGI("====================================================");
 HOOK_LIB_NO_ORIG("libUE4.so","0XC8E55E0",hook_strlen);//SKINHACK

    LOGI(OBFUSCATE("Done"));
#endif
	 
  return NULL;
}



__attribute__((constructor)) void mainload() {
    
   pthread_t ptid;
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_create(&ptid, &attr, THUNDER1_thread, NULL);
    pthread_attr_destroy(&attr);
   
   
    
}
