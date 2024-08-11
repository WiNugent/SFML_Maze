

#include "functions.hpp"




int main() {

    // create the window
    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");

    readGrid();
    
    sf::RectangleShape box(sf::Vector2f(window.getSize().x / maxSize - PMARGIN, window.getSize().y / maxSize - PMARGIN));
    box.setFillColor(sf::Color::White);


    

    // clear the window with black color
    window.clear(sf::Color::Black);

    getAstar(window, box);


    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}
