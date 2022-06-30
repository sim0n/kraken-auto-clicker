#pragma once

#include <functional>
#include <memory>
#include <vector>
#include <windows.h>

#include "sdk.h"

namespace kraken
{
    class c_clicks
    {
    public:
        std::vector<std::pair<int, int>> m_click_delays;

    	c_clicks(std::vector<std::pair<int, int>> click_delays)
    		: m_click_delays(std::move(click_delays))
        {}
    };

    class c_click_container
    {
    public:
        std::vector<std::pair<click_type_t, std::vector<std::pair<int, int>>>> m_clicks;
    };

    class c_clicker
    {
    protected:
        click_type_t m_click_type;

        std::function<void(int)> m_double_click_prevent_func;
        std::function<void(HWND, std::pair<int, int>)> m_iterative_dc_prevent_func;

        std::function<void(HWND, int)> m_click_press_callback;
        std::function<void(HWND, int)> m_click_release_callback;

        int m_extra_random_delay = 0;

    public:
        static bool running;
        static bool mouse_down;

        static HHOOK mouse_hook;
        static HHOOK keyboard_hook;

    	c_click_container m_click_container;
        LPCWSTR m_class_title;

        c_clicker(const click_type_t click_type, c_click_container click_container, LPCWSTR class_title)
            : m_click_type(click_type), m_click_container(std::move(click_container)), m_class_title(class_title)
        {}

    	auto run() -> void;

        auto register_clicks(click_type_t click_type, const c_clicks& clicks) -> void;

        auto register_double_click_prevent_func(std::function<void(int)> dc_prevent_callback) -> void;
        auto register_iterative_double_click_prevent_callback(std::function<void(HWND, std::pair<int, int>)> dc_prevent_callback) -> void;

        auto register_click_callback(int mode, std::function<void(HWND, int)> click_callback) -> void;

        auto send_click(HWND window, UINT msg) -> void;

        auto set_extra_random_delay(const int delay) -> void
        {
            m_extra_random_delay = delay;
        }

        auto get_extra_random_delay() const -> int
        {
            return m_extra_random_delay;
        }

        static LRESULT CALLBACK handle_keyboard_hook(int code, WPARAM wparam, LPARAM lparam);

        static LRESULT CALLBACK handle_mouse_hook(int code, WPARAM wparam, LPARAM lparam);

        static auto toggle() -> void;
    };

    typedef std::shared_ptr<c_clicker> clicker_ptr;
}
