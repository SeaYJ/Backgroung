#include "Background.h"

// �����ļ���Ϣ
PATHINFO pathInfo = { 0 };

// ��ǰ�����ļ���Ϣ
PATHINFO appPathInfo = { 0 };

// ��Ļ��Ϣ����ʼ�� 1920x1080p��
SCREENINFO screenInfo = { 1920, 1080 };

int main(int argc, char* argv[])
{
	// �����ʼ��
	appInit(argv[0]);

	// ���������ʼ��
	cmdInit();

	// ������������������Ӧ����ָ��ĺ�����ַ
	// ���û���ҵ���Ӧ������ͷ��� NOT_EXIT(0)
	unsigned long long indexFunc = 0;

	// ѭ�����Ҳ���
	for (int i = 1; i < argc; ++i) 
	{
		// ��������
		indexFunc = cmdSearch(argv[i]);

		if (indexFunc != NOT_EXIT)
		{
			// ���������Ӧ�ĺ�����ͨ��������ַ��
			// �����Զ���������һ��������������
			((SET_BACKGROUND)indexFunc)(filePathCheck(argv[++i]));
		}
	}

	return 0;
}

/*
 * ���ã�
 *   ��ʼ������
 *   ����Ϊ->
 *     1����ȡ���������̷���Ŀ¼������������չ�����ļ���������Ŀ¼���ļ�����·����������ȫ�ֱ��� appPathInfo �У���
 *     2����ȡ�豸��Ļ��Ϣ��������ȫ�ֱ��� screenInfo �У���
 * 
 */
int appInit(char* appPath)
{
	size_t drive_dir = 0,
		filePath_fileName = 0,
		baseName_extension = 0;

	// ��ȡ�ֽ����ַ����±�
	string temp = appPath;
	drive_dir = temp.find_first_of(':');
	filePath_fileName = temp.find_last_of('\\');
	baseName_extension = temp.find_last_of('.');

	// δ��ȷ��ȡ�ֽ����ַ����±�
	if (drive_dir >= strlen(temp.c_str())
		|| filePath_fileName >= strlen(temp.c_str())
		|| baseName_extension >= strlen(temp.c_str()))
	{
		return -1;
	}

	// ��ȡ�̷�����:
	// ���磺D:
	strcpy(appPathInfo.drive,
		temp.substr(0,
			(drive_dir - 0) + 1
		).c_str()
	);
	// ��ȡĿ¼�������̷�
	// ���磺\Demo\cpp\ 
	strcpy(appPathInfo.dir,
		temp.substr(drive_dir + 1,
			filePath_fileName - (drive_dir + 1) + 1
		).c_str()
	);
	// ��ȡ�ļ�������������.
	// ���磺Program
	strcpy(appPathInfo.baseName,
		temp.substr(filePath_fileName + 1,
			(baseName_extension - 1) - (filePath_fileName + 1) + 1
		).c_str()
	);
	// ��ȡ�ļ���չ��������.
	// ���磺exe
	strcpy(appPathInfo.extension,
		temp.substr(baseName_extension + 1,
			-1
		).c_str()
	);
	// ��ȡ�ļ�����Ŀ¼�����̷�
	// ���磺D:\Demo\cpp\ 
	sprintf(appPathInfo.workPath, "%s%s", appPathInfo.drive, appPathInfo.dir);
	// ��ȡ�ļ���
	// ���磺Program.exe
	sprintf(appPathInfo.fileName, "%s.%s", appPathInfo.baseName, appPathInfo.extension);
	// ��ȡ�ļ�����·��
	// ���磺D:\Demo\cpp\Program.exe 
	sprintf(appPathInfo.filePath, "%s%s", appPathInfo.workPath, appPathInfo.fileName);

#ifdef _DEBUG
	cout << "appPathInfo.drive= \t" << appPathInfo.drive << endl
		<< "appPathInfo.dir=\t" << appPathInfo.dir << endl
		<< "appPathInfo.baseName=\t" << appPathInfo.baseName << endl
		<< "appPathInfo.extension=\t" << appPathInfo.extension << endl
		<< "appPathInfo.workPath=\t" << appPathInfo.workPath << endl
		<< "appPathInfo.fileName=\t" << appPathInfo.fileName << endl
		<< "appPathInfo.filePath=\t" << appPathInfo.filePath << endl
		<< endl;
#endif // _DEBUG

	// ��ȡ������Ļ��С
	HDC hdc = GetDC(NULL);
	screenInfo.width = GetDeviceCaps(hdc, DESKTOPHORZRES);
	screenInfo.height = GetDeviceCaps(hdc, DESKTOPVERTRES);
	ReleaseDC(NULL, hdc);

#ifdef _DEBUG
	std::cout << "CLIENT WIDTH:" << screenInfo.width << std::endl;
	std::cout << "CLIENT HEIGHT:" << screenInfo.height << std::endl;
#endif // _DEBUG

	return 0;
}

