//
// Created by User on 9/29/2025.
//

#ifndef LEARNVULKANRAII_UTILS_H
#define LEARNVULKANRAII_UTILS_H

#include <memory>
#include <optional>
#include <print>
#include <functional>

namespace LearnVulkanRAII::Utils
{
    template <typename T>
    using Unique = std::unique_ptr<T>;

    template <typename T>
    using Shared = std::shared_ptr<T>;

    template <typename T>
    using Weak = std::weak_ptr<T>;

    template <typename T>
    using Optional = std::optional<T>;

    inline std::nullopt_t NullOptional = std::nullopt;

    template<typename T, typename ... Args>
    Unique<T> makeUnique(Args... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T, typename ... Args>
    Shared<T> makeShared(Args... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }
}

#define LOG(fmt, ...) std::println(fmt, ##__VA_ARGS__)
#define ASSERT_MSG(condition, msg)                                                                                  \
    do                                                                                                              \
    {                                                                                                               \
        if (!(condition))                                                                                           \
        {                                                                                                           \
            LOG("Assertion failed({}): {}. FILE: {}, LINE: {}", #condition, msg, __FILE__, __LINE__);               \
        }                                                                                                           \
    } while (false)

#define ASSERT(condition, msg) ASSERT_MSG(condition, msg)

#define DEFINE_SMART_POINTER_TYPES(className) \
    using Unique = Utils::Unique<className>; \
    using Shared = Utils::Shared<className>; \
    using Weak = Utils::Weak<className>; \
    using Optional = Utils::Optional<className>;

#define DEFINE_SMART_POINTER_FUNCS(className) \
    template <typename ... Args> \
    static Utils::Shared<className> makeShared(Args&& ... args) \
    { return Utils::makeShared<className, Args...>(args...); } \
    template <typename ... Args> \
    static Utils::Unique<className> makeUnique(Args&& ... args) \
    { return Utils::makeUnique<className, Args...>(args...); }

#define DEFINE_SMART_POINTER_HELPERS(className) \
    DEFINE_SMART_POINTER_TYPES(className) \
    DEFINE_SMART_POINTER_FUNCS(className)

#define BIT(x) (1 << x)

#define EVENT_FN(func) std::bind(func, this, std::placeholders::_1)

#endif //LEARNVULKANRAII_UTILS_H