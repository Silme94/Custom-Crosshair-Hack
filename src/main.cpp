#include <iostream>
#include <SFML/Graphics.hpp>
#include <Windows.h>
#include <TlHelp32.h>
#include <dwmapi.h>


struct _POS{
    LONG R; // right
    LONG L; // left
    LONG T; // top
    LONG B; // bottom
};


typedef _POS POS;


void GetWindowPosition(POS* dms, HWND hProcess) noexcept {
    RECT rect;

    GetWindowRect(hProcess, &rect);

    dms->R = rect.right;
    dms->L = rect.left;
    dms->T = rect.top;
    dms->B = rect.bottom;
}


int main(void) {

    system("cls");

    std::cout << R"(

         _____           _                    _____                   _           _      
        /  __ \         | |                  /  __ \                 | |         (_)     
        | /  \/_   _ ___| |_ ___  _ __ ___   | /  \/_ __ ___  ___ ___| |__   __ _ _ _ __ 
        | |   | | | / __| __/ _ \| '_ ` _ \  | |   | '__/ _ \/ __/ __| '_ \ / _` | | '__|
        | \__/| |_| \__ | || (_) | | | | | | | \__/| | | (_) \__ \__ | | | | (_| | | |   
         \____/\__,_|___/\__\___/|_| |_| |_|  \____|_|  \___/|___|___|_| |_|\__,_|_|_|   


    )" << std::endl;

    char window_name[MAX_PATH] = "AssaultCube";
    std::cout << "Window name (AssaultCube) -> \n";
    std::cin.getline(window_name, MAX_PATH);

    HWND hwnd = FindWindowA(NULL, window_name);
    if (hwnd == NULL) {
        std::cout << "Window not found.\n";
        return -1;
    }


    loop:
    char crossindex = 1;
    std::cout << "Crosshair index (1 - 7) -> \n";
    std::cin >> crossindex;

    if (crossindex < '1' || crossindex > '7') {
        std::cout << "Invalid crosshair index. Please enter a number between 1 and 7.\n";
        goto loop;
    }

    int crosshairIndex = crossindex - '0';

    sf::RenderWindow window(sf::VideoMode(80, 80), "Crosshair", sf::Style::None);
    window.setFramerateLimit(60);

    char cross_path[MAX_PATH];
    if (GetFullPathName(("../src/crosshairs/crosshair" + std::to_string(crosshairIndex) + ".png").c_str(), MAX_PATH, cross_path, NULL) == NULL) {
        std::cout << "Image not found.\n";
        goto loop;
    }

    sf::Texture texture;
    if (texture.loadFromFile(cross_path) == 0)
    {
        std::cerr << "Failed to load image.\n" << std::endl;
        return 1;
    }


    HWND sfmlWindow = window.getSystemHandle();
    MARGINS margins;
    margins.cxLeftWidth = -1;

    DwmExtendFrameIntoClientArea(sfmlWindow, &margins);
    SetWindowPos(sfmlWindow, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOSIZE);
    SetWindowLong(sfmlWindow, GWL_EXSTYLE, GetWindowLong(sfmlWindow, GWL_EXSTYLE) | WS_EX_LAYERED | WS_EX_TRANSPARENT);
    SetLayeredWindowAttributes(sfmlWindow, 0, 255, LWA_ALPHA);

    sf::Sprite sprite;
    sprite.setTexture(texture);
    float scaleX = 0.09f;
    float scaleY = 0.09f;

    sprite.setScale(scaleX, scaleY);

    sf::Vector2u windowSize = window.getSize();

    float xPos = (windowSize.x - sprite.getGlobalBounds().width) / 2.0f;
    float yPos = (windowSize.y - sprite.getGlobalBounds().height) / 2.0f;

    sprite.setPosition(xPos, yPos);

    std::cout << "Press 'F1' to change the crosshair.\n";
    std::cout << "Press 'DELETE' to exit the program.\n";

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            switch (event.type) {
            case sf::Event::Closed:
                window.close();
            }
        }

        POS screen_pos;
        GetWindowPosition(&screen_pos, hwnd);
        
        sf::Vector2i newPos;
        newPos.x = (screen_pos.R + screen_pos.L) / 2 - 39.5;
        newPos.y = (screen_pos.B + screen_pos.T) / 2 - 24;

        window.setPosition(newPos);

        window.clear(sf::Color::Transparent);

        window.draw(sprite);

        window.display();

        if (GetAsyncKeyState(VK_F1) & 0x8000) {
            window.close();
            goto loop;
        }

        if (GetAsyncKeyState(VK_DELETE) & 0x8000) {
            std::cout << "Exiting!\n";
            exit(EXIT_SUCCESS);
        }
    }

    return 0;
}
