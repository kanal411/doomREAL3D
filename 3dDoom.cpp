#include <iostream>
#include <array>
#include <fstream>
#include <vector>
#include <cmath>
#include <string>
#include <algorithm>
#include <cstdlib>

using namespace std;

array<array<int, 256>, 256> heightMap;


struct entityStruct {
    int x = 128;
    int y = 128;
    int z = 1;
    int turn = 0; // 0 - gora, 1 - prawo, 2 - dol, 3 - lewo
    bool zyje = true;
};

array<entityStruct, 4> enemies;

entityStruct player;


struct Vec3 {
    float x, y, z;
};


struct Triangle {
    Vec3 normal;
    Vec3 v1, v2, v3;
};

static Vec3 computeNormal(const Vec3& a, const Vec3& b, const Vec3& c) {
    Vec3 u = { b.x - a.x, b.y - a.y, b.z - a.z };
    Vec3 v = { c.x - a.x, c.y - a.y, c.z - a.z };
    Vec3 n = {
        u.y * v.z - u.z * v.y,
        u.z * v.x - u.x * v.z,
        u.x * v.y - u.y * v.x
    };
    float len = std::sqrt(n.x * n.x + n.y * n.y + n.z * n.z);
    if (len > 1e-9f) { n.x /= len; n.y /= len; n.z /= len; }
    return n;
}

static void addTriangle(std::vector<Triangle>& triangles, Vec3 a, Vec3 b, Vec3 c) {
    Triangle t;
    t.normal = computeNormal(a, b, c);
    t.v1 = a; t.v2 = b; t.v3 = c;
    triangles.push_back(t);
}

struct VoxelGrid {
    int ox, oy, oz;
    int nx, ny, nz;
    std::vector<unsigned char> data;

    VoxelGrid(int ox, int oy, int oz, int nx, int ny, int nz)
        : ox(ox), oy(oy), oz(oz), nx(nx), ny(ny), nz(nz),
        data((size_t)nx* ny* nz, 0) {
    }

    inline size_t index(int i, int j, int k) const {
        return ((size_t)k * ny + j) * nx + i;
    }
    inline bool inside(int i, int j, int k) const {
        return i >= 0 && i < nx && j >= 0 && j < ny && k >= 0 && k < nz;
    }

    inline bool solid(int i, int j, int k) const {
        return inside(i, j, k) && data[index(i, j, k)] != 0;
    }

    void setCube(int x, int y, int z = 0) {
        y = 255 - y;
        int i = x - ox, j = y - oy, k = z - oz;
        if (inside(i, j, k)) data[index(i, j, k)] = 1;
    }
};

bool saveSTL(const std::vector<Triangle>& triangles, const std::string& filename) {
    cout << "Saving STL, please wait.\n";
    std::ofstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Can't open file: " << filename << std::endl;
        return false;
    }

    float minX = 1e30f, minY = 1e30f, minZ = 1e30f;
    float maxX = -1e30f, maxY = -1e30f, maxZ = -1e30f;
    for (const auto& t : triangles) {
        const Vec3 vs[3] = { t.v1, t.v2, t.v3 };
        for (const Vec3& v : vs) {
            minX = std::min(minX, v.x); maxX = std::max(maxX, v.x);
            minY = std::min(minY, v.y); maxY = std::max(maxY, v.y);
            minZ = std::min(minZ, v.z); maxZ = std::max(maxZ, v.z);
        }
    }
    const float cx = (minX + maxX) * 0.5f;
    const float cy = (minY + maxY) * 0.5f;
    const float cz = (minZ + maxZ) * 0.5f;

    const float sx = 0.5f, sy = 0.5f, sz = 1.0f;
    auto scale = [&](const Vec3& p) -> Vec3 {
        return { cx + (p.x - cx) * sx, cy + (p.y - cy) * sy, cz + (p.z - cz) * sz };
    };

    file << "solid cubes\n";
    for (const auto& t : triangles) {
        Vec3 a = scale(t.v1), b = scale(t.v2), c = scale(t.v3);
        Vec3 n = computeNormal(a, b, c);
        file << "  facet normal " << n.x << " " << n.y << " " << n.z << "\n";
        file << "    outer loop\n";
        file << "      vertex " << a.x << " " << a.y << " " << a.z << "\n";
        file << "      vertex " << b.x << " " << b.y << " " << b.z << "\n";
        file << "      vertex " << c.x << " " << c.y << " " << c.z << "\n";
        file << "    endloop\n";
        file << "  endfacet\n";
    }
    file << "endsolid cubes\n";

    return true;
}

