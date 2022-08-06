#include <iostream>
#include <map>
#include <io.h>
#include <Windows.h>
#include <thread>
using namespace std;

#define NOT_EXIT 0
#define NOT_SUPPORT -1
#define F_OK 0
#define _MAX_COMMAND 1024
#define PLAYER_TITLE "SeaYJ_WndDWallpaper_Player"

map<string, unsigned long long> cmdBox;
typedef void (*SET_BACKGROUND)(char*);

const char* imgExt[] = {
	"png",
	"jpg",
	"jpeg",
	"bmp",
	"\0"
};

const char* videoExt[] = {
	"mp4",
	"wmv",
	"\0"
};

struct PATHINFO
{
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char baseName[_MAX_FNAME];
	char extension[_MAX_EXT];
	char fileName[_MAX_FNAME + _MAX_EXT + 1];
	char workPath[_MAX_DRIVE + _MAX_DIR];
	char filePath[_MAX_PATH];
};

struct SCREENINFO
{
	unsigned int width;
	unsigned int height;
};

int appInit(char* appPath);
int cmdInit(void);
unsigned long long cmdSearch(string cmd_str);

void setImgBg(char* filePath);
void setVideoBg(char* filePath);

char* filePathCheck(char* filePath);
char* PathRemoveSlash(char* filePath);
char* PathOnlyBackslash(char* filePath);
char* PathRemoveBackslash(char* filePath);
int imgExtLegality(char* imgExtName);
int videoExtLegality(char* videoExtName);
int ffmpeg(const char *inputFile, const char *outputFile);

void playVideo();
