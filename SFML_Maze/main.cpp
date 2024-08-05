#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


#define PMARGIN 1
#define inf 999999
#define MAXNEIGHBOURS 4

/*

SPACE:  O   0
WALL:   #   1
MOUSE:  M   2
END:    E   3

*/

typedef std::pair<int, int> coordinates;


enum Maze {
    SPACE,
    WALL,
    MOUSE,
    END
};

enum Dir {
    UP,
    DOWN,
    RIGHT,
    LEFT
};

coordinates startpos{};
coordinates endpos{};


class MazeNode {
private:

public:
    int G; // distance from start node
    int H; // distance from end node

    Maze getNodeType() { return nodetype; } // just get nodetype later
    Maze nodetype;
    MazeNode* neighbours[4]{};
    MazeNode* connection{};
    coordinates coords;

    int getF() { return G + H; }
    int getG() { return G; }

    void calcH(coordinates nodePos) { this->H = fabs(nodePos.first - endpos.first) + fabs(nodePos.second - endpos.second); }

    MazeNode(Maze nodetype, coordinates coords) {
        this->nodetype = nodetype;
        this->G = inf;
        this->H = inf;
        this->coords = coords;
    }

    bool operator==(MazeNode otherNode) {
 
        for (int i = 0; i < MAXNEIGHBOURS; i++) {
            // if only one null
            if ((this->neighbours[i] == NULL && otherNode.neighbours[i] != NULL) || (this->neighbours[i] != NULL && otherNode.neighbours[i] == NULL)) {
                std::cout << "Not same\n";
                return false;
            }

            // if both null
            if (this->neighbours[i] == NULL)
                continue;

            // if one pair identical
            if (this->neighbours[i]->coords.first == otherNode.neighbours[i]->coords.first && this->neighbours[i]->coords.second == otherNode.neighbours[i]->coords.second) {
                std::cout << "Same\n";
                return true;
            }

        }

        return (this->neighbours == otherNode.neighbours);
    }

    bool operator!=(MazeNode otherNode) {
        return !(this->neighbours == otherNode.neighbours);
    }
};

std::vector<std::vector<MazeNode>> grid{}; // 2D vector for the grid
int maxSize = 0; // Grid is square shaped, find it's largest between both dimensions



// Functions
void setColour(sf::RectangleShape& shape, int val);
void readGrid();
void aStarGen();




int main() {
    readGrid();

    // Print vector
    /*
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {
            std::cout << grid[i][j] << " ";
        }
        std::cout << "\n";
    }
    */


    // create the window
    sf::RenderWindow window(sf::VideoMode(400, 400), "My window");


    const int winHoriz = window.getSize().x;
    const int winVert = window.getSize().y;
    sf::RectangleShape box(sf::Vector2f(winHoriz / maxSize - PMARGIN, winVert / maxSize - PMARGIN));
    box.setFillColor(sf::Color::White);


    // run the program as long as the window is open
    while (window.isOpen())
    {
        // check all the window's events that were triggered since the last iteration of the loop
        sf::Event event;
        while (window.pollEvent(event))
        {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                window.close();
        }

        // clear the window with black color
        window.clear(sf::Color::Black);

        // draw everything here...
        for (int i = 0; i < maxSize; i++)
        {
            for (int j = 0; j < maxSize; j++) {
                box.setPosition(i * winHoriz / maxSize, j * winVert / maxSize);
                setColour(box, grid[j][i].getNodeType());
                window.draw(box);
            }
        }

        static bool generated = false;
        if (!generated) {
            aStarGen();
            generated = true;
        }
        sf::sleep(sf::milliseconds(200));


        // end the current frame
        window.display();
    }

    return 0;
}

