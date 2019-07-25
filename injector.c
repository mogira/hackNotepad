#include "forcl.h"

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#define mbError(msg) MessageBox(NULL, TEXT(msg), TEXT("Error"), MB_ICONSTOP|MB_OK)

#define EXE_CLASS_NAME "Notepad"   //ターゲットのメインウィンドウのクラス名
#define DLL_FILE_NAME  "exDLL.dll" //ターゲットに読み込ませるDLLのファイル名

int WINAPI _tWinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR pCmdLine, int nCmdShow) {
	HWND hwt;
	DWORD dwpid;
	HANDLE hProcess;
	TCHAR path[256];
	int len;
	PWSTR rpMemory;
	PTHREAD_START_ROUTINE ptsr;
	HANDLE hThread;

	hwt = FindWindow(TEXT(EXE_CLASS_NAME), NULL);
	//クラス名がEXE_CLASS_NAMEのウィンドウを探す
	//複数ある場合Zオーダーが一番高いものが返る
	if (hwt==NULL) { //条件に合うウィンドウが一つもない、もしくは関数が失敗した場合
		mbError("FindWindow()");
		return -1;
	}

	GetWindowThreadProcessId(hwt, &dwpid); //ウィンドウを持つプロセスのプロセスIDを取得
	if (dwpid==0) {
		mbError("GetThreadProcessId()");
		return -1;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwpid);
	//プロセスハンドルを開いて、アクセス可能状態にする
	if (hProcess==NULL) {
		mbError("OpenProcess()");
		return -1;
	}

	GetModuleFileName(NULL, path, sizeof(path)); //自プログラムの絶対パスを取得
	_tcscpy(_tcsrchr(path, _TEXT('\\'))+1, TEXT(DLL_FILE_NAME));
	//そこからDLLの絶対パスを作る
	//したがってDLLはこのプログラムと同じフォルダに入れておく必要がある
	if (PathFileExists(path)==FALSE) { //DLLのパスが間違っていたり、DLLがない場合
		mbError("PathFileExists()");
		return 0;
	}
	len = (lstrlen(path)+1) * sizeof(TCHAR);
	//DLLのパス名を書き込むのに必要なメモリサイズを計算

	rpMemory = (PWSTR)VirtualAllocEx(hProcess, NULL, len, MEM_COMMIT, PAGE_READWRITE);
	//ターゲットプロセスの仮想アドレス空間内にメモリ領域を確保
	if (rpMemory==NULL) {
		mbError("VirtualAllocEx()");
		return -1;
	}

	if (WriteProcessMemory(hProcess, rpMemory, (PVOID)path, len, NULL)==FALSE) {
		//先ほど確保したメモリ領域にDLLの絶対パスを書き込む
		mbError("WriteProcessMemory()");
		return -1;
	}

	ptsr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
	//Kernel32.dll中のLoadLibraryW()の開始アドレスを取得
	if (ptsr==NULL) {
		mbError("GetProcAddress()");
		return -1;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, ptsr, rpMemory, 0, NULL);
	//ターゲットプロセスのアドレス空間で動くスレッドを作り、
	//先ほどのメモリ領域にあるDLLの絶対パスを、
	//LoadLibraryW()で読み込ませる
	if (hThread==NULL) {
		mbError("CreateRemoteThread()");
		return -1;
	}

	return GetLastError();
}