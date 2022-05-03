/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

#include "stdafx.h"

/*
 * Intel SDE (Software Development Emulator) for Windows 7.49.0 ��œ������ƁA
 * LoadLibrary/FreeLibrary ���J��Ԃ��Ƃ��ꂾ���Ń��������[�N����Ƃ�����肪����B
 * ���̃e�X�g�ł̓R�[�f�b�N�� DLL �����[�h/�A�����[�h���J��Ԃ����߁A���̖��̉e����傫���󂯂�B
 * 32bit ���ƃ��[�U�[�̃A�h���X��Ԃ� 2GB �����Ȃ��̂ŁA���̂����A�h���X��Ԃ𖄂ߐs�����ăN���b�V������B
 *
 * ���炩���߃��[�h���Ă����Ă����Ζ���A�����[�h����Ȃ��̂ł��̖��̉e�����󂯂Ȃ��Ȃ�B
 * �c�͂��ł��邪�A�����ԃ}�V�ɂȂ������̂̂���ł��܂��������g�p�ʂ������Ă������ۂ��m�F����Ă���B
 *
 * ���Ȃ݂ɁA���[�h���邽�тɃo�C�i���ϊ�������炵���A
 * �A�����[�h����Ȃ��悤�ɂ��Ă����Ǝ��s�������Ȃ�B
 *
 * �A�����[�h�������Ƀo�C�i���ϊ��������ʂ��̂Ă���͂���
 * �����Ǝ̂Ă�ꂸ�Ƀ��[�N���Ă���c�̂ł͂Ȃ����Ǝv����B
 *
 * Windows �p�ȊO�� SDE �łǂ��Ȃ邩�͒��ׂĂ��Ȃ��B
 */

class CKeepLibrary
{
private:
	std::vector<HMODULE> vecLibs;

public:
	CKeepLibrary(std::vector<const char *> vecLibNames)
	{
		vecLibs.reserve(vecLibNames.size());
		for (auto name : vecLibNames)
		{
			HMODULE hModule = LoadLibrary(name);
			if (hModule != NULL)
			{
				vecLibs.push_back(hModule);
			}
			else
			{
				DWORD err = GetLastError();
				char* p;
				FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL, err, MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US), (LPSTR)&p, 0, NULL);
				fprintf(stderr, "CKeepLibrary: LoadLibrary(%s) failed: GetLastError()=%u %s", name, err, p);
				LocalFree(p);
			}
		}
	}

	~CKeepLibrary()
	{
		for (auto hModule : vecLibs)
		{
			FreeLibrary(hModule);
		}
	}
} kl({
	"C:\\windows\\system32\\utv_vcm.dll", // VCM �R�[�f�b�N�͏�� system32 �̉��ɂ�����̂��g����
	"C:\\windows\\system32\\utv_dmo.dll", // DMO �R�[�f�b�N�� system32 �ɂ�����̂� tests.exe �̉��ɂ�����̂Ƃ��ǂ��炪�g���邩�͏ꍇ�ɂ��̂ŁA�������[�h���Ă���
	"utv_dmo.dll",
});
