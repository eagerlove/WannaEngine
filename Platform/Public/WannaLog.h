// 日志头文件

#ifndef WANNA_LOG_H
#define WANNA_LOG_H

#include "WannaEngine.h"

#include "spdlog/common.h"

namespace WannaEngine{
    class WannaLog{
        public:
            // 移除WannaLog 避免外层new
            WannaLog() = delete;
            WannaLog(const WannaLog&) = delete;
            WannaLog &operator=(const WannaLog&) = delete;

            static void Init();
            static WannaLog* GetLoggerInstance(){
                return &sLoggerInstance;
            }
            template<typename... Args>
            void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, spdlog::format_string_t<Args...> fmt, Args &&...args) {
                spdlog::memory_buf_t buf;
                fmt::vformat_to(fmt::appender(buf), fmt, fmt::make_format_args(args...));
                Log(loc, lvl, buf);
            }
        private:
            void Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t &buffer);
            static WannaLog sLoggerInstance;
    };

#define WANNA_LOG_LOGGER_CALL(WannaLog, level, ...) \
    (WannaLog)->Log(spdlog::source_loc{__FILE__, __LINE__, SPDLOG_FUNCTION}, level, __VA_ARGS__)

#define LOG_TRACE(...)   WANNA_LOG_LOGGER_CALL(WannaEngine::WannaLog::GetLoggerInstance(), spdlog::level::trace, __VA_ARGS__)
#define LOG_DEBUG(...)   WANNA_LOG_LOGGER_CALL(WannaEngine::WannaLog::GetLoggerInstance(), spdlog::level::debug, __VA_ARGS__)
#define LOG_INFO(...)    WANNA_LOG_LOGGER_CALL(WannaEngine::WannaLog::GetLoggerInstance(), spdlog::level::info, __VA_ARGS__)
#define LOG_WARNING(...) WANNA_LOG_LOGGER_CALL(WannaEngine::WannaLog::GetLoggerInstance(), spdlog::level::warn, __VA_ARGS__)
#define LOG_ERROR(...)   WANNA_LOG_LOGGER_CALL(WannaEngine::WannaLog::GetLoggerInstance(), spdlog::level::err, __VA_ARGS__)

}

#endif