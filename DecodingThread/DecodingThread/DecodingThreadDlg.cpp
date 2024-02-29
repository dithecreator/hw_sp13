#include "DecodingThreadDlg.h"

DecodingThreadDlg* DecodingThreadDlg::ptr = NULL;
static int created = 0;
static int symbols = 0;
static int totalsize = 0;
DecodingThreadDlg::DecodingThreadDlg(void)
{
	ptr = this;
}

DecodingThreadDlg::~DecodingThreadDlg(void)
{
	
}

void DecodingThreadDlg::Cls_OnClose(HWND hwnd)
{
	EndDialog(hwnd, 0);
}

DWORD WINAPI Decoding_Thread(LPVOID lp)
{
	DecodingThreadDlg *ptr = (DecodingThreadDlg *)lp;
	char buf[4096];
	TCHAR str[MAX_PATH];
	_tcscpy(str, GetCommandLine());
	TCHAR seps[] = TEXT(" ");
	TCHAR *token, *last;
	token = _tcstok(str, seps); 
	while(token != NULL)
	{
		token = _tcstok(NULL, seps);
		if(token)
			last = token;
		created++;
	}
	wsprintf(str, TEXT("copymusic_%s.txt"), last);
	HANDLE hEvent = OpenEvent(EVENT_ALL_ACCESS, 0, TEXT("{AB1640AF-5DCB-4E23-B573-381EF27FB024}")); //открываем ивент
	DWORD dwAnswer = WaitForSingleObject(hEvent, INFINITE);
	SetEvent(hEvent); // перевод события в сигнальное состояние
	
		ifstream in(TEXT("coding.txt"), ios::binary | ios::in);
		if(!in)
		{
			MessageBox(ptr->hDialog, TEXT("Ошибка открытия файла!"), TEXT("Семафор"), MB_OK | MB_ICONINFORMATION);
			return 1;
		}
		ofstream out(str, ios::binary | ios::out | ios::trunc);
		const int KEY = 100;
		while(!in.eof())
		{
			in.read(buf, 4096);
			int n = in.gcount();
			symbols += n;
			totalsize += n * sizeof(wchar_t);
			for(int i = 0; i < n; i++)
			{
				buf[i] ^= KEY;
			}
			out.write(buf, n);
		}
		out.close();
		in.close();
		ResetEvent(hEvent); // перевод события в несигнальное состояние
		MessageBox(ptr->hDialog, TEXT("Чтение данных из файла coding.txt завершено!"), TEXT("Семафор"), MB_OK | MB_ICONINFORMATION);
	


	ofstream sizeFile("size.txt");
	sizeFile << totalsize;
	sizeFile.close();

	ofstream symbolsFile("symbols.txt");
	symbolsFile << symbols;
	symbolsFile.close();

	ofstream createdFile("created.txt");
	createdFile << created;
	createdFile.close();
	return 0;
}

BOOL DecodingThreadDlg::Cls_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam) 
{
	hDialog = hwnd;
	CreateThread(NULL, 0, Decoding_Thread, this, 0, NULL); 
	return TRUE;
}

BOOL CALLBACK DecodingThreadDlg::DlgProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		HANDLE_MSG(hwnd, WM_CLOSE, ptr->Cls_OnClose);
		HANDLE_MSG(hwnd, WM_INITDIALOG, ptr->Cls_OnInitDialog);
	}
	return FALSE;
}