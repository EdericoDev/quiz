#ifndef WELCOME_H
#define WELCOME_H

#include <iostream>
#include <string>
#include <random>
#include <vector>
#include <chrono>

struct Star {
    int x, y;
    char symbol;
};

void displayWelcome() {
    // Dimensioni schermo
    const int WIDTH = 80;
    const int HEIGHT = 20;
    const int NUM_STARS = 30;
    
    // Inizializzazione random 
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    std::mt19937 gen(seed);
    std::uniform_int_distribution<> xDist(0, WIDTH-1);
    std::uniform_int_distribution<> yDist(0, HEIGHT-1);
    std::uniform_int_distribution<> numStarsDist(20, 40);  // numero casuale di stelle
    std::uniform_int_distribution<> symbolDist(0, 2);      // distribuzione simboli
    
    // Generatore random di stelle
    std::vector<Star> stars;
    int actualStars = numStarsDist(gen);
    for (int i = 0; i < actualStars; i++) {
        char symbol;
        switch(symbolDist(gen)) {
            case 0: symbol = '*'; break;
            case 1: symbol = '.'; break;
            case 2: symbol = '+'; break;
        }
        
        Star star = {
            xDist(gen),
            yDist(gen),
            symbol
        };
        stars.push_back(star);
    }
    
    // inizializza schermo
    std::cout << "\033[2J\033[1;1H";
    
    // crea screen buffer
    std::vector<std::vector<char>> screen(HEIGHT, std::vector<char>(WIDTH, ' '));
    
    // Disposizione stelle
    for (const auto& star : stars) {
        screen[star.y][star.x] = star.symbol;
    }
    
    // Print buffer
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            std::cout << screen[y][x];
        }
        std::cout << '\n';
    }
    
    // Razzo
    std::cout << R"(
            /\
           /  \
          /    \
         /______\
        |        |
        |  QUIZ  |
        |        |
        |        |
      / |   ||   |\
     /  |   ||   | \
    |   |   ||   |  |
     \__|___||___/__/
        |   ||   |
        |___||___|
        |___||___|
)" << '\n';
}

#endif