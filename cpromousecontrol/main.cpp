#include <windows.h>
#include <iostream>
#include <locale>
#include <codecvt>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <cctype>
#include <algorithm>

#include "mousecontrol.h"
#include "main.h"


bool isClickChooseZhengtai = false;
bool isClickChoosePlentyTime  = false;
bool isThreadRunning = false;
bool isVariblesExist = false;
extern bool isSpecialPointShouldReget;
std::atomic<bool> exitFlag(false);
HWND hwnd_zt, hwnd_num, hwnd_location, hwnd_speed, hwnd_zt_text, hwnd_num_text, hwnd_concentrate, hwnd_normal_2, hwnd_time,
hwnd_loc_text, hwnd_speed_text, hwnd_time_text, hwnd_concentrate_text, hwnd_normal_2_text;
HWND hwnd_;
HANDLE action_Thread = NULL;

void separateAlphaNumeric(const std::string& str, std::string& alphaPart, std::vector<double>& num_vector);

std::map<int, std::string> textBoxes = {
    {ID_TEXT_TIME, "time_t"},
    {ID_TEXT_NORMALDISTRIBUTION, "normal_t"},
    {ID_TEXT_NUM, "num_t"},
    {ID_TEXT_LOCATION, "location_t"},
    {ID_TEXT_STEP, "step_t"},
    {ID_TEXT_NORMALDISTRIBUTION_2, "normal2_t"},
    {ID_TEXT_CONCENTRATE, "concentrate_t"},
    {ID_TEXT_TIME_2, "time_t"}
};

std::map<std::string, std::string> defaultValues = {
    {"time_t", "50"},
    {"normal_t", "1"},
    {"num_t", "1"},
    {"location_t", "[0,0]"},
    {"step_t", "10"},
    {"normal2_t", "1"},
    {"concentrate_t", "10"},
    {"time_t", "500"}
};

std::vector<std::string> varibles = {"","","","","","","",""};


struct INPUT_1
{
    int kind = 0;
    int style;
    int num;
    int times;
    int style_arg = 1;
    int num_arg = 1;
};
INPUT_1 inputs_1;

struct INPUT_2
{
    int kind;
    int m_style;
    int s_style;
    int p_style;
    POINT L;
    double speed;
    double time;
    double move_arg;
    double speed_arg;
};
INPUT_2 inputs_2;

struct InputUnion {
    enum class Type { Input1, Input2 };
    Type type;
    union {
        INPUT_1 input1;
        INPUT_2 input2;
    };

    InputUnion(INPUT_1 i) : type(Type::Input1), input1(i) {}
    InputUnion(INPUT_2 i) : type(Type::Input2), input2(i) {}
};
std::vector<InputUnion> inputs_all;

HHOOK g_hHook = NULL;

std::string ReadFromCOMBOX(HWND hwnd, int ID) {
    int selectedIndex = SendMessage(GetDlgItem(hwnd, ID), CB_GETCURSEL, 0, 0);
    if (!isVariblesExist) {
        const int bufferSize = 16;
        wchar_t buffer[bufferSize];
        std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;
        for (const auto& textBox : textBoxes) {
            std::string variable = textBox.second;
            GetWindowText(GetDlgItem(hwnd, textBox.first), buffer, bufferSize);
            if (std::wstring(buffer) == L"") {
                variable = defaultValues[variable];
            }
            else {
                variable = converter.to_bytes(buffer);
            }
            varibles[textBox.first - ID_TEXT_TIME] = variable;
        }
        isVariblesExist = true;
    }

    if (selectedIndex == CB_ERR) {
        return "";
    }
    switch (ID){
        case ID_CHOICE_1_1:
            switch (selectedIndex) {
            case 0: return "LC";
            case 1: return "RC";
            case 2: return "LP";
            case 3: return "RP";
            case 4: return "SPARE";
            case 5: return "";
            }
        case ID_CHOICE_1_2:
            switch (selectedIndex) {
            case 0: return ("AVER" + varibles[1]);
            case 1: return ("NORMAL" + varibles[1]);
            case 2: return "PRE";
            case 3: return "PRE";
            }
        case ID_CHOICE_1_3:
            switch (selectedIndex) {
            case 0: return ("SINGLE" + varibles[0]);
            case 1: return ("MANY" + varibles[0]+"&"+ varibles[2]);
            case 2: return ("SINGLE" + varibles[0]);
            }
        case ID_CHOICE_2_1:
            switch (selectedIndex) {
            case 0: return ("ADDMOVE" + varibles[4] + "&" + varibles[3]);
            case 1: return ("LOCMOVE" + varibles[4] + "&" + varibles[3]);
            case 2: return ("RANDMOVE" + varibles[4] + "&" + varibles[7]);
            case 3: return ("RANDMOVEC" + varibles[4] + "&" + varibles[7]);
            case 4: return "";
            }
        case ID_CHOICE_2_2:
            switch (selectedIndex) {
            case 0: return "NOERR";
            case 1: return ("RANDW"+varibles[6]);
            case 2: return "NOERR";
            }
        case ID_CHOICE_2_3:
            switch (selectedIndex) {
            case 0: return "SPPEDNOERR";
            case 1: return ("SPPEDNORMAL"+varibles[5]);
            case 2: return "SPPEDNOERR";
            }
        case ID_CHOICE_2_4:
            switch (selectedIndex) {
            case 0: return "NP";
            case 1: return "LP";
            case 2: return "RP";
            case 3: return "NP";
            }

    }
    return "";
}

