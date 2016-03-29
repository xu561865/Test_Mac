#include "mlib_utils.h"
#include "mlib_md5.h"
#include "mlib_buffer.h"
#include <zlib.h>
#include <regex>
#include "mlib_log.h"

using namespace std;

MLIB_NS_BEGIN

static const std::string base64_chars =
"ABCDEFGHIJKLMNOPQRSTUVWXYZ"
"abcdefghijklmnopqrstuvwxyz"
"0123456789+/";

//based on javascript encodeURIComponent()
std::string urlencode(const std::string &c)
{
    
    std::string escaped="";
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
            (65 <= c[i] && c[i] <= 90) ||//abc...xyz
            (97 <= c[i] && c[i] <= 122) || //ABC...XYZ
            (c[i]=='~' || c[i]=='!' || c[i]=='*' || c[i]=='(' || c[i]==')' || c[i]=='\'')
            )
        {
            escaped.append( &c[i], 1);
        }
        else
        {
            escaped.append("%");
            escaped.append( char2hex(c[i]) );//converts char 255 to string "ff"
        }
    }
    return escaped;
}

std::string char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48inascii
    if (10<= dig1 && dig1<=15) dig1+=97-10; //a,97inascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=97-10;
    
    std::string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}

std::string md5_string(const std::string &inputStr, bool rawOutput/* = false*/)
{
    MD5_CTX context;
    uint8_t digest[16];
    MD5Init(&context);
    MD5Update(&context, (const uint8_t*) (((inputStr.c_str()))),
              inputStr.size());
    MD5Final(digest, &context);
    if (rawOutput)
    {
        return std::string((char*) ((digest)), 16);
    }
    else
    {
        std::string ret;
        ret.reserve(32);
        for (uint8_t i=0; i<16; ++i) {
            ret.append(char2hex(digest[i]));
        }
        return ret;
    }
}

inline static bool is_base64(uint8_t c)
{
    return (isalnum(c) || (c == '+') || (c == '/'));
}

std::string base64_encode(const void* vp_bytes_to_encode, uint32_t in_len)
{
    const uint8_t* bytes_to_encode = (const uint8_t*) ((vp_bytes_to_encode));
    std::string ret;
    ret.reserve(in_len*2);
    int32_t i = 0;
    int32_t j = 0;
    uint8_t char_array_3[3];
    uint8_t char_array_4[4];
    while (in_len--)
    {
        char_array_3[i++] = *(bytes_to_encode++);
        if (i == 3)
        {
            char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
            char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
            + ((char_array_3[1] & 0xf0) >> 4);
            char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
            + ((char_array_3[2] & 0xc0) >> 6);
            char_array_4[3] = char_array_3[2] & 0x3f;
            for (i = 0; (i < 4); i++)
                ret += base64_chars[char_array_4[i]];
            i = 0;
        }
    }
    
    if (i)
    {
        for (j = i; j < 3; j++)
            char_array_3[j] = '\0';
        char_array_4[0] = (char_array_3[0] & 0xfc) >> 2;
        char_array_4[1] = ((char_array_3[0] & 0x03) << 4)
        + ((char_array_3[1] & 0xf0) >> 4);
        char_array_4[2] = ((char_array_3[1] & 0x0f) << 2)
        + ((char_array_3[2] & 0xc0) >> 6);
        char_array_4[3] = char_array_3[2] & 0x3f;
        for (j = 0; (j < i + 1); j++)
            ret += base64_chars[char_array_4[j]];
        while ((i++ < 3))
            ret += '=';
    }
    return ret;
}

std::string base64_decode(const std::string& encoded_string)
{
    int32_t in_len = encoded_string.size();
    int32_t i = 0;
    int32_t j = 0;
    int32_t in_ = 0;
    uint8_t char_array_4[4], char_array_3[3];
    std::string ret;
    ret.reserve(in_len);
    while (in_len-- && (encoded_string[in_] != '=')
           && is_base64(encoded_string[in_]))
    {
        char_array_4[i++] = encoded_string[in_];
        in_++;
        if (i == 4)
        {
            for (i = 0; i < 4; i++)
                char_array_4[i] = base64_chars.find(char_array_4[i]);
            char_array_3[0] = (char_array_4[0] << 2)
            + ((char_array_4[1] & 0x30) >> 4);
            char_array_3[1] = ((char_array_4[1] & 0xf) << 4)
            + ((char_array_4[2] & 0x3c) >> 2);
            char_array_3[2] = ((char_array_4[2] & 0x3) << 6)
            + char_array_4[3];
            for (i = 0; (i < 3); i++)
                ret += char_array_3[i];
            i = 0;
        }
    }
    
    if (i)
    {
        for (j = i; j < 4; j++)
            char_array_4[j] = 0;
        for (j = 0; j < 4; j++)
            char_array_4[j] = base64_chars.find(char_array_4[j]);
        char_array_3[0] = (char_array_4[0] << 2)
        + ((char_array_4[1] & 0x30) >> 4);
        char_array_3[1] = ((char_array_4[1] & 0xf) << 4)
        + ((char_array_4[2] & 0x3c) >> 2);
        char_array_3[2] = ((char_array_4[2] & 0x3) << 6) + char_array_4[3];
        for (j = 0; (j < i - 1); j++)
            ret += char_array_3[j];
    }
    return ret;
}

