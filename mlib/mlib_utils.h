#ifndef mlib_utils_h
#define mlib_utils_h

#include "mlib_helpers.h"

#include <string>
#include <sstream>
#include <vector>

MLIB_NS_BEGIN

#define SSTR(x) (static_cast<const std::stringstream &>(std::stringstream() << x).str())
#define VALUE_FROM_STRING(str, type) (mlib::value_from_string<type>(str))

std::string urlencode(const std::string &c);
std::string char2hex( char dec );
std::string md5_string(const std::string &inputStr, bool rawOutput = false);
std::string base64_encode(const void*, uint32_t len);
std::string base64_decode(std::string const& s);

std::string trim(const std::string& str, std::string toTrim = " \r\n\t");
std::vector<std::string> split_string(std::string s, std::string delimiter = " ");
bool check_email(std::string email);

uint32_t rand_by_seed(uint32_t seed);
uint32_t rand_in_range(uint32_t start, uint32_t end, uint32_t seed = 0);

std::string string_replace(const std::string &str, const std::string &strSrc, const std::string &strDst);

int32_t get_local_timezone_offset();    // in seconds
uint32_t get_utf8_length(const char *s);
std::string sub_str_for_utf8(std::string str, uint32_t maxLength);

template <typename T>
T value_from_string(std::string str)
{
    T ret;
    std::stringstream ss;
    ss << str;
    ss >> ret;
    return ret;
}

template <typename T>
T split_decimal(T num, uint32_t count)
{
    T front = floor(num);
    T back = num - front;
    std::stringstream ss;
    ss << back;
    std::string decimalStr = ss.str().substr(0, count + 2);
    T decimal = value_from_string<T>(decimalStr);
    
    return front + decimal;
}
MLIB_NS_END

#endif