/* 
 * ���ã�
 *   ���������ʼ����
 *   ����Ϊ->
 *     1��������֧�ֵ��������ȫ�ֱ��� cmdBox �д��棬���ڲ��ҡ�
 */
int cmdInit(void)
{
	cmdBox.insert(make_pair<string, unsigned long long>("-bgi", (unsigned long long)&setImgBg));
	cmdBox.insert(make_pair<string, unsigned long long>("-bgv", (unsigned long long)&setVideoBg));
	return 0;
}

/* 
 * ���ã�
 *   ���������
 *   ����Ϊ->
 *     1���� cmdBox ȫ�ֱ�����Ѱ�Ҷ�Ӧ������ҵ��˾ͷ��ض�Ӧ�ĺ�����ַ��û�ҵ��ͷ��� NOT_EXIT(0)��
 */
unsigned long long cmdSearch(string cmd_str)
{
	auto cmd = cmdBox.find(cmd_str);

	if (cmd != cmdBox.end())
	{
		return cmd->second;
	}

	return NOT_EXIT;
}

/* 
 * ���ã�
 *   �ļ�·����飨������ļ���ʽ����
 *   ����Ϊ->
 *     1������ļ��Ƿ���ڡ�
 *     2����·���ַ����е����С�б�ܡ��滻Ϊ����б�ܡ���
 *     3��ȥ������ķ�б�ܣ�ȷ��ÿ������ֻ�С�һ������б�ܡ�
 *     4�����ļ�·���ֽ�Ϊ�̷���Ŀ¼������������չ�����ļ���������Ŀ¼���ļ�����·����������ȫ�ֱ��� pathInfo �У���
 */
