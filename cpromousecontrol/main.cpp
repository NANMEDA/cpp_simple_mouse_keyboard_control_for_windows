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


bool isClickChooseZhengtai = false;
bool isClickChoosePlentyTime  = false;
bool isThreadRunning = false;
bool isVariblesExist = false;
extern bool isSpecialPointShouldReget;
std::atomic<bool> exitFlag(false);
HWND hwnd_zt, hwnd_num, hwnd_location, hwnd_speed, hwnd_zt_text, hwnd_num_text, hwnd_concentrate, hwnd_normal_2, hwnd_time;
HANDLE action_Thread = NULL;

void separateAlphaNumeric(const std::string& str, std::string& alphaPart, std::string& leftNumber, std::string& rightNumber);


#define ID_LISTBOX 1001

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

struct INPUT_2
{
    int c_index;
    int kind;
    int m_style;
    int s_style;
    int p_style;
    POINT L;
    double speed;
    double time = 500;
    double move_arg = 3;
    double speed_arg = 5;
};

INPUT_2 inputs_2;
std::vector<INPUT_2> inputses_2;


// �洢Ĭ��ֵ
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
                //Ĭ��ֵ
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
            case 1: return (varibles[2]+"MANY" + varibles[0]);
            case 2: return ("SINGLE" + varibles[0]);
            }
        case ID_CHOICE_2_1:
            switch (selectedIndex) {
            case 0: return (varibles[3] + "ADDMOVE" + varibles[4]);
            case 1: return (varibles[3] + "LOCMOVE" + varibles[4]);
            case 2: return (varibles[7] + "RANDMOVE"+varibles[4]);
            case 3: return (varibles[7] + "RANDMOVEC"+varibles[4]);
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

std::wstring FromListReadSetting(HWND hwnd) {
    HWND hListBox = GetDlgItem(hwnd, ID_LISTBOX); // ��ȡ ListBox �ؼ��ľ��
    int itemCount = SendMessage(hListBox, LB_GETCOUNT, 0, 0); // ��ȡ ListBox ����Ŀ������
    std::wstring reads = L"";

    // ���� ListBox �е�ÿ����Ŀ
    for (int i = 0; i < itemCount; ++i) {
        wchar_t buffer[256];
        SendMessage(hListBox, LB_GETTEXT, i, (LPARAM)buffer);
        reads += buffer ;
        reads += L",";
    }
    if (!reads.empty()) {
        reads.erase(reads.size() - 1); // ɾ�����һ���ַ������ţ�
    }
    return reads;
}

std::vector<std::vector<int>> FromSettingGetCommand(std::vector<std::string> lines) {
    std::vector<int> c_index;
    std::vector<std::vector<int>> c_indexs;
    inputses_2.clear();
    for (std::string line : lines) {
        if (exitFlag) { return c_indexs; }
        std::cout << "PROCESSING" << std::endl;
        std::vector<std::string> commands;
        int control_num = 0;
        int kind = 0;
        int style;
        int num;
        int times;
        int style_arg = 1;
        int num_arg = 1;

        // ���ַ��������ַ�������
        std::stringstream ss(line);

        // ʹ��std::getline�ָ��ַ���
        std::string command;
        while (std::getline(ss, command, '#')) {
            // ���ָ����Ӵ��洢��vector��
            commands.push_back(command);
        }
        if (commands[0] == "LC") {
            control_num = 1;
            kind = 0;
        }
        else if (commands[0] == "RC") {
            control_num = 1;
            kind = 1;
        }
        else if (commands[0] == "LP") {
            control_num = 1;
            kind = 2;
        }
        else if (commands[0] == "RP") {
            control_num = 1;
            kind = 3;
        }
        else if (commands[0] == "SPARE") {
            control_num = 1;
            kind = 4;
        }
        else {
            control_num = 2;
            inputs_2.c_index = 2;
        }
        //THERE IS NOT OK

        switch (control_num) {
        case 1: {
            std::string alphaPart, leftNumericPart, rightNumericPart;
            separateAlphaNumeric(commands[1], alphaPart, leftNumericPart, rightNumericPart);
            if (alphaPart == "AVER") {
                style = 0;
                style_arg = std::stoi(rightNumericPart);
            }
            else if (alphaPart == "NORMAL") {
                style = 1;
                style_arg = std::stoi(rightNumericPart);
            }
            else if (alphaPart == "PRE") {
                style = 2;
            }
            separateAlphaNumeric(commands[2], alphaPart, leftNumericPart, rightNumericPart);
            if (alphaPart == "SINGLE") {
                num = 0;
                times = std::stoi(rightNumericPart);
            }
            else if (alphaPart == "MANY") {
                num = 1;
                num_arg = std::stoi(leftNumericPart);
                times = std::stoi(rightNumericPart);
            }

            if (exitFlag) { return c_indexs; }
            c_index = {1, kind,style,num,times,style_arg ,num_arg };
        }
        break;

        case 2: {
            std::string alphaPart, leftNumericPart, rightNumericPart;
            separateAlphaNumeric(commands[0], alphaPart, leftNumericPart, rightNumericPart);
            inputs_2.speed = std::stoi(rightNumericPart);
            if (alphaPart == "ADDMOVE") {
                std::stringstream ss(leftNumericPart);
                std::vector<std::string> tokens;
                std::string token;
                while (std::getline(ss, token, ';')) {
                    tokens.push_back(token);
                }
                inputs_2.L.x = std::stoi(tokens[0]);
                inputs_2.L.y = std::stoi(tokens[1]);
                inputs_2.kind = 0;
            }
            else if (alphaPart == "LOCMOVE") {
                inputs_2.kind = 1;
                std::stringstream ss(leftNumericPart);
                std::vector<std::string> tokens;
                std::string token;
                while (std::getline(ss, token, ';')) {
                    tokens.push_back(token);
                }
                inputs_2.L.x = std::stoi(tokens[0]);
                inputs_2.L.y = std::stoi(tokens[1]);
            }
            else if (alphaPart == "RANDMOVE") {
                inputs_2.kind = 2;
                inputs_2.time = std::stoi(leftNumericPart);
            }
            else if (alphaPart == "RANDMOVEC") {
                inputs_2.kind = 3;
                inputs_2.time = std::stoi(leftNumericPart);
            }
            separateAlphaNumeric(commands[1], alphaPart, leftNumericPart, rightNumericPart);
            if (alphaPart == "NOERR") {
                inputs_2.m_style = 0;
            }
            else if (alphaPart == "RANDW") {
                inputs_2.m_style = 1;
                inputs_2.move_arg = std::stoi(rightNumericPart);
                
            }
            separateAlphaNumeric(commands[2], alphaPart, leftNumericPart, rightNumericPart);
            if (alphaPart == "SPPEDNOERR") {
                inputs_2.s_style = 0;
            }
            else if (alphaPart == "SPPEDNORMAL") {
                inputs_2.s_style = 1;
                inputs_2.speed_arg = std::stoi(rightNumericPart);
            }
            separateAlphaNumeric(commands[3], alphaPart, leftNumericPart, rightNumericPart);
            if (alphaPart == "NP") {
                inputs_2.p_style = 0;
            }
            else if (alphaPart == "LP") {
                inputs_2.p_style = 1;
            }
            else if (alphaPart == "RP") {
                inputs_2.p_style = 2;
            }
            

            if (exitFlag) { return c_indexs; }
        }
        break;

        }

        if (control_num == 1) {
            c_index = { 1, kind,style,num,times,style_arg ,num_arg };
        }
        else {
            c_index = { 2,0,0,0,0,0,0 };
            inputses_2.push_back(inputs_2);
        }
        c_indexs.push_back(c_index);
        
    }
    return c_indexs;
}

void separateAlphaNumeric(const std::string& str, std::string& alphaPart, std::string& leftNumber, std::string& rightNumber) {
    alphaPart.clear();
    leftNumber.clear();
    rightNumber.clear();

    size_t i = 0;

    // ����������ֲ���
    while ((i < str.length() && std::isdigit(str[i]))||(str[i]==';') || (str[i] == '-')) {
        leftNumber.push_back(str[i]);
        ++i;
    }

    // ������ĸ����
    while (i < str.length() && std::isalpha(str[i]) && str[i] != '-') {
        alphaPart.push_back(str[i]);
        ++i;
    }

    // �����Ҳ����ֲ���
    while ((i < str.length() && std::isdigit(str[i])) || (str[i] == '-')) {
        rightNumber.push_back(str[i]);
        ++i;
    }
}

void Actions(std::vector<std::vector<int>> commands) {
    int sec = 0;
    for (std::vector<int> command : commands) {
        if (command[0] == 1) {
            MouseControl_1(command[1], command[2], command[3], command[4], command[5], command[6]);
        }
        else {
            INPUT_2 inputs = inputses_2[sec];
            MouseControl_2(inputs.kind, inputs.m_style, inputs.s_style, inputs.p_style, inputs.L, inputs.speed, inputs.time, inputs.move_arg, inputs.speed_arg);
            sec++;
        }
    }   
    return;
}

void ActionStart(HWND hwnd) {
    std::wstring wreads;
    wreads = FromListReadSetting(hwnd);
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter;
    std::string reads = converter.to_bytes(wreads);
    std::stringstream ss(reads);
    std::vector<std::string> lines;
    std::string line;
    Sleep(3000);

    isSpecialPointShouldReget = true;
    while (std::getline(ss, line, ',')) {
        lines.push_back(line);
    }
    std::vector<std::vector<int>> commands = FromSettingGetCommand(lines);
    while (!exitFlag) {
        std::cout << "A NEw LOOP COME" << std::endl;
        Actions(commands);
    }
    return;
}

void ActionStop() {
    exitFlag = true;
    return;
}

// ���ڹ��̺���
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    bool isFirstComboEmpty = false;
    switch (uMsg) {

    case WM_CREATE: {
        HWND hListBox = CreateWindowEx(0, L"LISTBOX", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | LBS_NOTIFY,
            10, 10, 330, 200, hwnd, (HMENU)ID_LISTBOX, NULL, NULL);
        SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)L"#ɾ#�����<����>ʱ�䣬�������ѭ���ز���");

        CreateWindowW(L"BUTTON", L"ɾ��", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 210, 100, 30, hwnd, (HMENU)ID_BUTTON_DELETE_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"���", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 210, 100, 30, hwnd, (HMENU)ID_BUTTON_ADD_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"����", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 250, 100, 30, hwnd, (HMENU)ID_BUTTON_MOVE_UP, NULL, NULL);
        CreateWindowW(L"BUTTON", L"����", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 250, 100, 30, hwnd, (HMENU)ID_BUTTON_MOVE_DOWN, NULL, NULL);
        CreateWindowW(L"BUTTON", L"��ʼ��o��", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 10, 290, 100, 30, hwnd, (HMENU)ID_BUTTON_ACTION, NULL, NULL);
        CreateWindowW(L"BUTTON", L"ֹͣ��k��", WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON, 110, 290, 100, 30, hwnd, (HMENU)ID_BUTTON_STOP_ACTION, NULL, NULL);

        const wchar_t* choices1[][6] = {
            { L"������", L"�Ҽ����", L"�����ס", L"�Ҽ���ס", L"����", L"<��>" }, // ID_CHOICE_1_1
            { L"ƽ��", L"��̫", L"��׼", L"<��/��׼>" }, // ID_CHOICE_1_2
            { L"����/��", L"���/��", L"<��/��>" } // ID_CHOICE_1_3
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
            { L"�����ƶ�", L"�����ƶ�", L"����ƶ�", L"����ƶ������ģ�", L"<��>" }, // ID_CHOICE_2_1
            { L"�ƶ������", L"�����������", L"<��/�����>" }, // ID_CHOICE_2_2
            { L"�ٶ������", L"�ٶ���̫", L"<��/�����>" }, // ID_CHOICE_2_3
            { L"�ް�ס", L"����϶�", L"�Ҽ��϶�", L"<��/�ް�ס>" } // ID_CHOICE_2_4
        };
        int widths2[] = { 150, 150, 100, 100 }; // ���岻ͬ�����б��Ŀ��
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
        CreateWindowW(L"STATIC", L"������", WS_CHILD | WS_VISIBLE, 350, 5, 100, 20, hwnd, NULL, NULL, NULL);
        CreateWindowW(L"STATIC", L"����ƶ�����", WS_CHILD | WS_VISIBLE, 350, 75, 100, 20, hwnd, NULL, NULL, NULL);
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
                        hwnd_zt_text = CreateWindowW(L"STATIC", L"��׼��", WS_CHILD | WS_VISIBLE, 500, 60, 50, 20, hwnd, NULL, NULL, NULL);
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
                        hwnd_num_text = CreateWindowW(L"STATIC", L"����", WS_CHILD | WS_VISIBLE, 600, 60, 50, 20, hwnd, NULL, NULL, NULL);
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
                    }
                    if (hwnd_location == NULL) {
                        int screenWidth = GetSystemMetrics(SM_CXSCREEN) / 2;
                        int screenHeight = GetSystemMetrics(SM_CYSCREEN) / 2;
                        wchar_t title[24];
                        wsprintf(title, L"%d;%d", screenWidth, screenHeight);
                        hwnd_location = CreateWindowW(L"EDIT", title, WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 70, 20, hwnd, (HMENU)ID_TEXT_LOCATION, NULL, NULL);
                    }
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 430, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 430, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    break;
                case 3:
                    if (hwnd_location != NULL) {
                        DestroyWindow(hwnd_location);
                        hwnd_location = NULL;
                    }
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"8", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        hwnd_speed = CreateWindowW(L"EDIT", L"8", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    if (hwnd_time == NULL) {
                        hwnd_time = CreateWindowW(L"EDIT", L"600", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 400, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_time);
                        hwnd_time = CreateWindowW(L"EDIT", L"600", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 400, 130, 50, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                    }
                    break;
                case 2:
                    //isShowSpeed = true;
                    if (hwnd_speed == NULL) {
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_speed);
                        hwnd_speed = CreateWindowW(L"EDIT", L"10", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 350, 130, 50, 20, hwnd, (HMENU)ID_TEXT_STEP, NULL, NULL);
                    }
                    if (hwnd_location != NULL) {
                        DestroyWindow(hwnd_location);
                        hwnd_location = NULL;
                    }
                    if (hwnd_time == NULL) {
                        hwnd_time = CreateWindowW(L"EDIT", L"500", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 400, 130, 100, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
                    }
                    else {
                        DestroyWindow(hwnd_time);
                        hwnd_time = CreateWindowW(L"EDIT", L"500", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL, 400, 130, 100, 20, hwnd, (HMENU)ID_TEXT_TIME_2, NULL, NULL);
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
                    }
                    break;
                case 0:
                case 2:
                    DestroyWindow(hwnd_concentrate);
                    hwnd_concentrate = NULL;
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
                    }
                    break;
                case 0:
                case 2:
                    DestroyWindow(hwnd_normal_2);
                    hwnd_normal_2 = NULL;
                default:
                    break;
                }
            }
            break;
            
        case ID_LISTBOX:
            if (wmEvent == LBN_SELCHANGE) {
                int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
                if (selectedIndex != LB_ERR) {
                    std::cout << "Focus on a action" << std::endl;
                    SetFocus(hwnd);
                }
            }
            break;
            
        case ID_BUTTON_DELETE_ACTION: {
            int deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (deleteIndex != LB_ERR) {
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_DELETESTRING, deleteIndex, 0);
                SetFocus(hwnd);
            }
        }
            break;

        case ID_BUTTON_MOVE_UP: {
            int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (selectedIndex != LB_ERR && selectedIndex > 0) {
                TCHAR buffer[256];
                // ��ȡѡ������ı�
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // ɾ��ѡ����
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_DELETESTRING, selectedIndex, 0);
                // ���ı����뵽��һ��λ��
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_INSERTSTRING, selectedIndex - 1, (LPARAM)buffer);
                // ѡ�����ƺ����
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_SETCURSEL, selectedIndex - 1, 0);
            }
            SetFocus(hwnd);
        }
            break;

        case ID_BUTTON_MOVE_DOWN: {
            int selectedIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (selectedIndex != LB_ERR && selectedIndex < SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCOUNT, 0, 0)-1) {
                TCHAR buffer[256];
                // ��ȡѡ������ı�
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETTEXT, selectedIndex, (LPARAM)buffer);
                // ɾ��ѡ����
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_DELETESTRING, selectedIndex, 0);
                // ���ı����뵽��һ��λ��
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_INSERTSTRING, selectedIndex + 1, (LPARAM)buffer);
                // ѡ�����ƺ����
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_SETCURSEL, selectedIndex + 1, 0);
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
                std::cout << "�߳�,����" << std::endl;
                isThreadRunning = true;
                action_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ActionStart, hwnd, 0, NULL);
                if (action_Thread == NULL) {
                    std::cerr << "�޷������߳�" << std::endl;
                }
            }
            else {
                std::cout << "�߳��Ѿ�������" << std::endl;
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
                    std::cout << "ֹͣ�߳�" << std::endl;
                    action_Thread = NULL;
                }
                isThreadRunning = false;
                exitFlag = false;
            }
            else {
                std::cout << "�߳�,����?" << std::endl;
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

        // �����ڴ��豸������
        HDC memDC = CreateCompatibleDC(hdc);
        HBITMAP memBitmap = CreateCompatibleBitmap(hdc, width, height);
        HBITMAP hOldBitmap = (HBITMAP)SelectObject(memDC, memBitmap);

        FillRect(memDC, &rect, (HBRUSH)(COLOR_WINDOW + 1));

        // ���ڴ��л��Ƶ�ͼ���Ƶ�������
        BitBlt(hdc, 0, 0, width, height, memDC, 0, 0, SRCCOPY);

        // �ͷ���Դ
        SelectObject(memDC, hOldBitmap);
        DeleteObject(memBitmap);
        DeleteDC(memDC);

        EndPaint(hwnd, &ps);
        break;
    }
                 
    case WM_KEYDOWN: {
        int keyCode = wParam;

        switch (keyCode) {

        case 'K':
        case 'k':{
            if (isThreadRunning) {
                ActionStop();
                if (action_Thread != NULL) {
                    WaitForSingleObject(action_Thread, INFINITE);
                    CloseHandle(action_Thread);
                    std::cout << "ֹͣ�߳�" << std::endl;
                    action_Thread = NULL;
                }
                isThreadRunning = false;
                exitFlag = false;
            }
            else {
                std::cout << "�߳�,����?" << std::endl;
                }
            }
            SetFocus(hwnd);
            break;

        case 'O':
        case 'o':{
            if (!isThreadRunning) {
                exitFlag = false;
                std::cout << "�߳�,����" << std::endl;
                isThreadRunning = true;
                action_Thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ActionStart, hwnd, 0, NULL);
                if (action_Thread == NULL) {
                    std::cerr << "�޷������߳�" << std::endl;
                }
            }
            else {
                std::cout << "�߳��Ѿ�������" << std::endl;
            }
            }
            SetFocus(hwnd);
            break;

        case VK_DELETE: {
            int deleteIndex = SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_GETCURSEL, 0, 0);
            if (deleteIndex != LB_ERR) {
                SendMessage(GetDlgItem(hwnd, ID_LISTBOX), LB_DELETESTRING, deleteIndex, 0);
            }
            break;
        }

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
    // ע�ᴰ����
    WNDCLASS wc = { 0 };
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = L"mcv0.0";
    RegisterClass(&wc);

    // ��������
    int windowWidth = 800;
    int windowHeight = 400;
    int posX = (GetSystemMetrics(SM_CXSCREEN) - windowWidth) / 2;
    int posY = (GetSystemMetrics(SM_CYSCREEN) - windowHeight) / 2;
    HWND hwnd = CreateWindowEx(0, L"mcv0.0", L"������v0.0-test", WS_OVERLAPPEDWINDOW | WS_THICKFRAME | WS_MAXIMIZEBOX,
        posX, posY, windowWidth, windowHeight, NULL, NULL, hInstance, NULL);

    if (hwnd == NULL) {
        std::cout << "FAIL to create the hwnd" << std::endl;
        return 0;
    }
    ShowWindow(hwnd, nCmdShow);


    // ����Ϣѭ��
    MSG msg = { 0 };
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return msg.wParam;
}

int main() {
    //HWND consoleWindow = GetConsoleWindow();
    //ShowWindow(consoleWindow, SW_HIDE);

    LPSTR lpCmdLine = GetCommandLineA();
   
    return WinMain(GetModuleHandle(NULL), NULL, lpCmdLine, SW_SHOWNORMAL);
}