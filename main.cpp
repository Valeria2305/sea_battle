#include <ctime>
#include <random>
#include <SFML/Graphics.hpp>
#include <set>
#include <utility>
#include <vector>

using namespace sf;
using namespace std;

class Game {
private:
    // массив всех фигур (элементов), изображаемых (находящихся в данный момент) на экране
    vector<RectangleShape> figures;

    // (x1, y1) и (x2, y2) - координаты верхнего левого угла поля игрока и компьютера соответственно
    // side - сторона одного поля 10 на 10 в пикселях
    size_t x1 = 50, y1 = 150, x2 = 550, y2 = 150, side = 400;

    // количество остающихся на плаву кораблей у пользователя и компьютера соответственно
    size_t player_counter = 10, enemy_counter = 10;

    /* поля игроков
    обозначения чисел в клетках полей:
    0 - пустая клетка
    1 - пустая клетка, в которую производился выстрел
    10 - клетка, занятая кораблем
    11 - клетка, занятая кораблем, в которую производился выстрел
    -1 - пустая клетка, рядом с которой есть корабль
    -2 - пустая клетка, рядом с которой есть корабль, и в которую производился выстрел */
    vector<vector<int>> player_field, enemy_field;

    // множество неповрежденных клеток кораблей пользователя, которые располагаются рядом с
    // уже поврежденными клетками кораблей пользователя
    set<pair<size_t, size_t>> cells_to_shoot;

    std::mt19937 mersenne;  // генератор случайных чисел

    // создает 2 клетчатых поля размера 10 на 10 (поле игрока и компьютера)
    void CreateFields() {
        for (size_t i = 0; i < 11; ++i) {
            figures.push_back(RectangleShape(Vector2f(2.f, side)));
            figures[figures.size() - 1].move(x1 + i * side / 10, y1);
            figures[figures.size() - 1].setFillColor(Color::Blue);
        }
        for (size_t i = 0; i < 11; ++i) {
            figures.push_back(RectangleShape(Vector2f(2.f, side)));
            figures[figures.size() - 1].move(x2 + i * side / 10, y2);
            figures[figures.size() - 1].setFillColor(Color::Blue);
        }

        for (size_t i = 0; i < 11; ++i) {
            figures.push_back(RectangleShape(Vector2f(side, 2.f)));
            figures[figures.size() - 1].move(x1, y1 + i * side / 10);
            figures[figures.size() - 1].setFillColor(Color::Blue);
        }
        for (size_t i = 0; i < 11; ++i) {
            figures.push_back(RectangleShape(Vector2f(side, 2.f)));
            figures[figures.size() - 1].move(x2, y2 + i * side / 10);
            figures[figures.size() - 1].setFillColor(Color::Blue);
        }
    }

