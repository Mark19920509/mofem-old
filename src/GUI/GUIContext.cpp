#include "GUIContext.h"

#include <SFGUI/SFGUI.hpp>
#include <string>
#include <future>
#include <iostream>

namespace GUI{

    Status init(Context& context, unsigned int w, unsigned int h, std::string title){
        context.render_window.create(sf::VideoMode(w, h), title);
        return Status::SUCCESS;
    }

    Status start(Context& context){
        auto& sfgui = context.sfgui;
        auto& render_window = context.render_window;
        auto& desktop = context.desktop;

        sf::Event event;
        sf::Clock clock;
        while (render_window.isOpen()) {

            // Event processing.
            while (render_window.pollEvent(event)) {
                desktop.HandleEvent(event);
                // If window is about to be closed, leave program.
                if (event.type == sf::Event::Closed) {
                    render_window.close();
                }
            }
            // Update SFGUI with elapsed seconds since last call.
            desktop.Update(clock.restart().asSeconds());
            // Rendering.
            render_window.clear();
            sfgui.Display(render_window);
            render_window.display();
        }

        return Status::SUCCESS;
    }

}