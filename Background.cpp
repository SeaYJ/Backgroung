#include "Background.h"

// 背景文件信息
PATHINFO pathInfo = { 0 };

// 当前程序文件信息
PATHINFO appPathInfo = { 0 };

// 屏幕信息（初始化 1920x1080p）
SCREENINFO screenInfo = { 1920, 1080 };

int main(int argc, char* argv[])
{
	// 程序初始化
	appInit(argv[0]);

	// 命令解析初始化
	cmdInit();

	// 命令解析索引，储存对应命令指向的函数地址
	// 如果没有找到相应的命令，就返回 NOT_EXIT(0)
	unsigned long long indexFunc = 0;

	// 循环查找参数
	for (int i = 1; i < argc; ++i) 
	{
		// 解析命令
		indexFunc = cmdSearch(argv[i]);

		if (indexFunc != NOT_EXIT)
		{
			// 调用命令对应的函数（通过函数地址）
			// 并且自动将命令后的一个参数传给函数
			((SET_BACKGROUND)indexFunc)(filePathCheck(argv[++i]));
		}
	}

	return 0;
}

/*
 * 作用：
 *   初始化程序。
 *   具体为->
 *     1）获取程序所在盘符、目录、基础名、扩展名、文件名、工作目录和文件绝对路径（储存在全局变量 appPathInfo 中）。
 *     2）获取设备屏幕信息（储存在全局变量 screenInfo 中）。
 * 
 */
