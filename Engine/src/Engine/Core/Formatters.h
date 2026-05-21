#pragma once

#include <glm/glm.hpp>
#include <spdlog/fmt/fmt.h>


template<>
struct fmt::formatter<glm::vec2>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const glm::vec2& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {})", value.x, value.y);
    }
};

template<>
struct fmt::formatter<glm::vec3>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const glm::vec3& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {}, {})", value.x, value.y, value.z);
    }
};

template<>
struct fmt::formatter<glm::ivec2>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const glm::ivec2& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {})", value.x, value.y);
    }
};

template<>
struct fmt::formatter<glm::ivec3>
{
    constexpr auto parse(format_parse_context& ctx)
    {
        return ctx.begin();
    }

    template<typename FormatContext>
    auto format(const glm::ivec3& value, FormatContext& ctx) const
    {
        return fmt::format_to(ctx.out(), "({}, {}, {})", value.x, value.y, value.z);
    }
};