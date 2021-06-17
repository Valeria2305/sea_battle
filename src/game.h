#include <cassert>
#include <ctime>
#include <random>
#include <set>
#include <utility>
#include <vector>

/** класс, содержащий всю логику игры (не занимается отрисовкой и графическим интерфейсом!)
 хранит в себе массивы - поля игроков
 с классом GUI файла main.cpp взаимодействует посредством методов:
 MouseButtonPressed(size_t i, size_t j) - обработка выстрела в клетку (i, j) поля игрока-компьютера
 GetPlayerField() и GetEnemyField() - возвращают основной программе константные ссылки на 
 поддерживаемые поля игроков для их дальнейшей отрисовки
 IsFinished(), UserHasWon(), ComputerHasWon() - проверка на окончание игры и на то, кто выиграл
 GenerateRandomShipsArrangement(bool for_player) - генерирует случайную расстановку кораблей на поле
 остальные методы нужны только классу Game для его корректной работы, либо для тестирования класса Game*/
class Game {
private:
    //количество остающихся на плаву кораблей у пользователя и компьютера соответственно
    size_t player_counter, enemy_counter;

    /** поля игроков
    обозначения чисел в клетках полей:
    0 - пустая клетка
    1 - пустая клетка, в которую производился выстрел
    10 - клетка, занятая кораблем
    11 - клетка, занятая кораблем, в которую производился выстрел
    -1 - пустая клетка, рядом с которой есть корабль
    -2 - пустая клетка, рядом с которой есть корабль, и в которую производился выстрел */
    std::vector<std::vector<int>> player_field, enemy_field;

    /** множество неповрежденных клеток кораблей пользователя, которые располагаются рядом с
    уже поврежденными клетками кораблей пользователя*/
    std::set<std::pair<size_t, size_t>> cells_to_shoot;

    std::mt19937 mersenne;  // генератор случайных чисел

private:
    /** окружает потопленный корабль метками 
     (заполняет элементы массива вокруг потопленного корабля значением -2)
     i, j - номера строки и столбца одной из клеток потопленного корабля
     действие производится над полем игрока, если for_player == true, иначе над полем противника*/
    void SurroundDestroyedShip(size_t i, size_t j, bool for_player) {
    //field_ptr указывает на то поле, с которым мы сейчас работаем
        std::vector<std::vector<int>>* field_ptr;
        if (for_player) {
            field_ptr = &player_field;
        } else {
            field_ptr = &enemy_field;
        }
        
        (*field_ptr)[i][j] = -100;

        if ((*field_ptr)[i][j + 1] == 11) {
            SurroundDestroyedShip(i, j + 1, for_player);
        } else if ((*field_ptr)[i][j + 1] == -1) {
            (*field_ptr)[i][j + 1] = -2;
        }

        if ((*field_ptr)[i][j - 1] == 11) {
            SurroundDestroyedShip(i, j - 1, for_player);
        } else if ((*field_ptr)[i][j - 1] == -1) {
            (*field_ptr)[i][j - 1] = -2;
        }

        if ((*field_ptr)[i + 1][j] == 11) {
            SurroundDestroyedShip(i + 1, j, for_player);
        } else if ((*field_ptr)[i + 1][j] == -1) {
            (*field_ptr)[i + 1][j] = -2;
        }

        if ((*field_ptr)[i - 1][j] == 11) {
            SurroundDestroyedShip(i - 1, j, for_player);
        } else if ((*field_ptr)[i - 1][j] == -1) {
            (*field_ptr)[i - 1][j] = -2;
        }


        if ((*field_ptr)[i + 1][j + 1] == -1) {
            (*field_ptr)[i + 1][j + 1] = -2;
        }

        if ((*field_ptr)[i + 1][j - 1] == -1) {
            (*field_ptr)[i + 1][j - 1] = -2;
        }

        if ((*field_ptr)[i - 1][j + 1] == -1) {
            (*field_ptr)[i - 1][j + 1] = -2;
        }

        if ((*field_ptr)[i - 1][j - 1] == -1) {
            (*field_ptr)[i - 1][j - 1] = -2;
        }


        (*field_ptr)[i][j] = 11;
    }

