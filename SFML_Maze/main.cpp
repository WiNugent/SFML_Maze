#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


#define PMARGIN 1
#define inf 999999
#define MAXNEIGHBOURS 4
#define DELAY 100

/*

SPACE:  O   0
WALL:   #   1
MOUSE:  M   2
END:    E   3

*/


enum Maze {
    SPACE,
    WALL,
    MOUSE,
    END,
    OPENLIST,
    CLOSELIST,
    PATH
};

enum Dir {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

std::pair<int, int> endCoords;

class MazeNode {
private:
    int H; // distance from end node

public:
    MazeNode(Maze nodetype, int id) {
        this->nodetype = nodetype;
        this->id = id;
        this->G = 0;
        this->H = inf;
    }
    void setH(int i, int j) {
        this->H = fabs(i - endCoords.first) + fabs(j - endCoords.second);
    }
    int G; // distance from start node
    void setG(int val) { this->G = val; }
    int getH() { return H; }
    int getF() { return G + H; }

    int id = -1;
    Maze nodetype;
    MazeNode* connection = NULL;
    MazeNode* neighbours[4] = {NULL, NULL, NULL, NULL}; // UP, DOWN, RIGHT, LEFT

    bool operator==(MazeNode otherNode) { return id == otherNode.id; }

};

std::vector<std::vector<MazeNode>> grid{}; // 2D vector for the grid
int maxSize = 0; // Grid is square shaped, find it's largest between both dimensions
int newid = 0; // ID to be assigned to each piece
MazeNode* startNode;
MazeNode* endNode;


// Functions
void setColour(sf::RectangleShape& shape, int val);
void readGrid();
bool isValid(int i, int j);
void getAstar(sf::RenderWindow& window, sf::RectangleShape box, int winHoriz, int winVert);
void update_display(sf::RenderWindow &window, sf::RectangleShape box, int winHoriz, int winVert);
void setPath(sf::RenderWindow& window, sf::RectangleShape box, int winHoriz, int winVert);

// RETRY


int main() {
    readGrid();



    // create the window
    sf::RenderWindow window(sf::VideoMode(600, 600), "My window");

    

    const int winHoriz = window.getSize().x;
    const int winVert = window.getSize().y;
    sf::RectangleShape box(sf::Vector2f(winHoriz / maxSize - PMARGIN, winVert / maxSize - PMARGIN));
    box.setFillColor(sf::Color::White);


    

    // clear the window with black color
    window.clear(sf::Color::Black);

    sf::sleep(sf::milliseconds(10000));

    getAstar(window, box, winHoriz, winVert);


    while (window.isOpen()) {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }
    }

    return 0;
}

void setColour(sf::RectangleShape& shape, int val) {
    switch (val) {
    case Maze::SPACE: shape.setFillColor(sf::Color::White); break;            // SPACE
    case Maze::WALL: shape.setFillColor(sf::Color(125, 125, 125)); break;    // WALL
    case Maze::MOUSE: shape.setFillColor(sf::Color::Green); break;            // MOUSE
    case Maze::END: shape.setFillColor(sf::Color::Red); break;              // END
    case Maze::OPENLIST: shape.setFillColor(sf::Color(255, 200, 200)); break;              // OPENLIST
    case Maze::CLOSELIST: shape.setFillColor(sf::Color::Yellow); break;              // CLOSELIST
    case Maze::PATH: shape.setFillColor(sf::Color(255, 125, 125)); break;              // PATH
    }
}

