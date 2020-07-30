#pragma once
#include <cmath>
#include <iostream>
#include <iomanip>
#include <e/system/repr.h>
namespace e
{
    namespace system
    {
        struct datetime
        {
        private:
            inline constexpr static int32_t DaysPerYear = 365;
            inline constexpr static int32_t DaysPer4Years = DaysPerYear * 4 + 1;       // 1461
            inline constexpr static int32_t DaysPer100Years = DaysPer4Years * 25 - 1;  // 36524
            inline constexpr static int32_t DaysPer400Years = DaysPer100Years * 4 + 1; // 146097
            inline constexpr static int32_t DaysToMonth365[] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};
            inline constexpr static int32_t DaysToMonth366[] = {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335, 366};
            inline constexpr static int32_t TotalSecondsPerDay = 24 * 60 * 60;

        public:
            double value;

            constexpr datetime() noexcept : value(0)
            {
            }

            explicit constexpr datetime(double _v) noexcept : value(_v)
            {
            }

            datetime(int32_t year, int32_t month, int32_t day, int32_t hour = 0, int32_t minute = 0, int32_t second = 0)
            {
                //See Also https://www.codeguru.com/cpp/cpp/cpp_mfc/article.php/c765/An-ATL-replacement-for-COleDateTime.htm
                if (year < 0)
                {
                    throw std::invalid_argument("year is out of range");
                }
                if (month < 1 || month > 12)
                {
                    throw std::invalid_argument("month is out of range");
                }
                bool leapYear = ((year % 4) == 0) && ((year % 100) != 0 || (year % 400) == 0);
                int daysInMonth = DaysToMonth365[month] - DaysToMonth365[month - 1] + ((leapYear && day == 29 && month == 2) ? 1 : 0);
                if (day < 1 || day > daysInMonth)
                {
                    throw std::invalid_argument("day is out of range");
                }
                if (hour < 0 || hour > 23)
                {
                    throw std::invalid_argument("hour is out of range");
                }
                if (minute < 0 || minute > 59)
                {
                    throw std::invalid_argument("minute is out of range");
                }
                if (second < 0 || second > 60)
                {
                    throw std::invalid_argument("second is out of range");
                }
                else if (second == 60)
                {
                    //just ignore leap second
                    second = 59;
                }
                int32_t datePart;
                datePart = year * 365L + year / 4 - year / 100 + year / 400 + DaysToMonth365[month - 1] + day;
                if (month <= 2 && leapYear)
                {
                    --datePart;
                }
                datePart -= 693959;
                auto timePart = (hour * 3600 + minute * 60 + second) / 86400.0;
                this->value = (double)datePart + ((datePart >= 0) ? timePart : -timePart);
            }

            datetime &operator+=(const datetime &_Right) noexcept
            {
                this->value += _Right.value;
                return *this;
            }

            datetime &operator-=(const datetime &_Right) noexcept
            {
                this->value -= _Right.value;
                return *this;
            }

            int32_t day_of_year() const
            {
                //See Also https://referencesource.microsoft.com/#mscorlib/system/datetime.cs,28f94ab932951fd7
                int32_t n = static_cast<int32_t>(std::trunc(value) + 693593);
                int32_t y400 = n / DaysPer400Years;
                n -= y400 * DaysPer400Years;
                int32_t y100 = n / DaysPer100Years;
                if (y100 == 4)
                {
                    y100 = 3;
                }
                n -= y100 * DaysPer100Years;
                int32_t y4 = n / DaysPer4Years;
                n -= y4 * DaysPer4Years;
                int32_t y1 = n / DaysPerYear;
                if (y1 == 4)
                {
                    y1 = 3;
                }
                n -= y1 * DaysPerYear;
                return n + 1;
            }

            void get_date_part(int32_t *year, int32_t *month, int32_t *day) const
            {
                //See Also https://referencesource.microsoft.com/#mscorlib/system/datetime.cs,28f94ab932951fd7
                int32_t n = static_cast<int32_t>(std::trunc(value) + 693593);
                int32_t y400 = n / DaysPer400Years;
                n -= y400 * DaysPer400Years;
                int32_t y100 = n / DaysPer100Years;
                if (y100 == 4)
                {
                    y100 = 3;
                }
                n -= y100 * DaysPer100Years;
                int32_t y4 = n / DaysPer4Years;
                n -= y4 * DaysPer4Years;
                int32_t y1 = n / DaysPerYear;
                if (y1 == 4)
                {
                    y1 = 3;
                }
                if (year)
                {
                    *year = y400 * 400 + y100 * 100 + y4 * 4 + y1 + 1;
                }
                n -= y1 * DaysPerYear;
                bool leapYear = y1 == 3 && (y4 != 24 || y100 == 3);
                const int32_t *days = leapYear ? DaysToMonth366 : DaysToMonth365;
                int32_t m = (n >> 5) + 1;
                while (n >= days[m])
                {
                    m++;
                }
                if (month)
                {
                    *month = m;
                }
                if (day)
                {
                    *day = n - days[m - 1] + 1;
                }
            }