void setColour(sf::RectangleShape& shape, int val) {
    switch (val) {
    case Maze::SPACE: shape.setFillColor(sf::Color::White); break;            // SPACE
    case Maze::WALL: shape.setFillColor(sf::Color(125, 125, 125)); break;    // WALL
    case Maze::MOUSE: shape.setFillColor(sf::Color::Green); break;            // MOUSE
    case Maze::END: shape.setFillColor(sf::Color::Red); break;              // END
    }
}

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
            //std::cout << grid.size() << ", " << tempVec.size() << ") ";

            switch (c)
            {
            case 'O': tempVec.push_back(MazeNode(Maze::SPACE, { grid.size(), tempVec.size() })); break;
            case '#': tempVec.push_back(MazeNode(Maze::WALL, { grid.size(), tempVec.size() })); break;
            case 'M': tempVec.push_back(MazeNode(Maze::MOUSE, { grid.size(), tempVec.size() })); break;
            case 'E': tempVec.push_back(MazeNode(Maze::END, { grid.size(), tempVec.size() })); break;
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
    while (grid.size() < maxSize)
        grid.push_back({});
    for (int i = 0; i < grid.size(); i++) {
        while (grid[i].size() < maxSize)
            grid[i].push_back(MazeNode(Maze::WALL, { i, grid[i].size()}));
    }

    // Re-iterate through nodes to: set neighbours and set H
    for (int i = 0; i < grid.size(); i++) {
        for (int j = 0; j < grid[i].size(); j++) {

            // Check if start or end node
            Maze nodeType = grid[i][j].getNodeType();
            
            if (nodeType == Maze::END) {
                endpos = { i, j };
                grid[i][j].calcH({ i,j });
                continue; // No need to calculate neighbours for end node
            }
            else if (nodeType == Maze::WALL)
                continue; // No need to calculate neighbours for walls
            else if (nodeType == Maze::MOUSE) {
                startpos = { i, j };
                grid[i][j].G = 0;
            }

            grid[i][j].calcH({ i,j });


            // Set all neighbours
            // (0,0) is Top-Left
            int x;
            int y;

            // UP
            x = j;
            y = i - 1;
            // If valid neighbour then add it
            if (grid[y][x].getNodeType() != Maze::WALL) {
                grid[i][j].neighbours[Dir::UP] = &grid[y][x]; // Get pointer to neighbours
                /*
                grid[y][x].nodetype = Maze::END; // Neighbour set
                grid[i][j].nodetype = Maze::MOUSE; // Node set
                */
                //std::cout << "(" << i << ", " << j << ")->(" << x << ", " << y << ")";
            }

            // DOWN
            x = j;
            y = i + 1;
            if (grid[y][x].getNodeType() != Maze::WALL) {
                grid[i][j].neighbours[Dir::DOWN] = &grid[y][x]; // Get pointer to neighbours
            }

            // LEFT
            x = j - 1;
            y = i;
            if (grid[y][x].getNodeType() != Maze::WALL) {
                grid[i][j].neighbours[Dir::LEFT] = &grid[y][x]; // Get pointer to neighbours
            }

            // RIGHT
            x = j + 1;
            y = i;
            if (grid[y][x].getNodeType() != Maze::WALL) {
                grid[i][j].neighbours[Dir::RIGHT] = &grid[y][x]; // Get pointer to neighbours
            }

        }
    }


}

void aStarGen() {
    std::vector<MazeNode> toSearch = { grid[startpos.first][startpos.second] };
    std::vector<MazeNode> processed = {};

    // while there is a node to search
    while (!toSearch.empty()) {

        MazeNode current = toSearch[0];

        // find node with smallest F on nodes to search
        for (MazeNode t : toSearch) {
            //std::cout << t.getF() << "\n";
            if (t.getF() < current.getF() || t.getF() == current.getF() && t.H < current.H)
                current = t;
        }

        processed.push_back(current);
        toSearch.erase(std::remove(toSearch.begin(), toSearch.end(), current), toSearch.end());

        //std::cout << "(" << current.coords.first << ", " << current.coords.second << ")\n";

        // if endnode found
        if (current.nodetype == Maze::END) {
            std::cout << "Endnode found :)\n";
            break;
        }





        // for each neighbour
        for (int i = 0; i < MAXNEIGHBOURS; i++) {

            //std::cout << "Neighbour[" << i << "] (" << current.neighbours[i]->coords.first << ")\n";
            // If invalid neighbour, continue
            if (current.neighbours[i] == NULL || std::find(processed.begin(), processed.end(), *current.neighbours[i]) == processed.end())
                continue;

            MazeNode* neighbour = current.neighbours[i];

            // Bool for whether neighbour is in `toSearch`
            bool inSearch = std::find(toSearch.begin(), toSearch.end(), *neighbour) != toSearch.end();


            // Get costToNeighbour
            int costToNeighbour = current.G + 1;

            // if not inSearch or the calculated neighbour's G cost is lower than neighbour's set G cost
            if (!inSearch || costToNeighbour < neighbour->G) {
                neighbour->G = costToNeighbour; // Adjust G value of neighbour
                neighbour->connection = &current; // set connection

                // if not inSearch
                if (!inSearch) {
                    toSearch.push_back(*neighbour);
                    std::cout << "+(" << neighbour->coords.first << ", " << neighbour->coords.second << ")\n";
                }
                        // Set H (ALREADY DONE)
                        // Add neighbour to `toSearch`

            }


        }

        
        for (int i = 0; i < toSearch.size(); i++)
            std::cout << toSearch[i].coords.first << ", " << toSearch[i].coords.second << "\n";
        //sf::sleep(sf::milliseconds(1000));

    }

    
    //std::cout << grid[endpos.first][endpos.second].connection->coords.first << ", " << grid[endpos.first][endpos.second].connection->coords.second;

    std::cout << "Done\n";
}