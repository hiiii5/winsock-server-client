#include "Command.h"

#include <string>

#include "ClientExitCommand.h"
#include "RegisterCommand.h"

Command::Command(Event& CallbackEvent) : callbackEvent_(&CallbackEvent)
{
}

bool Command::TryExecute(char* Buffer, Command& FoundCommand, bool& IsDisconnecting)
{
    auto s = std::string(Buffer);
    
    if(const int commandPos = s.find("$register ") != std::string::npos)
    {
        const auto username = s.substr(commandPos+1+strlen("register"));
        FoundCommand = RegisterCommand(*FoundCommand.callbackEvent_, username);
        return true;
    }

    if(s.find("$exit") != std::string::npos)
    {
        //const auto username = s.substr(commandPos+1+strlen("register"));
        IsDisconnecting = true;
        FoundCommand = ClientExitCommand(*FoundCommand.callbackEvent_);
        return true;
    }

    return false;
}

bool Command::IsCommand(char* Buffer)
{
    auto s = std::string(Buffer);
    size_t pos;
    if((pos = s.find('$')) != std::string::npos)
    {
        if(s.substr(pos).find("register") != std::string::npos)
        {
            return true;
        }

        if(s.substr(pos).find("exit") != std::string::npos)
        {
            return true;
        }
    }

    return false;
}

bool Command::Execute()
{
    return false;
}

void Command::SetDelimiter(const char NewDelimiter)
{
    delimiter_ = NewDelimiter;
}

char Command::GetDelimiter() const
{
    return delimiter_;
}

/*bool Command::LoadCommandsFromFile(const char* Filename)
{
    auto location = File::ExePath();
    std::string data = File::ReadFile(Filename);

    size_t pos = 0;
    while ((pos = data.find('\n')) != std::string::npos)
    {
        auto token = data.substr(0, pos);
        data.erase(0, pos + 1);

        pos = token.find('=');
        if(pos == std::string::npos)
        {
            printf("ERROR: Command %s was invalidly formatted, make sure there is an = in the command! Skipping...", token.c_str());
            continue;
        }
        
        auto commandName = token.substr(0, pos);
        auto 
    }
}*/