char* filePathCheck(char* filePath)
{
	// ���ļ�/�ļ��У�������
	if ((_access(filePath, F_OK)) != 0) { return nullptr; }

	// ��б���뷴б�ܼ���������������淶
	filePath = PathRemoveSlash(filePath);// �����е�б���滻�ɷ�б��
	filePath = PathOnlyBackslash(filePath);// ȥ������ķ�б��
	//filePath = PathRemoveBackslash(filePath);// �����еķ�б���滻��б��

	size_t drive_dir = 0,
		filePath_fileName = 0,
		baseName_extension = 0;

	// ��ȡ�ֽ����ַ����±�
	string temp = filePath;
	drive_dir = temp.find_first_of(':');
	filePath_fileName = temp.find_last_of('\\');
	baseName_extension = temp.find_last_of('.');

	// δ��ȷ��ȡ�ֽ����ַ����±�
	if ( drive_dir >= strlen(temp.c_str()) 
		|| filePath_fileName >= strlen(temp.c_str()) 
		|| baseName_extension >= strlen(temp.c_str()) ) 
	{ return nullptr; }

	// ��ȡ�̷�����:
	// ���磺D:
	strcpy(pathInfo.drive,
		temp.substr(0,
			(drive_dir - 0) + 1
		).c_str()
	);
	// ��ȡĿ¼�������̷�
	// ���磺\Demo\cpp\ 
	strcpy(pathInfo.dir, 
		temp.substr(drive_dir + 1,
			filePath_fileName - (drive_dir + 1) + 1
		).c_str()
	);
	// ��ȡ�ļ�������������.
	// ���磺Program
	strcpy(pathInfo.baseName,
		temp.substr(filePath_fileName + 1, 
			(baseName_extension - 1) - (filePath_fileName + 1) + 1
		).c_str()
	);
	// ��ȡ�ļ���չ��������.
	// ���磺exe
	strcpy(pathInfo.extension,
		temp.substr(baseName_extension + 1,
			-1
		).c_str()
	);
	// ��ȡ�ļ�����Ŀ¼�����̷�
	// ���磺D:\Demo\cpp\ 
	sprintf(pathInfo.workPath, "%s%s", pathInfo.drive, pathInfo.dir);
	// ��ȡ�ļ���
	// ���磺Program.exe
	sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
	// ��ȡ�ļ�����·��
	// ���磺D:\Demo\cpp\Program.exe 
	sprintf(pathInfo.filePath, "%s%s", pathInfo.workPath, pathInfo.fileName);

#ifdef _DEBUG
	cout << "pathInfo.drive= \t" << pathInfo.drive << endl
		<< "pathInfo.dir=\t\t" << pathInfo.dir << endl
		<< "pathInfo.baseName=\t" << pathInfo.baseName << endl
		<< "pathInfo.extension=\t" << pathInfo.extension << endl
		<< "pathInfo.workPath=\t" << pathInfo.workPath << endl
		<< "pathInfo.fileName=\t" << pathInfo.fileName << endl
		<< "pathInfo.filePath=\t" << pathInfo.filePath << endl
		<< endl;
#endif // _DEBUG

	return filePath;
}

char* PathRemoveSlash(char* filePath)
{
	char* index = nullptr;

	for (index = filePath; *index != '\0'; ++index)
	{
		if (*index == '/') { *index = '\\'; }
	}
	return filePath;
}

char* PathOnlyBackslash(char* filePath)
{
	char* right = nullptr, * left = nullptr;

	for (right = filePath; *right != '\0'; ++right)
	{
		if (* right == '\\' && * (++right) == '\\')
		{
			left = right + 1;
			
			while (*left == '\\') { left++; }

			strcpy(right, left);
		}
	}

	return filePath;
}

char* PathRemoveBackslash(char* filePath)
{
	char* index = nullptr;

	for (index = filePath; *index != '\0'; ++index)
	{
		if (*index == '\\') { *index = '/'; }
	}
	return filePath;
}

/* 
 * ���ã�
 *   ͼƬ�ļ���׺��顣
 *   ���ָ����·����Ӧ�ĸ�ʽ�Ƿ������Ϊ��ֽ��Ҫ��
 */
int imgExtLegality(char* imgExtName)
{
	for (unsigned int i = 0; imgExt[i] != "\0"; ++i)
	{
		if (_stricmp(imgExtName, imgExt[i]) == 0) { return i; }
	}

	return NOT_SUPPORT;
}

/*
 * ���ã�
 *   ��Ƶ�ļ���׺��顣
 *   ���ָ����·����Ӧ�ĸ�ʽ�Ƿ������Ϊ��ֽ��Ҫ��
 */
int videoExtLegality(char* videoExtName)
{
	for (unsigned int i = 0; videoExt[i] != "\0"; ++i)
	{
		if (_stricmp(videoExtName, videoExt[i]) == 0) { return i; }
	}

	return NOT_SUPPORT;
}


/*
 * ���ã�
 *   ffmpeg ����������ͼƬ����Ƶ����֧�ֵĸ�ʽʱ������ת��Ϊ֧�ֵĸ�ʽ��
 * ������
 *   ���ڵ��õ�ʱ��ָ����ԭ�ļ�����·��(inputFile)���͡����ļ�����·��(outputFile)��
 */
