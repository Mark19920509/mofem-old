#pragma once

#include <Util/Status.h>
#include <SFGUI/SFGUI.hpp>

namespace GUI{
    struct Context{
        sfg::SFGUI sfgui;
        sfg::Desktop desktop;
        sf::RenderWindow render_window;
    };

    Status init(Context& context, unsigned int w, unsigned int h, std::string title);
    Status start(Context& context);
}