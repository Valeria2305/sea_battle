#include "game.h"
#include "doctest.h"

TEST_CASE("function PlaceShip() test") {
    Game g;
    g.PlaceShip(1, 1, 4, true, false);
    CHECK(g.GetCellValue(1, 1, false) == 10);
    CHECK(g.GetCellValue(1, 2, false) == 10);
    CHECK(g.GetCellValue(1, 3, false) == 10);
    CHECK(g.GetCellValue(1, 4, false) == 10);
    CHECK(g.GetCellValue(2, 1, false) == -1);
    CHECK(g.GetCellValue(2, 5, false) == -1);
    CHECK(g.GetCellValue(1, 5, false) == -1);
    CHECK(g.GetCellValue(2, 6, false) == 0);
    CHECK(g.GetCellValue(3, 1, false) == 0);

    g.PlaceShip(7, 10, 4, false, true);
    CHECK(g.GetCellValue(7, 10, true) == 10);
    CHECK(g.GetCellValue(8, 10, true) == 10);
    CHECK(g.GetCellValue(9, 10, true) == 10);
    CHECK(g.GetCellValue(10, 10, true) == 10);
    CHECK(g.GetCellValue(6, 10, true) == -1);
    CHECK(g.GetCellValue(6, 9, true) == -1);
    CHECK(g.GetCellValue(7, 9, true) == -1);
    CHECK(g.GetCellValue(10, 9, true) == -1);
    CHECK(g.GetCellValue(10, 8, true) == 0);
    CHECK(g.GetCellValue(7, 8, true) == 0);
}

TEST_CASE("function MouseButtonPressed() test") {
    Game g;
    g.PlaceShip(10, 1, 2, true, false);

    CHECK(g.GetCellValue(8, 3, false) == 0);
    g.MouseButtonPressed(8, 3);
    CHECK(g.GetCellValue(8, 3, false) == 1);

    CHECK(g.GetCellValue(9, 2, false) == -1);
    g.MouseButtonPressed(9, 2);
    CHECK(g.GetCellValue(9, 2, false) == -2);

    CHECK(g.GetCellValue(9, 3, false) == -1);
    g.MouseButtonPressed(9, 3);
    CHECK(g.GetCellValue(9, 3, false) == -2);

    CHECK(g.GetCellValue(10, 2, false) == 10);
    g.MouseButtonPressed(10, 2);
    CHECK(g.GetCellValue(10, 2, false) == 11);
}

TEST_CASE("function IsShipDestroyed() test") {
    Game g;
    g.PlaceShip(3, 2, 1, false, false);
    g.MouseButtonPressed(3, 2);
    CHECK(g.IsShipDestroyed(3, 2, false) == true);

    g.PlaceShip(3, 5, 3, false, false);
    g.MouseButtonPressed(4, 5);
    CHECK(g.IsShipDestroyed(4, 5, false) == false);
    g.MouseButtonPressed(5, 5);
    CHECK(g.IsShipDestroyed(5, 5, false) == false);
    g.MouseButtonPressed(3, 5);
    CHECK(g.IsShipDestroyed(3, 5, false) == true);
    CHECK(g.IsShipDestroyed(4, 5, false) == true);
    CHECK(g.IsShipDestroyed(5, 5, false) == true);
}

TEST_CASE("function SurroundDestroyedShip() test") {
    Game g;
    g.PlaceShip(5, 5, 1, false, false);
    CHECK(g.GetCellValue(4, 4, false) == -1);
    CHECK(g.GetCellValue(5, 5, false) == 10);
    g.MouseButtonPressed(4, 4);
    CHECK(g.GetCellValue(4, 4, false) == -2);
    g.MouseButtonPressed(5, 5);
    CHECK(g.GetCellValue(5, 5, false) == 11);
    CHECK(g.GetCellValue(4, 4, false) == -2);
    CHECK(g.GetCellValue(4, 5, false) == -2);
    CHECK(g.GetCellValue(4, 6, false) == -2);
    CHECK(g.GetCellValue(5, 4, false) == -2);
    CHECK(g.GetCellValue(5, 6, false) == -2);
    CHECK(g.GetCellValue(6, 4, false) == -2);
    CHECK(g.GetCellValue(6, 5, false) == -2);
    CHECK(g.GetCellValue(6, 6, false) == -2);

    g.PlaceShip(8, 2, 3, true, false);
    g.MouseButtonPressed(8, 2);
    g.MouseButtonPressed(8, 3);
    g.MouseButtonPressed(8, 4);
    CHECK(g.IsShipDestroyed(8, 3, false) == true);
    CHECK(g.GetCellValue(7, 1, false) == -2);
    CHECK(g.GetCellValue(8, 1, false) == -2);
    CHECK(g.GetCellValue(9, 1, false) == -2);
    CHECK(g.GetCellValue(7, 2, false) == -2);
    CHECK(g.GetCellValue(9, 2, false) == -2);
    CHECK(g.GetCellValue(7, 3, false) == -2);
    CHECK(g.GetCellValue(9, 3, false) == -2);
    CHECK(g.GetCellValue(7, 4, false) == -2);
    CHECK(g.GetCellValue(9, 4, false) == -2);
    CHECK(g.GetCellValue(7, 5, false) == -2);
    CHECK(g.GetCellValue(8, 5, false) == -2);
    CHECK(g.GetCellValue(9, 5, false) == -2);
}

