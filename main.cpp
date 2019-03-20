#include "exec.h"
#include <string>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <string>
#include <chrono>
#include <thread>

#ifdef _WIN32
#define timegm _mkgmtime
#endif

inline int ParseInt(const char* value)
{
    return std::strtol(value, nullptr, 10);
}

std::time_t ParseISO8601(const std::string& input)
{
    constexpr const size_t expectedLength = sizeof("1234-12-12T12:12:12Z") - 1;
    static_assert(expectedLength == 20, "Unexpected ISO 8601 date/time length");

    if (input.length() >= expectedLength)
    {
        return 0;
    }

    std::tm time = { 0 };
    time.tm_year = ParseInt(&input[0]) - 1900;
    time.tm_mon = ParseInt(&input[5]) - 1;
    time.tm_mday = ParseInt(&input[8]);
    time.tm_hour = ParseInt(&input[11]);
    time.tm_min = ParseInt(&input[14]);
    time.tm_sec = ParseInt(&input[17]);
    time.tm_isdst = 0;

    const int millis = input.length() > 20 ? ParseInt(&input[20]) : 0;


    return timegm(&time) * 1000 + millis;
}

std::string getLastLogin() {
    const char* lastLoginCmd = "wevtutil qe /rd /f:xml /c:1 /q:\"*[System[(EventID=4800 or EventID=4801)]]\" Security";
    std::string lastLoginXml = exec::process(lastLoginCmd);

    if (lastLoginXml.length() == 0) {
        std::cout << "No last login found!" << std::endl;
        return "";
    }

    std::string delimeter = "SystemTime='";
    std::string time = lastLoginXml.substr(lastLoginXml.find(delimeter) + delimeter.length(), lastLoginXml.length()-1);
    time = time.substr(0, time.find("'"));
    time = time.substr(0, time.find("."));

    return time;
}

std::string getLastInvalidLogin() {
    const char* lastLoginCmd = "wevtutil qe /rd /f:xml /c:1 /q:\"*[System[(EventID=4625)]]\" Security";
    std::string lastLoginXml = exec::process(lastLoginCmd);

    if (lastLoginXml.length() == 0) {
        std::cout << "No last login found!" << std::endl;
        return "";
    }

    std::string delimeter = "SystemTime='";
    std::string time = lastLoginXml.substr(lastLoginXml.find(delimeter) + delimeter.length(), lastLoginXml.length()-1);
    time = time.substr(0, time.find("'"));
    time = time.substr(0, time.find("."));

    return time;
}

void shutdown() {
    const char* abort = "shutdown /a";
    exec::process(abort);

    const char* shutdown = "shutdown /s /f /t 0";
    exec::process(shutdown);
}

int main()
{
    time_t start = time(0);

    std::string lastLogin = getLastLogin();
    std::string lastInvalid = getLastInvalidLogin();
    std::cout << "Last login: " << lastLogin << " - " << ParseISO8601(lastLogin) << std::endl;

    std::string temp;

    int wrongs = 0;
    while (true) {
        double seconds_since_start = difftime( time(0), start);

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));
        temp = getLastLogin();
        if (temp != lastLogin) {
            std::cout << std::endl << "[" << seconds_since_start << "] " << "NEW LOGIN DETECTED!" << std::endl << std::endl;
            lastLogin = temp;
            wrongs = 0;
        }

        temp = getLastInvalidLogin();
        if (temp != lastInvalid) {
            wrongs++;
            std::cout << "[" << seconds_since_start << "] " << "New wrong login detected; wrongs=" << wrongs << std::endl;
            lastInvalid = temp;

            if (wrongs == 3) {
                shutdown();
            }
        }
        //std::cout << "Last Invalid: " << lastInvalid << " - " << ParseISO8601(lastInvalid) << std::endl;
    }


    return 0;
}