std::string trim(const std::string& str, std::string toTrim)
{
    size_t start = str.find_first_not_of(toTrim);
    if (start == std::string::npos) return str;
    
    size_t end = str.find_last_not_of(toTrim);
    return str.substr(start, end - start + 1);
}

std::vector<std::string> split_string(std::string s, std::string delimiter)
{
    std::vector<std::string> valueString;
    //if c is not included value  return
    if (s.find(delimiter) == std::string::npos) {
        valueString.push_back(s);
        return valueString;
    }
    
    int32_t lastPos = 0;
    int32_t currentPos = 0;
    std::string str = delimiter + s + delimiter;
    int32_t length = str.length();
    lastPos = str.find(delimiter);
    if (!s.empty())
    {
        while (currentPos != length - 1) {
            currentPos = str.find(delimiter,lastPos + 1);
            std::string substr;
            substr = str.substr(lastPos + 1,currentPos - lastPos - 1);
            valueString.push_back(substr);
            lastPos = currentPos;
        }
    }
    return valueString;
}

bool check_email(std::string email)
{
    regex pattern("([0-9A-Za-z\\-_\\.]+)@([0-9a-z]+\\.[a-z]{2,3}(\\.[a-z]{2})?)");
    return regex_match(email, pattern);
}

uint32_t rand_by_seed(uint32_t seed)
{
    if (seed == 0)
    {
        seed = 16807;
    }
    uint32_t hi, lo;
    hi = 16807 * (seed >> 16);
    lo = 16807 * (seed & 0xFFFF);
    lo += (hi & 0x7FFF) << 16;
    lo += hi >> 15;
    if (lo > 0x7FFFFFFF)
    {
        lo -= 0x7FFFFFFF;
    }
    return lo;
}

uint32_t rand_in_range(uint32_t start, uint32_t end, uint32_t seed)
{
    static uint32_t localSeed = 0;
    
    if (end <= start) return start;
    
    if (seed == 0)
    {
        if (localSeed == 0) localSeed = time(NULL);
        localSeed = rand_by_seed(localSeed);
        seed = localSeed;
    }
    
    uint32_t offset = end - start + 1;
    offset = rand_by_seed(seed) % offset;
    return (start + offset);
}

std::string string_replace(const std::string &str, const std::string &strSrc, const std::string &strDst)
{
    std::string s = str;
    std::string::size_type pos = 0;
    
    while( (pos = s.find(strSrc, pos)) != std::string::npos)
    {
        s.replace(pos, strSrc.length(), strDst);
        pos += strDst.length();
    }
    
    return s;
}

int32_t get_local_timezone_offset()
{
    time_t time_utc;
    struct tm tm_local;
    
    // Get the UTC time
    time(&time_utc);
    
    // Get the local time
    // Use localtime_r for threads safe
    localtime_r(&time_utc, &tm_local);
    
    time_t time_local;
    struct tm tm_gmt;
    
    // Change tm to time_t
    time_local = mktime(&tm_local);
    
    // Change it to GMT tm
    gmtime_r(&time_utc, &tm_gmt);
    
    int time_zone = tm_local.tm_hour - tm_gmt.tm_hour;
    if (time_zone < -12) {
        time_zone += 24;
    } else if (time_zone > 12) {
        time_zone -= 24;
    }
    
    return time_zone * 3600;
}

uint32_t get_utf8_length(const char *s)
{
    uint32_t i = 0, j = 0;
    while (s[i])
    {
        if ((s[i] & 0xc0) != 0x80)
        {
            j++;
        }
        i++;
    }
    return j;
}


std::string sub_str_for_utf8(std::string str, uint32_t maxLength)
{
    const char * s = str.c_str();
    uint32_t i = 0, j = 0;
    while (s[i])
    {
        if ((s[i] & 0xc0) != 0x80)
        {
            j++;
            if (j > maxLength)
            {
                break;
            }
        }
        i++;
    }
    
    return str.substr(0, i);
}
MLIB_NS_END