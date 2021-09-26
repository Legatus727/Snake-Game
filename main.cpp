#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include <cstdlib>
#include <list>
#include <vector>
using namespace std;

enum Direction { UP, DOWN, LEFT, RIGHT, YET_TO_BEGIN, NUMBER_OF_DIRECTIONS };
enum Tile_type { EMPTY, SNAKE_HEAD, SNAKE_BODY, FRUIT, NUMBER_OF_TILE_TYPES };
const float SQUARE_PIXELS = 20.0;
const int X_SIZE = 20;
const int Y_SIZE = 20;

class Index_out_of_bounds {};
class Tile;
class SnakeHead;

class World
{
public:
    World(int numPlayers, int x = X_SIZE, int y = Y_SIZE, int fruit = rand() % 6);
    ~World();
    void display(sf::RenderWindow& window);
    void runGame(int difficulty, bool& runGame);
    void spawnFruit();
    void eatFruit() { numFruit--; }
    void killSnake();
    bool allSnakesDied();
    void playAgainPrompt(bool& runGame);
    void turn(Direction direction, bool& p1Dead, Direction playerTwoDirection, bool& p2Dead);
    vector<Tile*>& operator[](int index);
    const vector<Tile*>& operator[](int index) const;
    int get_width() { return grid.size(); }

private:
    vector<vector<Tile*>> grid;
    vector<Tile*> snakeLocation;
    int numFruit;
    int numPlayers;
    vector<bool> snakesAlive;
};

class Tile
{
public:
    Tile(int x = 0, int y = 0, float radius = SQUARE_PIXELS, size_t point_count = 30) : x(x), y(y), shape(radius, point_count) { }
    virtual void display(sf::RenderWindow& window) = 0;
    virtual Tile_type who() = 0;
    virtual void turn(World& w, Direction direction, bool& dead) {};
    int getScore() { return score; }
    void incrementScore() { score++; }
    static void tile_swap(Tile*& pTile1, Tile*& pTile2);

protected:
    int x;
    int y;
    int score;
    sf::CircleShape shape;
    void fix_shape_position();
};

class Snake : public Tile
{
public:
    Snake(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 30)
    {
        shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
    }
    virtual void move(World& w, Direction direction) {};
};

class SnakeBody : public Snake
{
public:
    SnakeBody(int x = 0, int y = 0) : Snake(x, y)
    {
        shape.setFillColor(sf::Color::Red);
    }
    virtual void display(sf::RenderWindow& window) { window.draw(shape); }
    virtual Tile_type who() { return SNAKE_BODY; }
};

class SnakeHead : public Snake
{
public:
    SnakeHead(int x = 0, int y = 0) : Snake(x, y)
    {
        shape.setFillColor(sf::Color::Green);
        vector<int> bodyGrid;
        bodyGrid.push_back(x);
        bodyGrid.push_back(y);
        snakeBody.push_back(bodyGrid);
    }
    virtual void display(sf::RenderWindow& window) { window.draw(shape); }
    virtual void turn(World& w, Direction direction, bool& dead);
    virtual Tile* newSnakeBody(int x, int y)
    {
        vector<int> bodyGrid;
        bodyGrid.push_back(x);
        bodyGrid.push_back(y);
        snakeBody.push_back(bodyGrid);

        return new SnakeBody(x, y);
    }
    // virtual void move(World& w, Direction direction);
    virtual Tile_type who() { return SNAKE_HEAD; }
protected:
    void die(World& w);
    list<vector<int>> snakeBody;
};

class Fruit : public Tile
{
public:
    Fruit(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 3)
    {
        shape.setFillColor(sf::Color::Blue);
        shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
    }
    virtual void display(sf::RenderWindow& window) { window.draw(shape); }
    virtual Tile_type who() { return FRUIT; }
};

class Empty : public Tile
{
public:
    Empty(int x = 0, int y = 0) : Tile(x, y, (SQUARE_PIXELS - 2) / 2, 3)
    {
        shape.setFillColor(sf::Color::White);
        shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
    }
    virtual void display(sf::RenderWindow& window) {}
    virtual Tile_type who() { return EMPTY; }
};

void displayWelcomeScreen(int& numPlayers);
void displayDifficultySelection(int& difficulty);
string myITOA(int given);

int main()
{
    srand(time(0));
    int numPlayers = 1;
    int difficulty = 0;
    bool runGame = true;

    while (runGame)
    {
        displayWelcomeScreen(numPlayers);
        displayDifficultySelection(difficulty);

        World w(numPlayers);

        w.runGame(difficulty, runGame);
    }

    return 0;
}

