#include "windowtools_win.h"
#include <tlhelp32.h>
#include "birdtrayapp.h"


/**
 * Helper data structure for the findMainWindow function.
 */
struct _WindowFindData {
    HWND windowHandle;
    DWORD processId;
};

/**
 * Determine whether a window handle is the main window of the corresponding process.
 *
 * @param handle The window handle.
 * @return True if the handle is the main window handle.
 */
static bool isMainWindow(HWND handle) {
    return GetWindow(handle, GW_OWNER) == static_cast<HWND>(nullptr) && IsWindowVisible(handle);
}

/**
 * Check if the given window is the main window of the given process.
 *
 * @param handle The handle of the window.
 * @param parameter In/Out parameter, determining the id of the process and
 *                  providing a space to store the window handle.
 * @return TRUE if the window does not match the search criteria.
 */
static BOOL CALLBACK enumWindowsCallback(HWND handle, LPARAM parameter) {
    _WindowFindData &data = *reinterpret_cast<_WindowFindData*>(parameter);
    DWORD processId = 0;
    GetWindowThreadProcessId(handle, &processId);
    if (data.processId != processId || !isMainWindow(handle)) {
        return TRUE;
    }
    data.windowHandle = handle;
    return FALSE;
}

/**
 * Find the main window of the given process.
 *
 * @param process_id The id of the target process.
 * @return The main window of the process or nullptr.
 */
static HWND findMainWindow(DWORD process_id) {
    _WindowFindData data = {};
    data.processId = process_id;
    data.windowHandle = nullptr;
    EnumWindows(enumWindowsCallback, reinterpret_cast<LPARAM>(&data));
    return data.windowHandle;
}

/**
 * Get the process id of a process by the name of the executable.
 *
 * @param processName The name of the executable of the process.
 * @return The process id or 0.
 */
static DWORD getProcessId(LPCWCH processName) {
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);
    
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (Process32First(snapshot, &entry) == TRUE) {
        while (Process32Next(snapshot, &entry) == TRUE) {
            if (_wcsicmp(entry.szExeFile, processName) == 0) {
                CloseHandle(snapshot);
                return entry.th32ProcessID;
            }
        }
    }
    CloseHandle(snapshot);
    return 0;
}


WindowTools_Win::WindowTools_Win() : WindowTools() {
    betterbirdWindow = nullptr;
    betterbirdMinimizeHook = nullptr;
}

WindowTools_Win::~WindowTools_Win() {
    betterbirdWindow = nullptr;
    if (betterbirdMinimizeHook != nullptr) {
        UnhookWinEvent(betterbirdMinimizeHook);
        betterbirdMinimizeHook = nullptr;
    }
}

bool WindowTools_Win::lookup() {
    if (isValid()) {
        return betterbirdWindow;
    }
    
    DWORD betterbirdProcessId = getProcessId(L"betterbird.exe");
    if (betterbirdProcessId == 0) {
        return false;
    }
    
    betterbirdWindow = findMainWindow(betterbirdProcessId);
    if (betterbirdWindow == nullptr) {
        return false;
    }
    DWORD betterbirdThreadId = GetWindowThreadProcessId(betterbirdWindow, &betterbirdProcessId);
    if (betterbirdThreadId) {
        betterbirdMinimizeHook = SetWinEventHook(
                EVENT_SYSTEM_MINIMIZESTART, EVENT_SYSTEM_MINIMIZESTART, nullptr,
                &WindowTools_Win::minimizeCallback, betterbirdProcessId,
                betterbirdThreadId, WINEVENT_OUTOFCONTEXT);
    }
    return true;
}

bool WindowTools_Win::show() {
    if (!checkWindow()) {
        return false;
    }
    if (IsIconic(this->betterbirdWindow)) {
        ShowWindow(this->betterbirdWindow, SW_RESTORE);
    } else {
        ShowWindow(this->betterbirdWindow, SW_SHOW);
    }
    if (SetForegroundWindow(this->betterbirdWindow) == TRUE) {
        emit onWindowShown();
        return true;
    }
    return false;
}

bool WindowTools_Win::hide() {
    if (!checkWindow()) {
        return false;
    }
    if (ShowWindow(this->betterbirdWindow, SW_HIDE) > 0) {
        emit onWindowHidden();
        return true;
    }
    return false;
}

bool WindowTools_Win::isHidden() {
    return isValid() && !IsWindowVisible(this->betterbirdWindow);
}

bool WindowTools_Win::closeWindow() {
    if (!checkWindow()) {
        return false;
    }
    show();
    return SendMessage(this->betterbirdWindow, WM_CLOSE, 0, 0) == 0;
}

bool WindowTools_Win::isValid() {
    return betterbirdWindow != nullptr && IsWindow(betterbirdWindow);
}

bool WindowTools_Win::checkWindow() {
    return isValid() || lookup();
}

void CALLBACK WindowTools_Win::minimizeCallback(
        HWINEVENTHOOK eventHook, DWORD event, HWND window, LONG idObject,
        LONG idChild, DWORD idEventThread, DWORD eventTime) {
    Q_UNUSED(eventHook)
    Q_UNUSED(idEventThread)
    Q_UNUSED(eventTime)
    BirdtrayApp* app = BirdtrayApp::get();
    auto* winTools = dynamic_cast<WindowTools_Win*>(app->getTrayIcon()->getWindowTools());
    if (event == EVENT_SYSTEM_MINIMIZESTART &&
        window == winTools->betterbirdWindow &&
        idObject == OBJID_WINDOW && idChild == INDEXID_CONTAINER &&
        app->getSettings()->mHideWhenMinimized && winTools->isValid() &&
        IsIconic(winTools->betterbirdWindow) && IsWindowVisible(winTools->betterbirdWindow)) {
        winTools->hide();
    }
}
