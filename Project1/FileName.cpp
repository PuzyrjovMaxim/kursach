#include <SFML/Graphics.hpp>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <windows.h>

using namespace std;
//––––––––––––––––––––ГЛОБАЛЬНЫЕ КОНСТАНТЫ–––––––––––––––––––
const int CELL_SIZE = 2; //РАЗМЕР КЛЕТКИ
const int GRID_WIDTH = 900; //ШИРИНА РЕШЕТКИ (В КЛЕТКАХ)
const int GRID_HEIGHT = 500; //ВЫСОТА РЕШЕТКИ (В КЛЕТКАХ)
const int WINDOW_WIDTH = GRID_WIDTH * CELL_SIZE; //ШИРИНА ОКНА ВЫВОДА
const int WINDOW_HEIGHT = GRID_HEIGHT * CELL_SIZE; //ВЫСОТА ОКНА ВЫВОДА

//СОСТОЯНИЯ КЛЕТОЧНОГО АВТОМАТА
struct Particle {
    int liquid;   // жидкость
    int impurity;  // примесь
};

enum CellType {
    EMPTY,
    OBSTACLE, // стенка
    PARTICLE  // частица
};

struct Cell {
    CellType type;
    Particle part;
    sf::CircleShape shape;
};

vector<vector<Cell>> grid(GRID_HEIGHT, vector<Cell>(GRID_WIDTH));
//–––––––––––––––––––ПРОЦЕДУРЫ––––––––––––––––––––––

//СОЗДАНИЕ ПУСТОЙ КЛЕТОЧНОЙ РЕШЁТКИ
void initializeGrid() {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            if ((i + j) % 2 == 0) {
                grid[i][j].type = EMPTY;
                grid[i][j].part.liquid = 0;
                grid[i][j].part.impurity = 0;
                grid[i][j].shape.setFillColor(sf::Color::White);
                grid[i][j].shape.setRadius(CELL_SIZE);
                grid[i][j].shape.setPointCount(6);
                grid[i][j].shape.setPosition(j * CELL_SIZE, i * CELL_SIZE);
            }
        }
    }
}

//CОЗДАНИЕ ПОРИСТОГО МАТЕРИАЛА
void placeObstaclesBorders() {
    // Границы поля

    for (int i = 0; i < GRID_HEIGHT; ++i) {
        grid[i][0].type = OBSTACLE;
        grid[i][0].shape.setFillColor(sf::Color::Black);
        grid[i][GRID_WIDTH - 1].type = OBSTACLE;
        grid[i][GRID_WIDTH - 1].shape.setFillColor(sf::Color::Black);
    }
    for (int j = 0; j < GRID_WIDTH; ++j) {
            
        grid[0][j].type = OBSTACLE;
        grid[0][j].shape.setFillColor(sf::Color::Black);
        grid[GRID_HEIGHT - 1][j].type = OBSTACLE;
        grid[GRID_HEIGHT - 1][j].shape.setFillColor(sf::Color::Black);
    }
}

void placeObstacles1() {
    for (int i = GRID_HEIGHT - 80; i < GRID_HEIGHT; ++i) {
        for (int j = GRID_WIDTH / 6; j < GRID_WIDTH; ++j) {
            //СТЕНКИ 
            grid[i][j].type = OBSTACLE;
            grid[i][j].shape.setFillColor(sf::Color::Black);
        }
    }
}

void placeObstacles2() {
    int k = 0;
    for (int i = GRID_HEIGHT / 2; i >= GRID_HEIGHT / 5; --i) {
        grid[i][k].type = OBSTACLE;
        grid[i][k].shape.setFillColor(sf::Color::Black);
        k++;
    }
    k = 0;
    for (int i = GRID_HEIGHT / 2 - 1; i >= GRID_HEIGHT / 5 - 1; --i) {
        grid[i][k].type = OBSTACLE;
        grid[i][k].shape.setFillColor(sf::Color::Black);
        k++;
    }
    k = 0;
    for (int i = GRID_HEIGHT / 2 + 1; i >= GRID_HEIGHT / 5 + 1; --i) {
        grid[i][k].type = OBSTACLE;
        grid[i][k].shape.setFillColor(sf::Color::Black);
        k++;
    }
}