void displayWelcomeScreen(int& numPlayers)
{
    sf::RenderWindow window(sf::VideoMode(400, 400), "Snake");
    sf::Text welcome;
    sf::Font font;
    sf::Text instructions;
    int count = 0;

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Font file not located on Computer" << endl;
        exit(1);
    }

    welcome.setFont(font);
    welcome.setString("\t\t\t\t\tSNAKE GAME\n\n\t\t\t\t\tENTER NUMBER\n\tOF PLAYERS ON KEYBOARD (1/2)");
    welcome.setCharacterSize(20);
    welcome.setFillColor(sf::Color::White);
    welcome.setStyle(sf::Text::Bold);

    instructions.setFont(font);
    instructions.setString("\n\n\n\n\n\n\n\n\n\t\t\t***GAME INSTRUCTIONS***\n\nSINGLE PLAYER: USE W,A,S,D\n\nSECOND PLAYER (IF APPLICABLE):\n\tUSE UP, DOWN,\nLEFT, RIGHT ARROWS");
    instructions.setCharacterSize(15);
    instructions.setFillColor(sf::Color::White);
    instructions.setStyle(sf::Text::Bold);

    window.isOpen();
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num1) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad1))
            {
                numPlayers = 1;
                window.close();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Num2) || sf::Keyboard::isKeyPressed(sf::Keyboard::Numpad2))
            {
                numPlayers = 2;
                window.close();
            }
        }
        window.clear();
        window.draw(welcome);
        window.draw(instructions);
        window.display();

    }
}

void displayDifficultySelection(int& difficulty)
{
    sf::RenderWindow window(sf::VideoMode(400, 400), "Snake");
    sf::Text difficultyMessage;
    sf::Font font;
    sf::Text instructions;
    int count = 0;

    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        cout << "Font file not located on Computer" << endl;
        exit(1);
    }

    difficultyMessage.setFont(font);
    difficultyMessage.setString("\t\t****DIFFICULTY SELECTION****\n\n\n\tEasy (E): You will probably be bored.\n\n\tMedium (M): Enough for the average person\n\n\tHard (H): A true challenge\n\n\tInsane (I): You will not survive");
    difficultyMessage.setCharacterSize(20);
    difficultyMessage.setFillColor(sf::Color::White);
    difficultyMessage.setStyle(sf::Text::Bold);


    window.isOpen();
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            {
                difficulty = 100000;
                window.close();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::M))
            {
                difficulty = 40000;
                window.close();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::H))
            {
                difficulty = 25000;
                window.close();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::I))
            {
                difficulty = 15000;
                window.close();
            }
        }
        window.clear();
        window.draw(difficultyMessage);
        window.display();

    }
}

World::World(int numPlayers, int x, int y, int fruit)
{
    vector<Tile*> column(y);
    numFruit = fruit;
    World::numPlayers = numPlayers;

    for (int i = 0; i < numPlayers; i++)
    {
        snakesAlive.push_back(true);
    }

    for (int i = 0; i < x; i++)
    {
        grid.push_back(column);
    }

    for (int i = 0; i < x; i++)
    {
        for (int j = 0; j < y; j++)
        {
            if (fruit > 0)
            {
                grid[i][j] = new Fruit(i, j);
                fruit--;
            }
            else if (numPlayers > 0)
            {
                grid[i][j] = new SnakeHead(i, j);
                snakeLocation.push_back(grid[i][j]);
                numPlayers--;
            }
            else
            {
                grid[i][j] = new Empty(i, j);
            }
        }
    }

    int x1, x2;
    int y1, y2;

    for (int i = 0; i < 50000; i++)
    {
        x1 = rand() % x;
        x2 = rand() % x;
        y1 = rand() % y;
        y2 = rand() % y;
        Tile::tile_swap(grid[x1][y1], grid[x2][y2]);
    }
}

World::~World()
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
        {
            delete grid[i][j];
        }
    }
}

