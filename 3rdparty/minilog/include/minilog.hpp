// The "MiniLog" library, in c++.
//
// Web: https://github.com/JaderoChan/MiniLog
// You can contact me by email: c_dl_cn@outlook.com

// MIT License
//
// Copyright (c) 2024 頔珞JaderoChan
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#ifndef MINILOG_HPP
#define MINILOG_HPP

#include <cstddef>          // size_t
#include <ctime>            // time_t, tm, time, localtime_s, strftime
#include <chrono>           // chrono For #StopWatch
#include <mutex>            // mutex, lock_guard For thread-safe
#include <string>           // string
#include <unordered_map>    // unordered_map
#include <iostream>         // ostream, cout, cerr, clog
#include <sstream>          // stringstream
#include <fstream>          // ofstream
#include <stdexcept>        // runtime_error

namespace mlog
{

using String = std::string;

/// @brief The log level.
enum Level
{
    LVL_DEBUG   = 0x01,
    LVL_INFO    = 0x02,
    LVL_WARNING = 0x04,
    LVL_ERROR   = 0x08,
    LVL_FATAL   = 0x10
};

enum OutFlag
{
    // Whether the output attach log level.
    OUT_WITH_LEVEL      = 0x01,
    // Whether the output attach timestamp.
    OUT_WITH_TIMESTAMP  = 0x02,
    // Whether colorize the output.
    // Just useful for std::cout, std::cerr, std::clog.
    OUT_WITH_COLORIZE   = 0x04
};

constexpr int LEVLE_FILTER_ALL    = 0xFF;
constexpr int LEVEL_FILTER_NONE   = 0x00;
constexpr int OUT_WITH_ALL        = 0xFF;
constexpr int OUT_WITH_NONE       = 0x00;

} // namespace mlog

namespace mlog
{

inline String levelToString(Level level)
{
    switch (level)
    {
        case LVL_DEBUG:
            return "[Debug]";
        case LVL_INFO:
            return "[Info]";
        case LVL_WARNING:
            return "[Warning]";
        case LVL_ERROR:
            return "[Error]";
        case LVL_FATAL:
            return "[Fatal]";
        default:
            return "";
    }
}

template <typename T>
String format(const String& fmt, const T& arg)
{
    std::stringstream ss;

    if (fmt.size() < 4)
    {
        size_t pos = fmt.find("{}");
        if (pos == String::npos)
            return fmt;

        ss << fmt.substr(0, pos);
        ss << arg;

        return ss.str() + fmt.substr(pos + 2);
    }

    String window(4, '\0');
    for (size_t i = 0; i < fmt.size();)
    {
        window[0] = fmt[i];
        window[1] = i < fmt.size() - 1 ? fmt[i + 1] : '\0';
        window[2] = i < fmt.size() - 2 ? fmt[i + 2] : '\0';
        window[3] = i < fmt.size() - 3 ? fmt[i + 3] : '\0';

        if (window == "{{}}")
        {
            ss << "{}";
            i += 4;
            continue;
        }

        if (window[0] == '{' && window[1] == '}')
        {
            ss << arg;
            return ss.str() + fmt.substr(i + 2);
        }
        else
        {
            ss << window[0];
            i += 1;
            continue;
        }
    }

    return ss.str();
}

template <typename T, typename... Args>
String format(const String& fmt, const T& arg, Args&&... args)
{
    std::stringstream ss;

    if (fmt.size() < 4)
    {
        size_t pos = fmt.find("{}");
        if (pos == String::npos)
            return fmt;

        ss << fmt.substr(0, pos);
        ss << arg;

        return ss.str() + fmt.substr(pos + 2);
    }

    String window(4, '\0');
    for (size_t i = 0; i < fmt.size();)
    {
        window[0] = fmt[i];
        window[1] = i < fmt.size() - 1 ? fmt[i + 1] : '\0';
        window[2] = i < fmt.size() - 2 ? fmt[i + 2] : '\0';
        window[3] = i < fmt.size() - 3 ? fmt[i + 3] : '\0';

        if (window == "{{}}")
        {
            ss << "{}";
            i += 4;
            continue;
        }

        if (window[0] == '{' && window[1] == '}')
        {
            ss << arg;
            return ss.str() + format(fmt.substr(i + 2), std::forward<Args>(args)...);
        }
        else
        {
            ss << window[0];
            i += 1;
            continue;
        }
    }

    return ss.str();
}

} // namespace mlog

namespace mlog
{

class StopWatch
{
    using Clock = std::chrono::steady_clock;
    template <typename T>
    using TimePoint = std::chrono::time_point<T>;

public:
    StopWatch() : startTime_(Clock::now()) {}

