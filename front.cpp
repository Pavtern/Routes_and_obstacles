#include <SFML/Graphics.hpp>
#include <vector>
#include <iostream>
#include "back.h"

const int WINDOW_SIZE = 800;
const double MIN_LAT = 48.980, MAX_LAT = 48.990;
const double MIN_LON = 8.390, MAX_LON = 8.410;

sf::Vector2f to_screen(double lat, double lon) {
    float x = (lon - MIN_LON) / (MAX_LON - MIN_LON) * WINDOW_SIZE;
    float y = (lat - MIN_LAT) / (MAX_LAT - MIN_LAT) * WINDOW_SIZE;
    return {x, y};
}

coordinates from_screen(float x, float y) {
    double lon = MIN_LON + (x / WINDOW_SIZE) * (MAX_LON - MIN_LON);
    double lat = MIN_LAT + (y / WINDOW_SIZE) * (MAX_LAT - MIN_LAT);
    return {lat, lon};
}

// Helper: Draw background for text
void draw_text_background(sf::RenderWindow& window) {
    sf::RectangleShape bg(sf::Vector2f(WINDOW_SIZE, 60));
    bg.setFillColor(sf::Color::White);
    bg.setPosition(0, WINDOW_SIZE - 60);
    window.draw(bg);
}

void draw_route(sf::RenderWindow& window, const std::vector<coordinates>& route, sf::Color color) {
    sf::VertexArray lines(sf::LineStrip, route.size());
    for (size_t i = 0; i < route.size(); ++i) {
        sf::Vector2f point = to_screen(route[i].latitude, route[i].longitude);
        lines[i].position = point;
        lines[i].color = color;

        sf::CircleShape step_circle(3);
        step_circle.setFillColor(color);
        step_circle.setPosition(point.x - 3, point.y - 3);
        window.draw(step_circle);
    }
    window.draw(lines);
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_SIZE, WINDOW_SIZE), "Route Planner");

    bool selecting_start = true, selecting_end = false, rerouting = false;
    coordinates start_point(0, 0), end_point(0, 0);
    std::vector<coordinates> obstacles = generate_obstacles(10, {});
    std::vector<coordinates> route;
    double total_distance = 0;
    std::string status = "Select the start point";

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();

            if (event.type == sf::Event::MouseButtonPressed) {
                if (selecting_start) {
                    start_point = from_screen(event.mouseButton.x, event.mouseButton.y);
                    selecting_start = false;
                    selecting_end = true;
                    status = "Select the end point";
                } else if (selecting_end) {
                    end_point = from_screen(event.mouseButton.x, event.mouseButton.y);
                    selecting_end = false;
                    route = interim_coo(start_point, end_point, 5);

                    total_distance = 0;
                    for (size_t i = 1; i < route.size(); ++i) {
                        total_distance += gps_distance(route[i - 1], route[i]);
                    }

                    if (is_route_valid(route, obstacles)) {
                        status = "Route built!";
                    } else {
                        status = "Route contains obstacles. Press R to reroute.";
                    }
                }
            }

            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::R) {
                if (!is_route_valid(route, obstacles)) {
                    route = reroute(start_point, end_point, obstacles);
                    rerouting = true;

                    total_distance = 0;
                    for (size_t i = 1; i < route.size(); ++i) {
                        total_distance += gps_distance(route[i - 1], route[i]);
                    }

                    status = route.empty() ? "Failed to reroute." : "Route rerouted!";
                }
            }
        }

        window.clear(sf::Color::White);

        for (const auto& obstacle : obstacles) {
            sf::CircleShape circle(5);
            circle.setFillColor(sf::Color::Red);
            circle.setPosition(to_screen(obstacle.latitude, obstacle.longitude));
            window.draw(circle);
        }

        if (!selecting_start) {
            sf::CircleShape start(8);
            start.setFillColor(sf::Color::Green);
            start.setPosition(to_screen(start_point.latitude, start_point.longitude));
            window.draw(start);
        }
        if (!selecting_end) {
            sf::CircleShape end(8);
            end.setFillColor(sf::Color::Blue);
            end.setPosition(to_screen(end_point.latitude, end_point.longitude));
            window.draw(end);
        }

        if (!route.empty()) {
            sf::Color route_color = rerouting ? sf::Color::Cyan : (is_route_valid(route, obstacles) ? sf::Color::Black : sf::Color::Yellow);
            draw_route(window, route, route_color);
        }

        draw_text_background(window);  // Clear text area

        sf::Font font;
        if (font.loadFromFile("C:/Windows/Fonts/arial.ttf")) {
            sf::Text status_text(status, font, 20);
            status_text.setFillColor(sf::Color::Black);
            status_text.setPosition(10, WINDOW_SIZE - 50);
            window.draw(status_text);

            if (!route.empty()) {
                std::string distance_info = "Total distance: " + std::to_string(total_distance) + " meters.";
                sf::Text distance_text(distance_info, font, 20);
                distance_text.setFillColor(sf::Color::Black);
                distance_text.setPosition(10, WINDOW_SIZE - 30);
                window.draw(distance_text);
            }
        }

        window.display();
    }
    return 0;
}