void World::runGame(int difficulty, bool& runGame)
{
    sf::RenderWindow window(sf::VideoMode(400, 400), "Snake");
    Direction lastDirection = YET_TO_BEGIN;
    Direction playerTwoLastDirection = YET_TO_BEGIN;
    bool p1Dead = false;
    bool p2Dead = false;

    window.isOpen();
    window.clear();
    turn(lastDirection, p1Dead, playerTwoLastDirection, p2Dead);
    display(window);
    window.display();
    int count = 0;
    int fruitCount = 0;

    while (window.isOpen())
    {
        sf::Event event;

        if (numFruit <= 1)
        {
            spawnFruit();
            fruitCount = 0;
        }
        else if (fruitCount > 10)
        {
            if (numFruit < 6)
                spawnFruit();
            fruitCount = 0;
        }

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
                lastDirection = UP;

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
                lastDirection = LEFT;

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                lastDirection = RIGHT;

            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
                lastDirection = DOWN;

            if (numPlayers > 1)
            {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up))
                    playerTwoLastDirection = UP;

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left))
                    playerTwoLastDirection = LEFT;

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right))
                    playerTwoLastDirection = RIGHT;

                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down))
                    playerTwoLastDirection = DOWN;
            }
        }

        if (count > difficulty)
        {
            window.clear();
            turn(lastDirection, p1Dead, playerTwoLastDirection, p2Dead);
            display(window);
            window.display();

            count = 0;
            fruitCount++;
        }

        if (allSnakesDied())
        {
            window.close();
        }
        count++;
    }

    playAgainPrompt(runGame);
}

void World::playAgainPrompt(bool& runGame)
{
    sf::RenderWindow window(sf::VideoMode(400, 400), "Snake");
    sf::Text welcome;
    sf::Font font;
    sf::Text instructions;
    string scorePlayer1;
    string scorePlayer2;
    scorePlayer1 = myITOA(snakeLocation[0]->getScore());


    if (!font.loadFromFile("C:/Windows/Fonts/arial.ttf"))
    {
        std::cout << "Font file not located on Computer" << endl;
        exit(1);
    }

    if (numPlayers == 1)
    {
        welcome.setFont(font);
        welcome.setString("\n\t\t\t\tPlayer Score: " + scorePlayer1 + "\n\n\n\n\t\t\t *** GAME OVER ***\n\n\n\n\t\t\t   Play again? (Y/N)");
        welcome.setCharacterSize(20);
        welcome.setFillColor(sf::Color::White);
        welcome.setStyle(sf::Text::Bold);
        welcome.setFont(font);
    }
    else
    {
        scorePlayer2 = myITOA(snakeLocation[1]->getScore());
        welcome.setFont(font);
        welcome.setString("\n\t\tPlayer 1 (WASD) Score: " + scorePlayer1 + "\n\n\t\tPlayer 2 (Arrows) Score: " + scorePlayer2 + "\n\n\n\n\t\t\t *** GAME OVER ***\n\n\n\n\t\t\t   Play again? (Y/N)");
        welcome.setCharacterSize(20);
        welcome.setFillColor(sf::Color::White);
        welcome.setStyle(sf::Text::Bold);
        welcome.setFont(font);
    }
    window.isOpen();
    while (window.isOpen())
    {
        sf::Event event;

        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Y))
            {
                runGame = true;
                window.close();
            }
            else if (sf::Keyboard::isKeyPressed(sf::Keyboard::N))
            {
                runGame = false;
                window.close();
            }
        }
        window.clear();
        window.draw(welcome);
        window.draw(instructions);
        window.display();

    }
}

void World::killSnake()
{
    if (numPlayers > 1)
    {
        if (snakesAlive[0] == true)
            snakesAlive[0] = false;

        else
            snakesAlive[1] = false;
    }
    else
        snakesAlive[0] = false;
}

bool World::allSnakesDied()
{
    bool allSnakesDied = false;

    if (numPlayers == 1)
        allSnakesDied = !snakesAlive[0];
    else
        if (snakesAlive[0] == false && snakesAlive[1] == false)
            allSnakesDied = true;

    return allSnakesDied;
}

void World::spawnFruit()
{
    int numFruitToSpawn = rand() % (6 - numFruit);


    if (!(numFruit >= 6))
    {
        for (int i = 0; i < numFruitToSpawn; i++)
        {
            int x, y;
            x = rand() % X_SIZE;
            y = rand() % Y_SIZE;
            while (grid[x][y]->who() != EMPTY)
            {
                x = rand() % X_SIZE;
                y = rand() % Y_SIZE;
            }
            grid[x][y] = new Fruit(x, y);
        }
        numFruit += numFruitToSpawn;
    }

}