    // генерирует случайную расстановку кораблей на поле, принимает 1 аргумент:
    // если for_player == true, то расставляет корабли игроку-пользователю
    // иначе (for_player == false) рассталяет корабли игроку-компьютеру
    void GenerateRandomShipsArrangement(bool for_player) {
        vector<vector<int>> field(12, vector<int>(12, 0));
        
        size_t i, j;

        // размещаем 4-палубник
        if (mersenne() % 2 == 0) {
            // размещаем горизонтально
            i = mersenne() % 10 + 1;
            j = mersenne() % 7 + 1;
            field[i][j - 1] = -1;
            field[i][j + 4] = -1;
            for (size_t k = 0; k < 4; ++k) {
                field[i][j + k] = 10;
            }
            for (size_t k = 0; k < 6; ++k) {
                field[i - 1][j - 1 + k] = -1;
                field[i + 1][j - 1 + k] = -1;
            }

            if (for_player) {
                // добавляем в нарисованные фигуры
                figures.push_back(RectangleShape(Vector2f(side / 10 * 4, side / 10)));
                figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
            }
        } else {
            // размещаем вертикально
            j = mersenne() % 10 + 1;
            i = mersenne() % 7 + 1;
            field[i - 1][j] = -1;
            field[i + 4][j] = -1;
            for (size_t k = 0; k < 4; ++k) {
                field[i + k][j] = 10;
            }
            for (size_t k = 0; k < 6; ++k) {
                field[i - 1 + k][j - 1] = -1;
                field[i - 1 + k][j + 1] = -1;
            }

            if (for_player) {
                // добавляем в нарисованные фигуры
                figures.push_back(RectangleShape(Vector2f(side / 10, side / 10 * 4)));
                figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
            }
        }

        // размещаем 3-палубники
        for (size_t ship = 0; ship < 2; ++ship) {
            if (mersenne() % 2 == 0) {
                // размещаем горизонтально
                i = mersenne() % 10 + 1;
                j = mersenne() % 8 + 1;
                while (field[i][j] != 0 || field[i][j + 1] != 0 || field[i][j + 2] != 0) {
                    i = mersenne() % 10 + 1;
                    j = mersenne() % 8 + 1;
                }
                field[i][j - 1] = -1;
                field[i][j + 3] = -1;
                for (size_t k = 0; k < 3; ++k) {
                    field[i][j + k] = 10;
                }
                for (size_t k = 0; k < 5; ++k) {
                    field[i - 1][j - 1 + k] = -1;
                    field[i + 1][j - 1 + k] = -1;
                }

                if (for_player) {
                    // добавляем в нарисованные фигуры
                    figures.push_back(RectangleShape(Vector2f(side / 10 * 3, side / 10)));
                    figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                    figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
                }
            } else {
                // размещаем вертикально
                j = mersenne() % 10 + 1;
                i = mersenne() % 8 + 1;
                while (field[i][j] != 0 || field[i + 1][j] != 0 || field[i + 2][j] != 0) {
                    j = mersenne() % 10 + 1;
                    i = mersenne() % 8 + 1;
                }
                field[i - 1][j] = -1;
                field[i + 3][j] = -1;
                for (size_t k = 0; k < 3; ++k) {
                    field[i + k][j] = 10;
                }
                for (size_t k = 0; k < 5; ++k) {
                    field[i - 1 + k][j - 1] = -1;
                    field[i - 1 + k][j + 1] = -1;
                }

                if (for_player) {
                    // добавляем в нарисованные фигуры
                    figures.push_back(RectangleShape(Vector2f(side / 10, side / 10 * 3)));
                    figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                    figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
                }
            }
        }

        // размещаем 2-палубники
        for (size_t ship = 0; ship < 3; ++ship) {
            if (mersenne() % 2 == 0) {
                // размещаем горизонтально
                i = mersenne() % 10 + 1;
                j = mersenne() % 9 + 1;
                while (field[i][j] != 0 || field[i][j + 1] != 0) {
                    i = mersenne() % 10 + 1;
                    j = mersenne() % 9 + 1;
                }
                field[i][j - 1] = -1;
                field[i][j + 2] = -1;
                for (size_t k = 0; k < 2; ++k) {
                    field[i][j + k] = 10;
                }
                for (size_t k = 0; k < 4; ++k) {
                    field[i - 1][j - 1 + k] = -1;
                    field[i + 1][j - 1 + k] = -1;
                }

                if (for_player) {
                    // добавляем в нарисованные фигуры
                    figures.push_back(RectangleShape(Vector2f(side / 10 * 2, side / 10)));
                    figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                    figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
                }
            } else {
                // размещаем вертикально
                j = mersenne() % 10 + 1;
                i = mersenne() % 9 + 1;
                while (field[i][j] != 0 || field[i + 1][j] != 0) {
                    j = mersenne() % 10 + 1;
                    i = mersenne() % 9 + 1;
                }
                field[i - 1][j] = -1;
                field[i + 2][j] = -1;
                for (size_t k = 0; k < 2; ++k) {
                    field[i + k][j] = 10;
                }
                for (size_t k = 0; k < 4; ++k) {
                    field[i - 1 + k][j - 1] = -1;
                    field[i - 1 + k][j + 1] = -1;
                }

                if (for_player) {
                    // добавляем в нарисованные фигуры
                    figures.push_back(RectangleShape(Vector2f(side / 10, side / 10 * 2)));
                    figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                    figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
                }
            }
        }

        // размещаем 1-палубники
        for (size_t ship = 0; ship < 4; ++ship) {
            i = mersenne() % 10 + 1;
            j = mersenne() % 10 + 1;
            while (field[i][j] != 0) {
                i = mersenne() % 10 + 1;
                j = mersenne() % 10 + 1;
            }
            field[i][j] = 10;
            field[i][j - 1] = -1;
            field[i][j + 1] = -1;
            for (size_t k = 0; k < 3; ++k) {
                field[i - 1][j - 1 + k] = -1;
                field[i + 1][j - 1 + k] = -1;
            }

            if (for_player) {
                // добавляем в нарисованные фигуры
                figures.push_back(RectangleShape(Vector2f(side / 10, side / 10)));
                figures[figures.size() - 1].move(x1 + (j - 1) * side / 10, y1 + (i - 1) * side / 10);
                figures[figures.size() - 1].setFillColor(Color(100, 100, 100, 200));
            }
        }

        if (for_player) {
            player_field = move(field);
        } else {
            enemy_field = move(field);
        }
    }

