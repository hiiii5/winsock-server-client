#pragma once
#include <string>

#include "Command.h"

class ClientExitCommand : public Command
{
public:
    ClientExitCommand(Event& CallbackEvent);
    
    bool Execute() override;
};
