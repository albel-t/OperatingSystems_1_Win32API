#include <windows.h>
#include <shlobj.h>
#include <iostream>
#include <string>
#include <vector>



class TaskRunException : public std::exception {
private:
    std::string message;

public:
    // Конструктор с сообщением об ошибке
    explicit TaskRunException(const std::string& msg, const DWORD& exc){
        message = msg + " \n\t An exception occurred: " + std::to_string(exc);
    }

    // Переопределяем метод what() из std::exception
    const char* what() const noexcept override {
        return message.c_str();
    }

    // Дополнительные методы для получения деталей
    const std::string& getMessage() const {
        return message;
    }
};

static std::string GetSystemDirectoryPath(int csidl) {
    char path[MAX_PATH] = { 0 };

    if (SUCCEEDED(SHGetFolderPathA(nullptr, csidl, nullptr, 0, path)))
        return std::string(path);
    else
        throw TaskRunException("При получении пути произошел сбой", GetLastError());
}

void PrintColorInfo(COLORREF color, const std::string& name) {
    int r = GetRValue(color), g = GetGValue(color), b = GetBValue(color);
    std::cout << "   RGB: " << r << ", " << g << ", " << b << "\n";
    std::cout << "   Цвет: \033[48;2;" << r << ";" << g << ";" << b << "m     \033[0m\n";
}
void GetSystemColors() {
    COLORREF color3DFace = GetSysColor(COLOR_3DFACE);
    std::cout << "1. COLOR_3DFACE:\n";
    PrintColorInfo(color3DFace, "3D Face");
    COLORREF captionText = GetSysColor(COLOR_CAPTIONTEXT);
    std::cout << "\n2. COLOR_CAPTIONTEXT:\n";
    PrintColorInfo(captionText, "Caption Text");
    COLORREF background = GetSysColor(COLOR_BACKGROUND);
    std::cout << "\n3. COLOR_BACKGROUND:\n";
    PrintColorInfo(background, "Desktop Background");
}
bool SetSystemColor(int element, COLORREF newColor) {
    if (!SetSysColors(1, &element, &newColor)) {
        std::cout << "Ошибка при изменении цвета. Запустите от имени администратора.\n";
        return false;
    }
    return true;
}
void DemoColorChanges(COLORREF new3DFace = RGB(240, 240, 240), COLORREF newCaptionText = RGB(0, 0, 0), COLORREF newBackground = RGB(0, 0, 0)) {
    if (SetSystemColor(COLOR_3DFACE, new3DFace)) {
        std::cout << "+ COLOR_3DFACE изменен\n";
    }
    if (SetSystemColor(COLOR_CAPTIONTEXT, newCaptionText)) {
        std::cout << "+ COLOR_CAPTIONTEXT изменен\n";
    }
    if (SetSystemColor(COLOR_BACKGROUND, newBackground)) {
        std::cout << "+ COLOR_BACKGROUND изменен\n";
    }
    SendMessage(HWND_BROADCAST, WM_SYSCOLORCHANGE, 0, 0);
}
#ifndef UNLEN
#define UNLEN 256
#endif // !UNLEN