int appInit(char* appPath)
{
	size_t drive_dir = 0,
		filePath_fileName = 0,
		baseName_extension = 0;

	// 获取分界点的字符串下标
	string temp = appPath;
	drive_dir = temp.find_first_of(':');
	filePath_fileName = temp.find_last_of('\\');
	baseName_extension = temp.find_last_of('.');

	// 未正确获取分界点的字符串下标
	if (drive_dir >= strlen(temp.c_str())
		|| filePath_fileName >= strlen(temp.c_str())
		|| baseName_extension >= strlen(temp.c_str()))
	{
		return -1;
	}

	// 获取盘符，带:
	// 例如：D:
	strcpy(appPathInfo.drive,
		temp.substr(0,
			(drive_dir - 0) + 1
		).c_str()
	);
	// 获取目录，不带盘符
	// 例如：\Demo\cpp\ 
	strcpy(appPathInfo.dir,
		temp.substr(drive_dir + 1,
			filePath_fileName - (drive_dir + 1) + 1
		).c_str()
	);
	// 获取文件基础名，不带.
	// 例如：Program
	strcpy(appPathInfo.baseName,
		temp.substr(filePath_fileName + 1,
			(baseName_extension - 1) - (filePath_fileName + 1) + 1
		).c_str()
	);
	// 获取文件扩展名，不带.
	// 例如：exe
	strcpy(appPathInfo.extension,
		temp.substr(baseName_extension + 1,
			-1
		).c_str()
	);
	// 获取文件工作目录，带盘符
	// 例如：D:\Demo\cpp\ 
	sprintf(appPathInfo.workPath, "%s%s", appPathInfo.drive, appPathInfo.dir);
	// 获取文件名
	// 例如：Program.exe
	sprintf(appPathInfo.fileName, "%s.%s", appPathInfo.baseName, appPathInfo.extension);
	// 获取文件绝对路径
	// 例如：D:\Demo\cpp\Program.exe 
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

	// 获取物理屏幕大小
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
 * 作用：
 *   命令解析初始化。
 *   具体为->
 *     1）将所有支持的命令放入全局变量 cmdBox 中储存，便于查找。
 */
int cmdInit(void)
{
	cmdBox.insert(make_pair<string, unsigned long long>("-bgi", (unsigned long long)&setImgBg));
	cmdBox.insert(make_pair<string, unsigned long long>("-bgv", (unsigned long long)&setVideoBg));
	return 0;
}

/* 
 * 作用：
 *   命令解析。
 *   具体为->
 *     1）在 cmdBox 全局变量中寻找对应的命令。找到了就返回对应的函数地址；没找到就返回 NOT_EXIT(0)。
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
 * 作用：
 *   文件路径检查（不针对文件格式）。
 *   具体为->
 *     1）检查文件是否存在。
 *     2）将路径字符串中的所有“斜杠”替换为“反斜杠”。
 *     3）去除多余的反斜杠，确保每处至多只有“一个”反斜杠。
 *     4）将文件路径分解为盘符、目录、基础名、扩展名、文件名、工作目录和文件绝对路径（储存在全局变量 pathInfo 中）。
 */
char* filePathCheck(char* filePath)
{
	// （文件/文件夹）不存在
	if ((_access(filePath, F_OK)) != 0) { return nullptr; }

	// “斜杠与反斜杠及其数量”处理与规范
	filePath = PathRemoveSlash(filePath);// 将所有的斜杠替换成反斜杠
	filePath = PathOnlyBackslash(filePath);// 去除多余的反斜杠
	//filePath = PathRemoveBackslash(filePath);// 将所有的反斜杠替换成斜杠

	size_t drive_dir = 0,
		filePath_fileName = 0,
		baseName_extension = 0;

	// 获取分界点的字符串下标
	string temp = filePath;
	drive_dir = temp.find_first_of(':');
	filePath_fileName = temp.find_last_of('\\');
	baseName_extension = temp.find_last_of('.');

	// 未正确获取分界点的字符串下标
	if ( drive_dir >= strlen(temp.c_str()) 
		|| filePath_fileName >= strlen(temp.c_str()) 
		|| baseName_extension >= strlen(temp.c_str()) ) 
	{ return nullptr; }

	// 获取盘符，带:
	// 例如：D:
	strcpy(pathInfo.drive,
		temp.substr(0,
			(drive_dir - 0) + 1
		).c_str()
	);
	// 获取目录，不带盘符
	// 例如：\Demo\cpp\ 
	strcpy(pathInfo.dir, 
		temp.substr(drive_dir + 1,
			filePath_fileName - (drive_dir + 1) + 1
		).c_str()
	);
	// 获取文件基础名，不带.
	// 例如：Program
	strcpy(pathInfo.baseName,
		temp.substr(filePath_fileName + 1, 
			(baseName_extension - 1) - (filePath_fileName + 1) + 1
		).c_str()
	);
	// 获取文件扩展名，不带.
	// 例如：exe
	strcpy(pathInfo.extension,
		temp.substr(baseName_extension + 1,
			-1
		).c_str()
	);
	// 获取文件工作目录，带盘符
	// 例如：D:\Demo\cpp\ 
	sprintf(pathInfo.workPath, "%s%s", pathInfo.drive, pathInfo.dir);
	// 获取文件名
	// 例如：Program.exe
	sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
	// 获取文件绝对路径
	// 例如：D:\Demo\cpp\Program.exe 
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
 * 作用：
 *   图片文件后缀检查。
 *   检查指定的路径对应的格式是否符合作为壁纸的要求。
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
 * 作用：
 *   视频文件后缀检查。
 *   检查指定的路径对应的格式是否符合作为壁纸的要求。
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
 * 作用：
 *   ffmpeg 程序，用于在图片、视频不是支持的格式时，将其转换为支持的格式。
 * 参数：
 *   须在调用的时候指定“原文件绝对路径(inputFile)”和“新文件绝对路径(outputFile)”
 */
int ffmpeg(const char* inputFile, const char* outputFile)
{
	// ffmpeg 程序所在位置
	TCHAR lpApplicationName[_MAX_PATH] = { 0 };
	// 命令行参数
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

	// 启动 ffmpeg
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
 * 作用：
 *   设置图片背景。
 *   具体为->
 *     1）调用“检查文件扩展名”的函数(imgExtLegality)，检查文件扩展名是否符合要求。
 *     如果符合要求，则进行下一步；如果不符合，则调用“ffmpeg”的启动函数(ffmpeg)，尝试对其进行格式转换。
 *     如果转换失败则自动结束函数；如果转换成功则更新全局变量 pathInfo 里的信息，并重新指定参数为正确的新文件路径。
 * 
 *     2）设置图片背景。
 */
void setImgBg(char* filePath)
{
	// 路径为空
	if (filePath == nullptr) { return; }

	// 图片文件扩展名检查
	if (imgExtLegality(pathInfo.extension) == NOT_SUPPORT)
	{
		cerr << "ERROR: Windows 不支持 ." << pathInfo.extension << " 格式的图片文件做背景。" << endl;
		cout << "正在为您自动转换格式，以用于 Windows 壁纸..." << endl;
		
		// 生成新文件路径，扩展名默认为 imgExt[0]
		char newFilePath[_MAX_PATH] = { 0 };
		sprintf(newFilePath, "%s%s.%s", pathInfo.workPath, pathInfo.baseName, imgExt[0]);

		// 启动 ffmpeg
		if (ffmpeg(pathInfo.filePath, newFilePath) == 0)
		{
			cerr << "ffmpeg 程序未能正常启动。" << endl;
			return;
		}
		else
		{
			// 更新文件信息
			strcpy(pathInfo.extension, imgExt[0]);
			sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
			sprintf(pathInfo.filePath, "%s%s", pathInfo.workPath, pathInfo.fileName);

			filePath = pathInfo.filePath;
		}
		
	}

	// 设置图片背景
	if (SystemParametersInfoA(SPI_SETDESKWALLPAPER, 0, (PVOID)filePath, SPIF_SENDCHANGE))
	{
		cout << "已设置 “" << filePath << "” 为 Windows 图片背景！" <<endl;
		return;
	}
	else 
	{
		cerr << "设置失败！" << endl
			<< "\timgPath: “" << filePath << "”" << endl;
		return;
	}
	
}

BOOL CALLBACK EnumWindowsProc(_In_ HWND hwnd, _In_ LPARAM Lparam)
{
	HWND hDefView = FindWindowEx(hwnd, 0, "SHELLDLL_DefView", 0);
	if (hDefView != 0) {
		// 找它的下一个窗口，类名为WorkerW，隐藏它
		HWND hWorkerw = FindWindowEx(0, hwnd, "WorkerW", 0);
		ShowWindow(hWorkerw, SW_HIDE);

		return FALSE;
	}
	return TRUE;
}

/*
 * 作用：
 *   设置视频背景。
 *   具体为->
 *     1）调用“检查文件扩展名”的函数(videoExtLegality)，检查文件扩展名是否符合要求。
 *     如果符合要求，则进行下一步；如果不符合，则调用“ffmpeg”的启动函数(ffmpeg)，尝试对其进行格式转换。
 *     如果转换失败则自动结束函数；如果转换成功则更新全局变量 pathInfo 里的信息，并重新指定参数为正确的新文件路径。
 *
 *     2）设置视频背景。
 */
void setVideoBg(char* filePath)
{
	// 路径为空
	if (filePath == nullptr) { return; }

	// 视频文件扩展名检查
	if (videoExtLegality(pathInfo.extension) == NOT_SUPPORT)
	{
		cerr << "ERROR: Windows 不支持 ." << pathInfo.extension << " 格式的视频文件做背景。" << endl;
		cout << "正在为您自动转换格式，以用于 Windows 壁纸..." << endl;

		// 生成新文件路径，扩展名默认为 videoExt[0]
		char newFilePath[_MAX_PATH] = { 0 };
		sprintf(newFilePath, "%s%s.%s", pathInfo.workPath, pathInfo.baseName, videoExt[0]);

		// 启动 ffmpeg
		if (ffmpeg(pathInfo.filePath, newFilePath) == 0)
		{
			cerr << "ERROR: ffmpeg 程序未能正常启动。" << endl;
			return;
		}
		else
		{
			// 更新文件信息
			strcpy(pathInfo.extension, videoExt[0]);
			sprintf(pathInfo.fileName, "%s.%s", pathInfo.baseName, pathInfo.extension);
			sprintf(pathInfo.filePath, "%s%s", pathInfo.workPath, pathInfo.fileName);

			filePath = pathInfo.filePath;
		}
		
	}

	thread myThread(playVideo);
	myThread.detach();

	// 防止视频还未来得及启动
	Sleep(1000);

	HWND hProgman = FindWindow("Progman", NULL);			// 找到Progman窗口
	SendMessageTimeout(hProgman, 0x52C, 0, 0, SMTO_NORMAL, 0x3e8, 0);	// 给它发特殊消息(0x52c)
	HWND hPlayer = FindWindow("SDL_app", NULL);				// 找到视频窗口
	SetParent(hPlayer, hProgman);							// 将视频窗口设置为PM的子窗口
	EnumWindows(EnumWindowsProc, 0);						// 找到第二个WorkerW窗口并隐藏它

	SetWindowText(hPlayer, PLAYER_TITLE);					// 设置窗口标题

	cout << "已设置 “" << filePath << "” 为 Windows 视频背景！" << endl;

}

void playVideo()
{
	// 启动 ffplay
	// ffplay 所在位置
	TCHAR ffplayExe[_MAX_PATH] = { 0 };
	sprintf(ffplayExe, "%sbin\\ffplay.exe", appPathInfo.workPath);

	// 命令行参数
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