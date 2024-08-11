#include "functions.hpp"

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

MazeNode::MazeNode(Maze nodetype) {
    this->nodetype = nodetype;
    this->G = 0;
    this->H = inf;
}
void MazeNode::setH(int i, int j) { this->H = fabs(i - endCoords.first) + fabs(j - endCoords.second); }
void MazeNode::setG(int val) { this->G = val; }
int MazeNode::getH() { return H; }
int MazeNode::getF() { return G + H; }

std::vector<std::vector<MazeNode>> grid{}; // 2D vector for the grid
MazeNode* startNode;
MazeNode* endNode;

int maxSize = 0; // Grid is square shaped, find its largest between both dimensions
std::pair<int, int> endCoords;

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
            case 'O': tempVec.push_back(MazeNode(Maze::SPACE)); break;
            case '#': tempVec.push_back(MazeNode(Maze::WALL)); break;
            case 'M': tempVec.push_back(MazeNode(Maze::MOUSE)); break;
            case 'E': tempVec.push_back(MazeNode(Maze::END)); endCoords = { grid.size(), tempVec.size() - 1 }; break;
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
            grid[i].push_back(MazeNode(Maze::WALL));
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
            grid[i][j].setH(i, j);

            // UP
            y = i - 1;
            x = j;
            if (isValid(y, x) && grid[y][x].nodetype != Maze::WALL) {
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



void getAstar(sf::RenderWindow& window, sf::RectangleShape box) {



    MazeNode* exampleNode = startNode;


    std::vector<MazeNode*> openList = { startNode };
    std::vector<MazeNode*> closeList = {};

    startNode->nodetype = Maze::OPENLIST;
    update_display(window, box);

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
        update_display(window, box);

        if (currentNode == endNode) {
            std::cout << "You win!";
            setPath(window, box);
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
            update_display(window, box);

        }

    }

    return;
}

void update_display(sf::RenderWindow& window, sf::RectangleShape box) {
    // draw everything here...
    for (int i = 0; i < maxSize; i++)
    {
        for (int j = 0; j < maxSize; j++) {
            box.setPosition(i * window.getSize().x / maxSize, j * window.getSize().y / maxSize);
            setColour(box, grid[j][i].nodetype);
            window.draw(box);
        }
    }
    window.display();

    sf::sleep(sf::milliseconds(DELAY));


}


// Display winning path
void setPath(sf::RenderWindow& window, sf::RectangleShape box) {


    MazeNode* followedNode = endNode;

    while (followedNode != NULL) {

        followedNode->nodetype = Maze::PATH;
        update_display(window, box);

        followedNode = followedNode->connection;

    }

    startNode->nodetype = Maze::MOUSE;
    endNode->nodetype = Maze::END;
    update_display(window, box);


}