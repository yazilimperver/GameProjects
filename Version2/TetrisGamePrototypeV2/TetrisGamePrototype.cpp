/**
* \file    TetrisGamePrototype.h
* \date    2018/02/22
* \author  developer
* \brief   The example SFML tetris game
*/
#include <iostream>
using namespace std;

#include <TetrisGame.h>

int main()
{
    TetrisGame game;
    
    if (true == game.Initialize())
    {
        game.Run();
    }
    else
    {
        cout << "Tetris oyununun ilklendirilmesinde hata!" << endl;
    }
    return 0;
}

