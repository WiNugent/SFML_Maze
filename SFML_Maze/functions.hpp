#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>

#define PMARGIN 2
#define inf 999999
#define MAXNEIGHBOURS 4
#define DELAY 100

enum Maze;
enum Dir;

class MazeNode {
private:
    int H; // distance from end node
public:
    MazeNode(Maze nodetype);
    void setH(int i, int j);
    int getH();
    int getF();
    int G; // distance from start node

    Maze nodetype;
    MazeNode* connection = NULL;
    MazeNode* neighbours[4] = { NULL, NULL, NULL, NULL }; // UP, DOWN, RIGHT, LEFT

};

// Global variables
extern MazeNode* startNode;
extern MazeNode* endNode;
extern std::vector<std::vector<MazeNode>> grid; // 2D vector for the grid of nodes
extern std::pair<int, int> endCoords;
extern int maxSize; // Grid is square shaped, find largest between its Row & Col


// Functions
void setColour(sf::RectangleShape& shape, int val);
void readGrid(); // Reads "Maze.txt" file and fills in 2d grid vector
bool isValid(int i, int j);
void getAstar(sf::RenderWindow& window, sf::RectangleShape box);
void update_display(sf::RenderWindow& window, sf::RectangleShape box);
void setPath(sf::RenderWindow& window, sf::RectangleShape box);


#endif