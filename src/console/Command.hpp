#ifndef CONSOLE_COMMAND_HPP
#define CONSOLE_COMMAND_HPP

#include "console/Line.hpp"

#include <storm/Hash.hpp>
#include <cstdint>

#define EXEC_BUFFER_SIZE 8192
#define CMD_BUFFER_SIZE 1024
#define MAX_CMD_LENGTH 64
#define HISTORY_DEPTH 32
#define NOHELP nullptr

enum CATEGORY {
    DEBUG,
    GRAPHICS,
    CONSOLE,
    COMBAT,
    GAME,
    DEFAULT,
    NET,
    SOUND,
    GM,
    NONE,
    LAST
};

typedef int32_t (*COMMANDHANDLER)(const char*, const char*);

class CONSOLECOMMAND : public TSHashObject<CONSOLECOMMAND, HASHKEY_STRI> {
    public:
        const char* m_command;
        COMMANDHANDLER m_handler;
        const char* m_helpText;
        CATEGORY m_category;
};

class ConsoleCommandList {
    public:
        const char* m_command;
        COMMANDHANDLER m_handler;
        const char* m_helpText;
};

#define CONSOLE_EXEC_BUFFER_SIZE 0x2000

enum EXECMODE {
    EM_PROMPTOVERWRITE = 0,
    EM_RECORDING = 1,
    EM_APPEND = 2,
    EM_WRITEFILE = 3,
    EM_NOTACTIVE = 4,
    EM_NUM_EXECMODES = 5
};

extern TSHashTable<CONSOLECOMMAND, HASHKEY_STRI> g_consoleCommandHash;
extern char g_commandHistory[HISTORY_DEPTH][CMD_BUFFER_SIZE];
extern uint32_t g_commandHistoryIndex;
extern char g_ExecBuffer[EXEC_BUFFER_SIZE];

void ConsoleCommandDestroy();

char* ConsoleCommandHistory(uint32_t index);

uint32_t ConsoleCommandHistoryDepth();

int32_t ConsoleCommandRegister(const char* command, COMMANDHANDLER handler, CATEGORY category, const char* helpText);

void ConsoleCommandInitialize();

void ConsoleInitializeCommonCommand();

void ConsoleInitializeDebugCommand();

void ConsoleInitializeScreenCommand();

void RegisterConsoleCommandList(CATEGORY category, ConsoleCommandList list[], size_t count);

#define CONSOLE_REGISTER_LIST(category, list) RegisterConsoleCommandList(category, list, std::size(list))

void ConsoleCommandUnregister(const char* command);

void ConsoleCommandExecute(char* commandLine, int32_t addToHistory);

void MakeCommandCurrent(CONSOLELINE* lineptr, char* command);

int32_t ConsoleCommand_Quit(const char* command, const char* arguments);
int32_t ConsoleCommand_Ver(const char* command, const char* arguments);
int32_t ConsoleCommand_SetMap(const char* command, const char* arguments);

int32_t ConsoleCommand_Help(const char* command, const char* arguments);
int32_t ConsoleCommand_FontColor(const char* command, const char* arguments);
int32_t ConsoleCommand_BackGroundColor(const char* command, const char* arguments);
int32_t ConsoleCommand_HighLightColor(const char* command, const char* arguments);
int32_t ConsoleCommand_FontSize(const char* command, const char* arguments);
int32_t ConsoleCommand_Font(const char* command, const char* arguments);
int32_t ConsoleCommand_BufferSize(const char* command, const char* arguments);
int32_t ConsoleCommand_ClearConsole(const char* command, const char* arguments);
int32_t ConsoleCommand_Proportional(const char* command, const char* arguments);
int32_t ConsoleCommand_CharSpacing(const char* command, const char* arguments);
int32_t ConsoleCommand_CurrentSettings(const char* command, const char* arguments);
int32_t ConsoleCommand_DefaultSettings(const char* command, const char* arguments);
int32_t ConsoleCommand_CloseConsole(const char* command, const char* arguments);
int32_t ConsoleCommand_RepeatHandler(const char* command, const char* arguments);
int32_t ConsoleCommand_AppendLogToFile(const char* command, const char* arguments);

#endif
