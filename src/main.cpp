#include "game.h"
#include <SFML/Graphics.hpp>
#include <vector>

// объявление и инициализация констант

/** ширина (в пикселях) окна приложения*/
const size_t WIDTH = 1000;
/** высота (в пикселях) окна приложения*/
const size_t HEIGTH = 700;

/** координаты (в пикселях) верхнего левого угла поля игрока-пользователя */
const size_t X1 = 50, Y1 = 150;

/** координаты (в пикселях) верхнего левого угла поля игрока-компьютера */
const size_t X2 = 550, Y2 = 150;

/** величина (в пикселях) стороны одного поля 10 на 10, а также величина стороны
 * одной клетки */
const size_t SIDE = 400, CELL_SIDE = 40;

/** класс, занимающийся отрисовкой всех фигур в окне приложения
 с функцией int main() взаимодействует посредством методов:
 MouseButtonPressed(float xf, float yf) - обрабатывает клик мыши в точку с
 координатами (xf, yf) Draw(sf::RenderWindow& window) - рисует все надписи,
 линии, значки и прочие фигуры на экране*/
class GUI {
  private:
    /**
     * объект класса Game, содержащий всю логику игры
     * */
    Game game;

    /**
     * используемый в надписях шрифт */
    sf::Font font;

    /** надписи, обозначающие поле игрока, поле противника, объявление о конце
     * игры соответственно */
    sf::Text player_field_label, enemy_field_label, endgame_label;

    /** вектор линий клетчатых полей игроков и вектор всех остальных фигур на
     * этих полях соответственно */
    std::vector<sf::RectangleShape> field_grids, figures;

  private:
    /**
     *     рисует линии клетчатых полей игроков
     */
    void CreateFieldGrids() {
        for (size_t i = 0; i < 11; ++i) {
            field_grids.push_back(sf::RectangleShape(sf::Vector2f(2.f, SIDE)));
            field_grids[field_grids.size() - 1].move(X1 + i * SIDE / 10, Y1);
            field_grids[field_grids.size() - 1].setFillColor(sf::Color::Blue);
        }
        for (size_t i = 0; i < 11; ++i) {
            field_grids.push_back(sf::RectangleShape(sf::Vector2f(2.f, SIDE)));
            field_grids[field_grids.size() - 1].move(X2 + i * SIDE / 10, Y2);
            field_grids[field_grids.size() - 1].setFillColor(sf::Color::Blue);
        }

        for (size_t i = 0; i < 11; ++i) {
            field_grids.push_back(sf::RectangleShape(sf::Vector2f(SIDE, 2.f)));
            field_grids[field_grids.size() - 1].move(X1, Y1 + i * SIDE / 10);
            field_grids[field_grids.size() - 1].setFillColor(sf::Color::Blue);
        }
        for (size_t i = 0; i < 11; ++i) {
            field_grids.push_back(sf::RectangleShape(sf::Vector2f(SIDE, 2.f)));
            field_grids[field_grids.size() - 1].move(X2, Y2 + i * SIDE / 10);
            field_grids[field_grids.size() - 1].setFillColor(sf::Color::Blue);
        }
    }

    /**
     * ставим попадание в клетку (i, j) поля, верхний левый угол которого имеет
     * координаты (x, y) в пикселях
     * @param x
     * @param y
     * @param i
     * @param j
     */
    void AddTimes(size_t x, size_t y, size_t i, size_t j) {
        float width = 4;
        figures.push_back(sf::RectangleShape(sf::Vector2f(SIDE / 10, width)));
        figures[figures.size() - 1].rotate(45.f);
        figures[figures.size() - 1].move(
            x + (j - 1) * float(SIDE) / 10 + width * 2.1,
            y + (i - 1) * float(SIDE) / 10 + width * 1.7);
        figures[figures.size() - 1].setFillColor(sf::Color::Red);

        figures.push_back(sf::RectangleShape(sf::Vector2f(SIDE / 10, width)));
        figures[figures.size() - 1].rotate(-45.f);
        figures[figures.size() - 1].move(
            x + (j - 1) * float(SIDE) / 10 + width * 1.7,
            y + (i - 1) * float(SIDE) / 10 + width * 8.6);
        figures[figures.size() - 1].setFillColor(sf::Color::Red);
    }

    /**
     * ставим промах в клетку (i, j) поля, верхний левый угол которого имеет
     * координаты (x, y) в пикселях
     * @param x
     * @param y
     * @param i
     * @param j
     */
    void AddMark(size_t x, size_t y, size_t i, size_t j) {
        float width = 12;
        figures.push_back(sf::RectangleShape(sf::Vector2f(width, width)));
        figures[figures.size() - 1].move(x + (j - 1) * SIDE / 10 + width + 3,
                                         y + (i - 1) * SIDE / 10 + width + 3);
        figures[figures.size() - 1].setFillColor(sf::Color(0, 0, 0, 0));
        figures[figures.size() - 1].setOutlineColor(sf::Color(255, 0, 0, 100));
        figures[figures.size() - 1].setOutlineThickness(2);
    }

    /**
     * ставим клетку некоторого корабля в клетку (i, j) поля,
     верхний левый угол которого имеет координаты (x, y) в пикселях
     * @param x
     * @param y
     * @param i
     * @param j
     */
    void AddShipCell(size_t x, size_t y, size_t i, size_t j) {
        figures.push_back(
            sf::RectangleShape(sf::Vector2f(CELL_SIDE, CELL_SIDE)));
        figures[figures.size() - 1].move(x + (j - 1) * CELL_SIDE,
                                         y + (i - 1) * CELL_SIDE);
        figures[figures.size() - 1].setFillColor(sf::Color(100, 100, 100, 200));
    }