    void AddTimes(size_t x, size_t y, size_t i, size_t j) {
        float width = float(side) / 10 / 10;
        if (1 <= i && i <= 10 && 1 <= j && j <= 10) {
            figures.push_back(RectangleShape(Vector2f(side / 10, width)));
            figures[figures.size() - 1].rotate(45.f);
            figures[figures.size() - 1].move(x + (j - 1) * float(side) / 10 + width * 2.1, y + (i - 1) * float(side) / 10 + width * 1.7);
            figures[figures.size() - 1].setFillColor(Color::Red);

            figures.push_back(RectangleShape(Vector2f(side / 10, width)));
            figures[figures.size() - 1].rotate(-45.f);
            figures[figures.size() - 1].move(x + (j - 1) * float(side) / 10 + width * 1.7, y + (i - 1) * float(side) / 10 + width * 8.6);
            figures[figures.size() - 1].setFillColor(Color::Red);
        }
    }

    void AddMark(size_t x, size_t y, size_t i, size_t j) {
        if (1 <= i && i <= 10 && 1 <= j && j <= 10) {
            float width = float(side) / 10 / 9;
            figures.push_back(RectangleShape(Vector2f(width, width)));
            figures[figures.size() - 1].move(x + (j - 1) * float(side) / 10 + width * 4, y + (i - 1) * float(side) / 10 + width * 4);
            figures[figures.size() - 1].setFillColor(Color::Red);
        }
    }

    bool IsShipDestroyed(vector<vector<int>>& field, size_t i, size_t j) {
        field[i][j] = -100;

        if (field[i][j + 1] == 10) {
            field[i][j] = 11;
            return false;
        } else if (field[i][j + 1] == 11) {
            if (!IsShipDestroyed(field, i, j + 1)) {
                field[i][j] = 11;
                return false;
            }
        }

        if (field[i][j - 1] == 10) {
            field[i][j] = 11;
            return false;
        } else if (field[i][j - 1] == 11) {
            if (!IsShipDestroyed(field, i, j - 1)) {
                field[i][j] = 11;
                return false;
            }
        }

        if (field[i + 1][j] == 10) {
            field[i][j] = 11;
            return false;
        } else if (field[i + 1][j] == 11) {
            if (!IsShipDestroyed(field, i + 1, j)) {
                field[i][j] = 11;
                return false;
            }
        }

        if (field[i - 1][j] == 10) {
            field[i][j] = 11;
            return false;
        } else if (field[i - 1][j] == 11) {
            if (!IsShipDestroyed(field, i - 1, j)) {
                field[i][j] = 11;
                return false;
            }
        }

        field[i][j] = 11;
        return true;
    }

    void SurroundDestroyedShip(vector<vector<int>>& field, size_t x, size_t y, size_t i, size_t j) {
        field[i][j] = -100;

        if (field[i][j + 1] == 11) {
            SurroundDestroyedShip(field, x, y, i, j + 1);
        } else if (field[i][j + 1] == -1) {
            field[i][j + 1] = -2;
            AddMark(x, y, i, j + 1);
        }

        if (field[i][j - 1] == 11) {
            SurroundDestroyedShip(field, x, y, i, j - 1);
        } else if (field[i][j - 1] == -1) {
            field[i][j - 1] = -2;
            AddMark(x, y, i, j - 1);
        }

        if (field[i + 1][j] == 11) {
            SurroundDestroyedShip(field, x, y, i + 1, j);
        } else if (field[i + 1][j] == -1) {
            field[i + 1][j] = -2;
            AddMark(x, y, i + 1, j);
        }

        if (field[i - 1][j] == 11) {
            SurroundDestroyedShip(field, x, y, i - 1, j);
        } else if (field[i - 1][j] == -1) {
            field[i - 1][j] = -2;
            AddMark(x, y, i - 1, j);
        }


        if (field[i + 1][j + 1] == -1) {
            field[i + 1][j + 1] = -2;
            AddMark(x, y, i + 1, j + 1);
        }

        if (field[i + 1][j - 1] == -1) {
            field[i + 1][j - 1] = -2;
            AddMark(x, y, i + 1, j - 1);
        }

        if (field[i - 1][j + 1] == -1) {
            field[i - 1][j + 1] = -2;
            AddMark(x, y, i - 1, j + 1);
        }

        if (field[i - 1][j - 1] == -1) {
            field[i - 1][j - 1] = -2;
            AddMark(x, y, i - 1, j - 1);
        }


        field[i][j] = 11;
    }
    