    /// @note Unit is millisecond.
    long long elapsed() const
    {
        return std::chrono::duration_cast<std::chrono::milliseconds>(Clock::now() - startTime_).count();
    }

    long long elapsedAndReset()
    {
        auto e = elapsed();
        reset();
        return e;
    }

    void reset() { startTime_ = Clock::now(); }

private:
    TimePoint<Clock> startTime_;
};

class Logger
{
public:
    Logger() = default;

    ~Logger() { removeAllOs(); }

    Logger(const Logger& other) = delete;

    Logger& operator=(const Logger& other) = delete;

    Logger(const String nameid, std::ostream& os, int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
    {
        addOs(nameid, os, outflag, levelFilter);
    }

    Logger(const String nameid, const String& filename,
        int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
    {
        addOs(nameid, filename, outflag, levelFilter);
    }

    /// @brief Get the global instance of Logger.
    static Logger& getGlobalInstance()
    {
        static Logger globalInstance;
        return globalInstance;
    }

    void addOs(const String& nameid, std::ostream& os,
        int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        if (outs_.find(nameid) != outs_.end())
            throw std::runtime_error("The name id is already exist");

        OutStream* os_ = new OutStream(&os, outflag, levelFilter);
        outs_.insert({ nameid, os_ });
    }

    void addOs(const String& nameid, const String& filename,
        int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        if (outs_.find(nameid) != outs_.end())
            throw std::runtime_error("The name id is already exist");

        OutStream* os_ = new FileOutStream(filename, outflag, levelFilter);
        outs_.insert({ nameid, os_} );
    }

    void removeOs(const String& nameid)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        delete outs_[nameid];
        outs_[nameid] = nullptr;

        outs_.erase(nameid);
    }

    void removeAllOs()
    {
        std::lock_guard<std::mutex> lock(mtx_);

        for (auto& var : outs_)
        {
            delete var.second;
            var.second = nullptr;
        }

        outs_.clear();
    }

    void setOsAttribute(const String& nameid, int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
    {
        std::lock_guard<std::mutex> lock(mtx_);

        if (outs_.find(nameid) == outs_.end())
            throw std::runtime_error("The nameid is not exist");

        outs_[nameid]->outflag = outflag;
        outs_[nameid]->levelFilter = levelFilter;
    }

    template <Level level, typename T>
    void log(const T& message)
    {
        String curtimeStr = currentTimeString_();
        String levelStr = levelToString(level);

        std::lock_guard<std::mutex> lock(mtx_);

        for (auto& var : outs_)
        {
            OutStream* os = var.second;

            if (!(level & os->levelFilter))
                continue;

            bool isConsole = os->os == &std::cout || os->os == &std::cerr || os->os == &std::clog;
            bool isColorize = isConsole && (os->outflag & OUT_WITH_COLORIZE);

            std::stringstream ss;

            if (os->outflag & OUT_WITH_TIMESTAMP)
            {
                ss << (isColorize ? "\033[0m\033[1;30m" : "");
                ss << curtimeStr;
                ss << (isColorize ? "\033[0m" : "");
                ss << ' ';
            }

            if (os->outflag & OUT_WITH_LEVEL)
            {
                if (isColorize)
                {
                    switch (level)
                    {
                        case LVL_DEBUG:     // Blue
                            ss << "\033[0m\033[34m";
                            break;
                        case LVL_INFO:      // Green
                            ss << "\033[0m\033[32m";
                            break;
                        case LVL_WARNING:   // Yellow
                            ss << "\033[0m\033[33m";
                            break;
                        case LVL_ERROR:     // Red
                            ss << "\033[0m\033[31m";
                            break;
                        case LVL_FATAL:     // Purple
                            ss << "\033[0m\033[35m";
                            break;
                        default:
                            ss << "\033[0m";
                            break;
                    }
                }

                ss << levelStr;
                ss << (isColorize ? "\033[0m" : "");
                ss << ' ';
            }

            ss << message << "\n";

            if (os->os)
                *os->os << ss.str();
        }
    }

    template <Level level, typename T>
    void log(const String& message, const T& arg) { log<level>(format(message, arg)); }

    template <Level level, typename T, typename... Args>
    void log(const String& message, const T& arg, Args&&... args)
    {
        log<level>(format(message, arg, std::forward<Args>(args)...));
    }

    template <typename T>
    void debug(const T& message) { log<LVL_DEBUG>(message); }

    template <typename T, typename... Args>
    void debug(const String& message, const T& arg, Args&&... args)
    {
        log<LVL_DEBUG>(message, arg, std::forward<Args>(args)...);
    }

    template <typename T>
    void info(const T& message) { log<LVL_INFO>(message); }

    template <typename T, typename... Args>
    void info(const String& message, const T& arg, Args&&... args)
    {
        log<LVL_INFO>(message, arg, std::forward<Args>(args)...);
    }

    template <typename T>
    void warning(const T& message) { log<LVL_WARNING>(message); }

    template <typename T, typename... Args>
    void warning(const String& message, const T& arg, Args&&... args)
    {
        log<LVL_WARNING>(message, arg, std::forward<Args>(args)...);
    }

    template <typename T>
    void error(const T& message) { log<LVL_ERROR>(message); }

    template <typename T, typename... Args>
    void error(const String& message, const T& arg, Args&&... args)
    {
        log<LVL_ERROR>(message, arg, std::forward<Args>(args)...);
    }

    template <typename T>
    void fatal(const T& message) { log<LVL_FATAL>(message); }

    template <typename T, typename... Args>
    void fatal(const String& message, const T& arg, Args&&... args)
    {
        log<LVL_FATAL>(message, arg, std::forward<Args>(args)...);
    }

private:
    struct OutStream
    {
        OutStream(std::ostream* os, int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
            : os(os), outflag(outflag), levelFilter(levelFilter)
        {}

        virtual ~OutStream() { os = nullptr; }

        int outflag       = OUT_WITH_ALL;
        int levelFilter   = LEVLE_FILTER_ALL;
        std::ostream* os    = nullptr;
    };

    struct FileOutStream final : public OutStream
    {
        FileOutStream(const String& filename, int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
            : OutStream(new std::ofstream(filename, std::ios_base::app), outflag, levelFilter)
        {
            if (!os || !dynamic_cast<std::ofstream*>(os)->is_open())
                throw std::runtime_error("Failed to open the file: " + filename);
        }

        ~FileOutStream()
        {
            if (os)
            {
                dynamic_cast<std::ofstream*>(os)->close();
                delete os;
            }
        }
    };

    static String currentTimeString_()
    {
        time_t time = 0;
        ::time(&time);

        tm lt;
        ::localtime_s(&lt, &time);

        char buffer[32] = {};
        ::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &lt);

        return buffer;
    }

    std::unordered_map<String, OutStream*> outs_;
    std::mutex mtx_;
};

}

namespace mlog
{

inline void addOs(const String& nameid, std::ostream& os,
    int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
{
    Logger::getGlobalInstance().addOs(nameid, os, outflag, levelFilter);
}

inline void addOs(const String& nameid, const String& filename,
    int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
{
    Logger::getGlobalInstance().addOs(nameid, filename, outflag, levelFilter);
}

inline void removeOs(const String& nameid)
{
    Logger::getGlobalInstance().removeOs(nameid);
}

inline void removeAllOs()
{
    Logger::getGlobalInstance().removeAllOs();
}

inline void setOsAttribute(const String& nameid, int outflag = OUT_WITH_ALL, int levelFilter = LEVLE_FILTER_ALL)
{
    Logger::getGlobalInstance().setOsAttribute(nameid, outflag, levelFilter);
}

template <Level level, typename T>
void log(const T& message) { Logger::getGlobalInstance().log<level>(message); }

template <Level level, typename T>
void log(const String& message, const T& arg) { Logger::getGlobalInstance().log<level>(message, arg); }

template <Level level, typename T, typename... Args>
void log(const String& message, const T& arg, Args&&... args)
{
    Logger::getGlobalInstance().log<level>(message, arg, std::forward<Args>(args)...);
}

template <typename T>
void debug(const T& message) { log<LVL_DEBUG>(message); }

template <typename T, typename... Args>
void debug(const String& message, const T& arg, Args&&... args)
{
    log<LVL_DEBUG>(message, arg, std::forward<Args>(args)...);
}

template <typename T>
void info(const T& message) { log<LVL_INFO>(message); }

template <typename T, typename... Args>
void info(const String& message, const T& arg, Args&&... args)
{
    log<LVL_INFO>(message, arg, std::forward<Args>(args)...);
}

template <typename T>
void warning(const T& message) { log<LVL_WARNING>(message); }

template <typename T, typename... Args>
void warning(const String& message, const T& arg, Args&&... args)
{
    log<LVL_WARNING>(message, arg, std::forward<Args>(args)...);
}

template <typename T>
void error(const T& message) { log<LVL_ERROR>(message); }

template <typename T, typename... Args>
void error(const String& message, const T& arg, Args&&... args)
{
    log<LVL_ERROR>(message, arg, std::forward<Args>(args)...);
}

template <typename T>
void fatal(const T& message) { log<LVL_FATAL>(message); }

template <typename T, typename... Args>
void fatal(const String& message, const T& arg, Args&&... args)
{
    log<LVL_FATAL>(message, arg, std::forward<Args>(args)...);
}

} // namespace mlog

#endif // !MINILOG_HPP