/*
Read the TXT file and generate a grid
*/
void readGrid()
{
    grid.clear();

    // Put Maze.txt into the 2D Grid vector
    std::cout << "OPENING FILE\n";
    std::ifstream file("Maze.txt");
    std::string str;



    while (std::getline(file, str))
    {
        std::vector<MazeNode> tempVec{};
        for (char& c : str)
        {

            switch (c)
            {
            case 'O': tempVec.push_back(MazeNode(Maze::SPACE, newid++)); break;
            case '#': tempVec.push_back(MazeNode(Maze::WALL, newid++)); break;
            case 'M': tempVec.push_back(MazeNode(Maze::MOUSE, newid++)); break;
            case 'E': tempVec.push_back(MazeNode(Maze::END, newid++)); endCoords = {grid.size(), tempVec.size()-1}; break;
            default: std::cout << "INVALID CHARACTER"; abort;
            }
        }
        //std::cout << "\n";
        grid.push_back(tempVec);

        // get max size with width
        if (tempVec.size() > maxSize)
            maxSize = tempVec.size();
    }
    file.close();
    std::cout << "CLOSING FILE\n";

    // Check max size with height
    if (maxSize < grid.size())
        maxSize = grid.size();

    // Square-ify the grid
    while (grid.size() < maxSize) // Add new rows
        grid.push_back({});
    for (int i = 0; i < grid.size(); i++) { // For each row, make sure they are the same length
        while (grid[i].size() < maxSize)
            grid[i].push_back(MazeNode(Maze::WALL, newid++));
    }


    // Reiterate through grid and connect all the pieces that are non wall
    // [i][j] : [Row][Column]

    int x, y;

    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {

            // Get pointers to start and end
            if (grid[i][j].nodetype == Maze::MOUSE)
                startNode = &grid[i][j];
            if (grid[i][j].nodetype == Maze::END)
                endNode = &grid[i][j];

            // Also set up their H
            grid[i][j].setH(i,j);

            // UP
            y = i-1;
            x = j;
            if (isValid(y,x) && grid[y][x].nodetype != Maze::WALL) {
                grid[i][j].neighbours[0] = &grid[y][x];
            }

            // DOWN
            y = i + 1;
            x = j;
            if (isValid(y, x) && grid[y][x].nodetype != Maze::WALL) {
                grid[i][j].neighbours[1] = &grid[y][x];
            }

            // RIGHT
            y = i;
            x = j + 1;
            if (isValid(y, x) && grid[y][x].nodetype != Maze::WALL) {
                grid[i][j].neighbours[2] = &grid[y][x];
            }

            // LEFT
            y = i;
            x = j - 1;
            if (isValid(y, x) && grid[y][x].nodetype != Maze::WALL) {
                grid[i][j].neighbours[3] = &grid[y][x];
            }

        }
    }

}

// [i][j] : [Row][Column]
bool isValid(int i, int j) {
    return (i >= 0 && j >= 0 && i < maxSize && j < maxSize);
}



void getAstar(sf::RenderWindow& window, sf::RectangleShape box, int winHoriz, int winVert) {



    MazeNode* exampleNode = startNode;


    std::vector<MazeNode*> openList = { startNode };
    std::vector<MazeNode*> closeList = {};

    startNode->nodetype = Maze::OPENLIST;
    update_display(window, box, winHoriz, winVert);

    while (!openList.empty()) {

        MazeNode* currentNode = openList[0];

        // Find open node with smallest F OR equivalent but less optimistic
        for (MazeNode* t : openList) {
            if (t->getF() < currentNode->getF() || (t->getF() == currentNode->getF() && t->getH() < currentNode->getH()))
                currentNode = t;
        }

        openList.erase(std::remove(openList.begin(), openList.end(), currentNode), openList.end());
        closeList.push_back(currentNode);

        currentNode->nodetype = Maze::CLOSELIST;
        update_display(window, box, winHoriz, winVert);

        if (currentNode == endNode) {
            std::cout << "You win!";
            setPath(window, box, winHoriz, winVert);
            return;
        }

        // Loop for each neighbour
        for (MazeNode* neighbour : currentNode->neighbours) {

            if (neighbour == NULL)
                continue;

            // if in closed list, ignore and check next neighbour
            if (std::find(closeList.begin(), closeList.end(), neighbour) != closeList.end())
                continue;

            // If child is in openList and this connection would be costlier, don't change anything
            if (std::find(openList.begin(), openList.end(), neighbour) != openList.end() && neighbour->G < currentNode->G + 1)
                continue;

            neighbour->G = currentNode->G + 1;
            neighbour->connection = currentNode;

            openList.push_back(neighbour);

            neighbour->nodetype = Maze::OPENLIST;
            update_display(window, box, winHoriz, winVert);

        }

    }

    return;
}

void update_display(sf::RenderWindow &window, sf::RectangleShape box, int winHoriz, int winVert) {
    // draw everything here...
    for (int i = 0; i < maxSize; i++)
    {
        for (int j = 0; j < maxSize; j++) {
            box.setPosition(i * winHoriz / maxSize, j * winVert / maxSize);
            setColour(box, grid[j][i].nodetype);
            window.draw(box);
        }
    }
    window.display();

    sf::sleep(sf::milliseconds(DELAY));


}


// Display winning path
void setPath(sf::RenderWindow& window, sf::RectangleShape box, int winHoriz, int winVert) {
    

    MazeNode* followedNode = endNode;

    while (followedNode != NULL) {

        followedNode->nodetype = Maze::PATH;
        update_display(window, box, winHoriz, winVert);

        followedNode = followedNode->connection;

    }

    startNode->nodetype = Maze::MOUSE;
    endNode->nodetype = Maze::END;
    update_display(window, box, winHoriz, winVert);


}