            int32_t total_seconds_in_time_part() const
            {
                double dataPart;
                double timePart = std::modf(value, &dataPart);
                int32_t result = static_cast<int32_t>(std::round(TotalSecondsPerDay * std::abs(timePart)));
                return result;
            }

            void get_time_part(int32_t *hour, int32_t *minute, int32_t *second) const
            {
                auto total_seconds = total_seconds_in_time_part();
                if (hour)
                {
                    *hour = total_seconds / 3600;
                }
                if (minute)
                {
                    *minute = (total_seconds / 60) % 60;
                }
                if (second)
                {
                    *second = total_seconds % 60;
                }
            }

            int32_t year() const
            {
                int32_t result;
                get_date_part(&result, nullptr, nullptr);
                return result;
            }

            int32_t month() const
            {
                int32_t result;
                get_date_part(nullptr, &result, nullptr);
                return result;
            }

            int32_t day() const
            {
                int32_t result;
                get_date_part(nullptr, nullptr, &result);
                return result;
            }

            int32_t hour() const
            {
                return total_seconds_in_time_part() / 3600;
            }

            int32_t minute() const
            {
                return (total_seconds_in_time_part() / 60) % 60;
            }

            int32_t second() const
            {
                return total_seconds_in_time_part() % 60;
            }

            int32_t day_of_week() const
            {
                return (static_cast<int32_t>(std::fmod(value, 7)) + 6) % 7 + 1;
            }

            friend std::ostream &operator<<(std::ostream &output, const datetime &x)
            {
                int32_t year, month, day, hour, minute, second;
                x.get_date_part(&year, &month, &day);
                x.get_time_part(&hour, &minute, &second);
                output << year << "-";
                if (month < 10)
                {
                    output << "0";
                }
                output << month << "-";
                if (day < 10)
                {
                    output << "0";
                }
                output << day << " ";
                if (hour < 10)
                {
                    output << "0";
                }
                output << hour << ":";
                if (minute < 10)
                {
                    output << "0";
                }
                output << minute << ":";
                if (second < 10)
                {
                    output << "0";
                }
                output << second;
                return output;
            }
        };

        inline datetime operator+(const datetime &_Left, const datetime &_Right) noexcept
        {
            datetime r = _Left;
            r += _Right;
            return r;
        }
        inline datetime operator-(const datetime &_Left, const datetime &_Right) noexcept
        {
            datetime r = _Left;
            r -= _Right;
            return r;
        }
        inline bool operator==(const datetime &_Left, const datetime &_Right) noexcept
        {
            return _Left.value == _Right.value;
        }
        inline bool operator!=(const datetime &_Left, const datetime &_Right) noexcept
        {
            return _Left.value != _Right.value;
        }
        inline bool operator<(const datetime &_Left, const datetime &_Right) noexcept
        {
            double dataPart_left;
            double timePart_left = std::fabs(std::modf(_Left.value, &dataPart_left));
            double dataPart_right;
            double timePart_right = std::fabs(std::modf(_Right.value, &dataPart_right));
            if (dataPart_left == dataPart_right)
            {
                return timePart_left < timePart_right;
            }
            return dataPart_left < dataPart_right;
        }
        inline bool operator<=(const datetime &_Left, const datetime &_Right) noexcept
        {
            double dataPart_left;
            double timePart_left = std::fabs(std::modf(_Left.value, &dataPart_left));
            double dataPart_right;
            double timePart_right = std::fabs(std::modf(_Right.value, &dataPart_right));
            if (dataPart_left == dataPart_right)
            {
                return timePart_left <= timePart_right;
            }
            return dataPart_left <= dataPart_right;
        }
        inline bool operator>=(const datetime &_Left, const datetime &_Right) noexcept
        {
            double dataPart_left;
            double timePart_left = std::fabs(std::modf(_Left.value, &dataPart_left));
            double dataPart_right;
            double timePart_right = std::fabs(std::modf(_Right.value, &dataPart_right));
            if (dataPart_left == dataPart_right)
            {
                return timePart_left >= timePart_right;
            }
            return dataPart_left >= dataPart_right;
        }
        inline bool operator>(const datetime &_Left, const datetime &_Right) noexcept
        {
            double dataPart_left;
            double timePart_left = std::fabs(std::modf(_Left.value, &dataPart_left));
            double dataPart_right;
            double timePart_right = std::fabs(std::modf(_Right.value, &dataPart_right));
            if (dataPart_left == dataPart_right)
            {
                return timePart_left > timePart_right;
            }
            return dataPart_left > dataPart_right;
        }
    }
}
template <>
struct e::system::repr_maker<e::system::datetime>
{
    static e::system::string of(const e::system::datetime &value)
    {
        std::ostringstream stream;
        stream << "[";
        stream << value;
        stream << "] (";
        stream << std::hexfloat << value.value;
        stream << ")";
        return e::system::string(stream.str().c_str());
    }
};
IMPLEMENT_DBG_INFO_VIA_REPR(e::system::datetime)