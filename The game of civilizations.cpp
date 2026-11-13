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

const long long MAX_X = 675; //(675 -> base test) //(3500 -> stress test for pixel size = 1)
const long long MAX_Y = 320; //(320 -> base test) //(1700 -> stress test for pixel size = 1)

const long long PIXEL_S = 3; //Pixel Size

const long long WIND_S_X = MAX_X * PIXEL_S + 3 * PIXEL_S;
const long long WIND_S_Y = MAX_Y * PIXEL_S + 25 * PIXEL_S;

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

bool print_space(std::vector<std::vector<int>>& space, const bool& is_rand, const int& i, const short& pos_f, sf::RenderWindow& window, sf::Font& font, sf::RectangleShape& square, std::mt19937& gen, std::uniform_int_distribution<int>& dist) {
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
                if (pos_f == 0) space[y][x] = dist(gen);
                else if (pos_f != 0 && x >= MAX_X / pos_f && x <= MAX_X - (MAX_X / pos_f) && y >= MAX_Y / pos_f && y <= MAX_Y - (MAX_Y / pos_f)) {
                    space[y][x] = dist(gen);
                }
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
    txt.setPosition({ 0.f, ((float)MAX_Y + 8) * PIXEL_S + add_pix });
    window.draw(txt);
    txt.setString("Red: " + std::to_string(t2v));
    txt.setPosition({ 0.f, ((float)MAX_Y + 16) * PIXEL_S + add_pix });
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
        }
        else { //eating
            space2[y][x] = t;
        }
    }
}

int main() {
    std::cout << ">> The Game Of Civilizations <<" << std::endl << "by Chekeld"<< std::endl << std::endl << "=======================================" << std::endl << std::endl;

    /* test
    long long max_x = 0;
    long long max_y = 0;
    std::cout << "Max X Pixel (0 if not): ";
    std::cin >> max_x;
    if (max_x <= 0) {
        max_x = 675; //(675 -> base test) //(3500 -> stress test for pixel size = 1)
    }
    std::cout << "Max Y Pixel (0 if not): ";
    std::cin >> max_y;
    if (max_y <= 0) {
        max_y = 320; //(320 -> base test) //(1700 -> stress test for pixel size = 1)
    }

    long long MAX_X = max_x;
    long long MAX_Y = max_y;
    long long WIND_S_X = MAX_X * PIXEL_S + 3 * PIXEL_S;
    long long WIND_S_Y = MAX_Y * PIXEL_S + 25 * PIXEL_S;
    */

    int seed = 0;
    short pos_f = 0;

    std::cout << "Is Random Test (1 - true or 0 - false): ";
    bool rand_var; std::cin >> rand_var;
    bool is_test_structure = 0;
    if (rand_var) {
        seed = -1;
    }
    else {
        std::cout << "Create a test structure (1 - true or 0 - false): ";
        std::cin >> is_test_structure;

        if (!is_test_structure) {
            std::cout << "Seed (-1 if not): ";
            std::cin >> seed;
            std::cout << "Position Factor (0 if not): ";
            std::cin >> pos_f;
        }
    }

    if (pos_f < 0) pos_f = 0;
    else if (pos_f > 10) pos_f = 10;
    else if (pos_f != 0 && pos_f < 3) pos_f = 3;

    bool no_spaces_in_spawn = 0;
    if(pos_f != 0) no_spaces_in_spawn = 1;

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
    std::cout << std::endl << "=======================================" << std::endl << std::endl << "Seed: " << seed << std::endl << "Pixels: x: " << MAX_X << " y: " << MAX_Y << std::endl << "Position Factor: " << pos_f << std::endl;
    std::mt19937 gen(seed++);
    std::uniform_int_distribution<int> dist(no_spaces_in_spawn, 2);

    std::vector<std::vector<int>> space(MAX_Y + 1, std::vector<int>(MAX_X + 1, 0));
    std::vector<std::vector<int>> space2(MAX_Y + 1, std::vector<int>(MAX_X + 1, 0));

    if (!is_test_structure) {
        print_space(space, 1, 0, pos_f, window, font, square, gen, dist);
    }
    else {
        space[MAX_Y / 2][MAX_X / 2] = 1; space[MAX_Y / 2][MAX_X / 2 + 1] = 2; space[MAX_Y / 2][MAX_X / 2 + 2] = 2; space[MAX_Y / 2][MAX_X / 2 + 3] = 2; space[MAX_Y / 2][MAX_X / 2 + 4] = 1;
        space[MAX_Y / 2 - 1][MAX_X / 2] = 2; space[MAX_Y / 2 - 1][MAX_X / 2 + 1] = 1; space[MAX_Y / 2 - 1][MAX_X / 2 + 2] = 1; space[MAX_Y / 2 - 1][MAX_X / 2 + 3] = 1; space[MAX_Y / 2 - 1][MAX_X / 2 + 4] = 2;
    }

    space2 = space;
    
    for (int i = 0; i < 120; i++) { //delay
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
    }

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

        if (print_space(space, 0, i, pos_f, window, font, square, gen, dist)) {
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
