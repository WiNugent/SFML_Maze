#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#pragma once

#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>


enum Maze;

enum Dir;


#define PMARGIN 1
#define inf 999999
#define MAXNEIGHBOURS 4
#define DELAY 100

class MazeNode {
private:
    int H; // distance from end node
public:
    MazeNode(Maze nodetype);
    void setH(int i, int j);
    int G; // distance from start node
    void setG(int val);
    int getH();
    int getF();

    Maze nodetype;
    MazeNode* connection = NULL;
    MazeNode* neighbours[4] = { NULL, NULL, NULL, NULL }; // UP, DOWN, RIGHT, LEFT

};



extern std::vector<std::vector<MazeNode>> grid; // 2D vector for the grid
extern MazeNode* startNode;
extern MazeNode* endNode;

extern int maxSize; // Grid is square shaped, find its largest between both dimensions
extern std::pair<int, int> endCoords;

/*
SPACE:  O   0
WALL:   #   1
MOUSE:  M   2
END:    E   3
*/




// Functions
void setColour(sf::RectangleShape& shape, int val);
void readGrid(); // Reads "Maze.txt" file and fills in 2d grid vector
bool isValid(int i, int j);
void getAstar(sf::RenderWindow& window, sf::RectangleShape box);
void update_display(sf::RenderWindow& window, sf::RectangleShape box);
void setPath(sf::RenderWindow& window, sf::RectangleShape box);


#endif