void World::turn(Direction direction, bool& p1Dead, Direction playerTwoDirection, bool& p2Dead)
{
    for (int i = 0; i <= snakeLocation.size() - 1; i++)
    {
        if (numPlayers == 1)
        {
            if (!p1Dead)
            {
                snakeLocation[i]->turn(*this, direction, p1Dead);
            }
        }
        else
        {
            if (i == 0)
            {
                if (!p1Dead)
                {
                    snakeLocation[i]->turn(*this, direction, p1Dead);
                }
            }
            else
            {
                if (!p2Dead)
                {
                    snakeLocation[i]->turn(*this, playerTwoDirection, p2Dead);
                }
            }
        }
    }
}

void World::display(sf::RenderWindow& window)
{
    for (int i = 0; i < grid.size(); i++)
    {
        for (int j = 0; j < grid[i].size(); j++)
            grid[i][j]->display(window);
    }
}

vector<Tile*>& World::operator[](int index)
{
    if (index >= grid.size())
    {
        throw Index_out_of_bounds();
    }
    return grid[index];
}

const vector<Tile*>& World::operator[](int index) const
{
    if (index >= grid.size())
        throw Index_out_of_bounds();

    return grid[index];
}

string myITOA(int given)
{
    string s = "";
    while (given)
    {
        switch (given % 10)
        {
        case 0:
            s = "0" + s;
            break;
        case 1:
            s = "1" + s;
            break;
        case 2:
            s = "2" + s;
            break;
        case 3:
            s = "3" + s;
            break;
        case 4:
            s = "4" + s;
            break;
        case 5:
            s = "5" + s;
            break;
        case 6:
            s = "6" + s;
            break;
        case 7:
            s = "7" + s;
            break;
        case 8:
            s = "8" + s;
            break;
        case 9:
            s = "9" + s;
            break;
        }
        given /= 10;
    }
    return s;
}

void Tile::tile_swap(Tile*& pTile1, Tile*& pTile2)
{
    swap(pTile1->x, pTile2->x);
    swap(pTile1->y, pTile2->y);
    swap(pTile1, pTile2);
    pTile1->fix_shape_position();
    pTile2->fix_shape_position();


}

void Tile::fix_shape_position()
{
    shape.setPosition(sf::Vector2f(x * SQUARE_PIXELS, y * SQUARE_PIXELS));
}

