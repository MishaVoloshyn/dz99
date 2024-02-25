// hw-13-02-cross.cpp : Определяет точку входа для приложения.
//

#include <windows.h>
#include <tchar.h>
#include <vector>
#include <string>
#include <Tlhelp32.h>
#include <locale.h>
#include <conio.h>
#include "Resource.h"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

using namespace std;

HWND hEdit1, hList;



void InitEdit(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    hList = GetDlgItem(hDlg, IDC_LIST2);
    hEdit1 = GetDlgItem(hDlg, IDC_EDIT1);

}


void ProcessList()
{

    TCHAR buff[100];
    HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    PROCESSENTRY32 pe32;

    memset(&pe32, 0, sizeof(PROCESSENTRY32));
    pe32.dwSize = sizeof(PROCESSENTRY32);

    SendMessage(hList, LB_RESETCONTENT, 0, 0);

    if (Process32First(hSnapShot, &pe32)) 
    {
        wsprintf(buff, TEXT("%s"), pe32.szExeFile);
        SendMessage(hList, LB_ADDSTRING, 0, LPARAM(buff));

        while (Process32Next(hSnapShot, &pe32)) 
        {
            wsprintf(buff, TEXT("%s"), pe32.szExeFile);
            SendMessage(hList, LB_ADDSTRING, 0, LPARAM(buff));

        }
    }

    
}


void ShowInfo(HWND hDlg) 
{
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if (index != LB_ERR) 
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe32;

        memset(&pe32, 0, sizeof(PROCESSENTRY32));
        pe32.dwSize = sizeof(PROCESSENTRY32);

        TCHAR processName[MAX_PATH]{ 0 };
        SendMessage(hList, LB_GETTEXT, index, (LPARAM)processName);

        if (Process32First(hSnapShot, &pe32)) 
        {
            do 
            {
                if (_tcscmp(pe32.szExeFile, processName) == 0) 
                {
                    TCHAR buff[MAX_PATH];
                    wsprintf(buff, TEXT("ID: %4u \nThreads: %5u \npriority: %2u \nname: %s"), pe32.th32ProcessID, pe32.cntThreads, pe32.pcPriClassBase, pe32.szExeFile);
                    MessageBox(hDlg, buff, TEXT("Информация"), MB_OK);
                    break;
                }
            } while (Process32Next(hSnapShot, &pe32));
        }
    }
    else
            MessageBox(hDlg, TEXT("Программа не выбрана!"), TEXT("Информация"), MB_OK | MB_ICONSTOP);
}


void DeleteProcess(HWND hDlg)
{
    int index = SendMessage(hList, LB_GETCURSEL, 0, 0);

    if (index != LB_ERR)
    {
        HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

        PROCESSENTRY32 pe32;

        memset(&pe32, 0, sizeof(PROCESSENTRY32));
        pe32.dwSize = sizeof(PROCESSENTRY32);

        TCHAR processName[100]{ 0 };
        SendMessage(hList, LB_GETTEXT, index, (LPARAM)processName);

        if (Process32First(hSnapShot, &pe32))
        {
            do
            {
                if (_tcscmp(pe32.szExeFile, processName) == 0)
                {
                    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe32.th32ProcessID);
                    TerminateProcess(hProcess, 0);
                    CloseHandle(hProcess);
                    TCHAR buff[100];
                    wsprintf(buff, TEXT("Процесс %s завершен"), pe32.szExeFile);
                    MessageBox(hDlg, buff, TEXT("Информация"), MB_OK);
                    break;
                }
            } while (Process32Next(hSnapShot, &pe32));
        }
    }
    else
        MessageBox(hDlg, TEXT("Программа не выбрана!"), TEXT("Информация"), MB_OK | MB_ICONSTOP);
}


void AddProcess(HWND hDlg)
{
    TCHAR processName[256];
    GetDlgItemText(hDlg, IDC_EDIT1, processName, 256);

    STARTUPINFO s = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION p;
    BOOL fs = CreateProcess(NULL, processName, NULL, NULL, FALSE, 0, NULL, NULL, &s, &p);
    if (fs)
    {
        CloseHandle(p.hThread);
        CloseHandle(p.hProcess);

    }
}

LRESULT CALLBACK DlgProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static PROCESSENTRY32 pe32;

    switch (uMsg)
    {
    case WM_INITDIALOG:
        InitEdit(hDlg, uMsg, wParam, lParam);
        ProcessList();
        _getch();
        break;

    case WM_COMMAND:
    {
        if (LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, 0);
        }

        //INFO BUTTON
        else if (LOWORD(wParam) == IDC_BUTTON3)
        {
            ShowInfo(hDlg);
        }

        //STOP PROCESS
        else if (LOWORD(wParam) == IDC_BUTTON2)
        {
            DeleteProcess(hDlg);
        }

        //UPDATE PROCESS
        else if (LOWORD(wParam) == IDC_BUTTON1)
        {
            ProcessList();
            _getch();
        }

        //ADD PROCESS
        else if (LOWORD(wParam) == IDC_BUTTON4)
        {
            AddProcess(hDlg);
        }

    }
    break;

    case WM_DESTROY:
        PostQuitMessage(0);
        break;

    }
    return 0;
}

int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR lpszCmdLine, int nCmdShow)
{
    return DialogBox(hInst, MAKEINTRESOURCE(IDD_DIALOG1), NULL, DlgProc);
}
