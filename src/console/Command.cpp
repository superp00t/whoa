#include "console/Command.hpp"
#include "console/Line.hpp"
#include <storm/Error.hpp>
#include <cctype>

int32_t ValidateFileName(const char* filename) {
    if (SStrStr(filename, "..") || SStrStr(filename, "\\")) {
        // TODO
        // ConsoleWrite("File Name cannot contain '\\' or '..'", ERROR_COLOR);
        return 0;
    }

    const char* extension = SStrChrR(filename, '.');

    if (extension && SStrCmpI(extension, ".wtf", -1)) {
        // TODO
        // ConsoleWrite("Only .wtf extensions are allowed", ERROR_COLOR);
        return 0;
    }

    return 1;
}

TSHashTable<CONSOLECOMMAND, HASHKEY_STRI> g_consoleCommandHash;
char g_commandHistory[HISTORY_DEPTH][CMD_BUFFER_SIZE];
uint32_t g_commandHistoryIndex;
char g_ExecBuffer[CONSOLE_EXEC_BUFFER_SIZE] = {0};
EXECMODE g_ExecCreateMode = EM_NOTACTIVE;

int32_t AddLineToExecFile(const char* currentLine) {
    char stringToWrite[STORM_MAX_PATH];

    if (g_ExecCreateMode != EM_PROMPTOVERWRITE) {
        if (!SStrCmpI(currentLine, "end", STORM_MAX_STR)) {
            if (g_ExecCreateMode != EM_APPEND) {
                g_ExecCreateMode = EM_WRITEFILE;
            }

            return 1;
        }

        SStrPrintf(stringToWrite, sizeof(stringToWrite), "%s\n", currentLine);

        if (((sizeof(g_ExecBuffer)-1) - SStrLen(g_ExecBuffer)) != SStrLen(stringToWrite)){
            SStrPack(g_ExecBuffer, stringToWrite, sizeof(g_ExecBuffer));
        }

        return 0;
    }

    if (currentLine[0] == 'n') {
        ConsoleWrite("Canceled File Creation", ECHO_COLOR);
        g_ExecCreateMode = EM_NOTACTIVE;
        return 0;
    }

    if (currentLine[0] != 'y') {
        ConsoleWrite("You must type 'y' to confirm overwrite. Process aborted!", ERROR_COLOR);
        g_ExecCreateMode = EM_NOTACTIVE;
        return 0;
    }

    // TODO

    return 1;
}


void ConsoleCommandDestroy() {
    g_consoleCommandHash.Clear();
}

char* ConsoleCommandHistory(uint32_t index) {
    // Return a pointer to the buffer at the specified index
    return g_commandHistory[((g_commandHistoryIndex + (HISTORY_DEPTH - 1) - index) & (HISTORY_DEPTH - 1))];
}

void AddToHistory(const char* command) {
    SStrCopy(g_commandHistory[g_commandHistoryIndex], command, CONSOLE_LINE_LENGTH);
    g_commandHistoryIndex = (g_commandHistoryIndex + 1) & (HISTORY_DEPTH-1);
}

uint32_t ConsoleCommandHistoryDepth() {
    return HISTORY_DEPTH;
}

void ConsoleCommandInitialize() {
    ConsoleCommandRegister("help", ConsoleCommand_Help, CONSOLE, "Provides help information about a command.");
}

int32_t ConsoleCommandRegister(const char* command, int32_t (*handler)(const char*, const char*), CATEGORY category, const char* helpText) {
    STORM_ASSERT(command);
    STORM_ASSERT(handler);

    if (SStrLen(command) > (MAX_CMD_LENGTH - 1) || g_consoleCommandHash.Ptr(command)) {
        // The command name exceeds MAX_CMD_LENGTH, minus the null terminator
        // or it has already been registered
        return 0;
    }

    // Register the new command
    auto commandPtr = g_consoleCommandHash.New(command, 0, 0);
    commandPtr->m_command = command;
    commandPtr->m_handler = handler;
    commandPtr->m_helpText = helpText;
    commandPtr->m_category = category;

    return 1;
}

void ConsoleCommandUnregister(const char* command) {
    if (command) {
        auto commandPtr = g_consoleCommandHash.Ptr(command);
        if (commandPtr) {
            g_consoleCommandHash.Delete(commandPtr);
        }
    }
}

CONSOLECOMMAND* ParseCommand(const char* commandLine, const char** command, const char** arguments) {
    STORM_ASSERT(commandLine);

    static char cmd[32] = {0};

    auto string = commandLine;

    SStrTokenize(&string, cmd, sizeof(cmd), " ,;\t\"\r\n", nullptr);

    if (command) {
        *command = cmd;
    }

    if (commandLine) {
        while (isspace(*commandLine)) {
            commandLine++;
        }

        *arguments = commandLine;
    }

    return g_consoleCommandHash.Ptr(cmd);
}

void MakeCommandCurrent(CONSOLELINE* lineptr, char* command) {
    auto len = lineptr->inputstart;
    lineptr->inputpos = len;
    lineptr->chars = len;
    lineptr->buffer[len] = '\0';
    len = SStrLen(command);
    ReserveInputSpace(lineptr, len);
    SStrCopy(lineptr->buffer + lineptr->inputpos, command, STORM_MAX_STR);
    len = lineptr->inputpos + len;
    lineptr->inputpos = len;
    lineptr->chars = len;
}

void ConsoleCommandExecute(char* commandLine, int32_t addToHistory) {
    auto em = g_ExecCreateMode;

    if (em == EM_RECORDING || em == EM_PROMPTOVERWRITE || em == EM_APPEND) {
        AddLineToExecFile(commandLine);
        return;
    }

    auto history = ConsoleCommandHistory(0);

    if (addToHistory && (history == nullptr || SStrCmp(commandLine, history, STORM_MAX_STR))) {
        AddToHistory(commandLine);
    }

    const char* command;
    const char* arguments;

    auto cmd = ParseCommand(commandLine, &command, &arguments);

    if (cmd) {
        cmd->m_handler(command, arguments);
    } else {
        ConsoleWrite("Unknown command", DEFAULT_COLOR);
    }
}

int32_t ConsoleCommand_Help(const char* command, const char* arguments) {
    ConsoleWriteA("Help for \"%s\":", DEFAULT_COLOR, command);
    // TODO
    return 0;
}

int32_t ConsoleCommand_Quit(const char* command, const char* arguments) {
    // TODO
    // ConsolePostClose()
    return 0;
}

int32_t ConsoleCommand_Ver(const char* command, const char* arguments) {
    // TODO
    return 0;
}