    /** после каждого хода очищает и заново создает вектор figures всех фигур на
     полях для их дальнейшей отрисовки*/
    void RecreateFiguresAfterTurn() {
        // очищение figures
        figures.clear();

        // отрисовка всех фигур на поле игрока-пользователя
        const std::vector<std::vector<int>> &player_field =
            game.GetPlayerField();
        for (size_t i = 1; i <= 10; ++i) {
            for (size_t j = 1; j <= 10; ++j) {
                // занята ли клетка кораблем
                if (player_field[i][j] == 10 || player_field[i][j] == 11) {
                    AddShipCell(X1, Y1, i, j);
                }

                // проверка на выстрел-промах в клетку (i, j)
                if (player_field[i][j] == 1 || player_field[i][j] == -2) {
                    AddMark(X1, Y1, i, j);
                }

                // проверка на выстрел-попадание в клетку (i, j)
                if (player_field[i][j] == 11) {
                    AddTimes(X1, Y1, i, j);
                }
            }
        }

        // отрисовка всех фигур на поле игрока-компьютера
        const std::vector<std::vector<int>> &enemy_field = game.GetEnemyField();
        for (size_t i = 1; i <= 10; ++i) {
            for (size_t j = 1; j <= 10; ++j) {
                // проверка на выстрел-промах в клетку (i, j)
                if (enemy_field[i][j] == 1 || enemy_field[i][j] == -2) {
                    AddMark(X2, Y2, i, j);
                }

                // проверка на выстрел-попадание в клетку (i, j)
                if (enemy_field[i][j] == 11) {
                    AddTimes(X2, Y2, i, j);
                }
            }
        }
    }

  public:
    /**
     * конструктор графического интерфейса
     */
    GUI() {
        // загрузка шрифта
        font.loadFromFile("C:/lera/src/Micra_Normal.ttf");

        // установка параметров надписи "Поле игрока"
        player_field_label.setString("Your field");
        player_field_label.setFont(font);
        player_field_label.setCharacterSize(30);
        player_field_label.setFillColor(sf::Color::Black);
        player_field_label.setPosition(100, 50);

        // установка параметров надписи "Поле противника"
        enemy_field_label.setString("Enemy field");
        enemy_field_label.setFont(font);
        enemy_field_label.setCharacterSize(30);
        enemy_field_label.setFillColor(sf::Color::Black);
        enemy_field_label.setPosition(600, 50);

        // установка параметров надписи об окончании игры
        endgame_label.setFont(font);
        endgame_label.setCharacterSize(30);
        endgame_label.setFillColor(sf::Color::Black);

        // Создание полей (а именно их решетки)
        CreateFieldGrids();

        // генерирует рандомную расстановку кораблей на полях игроков
        game.GenerateRandomShipsArrangement(true);
        game.GenerateRandomShipsArrangement(false);

        /** отрисовывает корабли в начальной позиции*/
        RecreateFiguresAfterTurn();
    }

    /**
     * обрабатывает клик мыши в точку с координатами (xf, yf)
     * @param xf
     * @param yf
     */
    void MouseButtonPressed(float xf, float yf) {
        // не закончилась ли игра
        if (game.IsFinished()) {
            return;
        }

        size_t xc, yc;
        xc = round(xf);
        yc = round(yf);

        // проверка, произвошел ли данный клик на поле игрока-компьютера
        if (X2 <= xc && xc <= X2 + SIDE && Y2 <= yc && yc <= Y2 + SIDE) {
            size_t i, j;
            j = (xc - X2) / (SIDE / 10) + 1;
            i = (yc - Y2) / (SIDE / 10) + 1;
            game.MouseButtonPressed(i, j);
            RecreateFiguresAfterTurn();

            // после каждого хода проверка: не закончилась ли игра
            if (game.UserHasWon()) {
                endgame_label.setString("Congratulations! You've won!");
                endgame_label.setPosition(160, 600);
            } else if (game.ComputerHasWon()) {
                endgame_label.setString("You've lost...");
                endgame_label.setPosition(370, 600);
            }
        }
    }

    /**
     * рисует все надписи, линии, значки и прочие фигуры в окне windows
     * @param window окно для рисования
     */
    void Draw(sf::RenderWindow &window) {
        for (const sf::RectangleShape &line : field_grids) {
            window.draw(line);
        }

        for (const sf::RectangleShape &fig : figures) {
            window.draw(fig);
        }

        window.draw(player_field_label);
        window.draw(enemy_field_label);
        if (game.IsFinished()) {
            window.draw(endgame_label);
        }
    }
};

int main() {
    // создание окна приложения
    sf::RenderWindow window(sf::VideoMode(WIDTH, HEIGTH), "Sea Battle");

    // объект GUI, занимающийся отрисовкой
    GUI gui;

    // пока окно открыто
    while (window.isOpen()) {
        // создаем событие
        sf::Event event;
        while (window.pollEvent(event)) {
            // проверка вида события
            if (event.type == sf::Event::Closed) {
                window.close();
            } else if (event.type == sf::Event::MouseButtonPressed) {
                gui.MouseButtonPressed(event.mouseButton.x,
                                       event.mouseButton.y);
            }
        }

        // очистить экран и сделать белый фон
        window.clear(sf::Color::White);
        // отрисовка всех элементов
        gui.Draw(window);
        // показ окна
        window.display();
    }

    return 0;
}