void drawAndFillSquare(pair<int, int> punkt1, pair<int, int> punkt2, int wartosc/*, array<array<int, 256>, 256>& map*/ )
{
    for (int x = punkt1.first; x < punkt2.first; x++)
    {
        for (int y = punkt1.second; y < punkt2.second; y++)
        {
            heightMap[x][y] = wartosc;
        }
    }
}

void generateMap(VoxelGrid& grid)
{
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            for (int z = 0; z < (heightMap[x][y] + 1); z++)
            {
                grid.setCube(x, y, z);
            }
        }
    }
}

void generateEntities(VoxelGrid& grid)
{
    // gracz
    for (int x = 0; x < 4; x++)
    {
        for (int y = 0; y < 4; y++)
        {
            for (int z = 0; z < 8; z++) grid.setCube(player.x + x, player.y + y, player.z + z);
        }
    }

    if (player.turn == 0) // gun
    {
        grid.setCube(player.x + 3, player.y - 1, player.z + 5);
        grid.setCube(player.x + 3, player.y - 2, player.z + 5);
        grid.setCube(player.x + 3, player.y - 3, player.z + 5);
    }
    else if (player.turn == 1)
    {
        grid.setCube(player.x + 4, player.y + 3, player.z + 5);
        grid.setCube(player.x + 5, player.y + 3, player.z + 5);
        grid.setCube(player.x + 6, player.y + 3, player.z + 5);
    }
    else if (player.turn == 2)
    {
        grid.setCube(player.x, player.y + 4, player.z + 5);
        grid.setCube(player.x, player.y + 5, player.z + 5);
        grid.setCube(player.x, player.y + 6, player.z + 5);
    }
    else
    {
        grid.setCube(player.x - 1, player.y + 3, player.z + 5);
        grid.setCube(player.x - 2, player.y + 3, player.z + 5);
        grid.setCube(player.x - 3, player.y + 3, player.z + 5);
    }

    // przeciwnicy
    for (entityStruct enemy : enemies)
    {
        if (enemy.zyje)
        {
            for (int x = 0; x < 12; x++)
            {
                for (int y = 0; y < 12; y++)
                {
                    for (int z = 1; z < 7; z++) grid.setCube(enemy.x + x, enemy.y + y, enemy.z + z);
                }
            }
        }
    }
}

static void addBox(vector<Triangle>& triangles,
    float x0, float y0, float z0, float x1, float y1, float z1)
{
    Vec3 p[8] = {
        {x0, y0, z0}, {x1, y0, z0}, {x1, y1, z0}, {x0, y1, z0},
        {x0, y0, z1}, {x1, y0, z1}, {x1, y1, z1}, {x0, y1, z1}
    };
    addTriangle(triangles, p[0], p[3], p[2]); addTriangle(triangles, p[0], p[2], p[1]); // dol
    addTriangle(triangles, p[4], p[5], p[6]); addTriangle(triangles, p[4], p[6], p[7]); // gora
    addTriangle(triangles, p[0], p[1], p[5]); addTriangle(triangles, p[0], p[5], p[4]); // przod
    addTriangle(triangles, p[3], p[6], p[2]); addTriangle(triangles, p[3], p[7], p[6]); // tyl
    addTriangle(triangles, p[0], p[4], p[7]); addTriangle(triangles, p[0], p[7], p[3]); // lewo
    addTriangle(triangles, p[1], p[2], p[6]); addTriangle(triangles, p[1], p[6], p[5]); // prawo
}

