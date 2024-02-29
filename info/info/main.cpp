#include "Windows.h"
#include "tchar.h"
#include "resource.h"
#include <fstream>
#include <ctime>
using namespace std;

HWND hEdit1;
HWND hEdit2;
HWND hEdit3;
HWND hEdit4;



void UpdateEditText(HWND hEdit, const char* filename) { //функция, которая считывает из файла информацию и выводит в статики
    FILE* file;
    if (fopen_s(&file, filename, "r") != 0)
    {
        MessageBox(NULL, TEXT("Ошибка открытия файла!"), TEXT("Ошибка"), MB_OK | MB_ICONERROR);
        return;
    }

    const int bufferSize = 4096;
    char buffer[bufferSize];

    while (fscanf_s(file, "%4095[^\n]", buffer, bufferSize) == 1) //находим инфу
    {
        strcat_s(buffer, "\r\n");
        SendMessageA(hEdit, WM_SETTEXT, 0, LPARAM(buffer)); //считываем в статик
    }

    fclose(file);
}

DWORD WINAPI INFO(LPVOID lp) {
    HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, TEXT("{AB1640AF-5DCB-4E23-B573-381EF27FB024}")); //открываем ивент
    DWORD dwAnswer = WaitForSingleObject(hEvent, INFINITE); //обьект ожидания синхронизации
    SetEvent(hEvent);// перевод события в сигнальное состояние

    UpdateEditText(hEdit2, "created.txt");
    UpdateEditText(hEdit3, "symbols.txt");
    UpdateEditText(hEdit4, "size.txt");


    ResetEvent(hEvent); // перевод события в несигнальное состояние
    return 1;
}

INT_PTR CALLBACK DlgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    switch (uMsg)
    {
    case WM_INITDIALOG:
    {
        hEdit1 = GetDlgItem(hwnd, IDC_EDIT1);
        hEdit2 = GetDlgItem(hwnd, IDC_EDIT2);
        hEdit3 = GetDlgItem(hwnd, IDC_EDIT3);
        hEdit4 = GetDlgItem(hwnd, IDC_EDIT4);
        //получаем время
        SYSTEMTIME currentTime;
        GetLocalTime(&currentTime);

        int len = GetWindowTextLength(hEdit1) + 1;
        wchar_t* buffer = new wchar_t[len];
        GetWindowText(hEdit1, buffer, len);

        wchar_t dateTime[256];
        swprintf_s(dateTime, L"%s %04d-%02d-%02d %02d:%02d:%02d",
            buffer, currentTime.wYear, currentTime.wMonth, currentTime.wDay,
            currentTime.wHour, currentTime.wMinute, currentTime.wSecond);

        SendMessage(hEdit1, WM_SETTEXT, 0, LPARAM(dateTime));

        delete[] buffer;
        CreateThread(NULL, 0, INFO, nullptr, 0, NULL); //запускаем поток



    }
    break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            EndDialog(hwnd, 0);
        }

        break;

    case WM_CLOSE:
        EndDialog(hwnd, 0);
        return TRUE;

    }
    return FALSE;
}

int WINAPI wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nShowCmd)
{
    return DialogBox(hInstance, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DLGPROC(DlgProc));
}