void PushSettingIntoList(HWND hwnd) {
    std::string pushs = "";
    std::string push;
    for (int i = 0; i < 3; i++) {
        push = ReadFromCOMBOX(hwnd, ID_CHOICE_1_1+i);
        if (push.empty()) {
            break;
        }
        if (pushs.empty()) {
            pushs = pushs + push;
        }
        else {
            pushs = pushs + "#" + push;
        }
    }
    if (!push.empty()) {
        std::wstring widePushs(pushs.begin(), pushs.end());
        SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_ADDSTRING, 0, (LPARAM)widePushs.c_str());
    }
    pushs = "";
    for (int i = 0; i < 4; i++) {
        push = ReadFromCOMBOX(hwnd, ID_CHOICE_2_1+i);
        if (push.empty()) {
            break;
        }
        if (pushs.empty()) {
            pushs = pushs + push;
        }
        else {
            pushs = pushs + "#" + push;
        }
    }
    if (!push.empty()) {
        std::wstring widePushs(pushs.begin(), pushs.end());
        SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_ADDSTRING, 0, (LPARAM)widePushs.c_str());
    }
    isVariblesExist = false;
    return;
}

std::wstring FromListReadSetting(HWND hListBox) {
    int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0); // 获取 ListBox 中项目的数量
    std::wstring reads = L"";

    // 遍历 ListBox 中的每个项目
    for (int i = 0; i < itemCount; ++i) {
        wchar_t buffer[256];
        SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)buffer);
        reads += buffer ;
        reads += L",";
    }
    if (!reads.empty()) {
        reads.erase(reads.size() - 1); // 删除最后一个字符（逗号）
    }
    return reads;
}