void meshGreedy(const VoxelGrid& grid, vector<Triangle>& triangles)
{
    const int nx = grid.nx, ny = grid.ny, nz = grid.nz;
    vector<unsigned char> used((size_t)nx * ny * nz, 0);

    auto at = [&](int i, int j, int k) -> size_t { return ((size_t)k * ny + j) * nx + i; };
    auto wolna = [&](int i, int j, int k) { return grid.solid(i, j, k) && !used[at(i, j, k)]; };

    for (int k = 0; k < nz; k++)
        for (int j = 0; j < ny; j++)
            for (int i = 0; i < nx; i++)
            {
                if (!wolna(i, j, k)) continue;

                int w = 1;
                while (i + w < nx && wolna(i + w, j, k)) w++;

                int d = 1;
                bool ok = true;
                while (j + d < ny && ok)
                {
                    for (int x = i; x < i + w; x++)
                        if (!wolna(x, j + d, k)) { ok = false; break; }
                    if (ok) d++;
                }

                int h = 1;
                ok = true;
                while (k + h < nz && ok)
                {
                    for (int y = j; y < j + d && ok; y++)
                        for (int x = i; x < i + w; x++)
                            if (!wolna(x, y, k + h)) { ok = false; break; }
                    if (ok) h++;
                }

                for (int z = k; z < k + h; z++)
                    for (int y = j; y < j + d; y++)
                        for (int x = i; x < i + w; x++)
                            used[at(x, y, z)] = 1;

                addBox(triangles,
                    float(i + grid.ox), float(j + grid.oy), float(k + grid.oz),
                    float(i + w + grid.ox), float(j + d + grid.oy), float(k + h + grid.oz));
            }
}

void buildTriangles(vector<Triangle>& triangles)
{
    triangles.clear();
    VoxelGrid grid(-8, -8, 0, 272, 272, 64);
    generateMap(grid);
    generateEntities(grid);
    meshGreedy(grid, triangles);
}

void printGame()
{
    int code = system("python print.py");

    if (code != 0) exit(0);

    cout << "Your print has started. When it finishes, remove it from the heatbed, then press Enter.\n";
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    cin.get();
}

void endGame(std::vector<Triangle>& triangles)
{
    VoxelGrid grid(0, 0, 0, 256, 256, 2);
    for (int x = 0; x < 256; x++)
    {
        for (int y = 0; y < 256; y++)
        {
            if ((abs(x - y) < 2) || (abs(x - (255 - y)) < 2)) grid.setCube(x, y);
        }
    }
    meshGreedy(grid, triangles);
    saveSTL(triangles, "mapa.stl");
    printGame();
    exit(0);
}

