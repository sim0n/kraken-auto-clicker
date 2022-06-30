#include <functional>
#include "recorded_clicks.h"
#include "kraken.h"

#include <optional>
#include <string>
#include <thread>

#include "math.h"

namespace kraken
{
    c_kraken* c_kraken::m_instance = nullptr;

    c_kraken::c_kraken(std::unique_ptr<c_clicker>&& clicker)
		: m_clicker(std::move(clicker))
    {

    }

    void c_kraken::initialize(std::unique_ptr<c_clicker>&& clicker, const std::function<void(c_kraken&)>& post_initialization_func) {
        m_instance = new c_kraken(std::move(clicker));

        std::vector<std::pair<int, int>> generated_delays;

        const auto generate_random_delays = [&]
        {
            for (int i = 0; i < 700; i++)
            {
                auto click_delay = math::get_random<int>(35, 80);
                auto release_delay = math::get_random<int>(20, 35);

                if (math::get_random<float>(0.f, 1.f) > 0.02f)
                {
                    click_delay *= math::get_random<float>(1.25f, 2.f);
                }


                if (math::get_random<float>(0.f, 1.f) > 0.05f)
                {
                    release_delay *= math::get_random<float>(1.15f, 2.3f);
                }

                generated_delays.emplace_back(std::pair(click_delay, release_delay));
            }
        };

        do
        {
            generated_delays.clear();
            generate_random_delays();
        } while (math::get_st_dev(generated_delays) / 50.f < 0.5f);

        // std::optional<std::vector<std::pair<int, int>>> delays = generated_delays;

        m_instance->m_clicker->register_clicks(butterfly, { recorded_clicks::butterfly_click_delays });
        m_instance->m_clicker->register_clicks(jitter, { recorded_clicks::jitter_click_delays });
        m_instance->m_clicker->register_clicks(generated, { generated_delays });

        // TODO: 
        const std::optional<std::function<void(HWND, std::pair<int, int>)>> double_click_prevent = std::nullopt;

        // TODO: make changeable
        m_instance->m_clicker->set_extra_random_delay(30);

        m_instance->m_clicker->register_double_click_prevent_func([](const int delay)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
            });

        m_instance->m_clicker->register_iterative_double_click_prevent_callback(double_click_prevent.value_or([](HWND window, std::pair<int, int> delays)
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(math::get_random<int>(30, 50)));
                m_instance->m_clicker->send_click(window, UP);
            }));

        m_instance->m_clicker->register_click_callback(DOWN, [](HWND window, const int click_delay)
            {
	            const auto extra_random_delay = m_instance->m_clicker->get_extra_random_delay();
                const auto delay = max(5, click_delay - math::get_random<int>(10 + extra_random_delay, 15 + extra_random_delay));

                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                m_instance->m_clicker->send_click(window, DOWN);
            });

        m_instance->m_clicker->register_click_callback(UP, [](HWND window, const int click_delay)
            {
                const auto extra_random_delay = m_instance->m_clicker->get_extra_random_delay();
                const auto delay = max(5, click_delay - math::get_random<int>(5 + extra_random_delay, 10 + extra_random_delay));

                std::this_thread::sleep_for(std::chrono::milliseconds(delay));
                m_instance->m_clicker->send_click(window, UP);
            });

        post_initialization_func(*m_instance);
    }

    c_kraken& c_kraken::get_instance()
    {
        return *m_instance;
    }
}
