#include "client_date_handler.h"
#include <string>
#include <ctime>
#include <ratio>
#include <chrono>

DateHandler::DateHandler(){}

void DateHandler::setStartAndEndDate(std::string &s_date, std::string &e_date){
    std::chrono::duration<int, std::ratio<60*60*24>> thirty_days(30);

    std::chrono::system_clock::time_point s = std::chrono::system_clock::now();
    std::chrono::system_clock::time_point end = s + thirty_days;

    time_t temp_date;

    temp_date = std::chrono::system_clock::to_time_t(s);
    //std::string starting_date = ctime(&temp_date);
    char sBuffer[DATE_BUFFER_MAX_LEN];
    ctime_r(&temp_date, sBuffer);
    std::string starting_date(sBuffer);
    s_date = starting_date.substr(4, starting_date.size()-5);

    temp_date = std::chrono::system_clock::to_time_t(end);
    //std::string ending_date = ctime(&temp_date);
    char eBuffer[DATE_BUFFER_MAX_LEN];
    ctime_r(&temp_date, eBuffer);
    std::string ending_date(eBuffer);
    e_date = ending_date.substr(4, ending_date.size()-5);
}