int ffmpeg(const char* inputFile, const char* outputFile)
{
	// ffmpeg ��������λ��
	TCHAR lpApplicationName[_MAX_PATH] = { 0 };
	// �����в���
	TCHAR lpCommandLine[_MAX_COMMAND] = { 0 };

	sprintf((char*)lpApplicationName, "%sbin\\ffmpeg.exe", appPathInfo.workPath);
	sprintf((char*)lpCommandLine, " -i \"%s\" \"%s\" -y", inputFile, outputFile);
	
#ifdef _DEBUG
	cout << "lpApplicationName=\t" << lpApplicationName << endl
		<< "lpCommandLine=\t\t" << lpCommandLine << endl;
#endif // _DEBUG


	STARTUPINFO si{ 0 };
	PROCESS_INFORMATION pi{ 0 };

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// ���� ffmpeg
	if (!CreateProcess(lpApplicationName, 
		lpCommandLine,
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		cerr << "ERROR: ffmpeg start failed." << GetLastError() << endl;
		return -1;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
	return 0;
}

/* 
 * ���ã�
 *   ����ͼƬ������
 *   ����Ϊ->
 *     1�����á�����ļ���չ�����ĺ���(imgExtLegality)������ļ���չ���Ƿ����Ҫ��
 *     �������Ҫ���������һ������������ϣ�����á�ffmpeg������������(ffmpeg)�����Զ�����и�ʽת����
 *     ���ת��ʧ�����Զ��������������ת���ɹ������ȫ�ֱ��� pathInfo �����Ϣ��������ָ������Ϊ��ȷ�����ļ�·����
 * 
 *     2������ͼƬ������
 */
void setImgBg(char* filePath)
{
	// ·��Ϊ��
	if (filePath == nullptr) { return; }

	// ͼƬ�ļ���չ�����
	if (imgExtLegality(pathInfo.extension) == NOT_SUPPORT)
	{
		cerr << "ERROR: Windows ��֧�� ." << pathInfo.extension << " ��ʽ��ͼƬ�ļ���������" << endl;
		cout << "����Ϊ���Զ�ת����ʽ�������� Windows ��ֽ..." << endl;
		
		// �������ļ�·������չ��Ĭ��Ϊ imgExt[0]
		char newFilePath[_MAX_PATH] = { 0 };
		sprintf(newFilePath, "%s%s.%s", pathInfo.workPath, pathInfo.baseName, imgExt[0]);

		// ���� ffmpeg
		if (ffmpeg(pathInfo.filePath, newFilePath) == 0)
		{
			cerr << "ffmpeg ����δ������������" << endl;
			return;
		}
		else
		{
			// �����ļ���Ϣ
			strcpy(pathInfo.extension, imgExt[0]);
			sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
			sprintf(pathInfo.filePath, "%s%s", pathInfo.workPath, pathInfo.fileName);

			filePath = pathInfo.filePath;
		}
		
	}

	// ����ͼƬ����
	if (SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)filePath, SPIF_SENDCHANGE))
	{
		cout << "������ ��" << filePath << "�� Ϊ Windows ͼƬ������" <<endl;
		return;
	}
	else 
	{
		cerr << "����ʧ�ܣ�" << endl
			<< "\timgPath: ��" << filePath << "��" << endl;
		return;
	}
	
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam)
{
	HWND hDefView = FindWindowEx(hwnd, 0, "SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		// ��������һ�����ڣ�����ΪWorkerW��������
		HWND hWorkerw = FindWindowEx(0, hwnd, "WorkerW", 0);
		ShowWindow(hWorkerw, SW_HIDE);

		return FALSE;
	}
	return TRUE;
}

/*
 * ���ã�
 *   ������Ƶ������
 *   ����Ϊ->
 *     1�����á�����ļ���չ�����ĺ���(videoExtLegality)������ļ���չ���Ƿ����Ҫ��
 *     �������Ҫ���������һ������������ϣ�����á�ffmpeg������������(ffmpeg)�����Զ�����и�ʽת����
 *     ���ת��ʧ�����Զ��������������ת���ɹ������ȫ�ֱ��� pathInfo �����Ϣ��������ָ������Ϊ��ȷ�����ļ�·����
 *
 *     2��������Ƶ������
 */
