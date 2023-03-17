#include "RegisterCommand.h"

#include <utility>

#include "Client.h"
#include "File.h"

RegisterCommand::RegisterCommand(Event& CallbackEvent, std::string Username, std::string TargetFolder, std::string TargetFile) : Command(CallbackEvent), username_(std::move(Username)), targetFolder_(std::move(TargetFolder)), targetFile_(std::move(TargetFile))
{
    RegisterCommand::Execute();
}

bool RegisterCommand::Execute()
{
    Command::Execute();

    const auto location = File::ExePath();
    if (location.empty())
    {
        printf("ERROR: Exe path failed when trying to execute RegisterCommand{%s}\n", username_.c_str());
        return false;
    }

    // Expects folder to already be there TODO: Fix this so that it creates the director if not there.
    const auto finalLocation = location + targetFolder_;
    const auto file = finalLocation + targetFile_;

    std::string fileData;
    if (CreateDirectory(finalLocation.c_str(), nullptr) || ERROR_ALREADY_EXISTS == GetLastError())
    {
        // Directory created
        // Check that the file exists.
        fileData = File::ReadFile(file.c_str());
    }

    size_t pos;
    while ((pos = fileData.find(',')) != std::string::npos)
    {
        std::string str = fileData.substr(0, pos);
        if (str == username_)
        {
            printf("User %s already registered!\n", username_.c_str());
            return true;
        }

        fileData = fileData.substr(pos + 2);
    }

    if(callbackEvent_)
    {
        callbackEvent_->Notify();
    }
    
    return File::WriteToFile(file.c_str(), (username_ + ",").c_str(), std::ios_base::app);
}