    //делает ход за компьютер
    void MakeTurn() {
        size_t i, j;
        if (cells_to_shoot.empty()) {
            /** если нет поврежденных но непотопленных кораблей, 
            то случайно выбираем новые координаты выстрела (i, j)*/
            i = mersenne() % 10 + 1;
            j = mersenne() % 10 + 1;
            while (player_field[i][j] == 1 || player_field[i][j] == 11 || player_field[i][j] == -2) {
                i = mersenne() % 10 + 1;
                j = mersenne() % 10 + 1;
            }
        } else {
            /** если есть поврежденные но непотопленные корабли, 
            то выбираем координату неповрежденной клетки одного из них*/
            i = cells_to_shoot.begin()->first;
            j = cells_to_shoot.begin()->second;
            cells_to_shoot.erase(cells_to_shoot.begin());
        }

        //стреляем в клетку (i, j) поля игрока-пользователя
        if (player_field[i][j] == 0) {
            player_field[i][j] = 1;
        } else if (player_field[i][j] == -1) {
            player_field[i][j] = -2;
        } else if (player_field[i][j] == 10) {
            player_field[i][j] = 11;
            if (IsShipDestroyed(i, j, true) && player_counter > 0) {
                --player_counter;
                SurroundDestroyedShip(i, j, true);
            } else {
                // обновляем множество неповрежденных клеток кораблей пользователя, в которые можно выстрелить
                if (player_field[i][j + 1] == 10) {
                    cells_to_shoot.insert(std::pair<size_t, size_t>(i, j + 1));
                }
                if (player_field[i][j - 1] == 10) {
                    cells_to_shoot.insert(std::pair<size_t, size_t>(i, j - 1));
                }
                if (player_field[i - 1][j] == 10) {
                    cells_to_shoot.insert(std::pair<size_t, size_t>(i - 1, j));
                }
                if (player_field[i + 1][j] == 10) {
                    cells_to_shoot.insert(std::pair<size_t, size_t>(i + 1, j));
                }
            }
        }
    }


public:
    Game() : mersenne(time(0)), player_counter(0), enemy_counter(0) {
        player_field.resize(12, std::vector<int>(12, 0));
        enemy_field.resize(12, std::vector<int>(12, 0));
    }

    //обрабатывает выстрел в поле игрока-пользователя с координатами (i, j)
    void MouseButtonPressed(size_t i, size_t j) {
        if (enemy_field[i][j] == 0) {
            enemy_field[i][j] = 1;
            MakeTurn();
        } else if (enemy_field[i][j] == -1) {
            enemy_field[i][j] = -2;
            MakeTurn();
        } else if (enemy_field[i][j] == 10) {
            enemy_field[i][j] = 11;
            if (IsShipDestroyed(i, j, false) && enemy_counter > 0) {
                --enemy_counter;
                SurroundDestroyedShip(i, j, false);
            }
        }
    }

    // возвращает константную ссылку на поле (двумерный вектор) игрока-пользователя
    const std::vector<std::vector<int>>& GetPlayerField() const {
        return player_field;
    }

    //возвращает константную ссылку на поле (двумерный вектор) игрока-компьютера
    const std::vector<std::vector<int>>& GetEnemyField() const {
        return enemy_field;
    }

    //проверяет, закончилась ли игра
    bool IsFinished() const {
        if (player_counter == 0 || enemy_counter == 0) {
            return true;
        } else {
            return false;
        }
    }

    //проверка: выиграл ли пользователь
    bool UserHasWon() const {
        if (enemy_counter == 0) {
            return true;
        } else {
            return false;
        }
    }

    //проверка: выиграл ли компьютер
    bool ComputerHasWon() const {
        if (player_counter == 0) {
            return true;
        } else {
            return false;
        }
    }


