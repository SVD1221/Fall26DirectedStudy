#include <iostream>
#include <vector>
#include <unistd.h>

// generate random grid, writing to file, and timing
#include <cstdlib>
#include <fstream>
#include <chrono>

std::vector<int> createRandomVec(int min, int max, int size, int seed)
{  
    std::vector<int> rand_vec(size);
    srand(seed);

    for (int i = 0; i < size; i++)
    {
        // rand_vec[i] = dist(gen);
        rand_vec[i] = min + rand() % (max - min + 1);
    }

    return rand_vec;
}

void writeTo(std::ofstream *file, std::vector<std::vector<int>> *mat)
{
    for (int i = 0; i < (*mat).size(); i++){
        for (int j = 0; j < (*mat)[0].size(); j++){
            *file << (*mat)[i][j] << " ";
        }
    }
}
//

const int WIDTH = 30;
const int HEIGHT = 30;

void printGrid(const std::vector<std::vector<int>>& grid) {
    for (const auto& row : grid) {
        for (int cell : row) {
            std::cout << (cell ? 'O' : '.') << " ";
        }
        std::cout << std::endl;
    }
}

int countNeighbors(const std::vector<std::vector<int>>& grid, int x, int y) {
    int count = 0;
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            //Updated implementation with periodic BCs
            if (i == 0 && j == 0)
                continue;
            int X = x + i;
            int Y = y + j;
            if (X < 0)
                X = HEIGHT - 1;
            if (x + i >= HEIGHT)
                X = 0;
            if ( y + j < 0)
                Y = WIDTH - 1;
            if (y + j >= WIDTH)
                Y = 0;
            count += grid[X][Y];
            //Before
            // if ((i == 0 && j == 0) || x + i < 0 || x + i >= HEIGHT || y + j < 0 || y + j >= WIDTH)
            //     continue;
            // count += grid[x + i][y + j];
        }
    }
    return count;
}

void updateGrid(std::vector<std::vector<int>>& grid) {
    std::vector<std::vector<int>> newGrid = grid;
    for (int i = 0; i < HEIGHT; ++i) {
        for (int j = 0; j < WIDTH; ++j) {
            int aliveNeighbors = countNeighbors(grid, i, j);
            if (grid[i][j] == 1) {
                newGrid[i][j] = (aliveNeighbors < 2 || aliveNeighbors > 3) ? 0 : 1;
            } else {
                newGrid[i][j] = (aliveNeighbors == 3) ? 1 : 0;
            }
        }
    }
    grid = newGrid;
}

int main() {
    auto start = std::chrono::steady_clock::now();

    std::vector<std::vector<int>> grid(HEIGHT, std::vector<int>(WIDTH, 0));
    std::vector<int> vectorGrid = createRandomVec(0, 1, HEIGHT*WIDTH, 1);

    int iterations = 150;
    for (int i = 0; i < HEIGHT; i++){
        for (int j = 0; j < WIDTH; j++){
            grid[i][j] = vectorGrid[WIDTH*i + j];
        }
    }
    std::ofstream file;
    file.open("output_files/2dca_benchmark_output.txt");
    file << HEIGHT << " " << WIDTH << std::endl;
    for (int i = 0; i < iterations; i++){
        writeTo(&file, &grid);
        file << std::endl;
        updateGrid(grid);
    }
    writeTo(&file, &grid);

    // grid[1][2] = grid[2][3] = grid[3][1] = grid[3][2] = grid[3][3] = 1; // initial state
    // while (true) {
    //     printGrid(grid);
    //     std::cout << std::endl;
    //     updateGrid(grid);
    //     usleep(500000); // pause for half a second
    // }

    auto end = std::chrono::steady_clock::now();
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();

    std::cout << "Benchmark finished in " << ms << " ms\n";

    return 0;
}