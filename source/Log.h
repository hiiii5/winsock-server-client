#pragma once
#include <chrono>
#include <string>

#include "File.h"

#ifndef LOG_DEFAULT_FOLDER
    #define LOG_DEFAULT_FOLDER "\\data"
#endif

#ifndef LOG_DEFAULT_FILE
    #define LOG_DEFAULT_FILE "\\log.log"
#endif

#ifndef LOG_SEV_MESSAGE
    #define LOG_SEV_MESSAGE " | MESSAGE: "
#endif

#ifndef LOG_SEV_WARNING
    #define LOG_SEV_WARNING " | WARNING: "
#endif

#ifndef LOG_SEV_ERROR
    #define LOG_SEV_ERROR " | ERROR: "
#endif

class Log
{
    template <typename T>
    static T Argument(T value) noexcept
    {
        return value;
    }

    template <typename T>
    static T const * Argument(std::basic_string<T> const & value) noexcept
    {
        return value.c_str();
    }
    
    template<typename ... Args>
    static void Write(char const* const format, Args const& ... args) noexcept
    {
        const std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        const auto timeMessage = std::string(std::ctime(&curTime));
        
        const auto path = File::ExePath();
        const auto target = path+LOG_DEFAULT_FOLDER+LOG_DEFAULT_FILE;

        const auto finalMessage = timeMessage + format;
        const std::string::size_type size = finalMessage.length();
        char* buf = new char[size];
        sprintf(buf, finalMessage.c_str(), Argument(args) ...);
        File::WriteToFile(target.c_str(), buf);
        
        printf(finalMessage.c_str(), Argument(args) ...);
    }

    static void Write(unsigned long long const * const value) noexcept
    {
        Write("%llu", value);
    }
    static void Write(char const * const value) noexcept
    {
        Write("%s", value);
    }
    static void Write(wchar_t const * const value) noexcept
    {
        Write("%ls", value);
    }
    template <typename T>
    static void Write(std::basic_string<T> const & value) noexcept
    {
        Write(value.c_str());
    }

public:
    enum LogSeverity
    {
        Message,
        Warning,
        Error,
        
    };
    
    template<typename ... Args>
    static void Write(LogSeverity Severity, char const* const format, Args const& ... args) noexcept
    {
        const std::time_t curTime = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

        const auto timeMessage = std::string(std::ctime(&curTime));

        std::string sevMessage = " | MESSAGE: ";
        switch (Severity)
        {
        case Message:
            {
                sevMessage = " | MESSAGE: ";
            }break;
        case Warning:
            {
                sevMessage = " | WARNING: ";
            }break;
        case Error:
            {
                sevMessage = " | ERROR: ";
            }break;
        }

        const auto path = File::ExePath();
        const auto target = path+LOG_DEFAULT_FOLDER+LOG_DEFAULT_FILE;

        const auto finalMessage = timeMessage + sevMessage + format + "\n";
        const std::string::size_type size = finalMessage.length();
        char* buf = new char[size];
        sprintf(buf, finalMessage.c_str(), Argument(args) ...);
        File::WriteToFile(target.c_str(), buf);
        
        printf(finalMessage.c_str(), Argument(args) ...);
    }
};