//ОБНОВЛЕНИЕ СОСТОЯНИЙ КЛЕТОК
void updateCellsLiquid() {
    for (int i = GRID_HEIGHT - 1; i >= 1; --i) {
        for (int j = 1; j <= GRID_WIDTH - 2; ++j) {
            if (grid[i][j].part.liquid == 1 and (i>=3 && i<= GRID_HEIGHT - 3) and (j>=2 && j<= GRID_WIDTH - 3)) {
                int s[6] = {0, 0, 0, 1, 0, 0}; int cnt = 1;
                if (grid[i + 2][j].type == OBSTACLE or grid[i + 2][j].part.liquid == 1) {
                    s[0] = 0; s[3] = 0; cnt--;
                }
                if (grid[i + 2][j].type == EMPTY) {
                    s[0] = 0;
                }
                if ((grid[i + 1][j - 1].part.liquid == 1 or grid[i + 1][j - 1].type == OBSTACLE) and (grid[i - 1][j + 1].part.liquid == 1 or grid[i - 1][j + 1].type == OBSTACLE)) {
                    s[5] = 0; s[2] = 0;
                }
                else if (grid[i - 1][j + 1].type == EMPTY and (grid[i + 1][j - 1].part.liquid == 1 or grid[i + 1][j - 1].type == OBSTACLE)) {
                    s[5] = 1; cnt += 1;
                }
                else if((grid[i - 1][j + 1].type == OBSTACLE or grid[i - 1][j + 1].part.liquid == 1) and grid[i + 1][j - 1].type == EMPTY) {
                    s[2] = 1; cnt += 1;
                }
                if ((grid[i + 1][j + 1].part.liquid == 1 or grid[i + 1][j + 1].type == OBSTACLE) or (grid[i - 1][j - 1].part.liquid == 1 or grid[i - 1][j - 1].type == OBSTACLE)) {
                    s[1] = 0; s[4] = 0;
                }
                else if (grid[i - 1][j - 1].type == EMPTY and (grid[i + 1][j + 1].part.liquid == 1 or grid[i + 1][j + 1].type == OBSTACLE)) {
                    s[1] = 1; cnt += 1;
                }
                else if ((grid[i - 1][j - 1].type == OBSTACLE or grid[i - 1][j - 1].part.liquid == 1) and grid[i + 1][j + 1].type == EMPTY) {
                    s[4] = 1; cnt += 1;
                }
                if (cnt == 3) break;
                else if (cnt == 1) {                           // если воздействует только 1 клетка
                    for (int l = 0; l < 6; l++) {
                        if (s[l] == 1) {
                            switch (l) {
                            /*case 0:
                                if (grid[i + 2][j].type != OBSTACLE and grid[i + 2][j].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 2][j].part.liquid = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(0, 180, 255));
                                }*/
                            case 1:
                                if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 1][j - 1].part.liquid = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(0, 180, 255));
                                }
                            case 2:
                                if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 1][j - 1].part.liquid = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(0, 180, 255));
                                }
                            case 3:
                                if (grid[i + 2][j].part.liquid != 1 and grid[i + 2][j].type != OBSTACLE) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 2][j].part.liquid = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(0, 180, 255));
                                }
                            case 4:
                                if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 1][j + 1].part.liquid = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(0, 180, 255));
                                }
                            case 5:
                                if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 1][j + 1].part.liquid = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(0, 180, 255));
                                }
                            }
                            break;
                        }
                    }
                }
                else if (cnt == 2) {                            // если воздействует 2 клетки
                    int summ = 0;
                    int k = 0;
                    for (int l = 0; l < 6; l++) {
                        if (s[l] == 1) {
                            summ += l + 1;
                            if (k == 0) k == l + 1;
                        }
                    }
                    switch (summ / 2 + summ % 2) {
                    case 2:                                                      // клетка движется в направлении 5
                        if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.liquid != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                            grid[i + 1][j - 1].part.liquid = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(0, 180, 255));
                        }
                    case 3:                                                      // клетка движется в направлении 0
                        if (grid[i + 2][j].type != OBSTACLE and grid[i + 2][j].part.liquid != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                            grid[i + 2][j].part.liquid = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(0, 180, 255));
                        }
                    case 5:                                                      // клетка движется в направлении 1
                        if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.liquid != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                            grid[i + 1][j + 1].part.liquid = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(0, 180, 255));
                        }
                    case 4:
                        if (k == 2) {                                            // клетка движется в направлении 5
                            if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.liquid != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                grid[i + 1][j - 1].part.liquid = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(0, 180, 255));
                            }
                        }
                        else {                                                  // клетка движется в направлении 1
                            if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.liquid != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                grid[i + 1][j + 1].part.liquid = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(0, 180, 255));
                            }
                        }
                    case 6:                                                     // клетка движется в направлении 1
                        if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.liquid != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                            grid[i + 1][j + 1].part.liquid = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(0, 180, 255));
                        }
                    }
                }
            }
        }
    }
}