void FromSettingGetCommand(std::vector<std::string> lines) {
    std::vector<int> c_index;
    std::vector<std::vector<int>> c_indexs;
    std::vector<double> num_part;
    inputs_all.clear();
    for (std::string line : lines) {
        if (exitFlag) { return; }
        std::cout << "PROCESSING" << std::endl;
        std::vector<std::string> commands;
        int control_num = 0;

        // 将字符串放入字符串流中
        std::stringstream ss(line);

        // 使用std::getline分割字符串
        std::string command;
        while (std::getline(ss, command, '#')) {
            // 将分割后的子串存储到vector中
            commands.push_back(command);
        }
        if (commands[0] == "LC") {
            control_num = 1;
            inputs_1.kind = 0;
        }
        else if (commands[0] == "RC") {
            control_num = 1;
            inputs_1.kind = 1;
        }
        else if (commands[0] == "LP") {
            control_num = 1;
            inputs_1.kind = 2;
        }
        else if (commands[0] == "RP") {
            control_num = 1;
            inputs_1.kind = 3;
        }
        else if (commands[0] == "SPARE") {
            control_num = 1;
            inputs_1.kind = 4;
        }
        else {
            control_num = 2;
        }

        switch (control_num) {
        case 1: {
            std::string alphaPart;
            separateAlphaNumeric(commands[1], alphaPart, num_part);
            if (alphaPart == "AVER") {
                inputs_1.style = 0;
                inputs_1.style_arg = num_part[0];
            }
            else if (alphaPart == "NORMAL") {
                inputs_1.style = 1;
                inputs_1.style_arg = num_part[0];
            }
            else if (alphaPart == "PRE") {
                inputs_1.style = 2;
            }
            separateAlphaNumeric(commands[2], alphaPart, num_part);
            if (alphaPart == "SINGLE") {
                inputs_1.num = 0;
                inputs_1.times = num_part[0];
            }
            else if (alphaPart == "MANY") {
                inputs_1.num = 1;
                inputs_1.num_arg = num_part[0];
                inputs_1.times = num_part[1];
            }

            if (exitFlag) { return; }
        }
        break;

        case 2: {
            std::string alphaPart;
            separateAlphaNumeric(commands[0], alphaPart, num_part);
            inputs_2.speed = num_part[0];
            if (alphaPart == "ADDMOVE") {
                inputs_2.L.x = num_part[1];
                inputs_2.L.y = num_part[2];
                inputs_2.kind = 0;
            }
            else if (alphaPart == "LOCMOVE") {
                inputs_2.kind = 1;                
                inputs_2.L.x = num_part[1];
                inputs_2.L.y = num_part[2];
            }
            else if (alphaPart == "RANDMOVE") {
                inputs_2.kind = 2;
                inputs_2.time = num_part[1];
            }
            else if (alphaPart == "RANDMOVEC") {
                inputs_2.kind = 3;
                inputs_2.time = num_part[1];
            }
            separateAlphaNumeric(commands[1], alphaPart, num_part);
            if (alphaPart == "NOERR") {
                inputs_2.m_style = 0;
            }
            else if (alphaPart == "RANDW") {
                inputs_2.m_style = 1;
                inputs_2.move_arg = num_part[0];
                
            }
            separateAlphaNumeric(commands[2], alphaPart, num_part);
            if (alphaPart == "SPPEDNOERR") {
                inputs_2.s_style = 0;
            }
            else if (alphaPart == "SPPEDNORMAL") {
                inputs_2.s_style = 1;
                inputs_2.speed_arg = num_part[0];
            }
            separateAlphaNumeric(commands[3], alphaPart, num_part);
            if (alphaPart == "NP") {
                inputs_2.p_style = 0;
            }
            else if (alphaPart == "LP") {
                inputs_2.p_style = 1;
            }
            else if (alphaPart == "RP") {
                inputs_2.p_style = 2;
            }
            

            if (exitFlag) { return; }
        }
        break;

        }

        if (control_num == 1) {
            inputs_all.push_back(inputs_1);
        }
        else {
            inputs_all.push_back(inputs_2);
        }
        
    }
    return;
}

void separateAlphaNumeric(const std::string& str, std::string& alphaPart, std::vector<double>& num_vector) {
    alphaPart.clear();
    num_vector.clear();
    std::string num_string;

    size_t i = 0;

    // 处理字母部分
    while (i < str.length() && std::isalpha(str[i]) && str[i] != '-') {
        alphaPart.push_back(str[i]);
        ++i;
    }

    // 处理右侧数字部分
    while (i < str.length()) {
        if ((str[i] == ';' || str[i] == '&' )&& num_string!= "") {
            num_vector.push_back(std::stoi(num_string));
            num_string = "";
            ++i;
        }
        else {
            num_string += str[i];
            ++i;
        }
    }
    if (num_string != "") {
        num_vector.push_back(std::stoi(num_string));
    }
}

void Actions() {

    for (const auto& inputs_u : inputs_all) {
        if (inputs_u.type == InputUnion::Type::Input1) {
            INPUT_1 inputs = inputs_u.input1;
            MouseControl_1(inputs.kind, inputs.style, inputs.num, inputs.times, inputs.style_arg, inputs.num_arg);
        }
        else if (inputs_u.type == InputUnion::Type::Input2) {
            INPUT_2 inputs = inputs_u.input2;
            MouseControl_2(inputs.kind, inputs.m_style, inputs.s_style, inputs.p_style, inputs.L, inputs.speed, inputs.time, inputs.move_arg, inputs.speed_arg);
        }
    }
    return;
}

