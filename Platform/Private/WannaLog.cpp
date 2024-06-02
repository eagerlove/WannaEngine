#include "WannaLog.h"
#include "spdlog/async.h"
#include "spdlog/sinks/stdout_color_sinks.h"


namespace WannaEngine{
    WannaLog WannaLog::sLoggerInstance{};
    static std::shared_ptr<spdlog::logger> sLogger{};

    void WannaLog::Init() {
        // 异步控制台打印输出
        sLogger = spdlog::stdout_color_mt<spdlog::async_factory>("async_logger");
        // 日志等级: trace
        sLogger->set_level(spdlog::level::trace);
        // 日志模板
        sLogger->set_pattern("%^%H:%M:%S:%e [%P-%t] [%1!L] [%20s:%-4#] - %v%$");
    }

    void WannaLog::Log(spdlog::source_loc loc, spdlog::level::level_enum lvl, const spdlog::memory_buf_t &buffer) {
        sLogger->log(loc, lvl, spdlog::string_view_t(buffer.data(), buffer.size()));
    }
};