void updateCellsImpurity() {
    for (int i = GRID_HEIGHT - 1; i >= 1; --i) {
        for (int j = 1; j <= GRID_WIDTH - 2; ++j) {
            if (grid[i][j].part.impurity == 1 and (i >= 3 && i <= GRID_HEIGHT - 3) and (j >= 2 && j <= GRID_WIDTH - 3)) {
            int s[6] = { 0, 0, 0, 1, 0, 0 }; int cnt = 1;
            if (grid[i + 2][j].type == OBSTACLE or grid[i + 2][j].part.impurity == 1) {
                s[0] = 0; s[3] = 0; cnt--;
            }
            if (grid[i + 2][j].type == EMPTY) {
                s[0] = 0;
            }
            if ((grid[i + 1][j - 1].part.impurity == 1 or grid[i + 1][j - 1].type == OBSTACLE) and (grid[i - 1][j + 1].part.impurity == 1 or grid[i - 1][j + 1].type == OBSTACLE)) {
                s[5] = 0; s[2] = 0;
            }
            else if (grid[i - 1][j + 1].type == EMPTY and (grid[i + 1][j - 1].part.impurity == 1 or grid[i + 1][j - 1].type == OBSTACLE)) {
                s[5] = 1; cnt += 1;
            }
            else if ((grid[i - 1][j + 1].type == OBSTACLE or grid[i - 1][j + 1].part.impurity == 1) and grid[i + 1][j - 1].type == EMPTY) {
                s[2] = 1; cnt += 1;
            }
            if ((grid[i + 1][j + 1].part.impurity == 1 or grid[i + 1][j + 1].type == OBSTACLE) or (grid[i - 1][j - 1].part.impurity == 1 or grid[i - 1][j - 1].type == OBSTACLE)) {
                s[1] = 0; s[4] = 0;
            }
            else if (grid[i - 1][j - 1].type == EMPTY and (grid[i + 1][j + 1].part.impurity == 1 or grid[i + 1][j + 1].type == OBSTACLE)) {
                s[1] = 1; cnt += 1;
            }
            else if ((grid[i - 1][j - 1].type == OBSTACLE or grid[i - 1][j - 1].part.impurity == 1) and grid[i + 1][j + 1].type == EMPTY) {
                s[4] = 1; cnt += 1;
            }
            if (cnt == 3) break;
            else if (cnt == 1) {                           // если воздействует только 1 клетка
                for (int l = 0; l < 6; l++) {
                    if (s[l] == 1) {
                        switch (l) {
                            /*case 0:
                                if (grid[i + 2][j].type != OBSTACLE and grid[i + 2][j].part.liquid != 1) {
                                    grid[i][j].type = EMPTY; grid[i][j].part.liquid = 0; grid[i][j].shape.setFillColor(sf::Color::White);
                                    grid[i + 2][j].part.liquid = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(0, 180, 255));
                                }*/
                        case 1:
                            if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.impurity != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                                grid[i + 1][j - 1].part.impurity = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(60, 30, 60));
                            }
                        case 2:
                            if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.impurity != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                                grid[i + 1][j - 1].part.impurity = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(60, 30, 60));
                            }
                        case 3:
                            if (grid[i + 2][j].part.impurity != 1 and grid[i + 2][j].type != OBSTACLE) {
                                grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                                grid[i + 2][j].part.impurity = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(60, 30, 60));
                            }
                        case 4:
                            if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.impurity != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                                grid[i + 1][j + 1].part.impurity = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(60, 30, 60));
                            }
                        case 5:
                            if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.impurity != 1) {
                                grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                                grid[i + 1][j + 1].part.impurity = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(60, 30, 60));
                            }
                        }
                        break;
                    }
                }
            }
            else if (cnt == 2) {                            // если воздействует 2 клетки
                int summ = 0;
                int k = 0;
                for (int l = 0; l < 6; l++) {
                    if (s[l] == 1) {
                        summ += l + 1;
                        if (k == 0) k == l + 1;
                    }
                }
                switch (summ / 2 + summ % 2) {
                case 2:                                                      // клетка движется в направлении 5
                    if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.impurity != 1) {
                        grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                        grid[i + 1][j - 1].part.impurity = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(60, 30, 60));
                    }
                case 3:                                                      // клетка движется в направлении 0
                    if (grid[i + 2][j].type != OBSTACLE and grid[i + 2][j].part.impurity != 1) {
                        grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                        grid[i + 2][j].part.impurity = 1; grid[i + 2][j].type = PARTICLE; grid[i + 2][j].shape.setFillColor(sf::Color(60, 30, 60));
                    }
                case 5:                                                      // клетка движется в направлении 1
                    if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.impurity != 1) {
                        grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                        grid[i + 1][j + 1].part.impurity = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(60, 30, 60));
                    }
                case 4:
                    if (k == 2) {                                            // клетка движется в направлении 5
                        if (grid[i + 1][j - 1].type != OBSTACLE and grid[i + 1][j - 1].part.impurity != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                            grid[i + 1][j - 1].part.impurity = 1; grid[i + 1][j - 1].type = PARTICLE; grid[i + 1][j - 1].shape.setFillColor(sf::Color(60, 30, 60));
                        }
                    }
                    else {                                                  // клетка движется в направлении 1
                        if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.impurity != 1) {
                            grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                            grid[i + 1][j + 1].part.impurity = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(60, 30, 60));
                        }
                    }
                case 6:                                                     // клетка движется в направлении 1
                    if (grid[i + 1][j + 1].type != OBSTACLE and grid[i + 1][j + 1].part.impurity != 1) {
                        grid[i][j].type = EMPTY; grid[i][j].part.impurity = 0; grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
                        grid[i + 1][j + 1].part.impurity = 1; grid[i + 1][j + 1].type = PARTICLE; grid[i + 1][j + 1].shape.setFillColor(sf::Color(60, 30, 60));
                    }
                }
                }
            }
        }
    }
}

