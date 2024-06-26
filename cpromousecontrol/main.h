#pragma once

#define ID_LISTBOX 1001
#define ID_LISTBOX_INIT 1002

#define ID_BUTTON_DELETE_ACTION 2001
#define ID_BUTTON_ADD_ACTION 2002
#define ID_BUTTON_MOVE_UP 2003
#define ID_BUTTON_MOVE_DOWN 2004
#define ID_BUTTON_ACTION 2100
#define ID_BUTTON_STOP_ACTION 2101

#define ID_CHOICE_1_1 3001
#define ID_CHOICE_1_2 3002
#define ID_CHOICE_1_3 3003
#define ID_CHOICE_2_1 3011
#define ID_CHOICE_2_2 3012
#define ID_CHOICE_2_3 3013
#define ID_CHOICE_2_4 3014

#define ID_TEXT_TIME 4000
#define ID_TEXT_NORMALDISTRIBUTION 4001
#define ID_TEXT_NUM 4002
#define ID_TEXT_LOCATION 4003
#define ID_TEXT_STEP 4004
#define ID_TEXT_NORMALDISTRIBUTION_2 4005
#define ID_TEXT_CONCENTRATE 4006
#define ID_TEXT_TIME_2 4007

std::map<int, std::string> keyMap = {
    {0, ""},
    {VK_LBUTTON, "Left mouse button"},
    {VK_RBUTTON, "Right mouse button"},
    {VK_CANCEL, "Break"},
    {VK_MBUTTON, "Middle mouse button"},
    {0x05, "X1 mouse button"}, // VK_XBUTTON1
    {0x06, "X2 mouse button"}, // VK_XBUTTON2
    {0x07, "Undefined"},
    {VK_BACK, "Backspace"},
    {VK_TAB, "Tab"},
    {0x0a, "Reserved"},
    {0x0b, "Reserved"},
    {VK_CLEAR, "Clear"},
    {VK_RETURN, "Enter"},
    {0x0e, "Undefined"},
    {0x0f, "Undefined"},
    {VK_SHIFT, "Shift"},
    {VK_CONTROL, "Ctrl"},
    {VK_MENU, "Alt"},
    {VK_PAUSE, "Pause"},
    {VK_CAPITAL, "Caps Lock"},
    {VK_KANA, "Kana"},
    {0x16, "Undefined"},
    {VK_JUNJA, "Junja"},
    {VK_FINAL, "Final"},
    {VK_KANJI, "Kanji"},
    {0x1a, "Undefined"},
    {VK_ESCAPE, "Esc"},
    {VK_CONVERT, "Convert"},
    {VK_NONCONVERT, "Nonconvert"},
    {VK_ACCEPT, "Accept"},
    {VK_MODECHANGE, "Mode change"},
    {VK_SPACE, "Space"},
    {VK_PRIOR, "Page Up"},
    {VK_NEXT, "Page Down"},
    {VK_END, "End"},
    {VK_HOME, "Home"},
    {VK_LEFT, "Left"},
    {VK_UP, "Up"},
    {VK_RIGHT, "Right"},
    {VK_DOWN, "Down"},
    {VK_SELECT, "Select"},
    {VK_PRINT, "Print"},
    {VK_EXECUTE, "Execute"},
    {VK_SNAPSHOT, "Print Screen"},
    {VK_INSERT, "Ins"},
    {VK_DELETE, "Del"},
    {VK_HELP, "Help"},
    {'0', "0"},
    {'1', "1"},
    {'2', "2"},
    {'3', "3"},
    {'4', "4"},
    {'5', "5"},
    {'6', "6"},
    {'7', "7"},
    {'8', "8"},
    {'9', "9"},
    {0x3a, "Undefined"},
    {0x3b, "Undefined"},
    {0x3c, "Undefined"},
    {0x3d, "Undefined"},
    {0x3e, "Undefined"},
    {0x3f, "Undefined"},
    {0x40, "Undefined"},
    {'A', "A"},
    {'B', "B"},
    {'C', "C"},
    {'D', "D"},
    {'E', "E"},
    {'F', "F"},
    {'G', "G"},
    {'H', "H"},
    {'I', "I"},
    {'J', "J"},
    {'K', "K"},
    {'L', "L"},
    {'M', "M"},
    {'N', "N"},
    {'O', "O"},
    {'P', "P"},
    {'Q', "Q"},
    {'R', "R"},
    {'S', "S"},
    {'T', "T"},
    {'U', "U"},
    {'V', "V"},
    {'W', "W"},
    {'X', "X"},
    {'Y', "Y"},
    {'Z', "Z"},
    {VK_LWIN, "Left Windows"},
    {VK_RWIN, "Right Windows"},
    {VK_APPS, "Application"},
    {0x5e, "Reserved"},
    {0x5f, "Sleep"},
    {VK_NUMPAD0, "Num 0"},
    {VK_NUMPAD1, "Num 1"},
    {VK_NUMPAD2, "Num 2"},
    {VK_NUMPAD3, "Num 3"},
    {VK_NUMPAD4, "Num 4"},
    {VK_NUMPAD5, "Num 5"},
    {VK_NUMPAD6, "Num 6"},
    {VK_NUMPAD7, "Num 7"},
    {VK_NUMPAD8, "Num 8"},
    {VK_NUMPAD9, "Num 9"},
    {VK_MULTIPLY, "Num *"},
    {VK_ADD, "Num +"},
    {VK_SEPARATOR, "Separator"},
    {VK_SUBTRACT, "Num -"},
    {VK_DECIMAL, "Num ."},
    {VK_DIVIDE, "Num /"},
    {VK_F1, "F1"},
    { VK_F2, "F2" },
    { VK_F3, "F3" },
    { VK_F4, "F4" },
    { VK_F5, "F5" },
    { VK_F6, "F6" },
    { VK_F7, "F7" },
    { VK_F8, "F8" },
    { VK_F9, "F9" },
    { VK_F10, "F10" },
    { VK_F11, "F11" },
    { VK_F12, "F12" },
    { VK_F13, "F13" },
    { VK_F14, "F14" },
    { VK_F15, "F15" },
    { VK_F16, "F16" },
    { VK_F17, "F17" },
    { VK_F18, "F18" },
    { VK_F19, "F19" },
    { VK_F20, "F20" },
    { VK_F21, "F21" },
    { VK_F22, "F22" },
    { VK_F23, "F23" },
    { VK_F24, "F24" },
    { 0x88, "Unassigned" },
    { 0x89, "Unassigned" },
    { 0x8a, "Unassigned" },
    { 0x8b, "Unassigned" },
    { 0x8c, "Unassigned" },
    { 0x8d, "Unassigned" },
    { 0x8e, "Unassigned" },
    { 0x8f, "Unassigned" },
    { VK_NUMLOCK, "Num Lock" },
    { VK_SCROLL, "Scroll Lock" },
    { 0x92, "OEM specific" },
    { 0x93, "OEM specific" },
    { 0x94, "OEM specific" },
    { 0x95, "OEM specific" },
    { 0x96, "OEM specific" },
    { 0x97, "Unassigned" },
    { 0x98, "Unassigned" },
    { 0x99, "Unassigned" },
    { 0x9a, "Unassigned" },
    { 0x9b, "Unassigned" },
    { 0x9c, "Unassigned" },
    { 0x9d, "Unassigned" },
    { 0x9e, "Unassigned" },
    { 0x9f, "Unassigned" },
    { VK_LSHIFT, "Left Shift" },
    { VK_RSHIFT, "Right Shift" },
    { VK_LCONTROL, "Left Ctrl" },
    { VK_RCONTROL, "Right Ctrl" },
    { VK_LMENU, "Left Alt" },
    { VK_RMENU, "Right Alt" },
    { 0xa6, "Browser Back" }, // VK_BROWSER_BACK
    { 0xa7, "Browser Forward" }, // VK_BROWSER_FORWARD
    { 0xa8, "Browser Refresh" }, // VK_BROWSER_REFRESH
    { 0xa9, "Browser Stop" }, // VK_BROWSER_STOP
    { 0xaa, "Browser Search" }, // VK_BROWSER_SEARCH
    { 0xab, "Browser Favorites" }, // VK_BROWSER_FAVORITES
    { 0xac, "Browser Start" }, // VK_BROWSER_HOME
    { 0xad, "Volume Mute" }, // VK_VOLUME_MUTE
    { 0xae, "Volume Down" }, // VK_VOLUME_DOWN
    { 0xaf, "Volume Up" }, // VK_VOLUME_UP
    { 0xb0, "Next Track" }, // VK_MEDIA_NEXT_TRACK
    { 0xb1, "Previous Track" }, // VK_MEDIA_PREV_TRACK
    { 0xb2, "Stop Media" }, // VK_MEDIA_STOP
    { 0xb3, "Play/Pause Media" }, // VK_MEDIA_PLAY_PAUSE
    { 0xb4, "Start Mail" }, // VK_LAUNCH_MAIL
    { 0xb5, "Select Media" }, // VK_LAUNCH_MEDIA_SELECT
    { 0xb6, "Start Application 1" }, // VK_LAUNCH_APP1
    { 0xb7, "Start Application 2" }, // VK_LAUNCH_APP2
    { 0xb8, "Reserved" },
    { 0xb9, "Reserved" },
    { 0xba, ";" },
    { 0xbb, "+" },
    { 0xbc, "," },
    { 0xbe, "-" },
    { 0xbd, "." },
    { 0xbf, "/" },
    { 0xc0, "`" },
    { VK_OEM_4, "[" },
    { VK_OEM_5, "|" },
    { VK_OEM_6, "]" },
    { VK_OEM_7, "'" },
};