void username_computername()
{
    std::cout << "\nИмя компьютера, имя пользователя" << std::endl;
    char  computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD computerNameSize = sizeof(computerName);

    if (!GetComputerNameA(computerName, &computerNameSize)) {
        throw TaskRunException("Имя компьютера не удалось извлечь", GetLastError());
    }
    char  userName[UNLEN + 1];
    DWORD userNameSize = sizeof(userName);

    if (!GetUserNameA(userName, &userNameSize)) {
        throw TaskRunException("Имя ползователя не удалось извлечь", GetLastError());
    }

    std::cout << "Имя компьютера: " << computerName << std::endl;
    std::cout << "Имя пользователя: " << userName << std::endl;
}
void system_directories()
{
    std::cout << "\nПути к системным каталогам Windows" << std::endl;
    std::cout << "Рабочий стол: " << GetSystemDirectoryPath(CSIDL_DESKTOP) << std::endl;
    std::cout << "Документы: " <<  GetSystemDirectoryPath(CSIDL_MYDOCUMENTS) << std::endl;
    std::cout << "Загрузки: " <<  GetSystemDirectoryPath(CSIDL_MYDOCUMENTS) + "\\Downloads" << std::endl;
    std::cout << "Program Files: " <<  GetSystemDirectoryPath(CSIDL_PROGRAM_FILES) << std::endl;
    std::cout << "Program Files (x86): " <<  GetSystemDirectoryPath(CSIDL_PROGRAM_FILESX86) << std::endl;
    std::cout << "AppData\\Roaming: " <<  GetSystemDirectoryPath(CSIDL_APPDATA) << std::endl;
    std::cout << "AppData\\Local: " <<  GetSystemDirectoryPath(CSIDL_LOCAL_APPDATA) << std::endl;
    std::cout << "Temp (AppData): " <<  GetSystemDirectoryPath(CSIDL_LOCAL_APPDATA) + "\\Temp" << std::endl;
}

void operating_system_version() {
    std::cout << "\nВерсия операционной системы" << std::endl;
    HKEY hKey;
    char version[256];
    char build[256];
    DWORD size = sizeof(version);

    if (RegOpenKeyExA(HKEY_LOCAL_MACHINE, "SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", 0, KEY_READ, &hKey) == ERROR_SUCCESS) 
        if (RegQueryValueExA(hKey, "CurrentVersion", NULL, NULL, (LPBYTE)version, &size) == ERROR_SUCCESS) 
            std::cout << "Версия: " << version << std::endl;
        else
            throw TaskRunException("Не удалось получить версию ОС", GetLastError());
}

void system_metrics() {
    std::cout << "\nСистемные метрики" << std::endl;

    // 1. Разрешение экрана
    int screenWidth = GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = GetSystemMetrics(SM_CYSCREEN);
    std::cout << "1. Разрешение экрана: " << screenWidth << "x" << screenHeight << "\n";

    // 2. Рабочая область (без панели задач)
    int workWidth = GetSystemMetrics(SM_CXFULLSCREEN);
    int workHeight = GetSystemMetrics(SM_CYFULLSCREEN);
    std::cout << "2. Рабочая область: " << workWidth << "x" << workHeight << "\n";

    // 3. Количество мониторов
    int monitorCount = GetSystemMetrics(SM_CMONITORS);
    std::cout << "3. Количество мониторов: " << monitorCount << "\n";
}
void system_parameters(){
    std::cout << "\nСистемные параметры " << std::endl;
    int mouseSpeed;
    SystemParametersInfo(SPI_GETMOUSESPEED, 0, &mouseSpeed, 0);
    std::cout << "1. Скорость мыши: " << mouseSpeed << "\n";
    BOOL swapButtons;
    SystemParametersInfo(SPI_GETSNAPTODEFBUTTON, 0, &swapButtons, 0);
    std::cout << "2. Кнопки переключены: " << (swapButtons ? "Да" : "Нет") << "\n";
    int screenSaverTimeout;
    SystemParametersInfo(SPI_GETSCREENSAVETIMEOUT, 0, &screenSaverTimeout, 0);
    std::cout << "3. Таймаут заставки: " << screenSaverTimeout << " сек.\n";
}
void system_colors()
{
    std::cout << "\nСистемные цвета " << std::endl;

    GetSystemColors();
    DemoColorChanges(RGB(100, 10, 250), RGB(0, 200, 200), RGB(100, 250, 140));
    GetSystemColors();
    //DemoColorChanges();

}
int main() {
    setlocale(LC_ALL, "Russian");
    try {
        username_computername();
        system_directories();
        operating_system_version();
        system_metrics();
        system_parameters();
        system_colors();
    }
    catch (const TaskRunException& e) {
        std::cerr << "Ошибка во время решения: " << e.what() << std::endl;
    }

    return 0;
}