    /** проверяет, уничтожен ли корабль, которому принадлежит клетка (i, j)
     принимает на вход координаты (i, j) клетки поврежденного корабля
     действие производится над полем игрока, если for_player == true, иначе над полем противника*/
    bool IsShipDestroyed(size_t i, size_t j, bool for_player) {
        // field_ptr указывает на то поле, с которым мы сейчас работаем
        std::vector<std::vector<int>>* field_ptr;
        if (for_player) {
            field_ptr = &player_field;
        } else {
            field_ptr = &enemy_field;
        }

        // является ли эта клетка поврежденной клеткой корабля
        if ((*field_ptr)[i][j] != 11) {
            return false;
        }

        (*field_ptr)[i][j] = -100;

        if ((*field_ptr)[i][j + 1] == 10) {
            (*field_ptr)[i][j] = 11;
            return false;
        } else if ((*field_ptr)[i][j + 1] == 11) {
            if (!IsShipDestroyed(i, j + 1, for_player)) {
                (*field_ptr)[i][j] = 11;
                return false;
            }
        }

        if ((*field_ptr)[i][j - 1] == 10) {
            (*field_ptr)[i][j] = 11;
            return false;
        } else if ((*field_ptr)[i][j - 1] == 11) {
            if (!IsShipDestroyed(i, j - 1, for_player)) {
                (*field_ptr)[i][j] = 11;
                return false;
            }
        }

        if ((*field_ptr)[i + 1][j] == 10) {
            (*field_ptr)[i][j] = 11;
            return false;
        } else if ((*field_ptr)[i + 1][j] == 11) {
            if (!IsShipDestroyed(i + 1, j, for_player)) {
                (*field_ptr)[i][j] = 11;
                return false;
            }
        }

        if ((*field_ptr)[i - 1][j] == 10) {
            (*field_ptr)[i][j] = 11;
            return false;
        } else if ((*field_ptr)[i - 1][j] == 11) {
            if (!IsShipDestroyed(i - 1, j, for_player)) {
                (*field_ptr)[i][j] = 11;
                return false;
            }
        }

        (*field_ptr)[i][j] = 11;
        return true;
    }

    /** проверяет, возможно ли разместить корабль размера size так, чтобы его верхняя левая клетка имела координаты (i, j)
     если is_horizontal == true, то проверяется возможность поставить корабль горизонтально, иначе - вертикально
     действие производится над полем игрока, если for_player == true, иначе над полем противника*/
    bool PossibleToPlaceShip(size_t i, size_t j, size_t size, bool is_horizontal, bool for_player) {
        // field_ptr указывает на то поле, с которым мы сейчас работаем
        std::vector<std::vector<int>>* field_ptr;
        if (for_player) {
            field_ptr = &player_field;
        } else {
            field_ptr = &enemy_field;
        }

        // лежат ли обе координаты в отрезке [1, 10]
        if (i < 1 || 10 < i || j < 1 || 10 < j) {
            return false;
        }

        if (is_horizontal) {
            if (j > 11 - size) {
                return false;
            }
            bool res = true;
            for (size_t k = 0; k < size; ++k) {
                if ((*field_ptr)[i][j + k] != 0) {
                    res = false;
                }
            }
            return res;
        } else {
            if (i > 11 - size) {
                return false;
            }
            bool res = true;
            for (size_t k = 0; k < size; ++k) {
                if ((*field_ptr)[i + k][j] != 0) {
                    res = false;
                }
            }
            return res;
        }
    }

    /** размещает корабль размера size так, чтобы его верхняя левая клетка имела координаты (i, j)
     если is_horizontal == true, то корабль размещается горизонтально, иначе - вертикально
     действие производится над полем игрока, если for_player == true, иначе над полем противника*/
    void PlaceShip(size_t i, size_t j, size_t size, bool is_horizontal, bool for_player) {
        // field_ptr указывает на то поле, с которым мы сейчас работаем
        std::vector<std::vector<int>>* field_ptr;
        if (for_player) {
            field_ptr = &player_field;
        } else {
            field_ptr = &enemy_field;
        }

        if (PossibleToPlaceShip(i, j, size, is_horizontal, for_player)) {
            if (is_horizontal) {
                // размещаем корабль
                for (size_t k = 0; k < size; ++k) {
                    (*field_ptr)[i][j + k] = 10;
                }

                // размечаем клетки вокруг него
                (*field_ptr)[i][j - 1] = -1;
                (*field_ptr)[i][j + size] = -1;
                for (size_t k = 0; k < size + 2; ++k) {
                    (*field_ptr)[i - 1][j - 1 + k] = -1;
                    (*field_ptr)[i + 1][j - 1 + k] = -1;
                }
            } else {
                // размещаем корабль
                for (size_t k = 0; k < size; ++k) {
                    (*field_ptr)[i + k][j] = 10;
                }

                // размечаем клетки вокруг него
                (*field_ptr)[i - 1][j] = -1;
                (*field_ptr)[i + size][j] = -1;
                for (size_t k = 0; k < size + 2; ++k) {
                    (*field_ptr)[i - 1 + k][j - 1] = -1;
                    (*field_ptr)[i - 1 + k][j + 1] = -1;
                }
            }
        }

        if (for_player) {
            ++player_counter;
        } else {
            ++enemy_counter;
        }
    }

