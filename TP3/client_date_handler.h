#ifndef DATE_HANDLER_H
#define DATE_HANDLER_H

#include <string>
#define DATE_BUFFER_MAX_LEN 50

class DateHandler{
    public:
    DateHandler();
    void setStartAndEndDate(std::string &s_date, std::string &e_date);
};

#endif
