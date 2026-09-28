#include <iostream>
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>
#include <random>
#include <map>
#include <chrono>
#include <thread>

//using namespace std;

const long long MAX_STEPS = 100000;
const long long MAX_X = 675;// 3500; (test)
const long long MAX_Y = 320;// 1700; (test)
const short POS_N = 5;

const long long PIXEL_S = 4; //5; (test)
const long long WIND_S_X = MAX_X*PIXEL_S + 3*PIXEL_S; //1250; (test)
const long long WIND_S_Y = MAX_Y*PIXEL_S + 25*PIXEL_S;

const short S_V = 1; //начальное значение в циклах for перебора поля

void wait(int w) {
    std::this_thread::sleep_for(std::chrono::milliseconds(w));
}

void t_value(int t, short& t1, short& t2) {
    if (t == 1) {
        t1++;
    }
    else if (t == 2) {
        t2++;
    }
}

bool print_space(std::vector<std::vector<int>>& space, const bool& is_rand, const int& i, sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& square, std::mt19937& gen, std::uniform_int_distribution<int>& dist) {
    unsigned int t1v = 0;
    unsigned int t2v = 0;

    float add_pix = 0;
    if (PIXEL_S % 2 == 0) {
        add_pix = (PIXEL_S / 2.0) + 0.5;
    }
    else {
        add_pix = (PIXEL_S + 1.0) / 2.0;
    }

    window.clear();

    for (int y = S_V; y < MAX_Y; y++) {
        for (int x = S_V; x < MAX_X; x++) {
            if (is_rand == 1) {
                if (x >= MAX_X / POS_N && x <= MAX_X - (MAX_X / POS_N) && y >= MAX_Y / POS_N && y <= MAX_Y - (MAX_Y / POS_N)) {
                    space[y][x] = dist(gen);
                }
                //space[y][x] = dist(gen);
            }
            if (space[y][x] == 1) {
                t1v++;
            }
            else if (space[y][x] == 2) {
                t2v++;
            }

            if (space[y][x] != 0) {
                square.setPosition({ (float)x * PIXEL_S + add_pix, (float)y * PIXEL_S + add_pix });
                if (space[y][x] == 2) square.setFillColor(sf::Color::Red);
                else square.setFillColor(sf::Color::White);
                window.draw(square);
            }
        }
    }
    //cout << "Step: " << i << endl;
    //cout << "White: " << t1v << endl;
    //cout << "Red: " << t2v << endl;

    sf::Text txt(font);
    txt.setString("Step: " + std::to_string(i));
    txt.setPosition({ 0.f, ((float)MAX_Y) * PIXEL_S + add_pix });
    window.draw(txt);
    txt.setString("White: " + std::to_string(t1v));
    txt.setPosition({ 0.f, ((float)MAX_Y + 6) * PIXEL_S + add_pix });
    window.draw(txt);
    txt.setString("Red: " + std::to_string(t2v));
    txt.setPosition({ 0.f, ((float)MAX_Y + 12) * PIXEL_S + add_pix });
    window.draw(txt);

    window.display();
    if (t1v == 0 && t2v == 0) {
        return 1;
    }
    return 0;
}

void check_rules(short& t1, short& t2, std::vector<std::vector<int>>& space, std::vector<std::vector<int>>& space2, int y, int x, short t) {
    if (t2 == 0 && t1 < 8) {
        space2[y][x] = 0;
    }
    else {
        if (space[y][x] == 0) {
            if (t1 >= 2) { //birth
                space2[y][x] = t;
            }
            else { //added for release mode(test)
                space2[y][x] = 0;
            }
        }
        else { //eating
            space2[y][x] = t;
        }
    }
}

int main() {
    std::cout << "-> The Game Of Civilizations <-\n" << "by the Ch \n" << "=======================================\n" << "-> Spaces In Spawn? (0 or 1): ";
    bool no_spaces_in_spawn = 1; std::cin >> no_spaces_in_spawn;
    std::cout << std::endl << "Seed (-1 if not): ";
    int seed = 0; std::cin >> seed; //seed 0 is successful for (675x, 320y) (test)
    std::cout << "\n";

    sf::RenderWindow window( //window
        sf::VideoMode({WIND_S_X, WIND_S_Y}), 
        "The Game Of Civilizations",
        sf::Style::Titlebar | sf::Style::Close
    );
    sf::Font font;
    if (!font.openFromFile("C:/Windows/Fonts/arial.ttf")) {
        std::cerr << "Failed to load font \n";
        return -1;
    }
    sf::RectangleShape square(sf::Vector2((float)PIXEL_S, (float)PIXEL_S)); //pixel-square

    if (seed == -1) {
        std::random_device rd; seed = rd(); //random start
    }
    std::cout << "Seed: " << seed << std::endl;
    std::mt19937 gen(seed++);
    std::uniform_int_distribution<int> dist(no_spaces_in_spawn, 2);

    std::vector<std::vector<int>> space(MAX_Y + 1, std::vector<int>(MAX_X + 1, 0));
    std::vector<std::vector<int>> space2(MAX_Y + 1, std::vector<int>(MAX_X + 1, 0));
    print_space(space, 1, 0, window, font, square, gen, dist);
    space2 = space;

    /* old
    while (window.isOpen()) {
        if (const optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                break;
            }
        }
    }
    */

    //wait(3000);

    for (long long i = 0; i < MAX_STEPS; i++) {

        while (const std::optional event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
                return 0;
            }
            if (const auto* key = event->getIf<sf::Event::KeyPressed>()) {
                if (key->code == sf::Keyboard::Key::Escape) {
                    window.close();
                    return 0;
                }
            }
        }

        wait(1);

        if (print_space(space, 0, i, window, font, square, gen, dist)) {
            break;
        }
        for (int y = S_V; y < MAX_Y; y++) {
            for (int x = S_V; x < MAX_X; x++) {

                short t1 = 0;
                short t2 = 0;

                t_value(space[y][x - 1], t1, t2);
                t_value(space[y - 1][x - 1], t1, t2);
                t_value(space[y - 1][x], t1, t2);
                t_value(space[y - 1][x + 1], t1, t2);
                t_value(space[y][x + 1], t1, t2);
                t_value(space[y + 1][x + 1], t1, t2);
                t_value(space[y + 1][x], t1, t2);
                t_value(space[y + 1][x - 1], t1, t2);

                if (t1 == t2 && t1 == 0) {
                    space2[y][x] = 0;
                }
                else if (t1 > t2) {
                    check_rules(t1, t2, space, space2, y, x, 1);
                }
                else if (t2 > t1) {
                    check_rules(t2, t1, space, space2, y, x, 2);
                }
            }
        }
        space = space2;
    }

    window.close();
    return 0;
}
