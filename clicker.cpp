#include <utility>
#include <vector>
#include "clicker.h"

#include <chrono>
#include <functional>
#include <optional>
#include <thread>

#include "lazy_importer.hpp"

namespace kraken
{
	bool c_clicker::running = false;
	bool c_clicker::mouse_down = false;

	HHOOK c_clicker::mouse_hook = nullptr;
	HHOOK c_clicker::keyboard_hook = nullptr;

	/**
	 * Registers specific clicks for a click type
	 * @param click_type The click type to register clicks to
	 * @param clicks The clicks to register
	 */
	auto c_clicker::register_clicks(click_type_t click_type, const c_clicks& clicks) -> void
	{
		m_click_container.m_clicks.emplace_back(click_type, clicks.m_click_delays);
	}

	/**
	 * registers the double click prevention callback that will be called when not clicking
	 * @param dc_prevent_callback The double click prevent callback to register
	 */
	auto c_clicker::register_double_click_prevent_func(std::function<void(int)> dc_prevent_callback) -> void
	{
		m_double_click_prevent_func = std::move(dc_prevent_callback);
	}

	/**
	 * Registers an iterative double click prevention function that will be called on first click
	 * @param dc_prevent_callback The double click prevent callback to register
	 */
	auto c_clicker::register_iterative_double_click_prevent_callback(std::function<void(HWND, std::pair<int, int>)> dc_prevent_callback) -> void
	{
		m_iterative_dc_prevent_func = std::move(dc_prevent_callback);
	}

	/**
	 * Registers a specific click callback for either DOWN or UP
	 *
	 * @param mode The click mode
	 * @param click_callback The click callback
	 */
	auto c_clicker::register_click_callback(int mode, std::function<void(HWND, int)> click_callback) -> void
	{
		switch (mode)
		{
		case DOWN:
			m_click_press_callback = std::move(click_callback);
			break;

		case UP:
			m_click_release_callback = std::move(click_callback);
			break;
		}
	}

	/**
	 * Sends a click to a window
	 *
	 * @param window The window to send a click to
	 * @param msg The click message to send
	 */
	void c_clicker::send_click(const HWND window, const UINT msg)
	{
		POINT pos;
		GetCursorPos(&pos);

		LFN(SendMessage, window, msg, 0, MAKELPARAM(pos.x, pos.y));
	}

	auto c_clicker::run() -> void
	{
		auto& delays = m_click_container.m_clicks.at(m_click_type).second;

		while (true)
		{
			// prevent double clicking
			m_double_click_prevent_func(50);

			const auto window = FindWindow(m_class_title, nullptr);

			// only click in desired window
			if (GetForegroundWindow() != window)
				continue;

			auto clicks = 0;

			for (auto& [click_delay, release_delay] : delays)
			{
				if (!running || !mouse_down)
				{
					break;
				}

				// prevent double clicking
				if (clicks++ == 0)
				{
					m_iterative_dc_prevent_func(window, { click_delay, release_delay });
					continue;
				}

				m_click_press_callback(window, click_delay);
				m_click_release_callback(window, release_delay);
			}
		}
	}

	LRESULT CALLBACK c_clicker::handle_keyboard_hook(int code, WPARAM wparam, LPARAM lparam)
	{
		if (code == HC_ACTION && wparam == WM_KEYDOWN)
		{
			const auto* keyboard = reinterpret_cast<PKBDLLHOOKSTRUCT>(lparam);

			if (keyboard->flags == 0) {
				switch (keyboard->vkCode)
				{
					// TODO: make changeable
				case VK_OEM_3:
					toggle();
					break;
				}
			}
		}

		return CallNextHookEx(keyboard_hook, code, wparam, lparam);
	}

	LRESULT c_clicker::handle_mouse_hook(int code, WPARAM wparam, LPARAM lparam)
	{
		if (wparam != WM_MOUSEMOVE)
		{
			const auto* mouse = reinterpret_cast<PMSLLHOOKSTRUCT>(lparam);

			if (mouse->flags == 0)
			{
				switch (wparam)
				{
				case DOWN:
					mouse_down = true;
					break;

				case UP:
					mouse_down = false;
					break;
				}
			}
		}

		return CallNextHookEx(mouse_hook, code, wparam, lparam);
	}

	auto c_clicker::toggle() -> void
	{
		running = !running;
	}
}
