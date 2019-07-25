#include "forcl.h"

#include <windows.h>
#include <tchar.h>
#include <shlwapi.h>
#define mbError(msg) MessageBox(NULL, TEXT(msg), TEXT("Error"), MB_ICONSTOP|MB_OK)

#define EXE_CLASS_NAME "Notepad"   //�^�[�Q�b�g�̃��C���E�B���h�E�̃N���X��
#define DLL_FILE_NAME  "exDLL.dll" //�^�[�Q�b�g�ɓǂݍ��܂���DLL�̃t�@�C����

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
	//�N���X����EXE_CLASS_NAME�̃E�B���h�E��T��
	//��������ꍇZ�I�[�_�[����ԍ������̂��Ԃ�
	if (hwt==NULL) { //�����ɍ����E�B���h�E������Ȃ��A�������͊֐������s�����ꍇ
		mbError("FindWindow()");
		return -1;
	}

	GetWindowThreadProcessId(hwt, &dwpid); //�E�B���h�E�����v���Z�X�̃v���Z�XID���擾
	if (dwpid==0) {
		mbError("GetThreadProcessId()");
		return -1;
	}

	hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwpid);
	//�v���Z�X�n���h�����J���āA�A�N�Z�X�\��Ԃɂ���
	if (hProcess==NULL) {
		mbError("OpenProcess()");
		return -1;
	}

	GetModuleFileName(NULL, path, sizeof(path)); //���v���O�����̐�΃p�X���擾
	_tcscpy(_tcsrchr(path, _TEXT('\\'))+1, TEXT(DLL_FILE_NAME));
	//��������DLL�̐�΃p�X�����
	//����������DLL�͂��̃v���O�����Ɠ����t�H���_�ɓ���Ă����K�v������
	if (PathFileExists(path)==FALSE) { //DLL�̃p�X���Ԉ���Ă�����ADLL���Ȃ��ꍇ
		mbError("PathFileExists()");
		return 0;
	}
	len = (lstrlen(path)+1) * sizeof(TCHAR);
	//DLL�̃p�X�����������ނ̂ɕK�v�ȃ������T�C�Y���v�Z

	rpMemory = (PWSTR)VirtualAllocEx(hProcess, NULL, len, MEM_COMMIT, PAGE_READWRITE);
	//�^�[�Q�b�g�v���Z�X�̉��z�A�h���X��ԓ��Ƀ������̈���m��
	if (rpMemory==NULL) {
		mbError("VirtualAllocEx()");
		return -1;
	}

	if (WriteProcessMemory(hProcess, rpMemory, (PVOID)path, len, NULL)==FALSE) {
		//��قǊm�ۂ����������̈��DLL�̐�΃p�X����������
		mbError("WriteProcessMemory()");
		return -1;
	}

	ptsr = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(TEXT("Kernel32")), "LoadLibraryW");
	//Kernel32.dll����LoadLibraryW()�̊J�n�A�h���X���擾
	if (ptsr==NULL) {
		mbError("GetProcAddress()");
		return -1;
	}

	hThread = CreateRemoteThread(hProcess, NULL, 0, ptsr, rpMemory, 0, NULL);
	//�^�[�Q�b�g�v���Z�X�̃A�h���X��Ԃœ����X���b�h�����A
	//��قǂ̃������̈�ɂ���DLL�̐�΃p�X���A
	//LoadLibraryW()�œǂݍ��܂���
	if (hThread==NULL) {
		mbError("CreateRemoteThread()");
		return -1;
	}

	return GetLastError();
}