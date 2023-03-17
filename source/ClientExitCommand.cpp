#include "ClientExitCommand.h"

ClientExitCommand::ClientExitCommand(Event& CallbackEvent) : Command(CallbackEvent)
{
    
}

bool ClientExitCommand::Execute()
{
    Command::Execute();

    if(callbackEvent_)
    {
        callbackEvent_->Notify();
    }
    
    return true;
}