    // делает ход за компьютер
    void MakeMove() {
        size_t i, j;
        if (cells_to_shoot.empty()) {
            i = mersenne() % 10 + 1;
            j = mersenne() % 10 + 1;
            while (player_field[i][j] == 1 || player_field[i][j] == 11 || player_field[i][j] == -2) {
                i = mersenne() % 10 + 1;
                j = mersenne() % 10 + 1;
            }
        } else {
            i = cells_to_shoot.begin()->first;
            j = cells_to_shoot.begin()->second;
            cells_to_shoot.erase(cells_to_shoot.begin());
        }
        

        if (player_field[i][j] == 0) {
            player_field[i][j] = 1;
            AddMark(x1, y1, i, j);
        } else if (player_field[i][j] == -1) {
            player_field[i][j] = -2;
            AddMark(x1, y1, i, j);
        } else if (player_field[i][j] == 10) {
            player_field[i][j] = 11;
            AddTimes(x1, y1, i, j);
            if (IsShipDestroyed(player_field, i, j) && player_counter > 0) {
                --player_counter;
                SurroundDestroyedShip(player_field, x1, y1, i, j);
            } else {
                if (player_field[i][j + 1] == 10) {
                    cells_to_shoot.insert(pair<size_t, size_t>(i, j + 1));
                }
                if (player_field[i][j - 1] == 10) {
                    cells_to_shoot.insert(pair<size_t, size_t>(i, j - 1));
                }
                if (player_field[i - 1][j] == 10) {
                    cells_to_shoot.insert(pair<size_t, size_t>(i - 1, j));
                }
                if (player_field[i + 1][j] == 10) {
                    cells_to_shoot.insert(pair<size_t, size_t>(i + 1, j));
                }
            }
        }
    }

public:
    Game() : mersenne(time(0)) {
        CreateFields();
        GenerateRandomShipsArrangement(true);
        GenerateRandomShipsArrangement(false);
    }

    // возвращает костантную ссылку на массив всех фигур, изображаемых в данный момент на экране
    const vector<RectangleShape>& GetFigures() const {
        return figures;
    }

    // закончилась ли игра
    bool IsFinished() const {
        if (player_counter == 0 || enemy_counter == 0) {
            return true;
        } else {
            return false;
        }
    }

    // проверка: выиграл ли пользователь
    bool UserHasWon() const {
        if (enemy_counter == 0) {
            return true;
        } else {
            return false;
        }
    }

    // проверка: выиграл ли компьютер
    bool ComputerHasWon() const {
        if (player_counter == 0) {
            return true;
        } else {
            return false;
        }
    }

    // обрабатывает нажатие клавиши мыши (на вход получает координаты курсора (xf, yf)
    void MouseButtonPressed(float xf, float yf) {
        if (IsFinished()) {
            return;
        }
        
        size_t xc, yc;
        xc = round(xf);
        yc = round(yf);
        if (x2 <= xc && xc <= x2 + side && y2 <= yc && yc <= y2 + side) {
            size_t i, j;
            j = (xc - x2) / (side / 10) + 1;
            i = (yc - y2) / (side / 10) + 1;

            if (enemy_field[i][j] == 0) {
                enemy_field[i][j] = 1;
                AddMark(x2, y2, i, j);
                MakeMove();
            } else if (enemy_field[i][j] == -1) {
                enemy_field[i][j] = -2;
                AddMark(x2, y2, i, j);
                MakeMove();
            } else if (enemy_field[i][j] == 10) {
                enemy_field[i][j] = 11;
                AddTimes(x2, y2, i, j);
                if (IsShipDestroyed(enemy_field, i, j) && enemy_counter > 0) {
                    --enemy_counter;
                    SurroundDestroyedShip(enemy_field, x2, y2, i, j);
                }
            }
        }
    }
};

// рисует все фигуры из массива figures на экране
void Draw(RenderWindow& window, const vector<RectangleShape>& figures) {
    for (const RectangleShape& fig: figures) {
        window.draw(fig);
    }
}

int main()
{
    RenderWindow window(VideoMode(1000, 700), "Sea Battle");
    
    Game game;

    Font font;
    font.loadFromFile("Micra Normal.ttf");
    Text player_field_label("Your field", font, 30), enemy_field_label("Enemy field", font, 30);
    player_field_label.setFillColor(Color::Black);
    enemy_field_label.setFillColor(Color::Black);
    player_field_label.setPosition(100, 50);
    enemy_field_label.setPosition(600, 50);

    Text final_label("", font, 30);
    final_label.setFillColor(Color::Black);

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                game.MouseButtonPressed(event.mouseButton.x, event.mouseButton.y);
                if (game.UserHasWon()) {
                    final_label.setString("Congratulations! You've won!");
                    final_label.setPosition(160, 600);
                } else if (game.ComputerHasWon()) {
                    final_label.setString("You've lost...");
                    final_label.setPosition(370, 600);
                }
            }
        }

        window.clear(Color::White);
        Draw(window, game.GetFigures());
        window.draw(player_field_label);
        window.draw(enemy_field_label);
        if (game.IsFinished()) {
            window.draw(final_label);
        }
        window.display();
    }

    return 0;
}