void ActionStart(HWND hwnd) {
    HWND hListBox = GetDlgItem(hwnd, ID_LISTBOX+1);
    std::wstring wreads;
    wreads = FromListReadSetting(hListBox);
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string reads = converter.to_bytes(wreads);
    std::stringstream ss(reads);
    std::vector<std::string> lines;
    std::string line;

    isSpecialPointShouldReget = true;
    while (std::getline(ss, line, ',')) {
        lines.push_back(line);
    }
    FromSettingGetCommand(lines);
    Actions();//！下面的步骤要一定时间，导致出现时间空隙，待解决！

    hListBox = GetDlgItem(hwnd, ID_LISTBOX);
    wreads = FromListReadSetting(hListBox);
    reads = converter.to_bytes(wreads);
    ss.clear();
    ss.str(reads);
    lines.clear();
    line = "";

    isSpecialPointShouldReget = true;
    while (std::getline(ss, line, ',')) {
        lines.push_back(line);
    }
    FromSettingGetCommand(lines);
    while (!exitFlag) {
        std::cout << "A NEW LOOP COME" << std::endl;
        Actions();
    }
    return;
}

void ActionStop() {
    exitFlag = true;
    return;
}

LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0 && wParam == WM_KEYDOWN)
    {
        KBDLLHOOKSTRUCT* pKbd = (KBDLLHOOKSTRUCT*)lParam;
        int keyCode = pKbd->vkCode;

        switch (keyCode) {
        case 'K':
        case 'k': {
            std::cout << "试图停止线程" << std::endl;
            if (isThreadRunning) {
                ActionStop();
                if (action_Thread != NULL) {
                    WaitForSingleObject(action_Thread, INFINITE);
                    CloseHandle(action_Thread);
                    std::cout << "停止线程" << std::endl;
                    action_Thread = NULL;
                }
                isThreadRunning = false;
                exitFlag = false;
            }
            else {
                std::cout << "线程未在运行" << std::endl;
            }
            return 1;
        }
        case 'O':
        case 'o': {
            std::cout << "试图开启线程" << std::endl;
            if (!isThreadRunning) {
                exitFlag = false;
                std::cout << "启动线程" << std::endl;
                isThreadRunning = true;
                action_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ActionStart, hwnd_, 0, NULL);
                if (action_Thread == NULL) {
                    std::cerr << "无法创建线程" << std::endl;
                }
            }
            else {
                std::cout << "线程已经在运行" << std::endl;
            }
            return 1;
        }
        }
    }

    return CallNextHookEx(g_hHook, nCode, wParam, lParam);
}

void InstallHook()
{
    g_hHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, NULL, 0);
}