void SnakeHead::turn(World& w, Direction direction, bool& dead)
{
    // check type and move by tile swapping in that direction
    // if wall is encounted, die
    // if fruit is encountered delete fruit and create body behind self

    switch (direction)
    {
    case UP:
        if (y <= 0 || w[x][y - 1]->who() == SNAKE_BODY || w[x][y - 1]->who() == SNAKE_HEAD)
        {
            // die
            w.killSnake();
            w[x][y] = new Empty(x, y);
            dead = true;
        }
        else if (w[x][y - 1]->who() == FRUIT)
        {
            // move head to fruit, spawn body in previous head position
            Tile::tile_swap(w[x][y], w[x][y - 1]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;
            w[x][y + 1] = newSnakeBody(x, y + 1);
            w.eatFruit();
            incrementScore();

            if (snakeBody.size() > 2)
            {
                list<vector<int>>::iterator p = snakeBody.begin();
                advance(p, 1);
                snakeBody.insert(p, snakeBody.back());
                snakeBody.pop_back();
            }
        }
        else if (w[x][y - 1]->who() == EMPTY)
        {
            // move in UP direction
            // swap head with next tile, move last body in previous head position

            Tile::tile_swap(w[x][y], w[x][y - 1]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;

            if (snakeBody.size() > 1)
            {
                // Change pointers, head->next needs to be current tail and tail->previous needs to be new tail
                Tile::tile_swap(w[snakeBody.back()[0]][snakeBody.back()[1]], w[x][y + 1]);
                snakeBody.back()[0] = x;
                snakeBody.back()[1] = y + 1;

                if (snakeBody.size() > 2)
                {
                    list<vector<int>>::iterator p = snakeBody.begin();
                    advance(p, 1);
                    snakeBody.insert(p, snakeBody.back());
                    snakeBody.pop_back();
                }
            }
        }
        break;

    case DOWN:
        if (y >= w[x].size() - 1 || w[x][y + 1]->who() == SNAKE_BODY || w[x][y + 1]->who() == SNAKE_HEAD)
        {
            //die
            w.killSnake();
            w[x][y] = new Empty(x, y);
            dead = true;
        }
        else if (w[x][y + 1]->who() == FRUIT)
        {
            // move head to fruit, spawn body in previous head position
            Tile::tile_swap(w[x][y], w[x][y + 1]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;
            w[x][y - 1] = newSnakeBody(x, y - 1);
            w.eatFruit();
            incrementScore();

            if (snakeBody.size() > 2)
            {
                list<vector<int>>::iterator p = snakeBody.begin();
                advance(p, 1);
                snakeBody.insert(p, snakeBody.back());
                snakeBody.pop_back();
            }
        }
        else if (w[x][y + 1]->who() == EMPTY)
        {
            // move in UP direction
            // swap head with next tile, move last body in previous head position
            Tile::tile_swap(w[x][y], w[x][y + 1]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;

            if (snakeBody.size() > 1)
            {
                // Change pointers, head->next needs to be current tail and tail->previous needs to be new tail
                Tile::tile_swap(w[snakeBody.back()[0]][snakeBody.back()[1]], w[x][y - 1]);
                snakeBody.back()[0] = x;
                snakeBody.back()[1] = y - 1;

                if (snakeBody.size() > 2)
                {
                    list<vector<int>>::iterator p = snakeBody.begin();
                    advance(p, 1);
                    snakeBody.insert(p, snakeBody.back());
                    snakeBody.pop_back();
                }
            }
        }
        break;

    case LEFT:
        if (x <= 0 || w[x - 1][y]->who() == SNAKE_BODY || w[x - 1][y]->who() == SNAKE_HEAD)
        {
            //die
            w.killSnake();
            w[x][y] = new Empty(x, y);
            dead = true;
        }
        else if (w[x - 1][y]->who() == FRUIT)
        {
            //swap head with fruit, replace fruit with body
            Tile::tile_swap(w[x][y], w[x - 1][y]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;
            w[x + 1][y] = newSnakeBody(x + 1, y);
            w.eatFruit();
            incrementScore();

            if (snakeBody.size() > 2)
            {
                list<vector<int>>::iterator p = snakeBody.begin();
                advance(p, 1);
                snakeBody.insert(p, snakeBody.back());
                snakeBody.pop_back();
            }
        }
        else if (w[x - 1][y]->who() == EMPTY)
        {
            //swap head with empty, swap tail with previous head spot
            Tile::tile_swap(w[x][y], w[x - 1][y]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;

            if (snakeBody.size() > 1)
            {
                // Change pointers, head->next needs to be current tail and tail->previous needs to be new tail
                Tile::tile_swap(w[snakeBody.back()[0]][snakeBody.back()[1]], w[x + 1][y]);
                snakeBody.back()[0] = x + 1;
                snakeBody.back()[1] = y;

                if (snakeBody.size() > 2)
                {
                    list<vector<int>>::iterator p = snakeBody.begin();
                    advance(p, 1);
                    snakeBody.insert(p, snakeBody.back());
                    snakeBody.pop_back();
                }
            }
        }
        break;
    case RIGHT:
        if (x >= w.get_width() - 1 || w[x + 1][y]->who() == SNAKE_BODY || w[x + 1][y]->who() == SNAKE_HEAD)
        {
            //die
            w.killSnake();
            w[x][y] = new Empty(x, y);
            dead = true;
        }
        else if (w[x + 1][y]->who() == FRUIT)
        {
            //swap head with fruit, replace fruit with body
            Tile::tile_swap(w[x][y], w[x + 1][y]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;
            w[x - 1][y] = newSnakeBody(x - 1, y);
            w.eatFruit();
            incrementScore();

            if (snakeBody.size() > 2)
            {
                list<vector<int>>::iterator p = snakeBody.begin();
                advance(p, 1);
                snakeBody.insert(p, snakeBody.back());
                snakeBody.pop_back();
            }
        }
        else if (w[x + 1][y]->who() == EMPTY)
        {
            //swap head with empty, swap tail with previous head spot
            Tile::tile_swap(w[x][y], w[x + 1][y]);
            snakeBody.front()[0] = x;
            snakeBody.front()[1] = y;

            if (snakeBody.size() > 1)
            {
                // Change pointers, head->next needs to be current tail and tail->previous needs to be new tail
                Tile::tile_swap(w[snakeBody.back()[0]][snakeBody.back()[1]], w[x - 1][y]);
                snakeBody.back()[0] = x - 1;
                snakeBody.back()[1] = y;

                if (snakeBody.size() > 2)
                {
                    list<vector<int>>::iterator p = snakeBody.begin();
                    advance(p, 1);
                    snakeBody.insert(p, snakeBody.back());
                    snakeBody.pop_back();
                }
            }
        }
        break;
    }
}