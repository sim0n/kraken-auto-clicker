#pragma once

#include "clicker.h"

namespace kraken
{
    class c_kraken
    {
        static c_kraken* m_instance;

        explicit c_kraken(std::unique_ptr<c_clicker>&& clicker);

    public:
        std::unique_ptr<c_clicker> m_clicker;

        static void initialize(std::unique_ptr<c_clicker>&& clicker, const std::function<void(c_kraken&)>& post_initialization_func);

        static c_kraken& get_instance();
    };

}