void setVideoBg(char* filePath)
{
	// ·��Ϊ��
	if (filePath == nullptr) { return; }

	// ��Ƶ�ļ���չ�����
	if (videoExtLegality(pathInfo.extension) == NOT_SUPPORT)
	{
		cerr << "ERROR: Windows ��֧�� ." << pathInfo.extension << " ��ʽ����Ƶ�ļ���������" << endl;
		cout << "����Ϊ���Զ�ת����ʽ�������� Windows ��ֽ..." << endl;

		// �������ļ�·������չ��Ĭ��Ϊ videoExt[0]
		char newFilePath[_MAX_PATH] = { 0 };
		sprintf(newFilePath, "%s%s.%s", pathInfo.workPath, pathInfo.baseName, videoExt[0]);

		// ���� ffmpeg
		if (ffmpeg(pathInfo.filePath, newFilePath) == 0)
		{
			cerr << "ERROR: ffmpeg ����δ������������" << endl;
			return;
		}
		else
		{
			// �����ļ���Ϣ
			strcpy(pathInfo.extension, videoExt[0]);
			sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
			sprintf(pathInfo.filePath, "%s%s", pathInfo.workPath, pathInfo.fileName);

			filePath = pathInfo.filePath;
		}
		
	}

	thread myThread(playVideo);
	myThread.detach();

	// ��ֹ��Ƶ��δ���ü�����
	Sleep(1000);

	HWND hProgman = FindWindow("Progman", NULL);			// �ҵ�Progman����
	SendMessageTimeout(hProgman, 0x52C, 0, 0, SMTO_NORMAL, 0x3e8, 0);	// ������������Ϣ(0x52c)
	HWND hPlayer = FindWindow("SDL_app", NULL);				// �ҵ���Ƶ����
	SetParent(hPlayer, hProgman);							// ����Ƶ��������ΪPM���Ӵ���
	EnumWindows(EnumWindowsProc, 0);						// �ҵ��ڶ���WorkerW���ڲ�������

	SetWindowText(hPlayer, PLAYER_TITLE);					// ���ô��ڱ���

	cout << "������ ��" << filePath << "�� Ϊ Windows ��Ƶ������" << endl;

}

void playVideo()
{
	// ���� ffplay
	// ffplay ����λ��
	TCHAR ffplayExe[_MAX_PATH] = { 0 };
	sprintf(ffplayExe, "%sbin\\ffplay.exe", appPathInfo.workPath);

	// �����в���
	TCHAR lpCommandLine[_MAX_COMMAND] = { 0 };
	sprintf(lpCommandLine,
		" \"%s\" -noborder -x %d -y %d -left 0 -top 0 -loop 0 -volume 30 -window_title " PLAYER_TITLE,
		pathInfo.filePath,
		screenInfo.width,
		screenInfo.height
	);

	STARTUPINFO si{ 0 };
	PROCESS_INFORMATION pi{ 0 };

	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);
	ZeroMemory(&pi, sizeof(pi));

	// Start the child process. 
	if (!CreateProcess(ffplayExe,
		lpCommandLine,
		NULL,           // Process handle not inheritable
		NULL,           // Thread handle not inheritable
		FALSE,          // Set handle inheritance to FALSE
		0,              // No creation flags
		NULL,           // Use parent's environment block
		NULL,           // Use parent's starting directory 
		&si,            // Pointer to STARTUPINFO structure
		&pi)           // Pointer to PROCESS_INFORMATION structure
		)
	{
		printf("ffpaly start failed (%d).\n", GetLastError());
		return;
	}

	// Wait until child process exits.
	WaitForSingleObject(pi.hProcess, INFINITE);

	// Close process and thread handles. 
	CloseHandle(pi.hProcess);
	CloseHandle(pi.hThread);
}