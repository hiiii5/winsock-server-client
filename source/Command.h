#pragma once
#include "Event.h"

class Command
{
public:
    Command(Event& CallbackEvent);
    static bool TryExecute(char* Buffer, Command& FoundCommand, bool& IsDisconnecting);
    static bool IsCommand(char* Buffer);

    virtual bool Execute();
    
    void SetDelimiter(char NewDelimiter);
    char GetDelimiter() const;

    //bool LoadCommandsFromFile(const char* Filename);
    
protected:
    //std::map<std::string, Command> loadedCommands_{};
    char delimiter_ = '$';
    Event* callbackEvent_;
};