void UninstallHook()
{
    if (g_hHook != NULL)
    {
        UnhookWindowsHookEx(g_hHook);
        g_hHook = NULL;
    }
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    bool isFirstComboEmpty = false;
    switch (uMsg) {

    case WM_CREATE: {
        HWND hListBox = CreateWindowEx(0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            10, 76, 330, 134, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)L"#删#最好有<空闲>时间，避免程序循环关不掉");
        HWND hListBox_i = CreateWindowEx(0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            10, 10, 330, 66, hwnd, (HMENU)ID_LISTBOX_INIT, NULL, NULL);
        SendMessage(hListBox_i, LB_ADDSTRING, 0, (LPARAM)L"#删#这里只执行一次（初始）");

        CreateWindowW(L"BUTTON", L"删除", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 210, 100, 30, hwnd, (HMENU)ID_BUTTON_DELETE_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"添加", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 210, 100, 30, hwnd, (HMENU)ID_BUTTON_ADD_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"上移", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 250, 100, 30, hwnd, (HMENU)ID_BUTTON_MOVE_UP, NULL, NULL);
        CreateWindowW(L"BUTTON", L"下移", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 250, 100, 30, hwnd, (HMENU)ID_BUTTON_MOVE_DOWN, NULL, NULL);
        CreateWindowW(L"BUTTON", L"开始（o）", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 290, 100, 30, hwnd, (HMENU)ID_BUTTON_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"停止（k）", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 290, 100, 30, hwnd, (HMENU)ID_BUTTON_STOP_ACTION, NULL, NULL);

        const wchar_t* choices1[][6] = {
            { L"左键点击", L"右键点击", L"左键按住", L"右键按住", L"空闲", L"<空>" }, // ID_CHOICE_1_1
            { L"平均", L"正太", L"精准", L"<空/精准>" }, // ID_CHOICE_1_2
            { L"单次/倍", L"多次/倍", L"<空/单>" } // ID_CHOICE_1_3
        };
        for (int i = 0; i < 3; ++i) {
            HWND hComboBox = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                350 + i * 100, 30, 100, 200, hwnd, (HMENU)(ID_CHOICE_1_1 + i), NULL, NULL);

            for (const auto& choice : choices1[i]) {
                if (choice == NULL) { continue; }
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)choice);
            }
        }

        const wchar_t* choices2[][5] = {
            { L"增量移动", L"坐标移动", L"随机移动", L"随机移动（向心）", L"<空>" }, // ID_CHOICE_2_1
            { L"移动无误差", L"向心随机游走", L"<空/无误差>" }, // ID_CHOICE_2_2
            { L"速度无误差", L"速度正太", L"<空/无误差>" }, // ID_CHOICE_2_3
            { L"无按住", L"左键拖动", L"右键拖动", L"<空/无按住>" } // ID_CHOICE_2_4
        };
        int widths2[] = { 150, 150, 100, 100 }; // 定义不同下拉列表框的宽度
        int loc = 350;
        for (int i = 0; i < 4; ++i) {
            HWND hComboBox;
            if (i != 0) {
                hComboBox = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                    loc, 100, widths2[i], 200, hwnd, (HMENU)(ID_CHOICE_2_1 + i), NULL, NULL);
                loc += widths2[i];
            }
            else {
                hComboBox = CreateWindowW(L"COMBOBOX", NULL, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_VISIBLE,
                    loc + i * widths2[i], 100, widths2[i], 200, hwnd, (HMENU)(ID_CHOICE_2_1 + i), NULL, NULL);
                loc += widths2[i];
            }

            for (const auto& choice : choices2[i]) {
                if (choice == NULL) { continue; }
                SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM)choice);
            }
        }

        CreateWindowW(L"EDIT", L"50", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 650, 30, 100, 20, hwnd, (HMENU)ID_TEXT_TIME, NULL, NULL);

        CreateWindowW(L"STATIC", L"ms", WS_CHILD | WS_VISIBLE, 750, 30, 50, 20, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"STATIC", L"鼠标操作", WS_CHILD | WS_VISIBLE, 350, 5, 100, 20, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"STATIC", L"鼠标移动操作", WS_CHILD | WS_VISIBLE, 350, 75, 100, 20, hwnd, NULL, NULL, NULL);
        break;
    }
                  
    case WM_COMMAND: {
        
        int wmId = LOWORD(wParam);
        int wmEvent = HIWORD(wParam);
        switch (wmId) {

        case ID_CHOICE_1_1:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_1_1), CB_GETCURSEL, 0, 0);
                if (selectedIndex == 5) {
                    SendMessage(GetDlgItem(hwnd, ID_CHOICE_1_2), CB_SETCURSEL, 3, 0);
                    SendMessage(GetDlgItem(hwnd, ID_CHOICE_1_3), CB_SETCURSEL, 2, 0);
                    isFirstComboEmpty = true;
                    isClickChooseZhengtai = false;
                    isClickChoosePlentyTime = false;
                    DestroyWindow(hwnd_zt);
                    hwnd_zt = NULL;
                    DestroyWindow(hwnd_num);
                    hwnd_num = NULL;
                    DestroyWindow(hwnd_zt_text);
                    hwnd_zt = NULL;
                    DestroyWindow(hwnd_num_text);
                    hwnd_num_text = NULL;
                }
                else {
                    isFirstComboEmpty = false;
                }
            }
            break;

        case ID_CHOICE_1_2:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_1_2), CB_GETCURSEL, 0, 0);
                if ((selectedIndex != 2) & (selectedIndex != 3)) {
                    isClickChooseZhengtai = true;
                    if (isClickChooseZhengtai && (hwnd_zt == NULL)) {
                        hwnd_zt = CreateWindowW(L"EDIT", L"1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 450, 60, 50, 20, hwnd, (HMENU)ID_TEXT_NORMALDISTRIBUTION, NULL, NULL);
                        hwnd_zt_text = CreateWindowW(L"STATIC", L"标准差", WS_CHILD | WS_VISIBLE, 500, 60, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                }
                else {
                    isClickChooseZhengtai = false;
                    DestroyWindow(hwnd_zt);
                    hwnd_zt = NULL;
                    DestroyWindow(hwnd_zt_text);
                    hwnd_zt_text = NULL;
                }
            }
            break;

        case ID_CHOICE_1_3:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_1_3), CB_GETCURSEL, 0, 0);
                if (selectedIndex == 1) {
                    isClickChoosePlentyTime = true;
                    if (isClickChoosePlentyTime && (hwnd_num == NULL)) {
                        hwnd_num = CreateWindowW(L"EDIT", L"1", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 550, 60, 50, 20, hwnd, (HMENU)ID_TEXT_NUM, NULL, NULL);
                        hwnd_num_text = CreateWindowW(L"STATIC", L"次数", WS_CHILD | WS_VISIBLE, 600, 60, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                }
                else {
                    isClickChoosePlentyTime = false;
                    DestroyWindow(hwnd_num);
                    hwnd_num = NULL;
                    DestroyWindow(hwnd_num_text);
                    hwnd_num_text = NULL;
                }
            }
            break;

        case ID_CHOICE_2_1:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_1), CB_GETCURSEL, 0, 0);
                switch (selectedIndex) {
                case 0:
                case 1:
                    if (hwnd_time != NULL) {
                        DestroyWindow(hwnd_time);
                        hwnd_time = NULL;
                        DestroyWindow(hwnd_time_text);
                        hwnd_time_text = NULL;
                    }
                    if (hwnd_location == NULL) {
                        int screenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
                        int screenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
                        wchar_t title[24];
                        wsprintf(title, L"%d;%d", screenWidth, screenHeight);
                        hwnd_location = CreateWindowW(L"EDIT", title, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 70, 20, hwnd, (HMENU)ID_TEXT_LOCATION, NULL, NULL);
                        hwnd_loc_text = CreateWindowW(L"STATIC", L"坐标", WS_CHILD | WS_VISIBLE, 350, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 430, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 430, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        DestroyWindow(hwnd_speed_text);
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 430, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 430, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    break;
                case 3:
                    if (hwnd_location != NULL) {
                        DestroyWindow(hwnd_location);
                        hwnd_location = NULL;
                        DestroyWindow(hwnd_loc_text);
                        hwnd_loc_text = NULL;
                    }
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"8", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 350, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        DestroyWindow(hwnd_speed_text);
                        hwnd_speed = CreateWindowW(L"EDIT", L"8", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 350, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    if (hwnd_time == NULL) {
                        hwnd_time = CreateWindowW(L"EDIT", L"600", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 420, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                        hwnd_time_text = CreateWindowW(L"STATIC", L"时间(ms)", WS_CHILD | WS_VISIBLE, 420, 150, 80, 20, hwnd, NULL, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_time);
                        DestroyWindow(hwnd_time_text);
                        hwnd_time = CreateWindowW(L"EDIT", L"600", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 420, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                        hwnd_time_text = CreateWindowW(L"STATIC", L"时间(ms)", WS_CHILD | WS_VISIBLE, 420, 150, 80, 20, hwnd, NULL, NULL, NULL);
                    }
                    break;
                case 2:
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 350, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        DestroyWindow(hwnd_speed_text);
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                        hwnd_speed_text = CreateWindowW(L"STATIC", L"速度", WS_CHILD | WS_VISIBLE, 350, 150, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    if (hwnd_location != NULL) {
                        DestroyWindow(hwnd_location);
                        hwnd_location = NULL;
                        DestroyWindow(hwnd_loc_text);
                        hwnd_loc_text = NULL;
                    }
                    if (hwnd_time == NULL) {
                        hwnd_time = CreateWindowW(L"EDIT", L"500", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 420, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                        hwnd_time_text = CreateWindowW(L"STATIC", L"时间(ms)", WS_CHILD | WS_VISIBLE, 420, 150, 80, 20, hwnd, NULL, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_time);
                        DestroyWindow(hwnd_time_text);
                        hwnd_time = CreateWindowW(L"EDIT", L"500", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 420, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                        hwnd_time_text = CreateWindowW(L"STATIC", L"时间(ms)", WS_CHILD | WS_VISIBLE, 420, 150, 80, 20, hwnd, NULL, NULL, NULL);
                    }
                    break;
                case 4:
                    SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_2), CB_SETCURSEL, 2, 0);
                    SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_3), CB_SETCURSEL, 2, 0);
                    SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_4), CB_SETCURSEL, 3, 0);
                    DestroyWindow(hwnd_location);
                    hwnd_location = NULL;
                    DestroyWindow(hwnd_speed);
                    hwnd_speed = NULL;
                    DestroyWindow(hwnd_concentrate);
                    hwnd_concentrate = NULL;
                    DestroyWindow(hwnd_normal_2);
                    hwnd_normal_2 = NULL;
                    DestroyWindow(hwnd_time);
                    hwnd_time = NULL;
                    DestroyWindow(hwnd_time_text);
                    hwnd_time_text = NULL;
                    DestroyWindow(hwnd_loc_text);
                    hwnd_loc_text = NULL;
                    DestroyWindow(hwnd_speed_text);
                    hwnd_speed_text = NULL;
                    DestroyWindow(hwnd_concentrate_text);
                    hwnd_concentrate_text = NULL;
                    DestroyWindow(hwnd_normal_2_text);
                    hwnd_normal_2_text = NULL;
                    break;
                default:
                    break;
                }
            }
            break;

        case ID_CHOICE_2_2:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_2), CB_GETCURSEL, 0, 0);
                switch (selectedIndex) {
                case 1:
                    if (hwnd_concentrate == NULL) {
                        hwnd_concentrate = CreateWindowW(L"EDIT", L"4", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 500, 130, 50, 20, hwnd, (HMENU)ID_TEXT_CONCENTRATE, NULL, NULL);
                        hwnd_concentrate_text = CreateWindowW(L"STATIC", L"向心程度(越小越向心)", WS_CHILD | WS_VISIBLE, 500, 150, 150, 20, hwnd, NULL, NULL, NULL);
                    }
                    break;
                case 0:
                case 2:
                    DestroyWindow(hwnd_concentrate);
                    hwnd_concentrate = NULL;
                    DestroyWindow(hwnd_concentrate_text);
                    hwnd_concentrate_text = NULL;
                default:
                    break;
                }
            }
            break;

        case ID_CHOICE_2_3:
            if (wmEvent == CBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_CHOICE_2_3), CB_GETCURSEL, 0, 0);
                switch (selectedIndex) {
                case 1:
                    if (hwnd_normal_2 == NULL) {
                        hwnd_normal_2 = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 650, 130, 50, 20, hwnd, (HMENU)ID_TEXT_NORMALDISTRIBUTION_2, NULL, NULL);
                        hwnd_normal_2_text = CreateWindowW(L"STATIC", L"标准差", WS_CHILD | WS_VISIBLE, 700, 130, 50, 20, hwnd, NULL, NULL, NULL);
                    }
                    break;
                case 0:
                case 2:
                    DestroyWindow(hwnd_normal_2);
                    hwnd_normal_2 = NULL;
                    DestroyWindow(hwnd_normal_2_text);
                    hwnd_normal_2_text = NULL;
                default:
                    break;
                }
            }
            break;
            
        case ID_LISTBOX:
            SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_SETCURSEL, -1, 0);
            if (wmEvent == LBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    std::cout << "Focus on a action #normal" << std::endl;
                    SetFocus(hwnd);
                }
            }
            break;

        case ID_LISTBOX_INIT:
            SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_SETCURSEL, -1, 0);
            if (wmEvent == LBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    std::cout << "Focus on a action #init" << std::endl;
                    SetFocus(hwnd);
                }
            }
            break;
            
        case ID_BUTTON_DELETE_ACTION: {
            int isInit = 0;
            int deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (deleteIndex == -1) {
                deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_GETCURSEL, 0, 0);
                isInit = 1;
            }
            if (deleteIndex != LB_ERR) {
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX+ isInit), LB_DELETESTRING, deleteIndex, 0);
                SetFocus(hwnd);
            }
        }
            break;

        case ID_BUTTON_MOVE_UP: {
            int isInit = 0;
            int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (selectedIndex == -1) {
                selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_GETCURSEL, 0, 0);
                isInit = 1;
            }
            if (selectedIndex != LB_ERR && selectedIndex > 0) {
                TCHAR buffer[64];
                // 获取选定项的文本
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // 删除选定项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_DELETESTRING, selectedIndex, 0);
                // 将文本插入到上一个位置
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_INSERTSTRING, selectedIndex - 1, (LPARAM)buffer);
                // 选中上移后的项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_SETCURSEL, selectedIndex - 1, 0);
            }
            else if (selectedIndex == 0 && isInit == 0) {
                TCHAR buffer[64];
                // 获取选定项的文本
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // 删除选定项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_DELETESTRING, selectedIndex, 0);
                // 将文本插入到上一个位置
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + 1), LB_ADDSTRING, 0, (LPARAM)buffer);
                // 选中上移后的项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + 1), LB_SETCURSEL, SendMessage(GetDlgItem(hwnd, ID_LISTBOX + 1), LB_GETCOUNT, 0, 0) - 1, 0);
            }
            SetFocus(hwnd);
        }
            break;

        case ID_BUTTON_MOVE_DOWN: {
            int isInit = 0;
            int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (selectedIndex == -1) {
                selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_GETCURSEL, 0, 0);
                isInit = 1;
            }
            if (selectedIndex != LB_ERR && selectedIndex < SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETCOUNT, 0, 0)-1) {
                TCHAR buffer[64];
                // 获取选定项的文本
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // 删除选定项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_DELETESTRING, selectedIndex, 0);
                // 将文本插入到下一个位置
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_INSERTSTRING, selectedIndex + 1, (LPARAM)buffer);
                // 选中上移后的项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_SETCURSEL, selectedIndex + 1, 0);
            }
            else if (isInit == 1 && selectedIndex == SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETCOUNT, 0, 0) - 1) {
                TCHAR buffer[64];
                // 获取选定项的文本
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // 删除选定项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_DELETESTRING, selectedIndex, 0);
                // 将文本插入到下一个位置
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_INSERTSTRING, 0, (LPARAM)buffer);
                // 选中上移后的项
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_SETCURSEL, 0, 0);
            }
            SetFocus(hwnd);
        }
            break;

        case ID_BUTTON_ADD_ACTION: {
            PushSettingIntoList(hwnd);
            SetFocus(hwnd);
        }
            break;

        case ID_BUTTON_ACTION: {
            if (!isThreadRunning) {
                exitFlag = false;
                std::cout << "线程,启动" << std::endl;
                isThreadRunning = true;
                action_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ActionStart, hwnd, 0, NULL);
                if (action_Thread == NULL) {
                    std::cerr << "无法创建线程" << std::endl;
                }
            }
            else {
                std::cout << "线程,启动。" << std::endl;
            }
        }
            SetFocus(hwnd);
            break;

        case ID_BUTTON_STOP_ACTION: {
            if (isThreadRunning) {
                ActionStop();
                if (action_Thread != NULL) {
                    WaitForSingleObject(action_Thread, INFINITE);
                    CloseHandle(action_Thread);
                    std::cout << "停止线程" << std::endl;
                    action_Thread = NULL;
                }
                isThreadRunning = false;
                exitFlag = false;
            }
            else {
                std::cout << "线程,启动?" << std::endl;
            }
        }
            SetFocus(hwnd);
            break;

        }
        break;
    }
    
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);

        RECT rect;
        GetClientRect(hwnd, &rect);
        int width = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        FillRect(memDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // 将内存中绘制的图像复制到窗口上
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // 释放资源
        SelectObject(memDC, hOldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        break;
    }
                 
    case WM_KEYDOWN: {
        int keyCode = wParam;

        switch (keyCode) {

        case VK_DELETE: {
            int isInit = 0;
            int deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (deleteIndex == -1) {
                deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX_INIT), LB_GETCURSEL, 0, 0);
                isInit = 1;
            }
            if (deleteIndex != LB_ERR) {
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX + isInit), LB_DELETESTRING, deleteIndex, 0);
                SetFocus(hwnd);
            }
        }
        break;

        case VK_ESCAPE:
            PostQuitMessage(0);
            break;

        }
        break;
    }
    /*
    case WM_LBUTTONDOWN:
    {
        POINT clickedPoint;
        clickedPoint.x = LOWORD(lParam);
        clickedPoint.y = HIWORD(lParam);

        BOOL outsideItems = TRUE;
        HWND hwndListBox = GetDlgItem(hwnd, ID_LISTBOX);
        RECT listBoxRect;
        GetClientRect(hwndListBox, &listBoxRect);
        if (PtInRect(&listBoxRect, clickedPoint))
        {
            outsideItems = FALSE;
        }

        if (outsideItems)
        {
            SetFocus(hwnd);
        }

        break;
    }*/

    default:
        return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"Win_mcv0.0";
    RegisterClass(&wc);

    int windowWidth = 800;
    int windowHeight = 400;
    int posX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
    int posY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
    HWND hwnd = CreateWindowEx(0, L"Win_mcv0.0", L"操作控制v0.0-test", WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_MAXIMIZEBOX,
        posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);
    hwnd_ = hwnd;

    if (hwnd == NULL) {
        std::cout << "FAIL to create the hwnd" << std::endl;
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);

    InstallHook();
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    UninstallHook();

    return msg.wParam;
}

int main() {
    //HWND consoleWindow = GetConsoleWindow();
    //ShowWindow(consoleWindow, SW_HIDE);

    LPSTR lpCmdLine = GetCommandLineA();

    WinMain(GetModuleHandle(NULL), NULL, lpCmdLine, SW_SHOWNORMAL);

    return 0;
}