int main()
{
    std::vector<Triangle> triangles;
    // setup mapki wysokosci
    drawAndFillSquare({ 96, 0 }, { 160, 64 }, 8); // podniesienie

    string answer;
    cout << "Do you want to use automatic printing (check github repo for details) (yes/no): ";
    cin >> answer;

    for (int x = 96; x < 160; x++) // schodki
    {
        for (int y = 64; y < 72; y++)
        {
            heightMap[x][y] = 71 - y;
        }
    }


    if (answer == "yes")
    {
        cout << "This script will take control of your mouse and keyboard. It is recommended to close any important programs before running it. If you do not feel comfortable with this, turn off printing automation. Press enter to continue...\n";
        cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        cin.get();

        string calAns;
        cout << "Do you want to run setup (it is necessary if it is your first time running this program or if you restarted bambu studio since last time) (yes/no): ";
        cin >> calAns;

        if (calAns == "yes")
        {
            buildTriangles(triangles);
            saveSTL(triangles, "mapa.stl");

            cout << "Open bambu studio, press ctrl+i, find mapa.stl (it will be in the same directory as this script) and select it. Delete it, and then open terminal running this script (it is important to not select any other program in between). Press enter to continue...\n";
            cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            cin.get();
        }
    }

    drawAndFillSquare({0, 0}, {96, 73}, 16);
    drawAndFillSquare({ 160, 0 }, { 255, 73 }, 16);

    drawAndFillSquare({95, 0}, {160, 1}, 16); // gorna scianka
    //drawAndFillSquare({ 95, 0 }, { 96, 72 }, 16); // lewa scianka
    //drawAndFillSquare({ 160, 0 }, { 161, 72 }, 16); // prawa scianka

    //drawAndFillSquare({ 0, 72 }, { 96, 73 }, 16); // gorna scianka P1
    //drawAndFillSquare({ 160, 72 }, { 256, 73 }, 16); // gorna scianka P2
    drawAndFillSquare({ 0, 73 }, { 1, 256 }, 16); // lewa scianka
    drawAndFillSquare({ 255, 73 }, { 256, 256 }, 16); // prawa scianka

    drawAndFillSquare({ 92, 129 }, { 98, 135 }, 16); // filary 
    drawAndFillSquare({ 157, 129 }, { 163, 135 }, 16);
    drawAndFillSquare({ 92, 229 }, { 98, 235 }, 16);
    drawAndFillSquare({ 157, 229 }, { 163, 235 }, 16); // filary

    //= { { {30, 250}, {60, 250}, {196, 250}, {226, 250} } }
    enemies[0].x = 30;
    enemies[0].y = 230;
    enemies[1].x = 60;
    enemies[1].y = 230;
    enemies[2].x = 196;
    enemies[2].y = 230;
    enemies[3].x = 226;
    enemies[3].y = 230;
    for (int i = 0; i < 4; i++) { enemies[i].z = 1; enemies[i].turn = 0; }

    buildTriangles(triangles);
    saveSTL(triangles, "mapa.stl");
    if (answer == "yes") printGame();
    else cout << "Your file is ready to print (mapa.stl in the same directory as this script).\n";

    string wagaStr;
    string kierunekStr;
    string strzelicStr;
    while (true)
    {
        cout << "What move do you want to make (<distance> <direction (north, west, etc.)> <do you want to shoot (yes/no)>): ";
        cin >> wagaStr;
        cin >> kierunekStr;
        cin >> strzelicStr;

        if (strzelicStr == "yes")
        {
            for (int i = 0; i < 4; i++)
            {
                if (enemies[i].zyje)
                {
                    if (player.turn == 0)
                    {
                        if (enemies[i].y < player.y)
                        {
                            if ((abs(player.x - enemies[i].x) < 15) && (abs(player.z - enemies[i].z) < 5))
                            {
                                enemies[i].zyje = false;
                                enemies[i].x = -512;
                            }
                        }
                    }
                    else if (player.turn == 1)
                    {
                        if (enemies[i].x > player.x)
                        {
                            if (abs(player.y - enemies[i].y) < 15)
                            {
                                enemies[i].zyje = false;
                                enemies[i].x = -512;
                            }
                        }
                    }
                    else if (player.turn == 2)
                    {
                        if (enemies[i].y > player.y)
                        {
                            if (abs(player.x - enemies[i].x) < 15)
                            {
                                enemies[i].zyje = false;
                                enemies[i].x = -512;
                            }
                        }
                    }
                    else if (player.turn == 3)
                    {
                        if (enemies[i].x < player.x)
                        {
                            if (abs(player.y - enemies[i].y) < 15)
                            {
                                enemies[i].zyje = false;
                                enemies[i].x = -512;
                            }
                        }
                    }
                }
            }
        }
        else if (strzelicStr != "no")
        {
            cout << "Answer yes/no. Try again\n";
            continue;
        }

        int waga;
        try
        {
            waga = stoi(wagaStr);
        }
        catch (...)
        {
            cout << "Distance must be a number. Try again.\n";
            continue;
        }

        int kieruneczek = -1;
        if (kierunekStr == "north") { kieruneczek = 0; player.y -= waga; }
        else if (kierunekStr == "east") { kieruneczek = 1; player.x += waga; }
        else if (kierunekStr == "south") { kieruneczek = 2; player.y += waga; }
        else if (kierunekStr == "west") { kieruneczek = 3; player.x -= waga; }
        if (kieruneczek == -1) { cout << "Please use one of these directions: north/south/west/east. Try again.\n"; continue; }
        
        player.turn = kieruneczek;

        player.x = clamp(player.x, 1, 254); // mniejsze bo ponizsza petla nie jest przygotowana na boczne sciany
        player.y = clamp(player.y, 0, 255);

        while (heightMap[player.x][player.y] == 16)
        {
            player.y++;
        }

        if (heightMap[player.x][player.y] > 0)
        {
            player.z = heightMap[player.x][player.y] + 1;
        }

        float budzet = waga * 1.3f;
        for (int i = 0; i < 4; i++)
        {
            if (enemies[i].zyje == true)
            {
                float odlegloscX = player.x - enemies[i].x;
                float odlegloscY = player.y - enemies[i].y;
                bool zmienilX = false;
                bool zmienilY = false;

                if (odlegloscX != 0 && odlegloscY != 0) // obliczanie ruchu na podstawie proporcji
                {
                    float xDoY = abs(odlegloscX / odlegloscY);

                    float kawalek = budzet / (xDoY + 1.0f);

                    if (player.x > enemies[i].x) enemies[i].x += min((kawalek * xDoY), abs(odlegloscX));
                    else enemies[i].x -= min(kawalek * xDoY, abs(odlegloscX));

                    if (player.y > enemies[i].y) enemies[i].y += min(kawalek, abs(odlegloscY));
                    else enemies[i].y -= min(kawalek, abs(odlegloscY));
                }
                else
                {
                    if (odlegloscX == 0)
                    {
                        if (player.y > enemies[i].y) enemies[i].y += min(budzet, abs(odlegloscY));
                        else enemies[i].y -= min(budzet, abs(odlegloscY));
                    }
                    else
                    {
                        if (player.x > enemies[i].x) enemies[i].x += min(budzet, abs(odlegloscX));
                        else enemies[i].x -= min(budzet, abs(odlegloscX));
                    }
                }

                bool zmiana = false;

                for (int j = 0; j < i; j++) // ochrona prze overlapem
                {
                    int roznicaX = enemies[j].x - enemies[i].x;
                    int roznicaY = enemies[j].y - enemies[i].y;
                    if ((abs(roznicaX) + abs(roznicaY)) < 20)
                    {
                        if (abs(roznicaX) < 10) { enemies[i].x -= roznicaX; zmienilX = true; zmiana = true; }
                        if (abs(roznicaY) < 10) { enemies[i].y -= roznicaY; zmienilY = true; zmiana = true; }
                    }
                }

                // ochrona przed byciem w scianie
                if (!zmienilX && !zmienilY)
                {
                    while (true)
                    {
                        if (clamp(enemies[i].x, 0, 255) == enemies[i].x && clamp(enemies[i].y, 0, 255) == enemies[i].y)
                        {
                            if (heightMap[enemies[i].x][enemies[i].y] == 16)
                            {
                                if (enemies[i].y > 127) enemies[i].y -= 1;
                                else enemies[i].y += 1;
                            }
                            else break;
                        }
                        else
                        {
                            enemies[i].x = 55 * i;
                            enemies[i].y = 230;
                            break;
                        }
                    }
                }
                else
                {
                    enemies[i].x = 55 * i;
                    enemies[i].y = 230;
                }

                if (heightMap[enemies[i].x][enemies[i].y] > 0)
                {
                    enemies[i].z = heightMap[enemies[i].x][enemies[i].y] + 1;
                }

                // sprawdzanie czy przewalil
                if ((abs(player.x - enemies[i].x) + abs(player.y - enemies[i].y) < 25)) endGame(triangles);
            }
        }

        buildTriangles(triangles);
        saveSTL(triangles, "mapa.stl");
        if (answer == "yes") printGame();
        else cout << "Your file is ready to print (mapa.stl in the same directory as this script).\n";
    }
}