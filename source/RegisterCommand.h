#pragma once
#include <string>

#include "Command.h"

class RegisterCommand : public Command
{
public:
    RegisterCommand(Event& CallbackEvent, std::string Username, std::string TargetFolder = "\\data", std::string TargetFile = "\\users.csv");

    bool Execute() override;

private:
    std::string username_;
    std::string targetFolder_;
    std::string targetFile_;
};