TEST_CASE("function IsFinished() test") {
    Game g;
    CHECK(g.IsFinished() == true);
    g.PlaceShip(3, 2, 1, false, false);
    CHECK(g.IsFinished() == true);
    g.PlaceShip(7, 3, 4, false, true);
    CHECK(g.IsFinished() == false);
    g.MouseButtonPressed(3, 2);
    CHECK(g.IsFinished() == true);
}

TEST_CASE("functions UserHasWon() and ComputerHasWon() test") {
    Game g;
    CHECK(g.IsFinished() == true);
    CHECK(g.UserHasWon() == true);
    CHECK(g.ComputerHasWon() == true);
    g.PlaceShip(1, 5, 2, true, false);
    CHECK(g.ComputerHasWon() == true);
    CHECK(g.UserHasWon() == false);
    g.PlaceShip(7, 3, 4, false, true);
    CHECK(g.UserHasWon() == false);
    CHECK(g.ComputerHasWon() == false);
    CHECK(g.IsFinished() == false);

    g.MouseButtonPressed(1, 5);
    g.MouseButtonPressed(1, 6);
    CHECK(g.IsShipDestroyed(1, 5, false) == true);
    CHECK(g.UserHasWon() == true);
    CHECK(g.ComputerHasWon() == false);


    g.PlaceShip(4, 4, 2, true, false);
    g.PlaceShip(8, 4, 1, true, false);
    CHECK(g.UserHasWon() == false);
    CHECK(g.ComputerHasWon() == false);

    g.MouseButtonPressed(4, 4);
    g.MouseButtonPressed(4, 5);
    CHECK(g.IsShipDestroyed(4, 5, false) == true);
    CHECK(g.UserHasWon() == false);
    CHECK(g.ComputerHasWon() == false);
    g.MouseButtonPressed(8, 4);
    CHECK(g.UserHasWon() == true);
    CHECK(g.ComputerHasWon() == false);
}

TEST_CASE("function PossibleToPlaceShip() test") {
    Game g;
    CHECK(g.PossibleToPlaceShip(7, 10, 4, false, true) == true);
    CHECK(g.PossibleToPlaceShip(1, 1, 4, true, true) == true);
    CHECK(g.PossibleToPlaceShip(9, 7, 3, false, true) == false);
    CHECK(g.PossibleToPlaceShip(7, 10, 2, true, false) == false);

    CHECK(g.PossibleToPlaceShip(0, 5, 2, true, false) == false);
    CHECK(g.PossibleToPlaceShip(-10, 1, 2, true, false) == false);
    CHECK(g.PossibleToPlaceShip(11, 10, 2, false, true) == false);

    g.PlaceShip(3, 3, 3, false, false);
    CHECK(g.PossibleToPlaceShip(3, 4, 1, true, false) == false);
    CHECK(g.PossibleToPlaceShip(2, 3, 1, false, false) == false);
    CHECK(g.PossibleToPlaceShip(2, 1, 3, true, false) == false);
    CHECK(g.PossibleToPlaceShip(4, 2, 4, true, false) == false);
    CHECK(g.PossibleToPlaceShip(6, 4, 2, false, false) == false);

    CHECK(g.PossibleToPlaceShip(1, 2, 4, true, false) == true);
    CHECK(g.PossibleToPlaceShip(3, 1, 4, false, false) == true);
    CHECK(g.PossibleToPlaceShip(4, 5, 2, false, false) == true);
    CHECK(g.PossibleToPlaceShip(7, 2, 3, true, false) == true);
}