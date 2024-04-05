#include "raylib.h"
#include <iostream>
#include <vector>
#include <stdlib.h>
#include <fstream>
#include <string>
#include <sstream>


#define SCREENWIDTH 1280
#define SCREENHEIGHT 720


int silnia(int n) {
    if (n < 1) {
        return 1;
    }
    else {
        return n * silnia(n - 1);
    }
}


int newton(int n, int k) {
    int licznik = silnia(n);
    int mianownik = silnia(k) * silnia(n - k);
    return licznik / mianownik;
}


class Bezeir_surface {
    int size_m;
    int size_n;
    int density_m;
    int density_n;
    std::vector<Vector3> Points;
    std::vector<Vector3> T;
public:
    Bezeir_surface(int density, int size_m, int size_n) {
        this->size_m = size_m;
        this->size_n = size_n;
        density_m = density * size_m;
        density_n = density * size_n;
        //Points.resize(size_m * size_n);
        T.resize(size_m * density * size_n* density);
    }

    void random_points() {
        for (int i = 0; i < size_m; i++) {
            for (int j = 0; j < size_n; j++) {
                Vector3 vec;
                vec.x = j;
                vec.y = (rand() % 10000) / 5000 - 1;
                vec.z = i;
                Points.push_back(vec);
            }
        }
    }

    void add_point(float x, float y, float z) {
        Points.push_back({ x,y,z });
    }

private:
    float b_func(float ij, float mn, float vw) {
        float x = newton(mn, ij) * pow(vw, ij) * pow((1 - vw), (mn - ij));
        return x;
    }

    Vector3 calculate_sum(float v, float w) {
        float sum_x = 0;
        float sum_y = 0;
        float sum_z = 0;

        for (int i = 0; i < size_m; i++) {
            for (int j = 0; j < size_n; j++) {
                sum_x += Points[i * size_n + j].x * b_func(i, size_m - 1, v) * b_func(j, size_n - 1, w);
                sum_y += Points[i * size_n + j].y * b_func(i, size_m - 1, v) * b_func(j, size_n - 1, w);
                sum_z += Points[i * size_n + j].z * b_func(i, size_m - 1, v) * b_func(j, size_n - 1, w);

            }
        }
        return { sum_x, sum_y, sum_z };
    }

public:
    void calculate_surface() {
        for (int i = 0; i < density_m; i++) {
            float v = i / (float)(density_m - 1);
            for (int j = 0; j < density_n; j++) {
                float w = j / (float)(density_n - 1);
                Vector3 sum = calculate_sum(v, w);
                T[i * density_n + j] = sum;
            }
        }
    }

    void read_points_from_file(std::string path) {

    }

    void draw() {
        for (int i = 0; i < density_m; i++) {
            for (int j = 0; j < density_n; j++) {
                DrawCube({ T[i * density_n + j].x, T[i * density_n + j].y, T[i * density_n + j].z}, 0.01f, 0.01f, 0.01f, RED);
            }
        }
    }
    void print() {
        for (int i = 0; i < size_m; i++) {
            for (int j = 0; j < size_n; j++) {
                std::cout << Points[i * size_n + j].x << " " << Points[i * size_n + j].y << " " << Points[i * size_n + j].z << std::endl;
            }
        }
    }
};


std::vector<Bezeir_surface> jakas_funkcja_bez_konkretnej_nazwy() {
    std::vector<Bezeir_surface> patches;
    std::ifstream file("dane.txt");
    float x, y, z;
    int size_m, size_n;
    int l = 0;
    if (file.is_open()) {
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream ss(line);
            l = 0;
            for (char znak : line) {
                if (isspace(znak)) {
                    l++;
                }
            }
            if (l == 1) {
                ss >> size_m >> size_n;
                patches.push_back(Bezeir_surface(7, size_m+1, size_n+1));
            }
            if (l == 2) {
                ss >> x >> y >> z;
                patches.back().add_point(y, z, x);
            }
        }
    }
    file.close();

    return patches;
}

int main() {
    int density = 3;
    int size_m = 10;
    int size_n = 10;

    Bezeir_surface surface = Bezeir_surface(10, 5, 5);
    surface.random_points();
    surface.calculate_surface();

    std::vector<Bezeir_surface> patches = jakas_funkcja_bez_konkretnej_nazwy();
    for (int i = 0; i < patches.size(); i++) {
        patches[i].calculate_surface();
    }
    
    InitWindow(SCREENWIDTH, SCREENHEIGHT, "xpp");
    Camera3D camera = { 0 };
    camera.position = { 10.0f, 10.0f, 10.0f }; 
    camera.target = { 0.0f, 0.0f, 0.0f };      
    camera.up = { 0.0f, 1.0f, 0.0f };          
    camera.fovy = 45.0f;                       
    camera.projection = CAMERA_PERSPECTIVE;    

    DisableCursor();                    

    SetTargetFPS(60);                   

    while (!WindowShouldClose())       
    {
        UpdateCamera(&camera, CAMERA_FREE);

        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);

            //DrawCube(cubePosition, 0.1f, 0.1f, 0.1f, RED);
            //surface.draw();
            //surface1.draw();
            for (int i = 0; i < patches.size(); i++) {
                patches[i].draw();
            }
            DrawGrid(100, 1.0f);

        EndMode3D();
        DrawRectangle(10, 10, 320, 93, Fade(SKYBLUE, 0.5f));
        DrawRectangleLines(10, 10, 320, 93, BLUE);

        DrawText("Skolem sie przybliza:", 40, 20, 10, DARKGREEN);
        DrawText("Strzalkami sie lata", 40, 40, 10, DARKGREEN);
        DrawText("kontrolem opada", 40, 60, 10, DARKGREEN);
        DrawText("spaca leci w gore", 40, 80, 10, DARKGREEN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