    /** генерирует случайную расстановку кораблей на поле, принимает 1 аргумент:
     если for_player == true, то расставляет корабли игроку-пользователю
     иначе (for_player == false) рассталяет корабли игроку-компьютеру*/
    void GenerateRandomShipsArrangement(bool for_player) {
        size_t i, j;

        // размещаем 4-палубник
        if (mersenne() % 2 == 0) {
            // размещаем горизонтально
            i = mersenne() % 10 + 1;
            j = mersenne() % 7 + 1;
            PlaceShip(i, j, 4, true, for_player);
        } else {
            // размещаем вертикально
            j = mersenne() % 10 + 1;
            i = mersenne() % 7 + 1;
            PlaceShip(i, j, 4, false, for_player);
        }

        // размещаем 3-палубники
        for (size_t ship = 0; ship < 2; ++ship) {
            if (mersenne() % 2 == 0) {
                // размещаем горизонтально
                i = mersenne() % 10 + 1;
                j = mersenne() % 8 + 1;
                while (!PossibleToPlaceShip(i, j, 3, true, for_player)) {
                    i = mersenne() % 10 + 1;
                    j = mersenne() % 8 + 1;
                }
                PlaceShip(i, j, 3, true, for_player);
            } else {
                //размещаем вертикально
                j = mersenne() % 10 + 1;
                i = mersenne() % 8 + 1;
                while (!PossibleToPlaceShip(i, j, 3, false, for_player)) {
                    j = mersenne() % 10 + 1;
                    i = mersenne() % 8 + 1;
                }
                PlaceShip(i, j, 3, false, for_player);
            }
        }

        // размещаем 2-палубники
        for (size_t ship = 0; ship < 3; ++ship) {
            if (mersenne() % 2 == 0) {
                // размещаем горизонтально
                i = mersenne() % 10 + 1;
                j = mersenne() % 9 + 1;
                while (!PossibleToPlaceShip(i, j, 2, true, for_player)) {
                    i = mersenne() % 10 + 1;
                    j = mersenne() % 9 + 1;
                }
                PlaceShip(i, j, 2, true, for_player);
            } else {
                // размещаем вертикально
                j = mersenne() % 10 + 1;
                i = mersenne() % 9 + 1;
                while (!PossibleToPlaceShip(i, j, 2, false, for_player)) {
                    j = mersenne() % 10 + 1;
                    i = mersenne() % 9 + 1;
                }
                PlaceShip(i, j, 2, false, for_player);
            }
        }

        // размещаем 1-палубники
        for (size_t ship = 0; ship < 4; ++ship) {
            i = mersenne() % 10 + 1;
            j = mersenne() % 10 + 1;
            while (!PossibleToPlaceShip(i, j, 1, true, for_player)) {
                i = mersenne() % 10 + 1;
                j = mersenne() % 10 + 1;
            }
            PlaceShip(i, j, 1, true, for_player);
        }
    }

    
    /** следующие функции нужны только для тестирования и не для чего больше
     возвращает значение, записанное в клетке (i, j) поля игрока-пользователя, если for_player == true, 
     иначе - поля игрока-компьютера*/
    int GetCellValue(size_t i, size_t j, bool for_player) const {
        assert(i >= 1 && i <= 10 && j >= 1 && j <= 10);  /** лежат ли обе координаты в отрезке [1, 10]*/ 
        
        if (for_player) {
            return player_field[i][j];
        } else {
            return enemy_field[i][j];
        }
    }
};