//ЗАПОЛНЕНИЕ ОКНА
void drawGrid(sf::RenderWindow& window) {
    for (int i = 0; i < GRID_HEIGHT; ++i) {
        for (int j = 0; j < GRID_WIDTH; ++j) {
            if((i+j)%2==0)
                window.draw(grid[i][j].shape);
        }
    }
}

int main() {
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Liquid Simulation");
    int count = 0, maxcount = 0;


    cout << "Enter number of iterations: \n";
    cin >> maxcount;


    initializeGrid();
    placeObstaclesBorders();
    placeObstacles1();
    placeObstacles2();


    //ДОБАВЛЕНИЕ ЖИДКОСТИ
    srand(time(NULL));
    /*for (int i = GRID_HEIGHT / 4; i < GRID_HEIGHT * 3 / 4; ++i) {
        for (int j = GRID_WIDTH / 4; j < GRID_WIDTH*3/4; ++j) {
            if (rand() % 3 == 0 or rand() % 3 == 1) {
                grid[i][j].type = PARTICLE;
                grid[i][j].part.liquid = 1;
                grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
            }
        }
    }

    for (int i = 2; i < 7; ++i) {
        for (int j = 2; j < 7; ++j) {
            grid[i][j].type = PARTICLE;
            grid[i][j].part.liquid = 1;
            grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
        }
    }*/

    for (int i = GRID_HEIGHT - 2; i >= 1; --i) {
        for (int j = 1; j < GRID_WIDTH - 2; ++j) {
            grid[i][j].type = PARTICLE;
            grid[i][j].part.liquid = 1;
            grid[i][j].shape.setFillColor(sf::Color(0, 180, 255));
        }
    }

    for (int i = GRID_HEIGHT / 2; i >= GRID_HEIGHT / 4; --i) {
        for (int j = GRID_WIDTH / 2; j <= GRID_WIDTH * 3 / 4; ++j) {
            if (rand() % 2 == 0) {
                grid[i][j].type = PARTICLE;
                grid[i][j].part.impurity = 1;
                grid[i][j].shape.setFillColor(sf::Color(60, 30, 60));
            }
        }
    }

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                Sleep(20000);
                window.close();
            }
            if (count > maxcount) window.close();
        }
        if (count == 1) {
            Sleep(10000);
        }
        updateCellsLiquid();
        updateCellsImpurity();
        count++;
        window.clear(sf::Color::White);
        drawGrid(window);
        window.display();
        cout << count << endl;
    }
    return 0;
}
