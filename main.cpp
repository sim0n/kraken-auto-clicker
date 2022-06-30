#include <algorithm>
#include <cstdio>
#include <functional>
#include <optional>
#include <thread>

#include "Windows.h"

#include "kraken.h"
#include "lazy_importer.hpp"
#include "recorded_clicks.h"
#include "xorstr.hpp"

using namespace::kraken;

auto initialize_console() -> void
{
	HWND console_hwnd{ LFN_(GetConsoleWindow)() };

	LFN(SetConsoleTitleA, xorstr_("krak3n"));
	LFN(SetLayeredWindowAttributes, console_hwnd, 0, 230, LWA_ALPHA);
	LFN(SetWindowLong, console_hwnd, GWL_STYLE, GetWindowLong(console_hwnd, GWL_STYLE) | WS_BORDER);
	LFN(SetWindowLong, console_hwnd, GWL_EXSTYLE, GetWindowLong(console_hwnd, GWL_STYLE) | WS_EX_LAYERED);
	//LFN(ShowScrollBar, console_hwnd, SB_VERT, false);
}

auto initialize_clicker(const click_type_t click_type, LPCWSTR class_title) -> c_clicker
{
	const c_click_container click_container;

    return { click_type, click_container, class_title };
}

auto initialize_kraken(const c_clicker& clicker) -> void
{
	// setup actual application
	c_kraken::initialize(std::make_unique<c_clicker>(clicker), [](const c_kraken& app)
		{
			const auto println = [](const HANDLE handle, const WORD color, char const* text)
			{
				SetConsoleTextAttribute(handle, color);
				LFN(printf, text);
			};

			const auto get_running_text = [](const bool running) -> std::string
			{
				std::string text = xorstr_("status: ");
				text += running ? xorstr_("ON") : xorstr_("OFF");

				return text;
			};

			std::vector<std::thread> threads;

			// clicker thread
			threads.emplace_back([&]
				{
					app.m_clicker->run();
				});

			// hook thread
			threads.emplace_back([]
				{
					c_clicker::keyboard_hook = LFN(SetWindowsHookEx, WH_KEYBOARD_LL, c_clicker::handle_keyboard_hook, nullptr, 0);
					c_clicker::mouse_hook = LFN(SetWindowsHookEx, WH_MOUSE_LL, c_clicker::handle_mouse_hook, nullptr, 0);

					MSG msg;
					while (GetMessage(&msg, nullptr, 0, 0)) {
						TranslateMessage(&msg);
						DispatchMessage(&msg);
					}
				});

			// console thread
			threads.emplace_back([&]
				{
					while (true)
					{
						const auto handle = GetStdHandle(STD_OUTPUT_HANDLE);

						LFN(system, xorstr_("cls"));

						println(handle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY, xorstr_("kraken clicker \n\n"));
						println(handle, FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY, get_running_text(c_clicker::running).c_str());

						std::this_thread::sleep_for(std::chrono::milliseconds(150));
					}
				});

			// detach every thread so they don't terminate
			std::for_each(threads.begin(), threads.end(), [](std::thread& thread)
				{
					thread.detach();
				});
		});

	const auto& app = c_kraken::get_instance();
}

int main()
{
	const auto get_click_type = []() -> std::optional<click_type_t>
	{
		if (recorded_clicks::butterfly_click_delays.empty() && recorded_clicks::jitter_click_delays.empty())
			return generated;

		return std::nullopt;
	};

	const auto class_title = L"LWJGL";

	// TODO: implement a gui
	initialize_console();
	initialize_kraken(initialize_clicker(get_click_type().value_or(recorded_clicks::butterfly_click_delays.empty() ? jitter : butterfly), class_title));

	while (true)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}

    return 0;
}
