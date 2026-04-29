//23I3015 SEA
//23I3030 SEA
//23I3046 SEA

#include <SFML/Graphics.h>
#include <pthread.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <time.h>
#include<stdbool.h>
#include <SFML/System.h> //for clock and timer
#include <semaphore.h>
#include <SFML/Audio.h>


typedef enum {UP, DOWN, LEFT, RIGHT, NONE} Direction;
Direction currentDir;
#define TILE_SIZE 60  
#define MAP_ROWS 11   
#define MAP_COLS 21   
#define GHOST_COUNT 4 
sem_t speedBoosts;
#define MAX_SPEED_BOOSTS 2
typedef struct {
    sfVector2f position;
    bool active;
} SpeedBoostItem;
SpeedBoostItem speedBoostsItems[MAX_SPEED_BOOSTS];
void initSpeedBoosts() {
    speedBoostsItems[0].position = (sfVector2f){100, 100}; // Adjust as needed
    speedBoostsItems[0].active = true;

    speedBoostsItems[1].position = (sfVector2f){500, 300};
    speedBoostsItems[1].active = true;
}

void showHowToPlayScreen(sfRenderWindow* window, sfFont* font) {
    sfText* title = sfText_create();
    sfText_setFont(title, font);
    sfText_setCharacterSize(title, 40);
    sfText_setFillColor(title, sfYellow);
    sfText_setString(title, "How to Play");
    sfText_setPosition(title, (sfVector2f){100, 30});

  const char* instructions[] = {
    "* Use Arrow Keys to move Pac-Man.",
    "* White dots are regular food (+1 point).",
    "* Yellow pellets are power food (+10 points).",
    "* Eat all food and reach 106 points to win.",
    "* Avoid the 4 ghosts — if they touch you, you lose a life.",
    "* You have 3 lives in total.",
    "* Press 'P' to pause/resume the game.",
    "* Press 'E' to exit the game."
};

    sfText* line = sfText_create();
    sfText_setFont(line, font);
    sfText_setCharacterSize(line, 24);
    sfText_setFillColor(line, sfWhite);

    sfText* prompt = sfText_create();
    sfText_setFont(prompt, font);
    sfText_setCharacterSize(prompt, 20);
    sfText_setFillColor(prompt, sfCyan);
    sfText_setString(prompt, "Press any key to return...");
    sfText_setPosition(prompt, (sfVector2f){120, 500});

    while (sfRenderWindow_isOpen(window)) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(window, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(window);
            if (event.type == sfEvtKeyPressed || event.type == sfEvtMouseButtonPressed)
                return;  // Exit the screen
        }

        sfRenderWindow_clear(window, sfBlack);
        sfRenderWindow_drawText(window, title, NULL);

        for (int i = 0; i < sizeof(instructions) / sizeof(instructions[0]); i++) {
            sfText_setString(line, instructions[i]);
            sfText_setPosition(line, (sfVector2f){50, 100 + i * 50});
            sfRenderWindow_drawText(window, line, NULL);
        }

        sfRenderWindow_drawText(window, prompt, NULL);
        sfRenderWindow_display(window);
    }

    sfText_destroy(title);
    sfText_destroy(line);
    sfText_destroy(prompt);
}

void showWinScreen(sfRenderWindow *windowscreen, sfFont *font){
    sfVector2u windowSize = sfRenderWindow_getSize(windowscreen);

    // creating title in sfml
    sfText *Outcome = sfText_create();
    sfText_setString(Outcome, "You won YAYYYYYY");
    sfText_setFont(Outcome, font);
    sfText_setCharacterSize(Outcome, 64);
    sfText_setFillColor(Outcome, sfYellow);
    sfText_setOutlineColor(Outcome, sfBlack);
    sfText_setOutlineThickness(Outcome, 3);
    
    sfFloatRect titleBounds = sfText_getLocalBounds(Outcome);
    //definign the position of title
    sfVector2f titlePoition = {
        (windowSize.x - titleBounds.width) / 2 - titleBounds.left,
        (windowSize.y / 2) - 120
    };
    sfText_setPosition(Outcome, titlePoition);
    sfCircleShape* circles[4];
    for (int i = 0; i < 4; i++) {
        circles[i] = sfCircleShape_create();
        sfCircleShape_setRadius(circles[i], rand() % 30 + 20); // radius between 20 and 50
        sfCircleShape_setFillColor(circles[i], sfColor_fromRGB(rand() % 256, rand() % 256, rand() % 256));
        sfVector2f position = {
            rand() % (windowSize.x - 50), // -50 to prevent going out of window
            rand() % (windowSize.y - 50)
        };
        sfCircleShape_setPosition(circles[i], position);
    }
    bool proceed = false;
    while (sfRenderWindow_isOpen(windowscreen) && !proceed) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(windowscreen, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(windowscreen);
            if (event.type == sfEvtKeyPressed)
                proceed = true;
        }
       //setting window background color to dark blue
        sfRenderWindow_clear(windowscreen, sfColor_fromRGB(15, 15, 50)); 
        for (int i = 0; i < 4; i++) {
            sfRenderWindow_drawCircleShape(windowscreen, circles[i], NULL);
        }
        sfRenderWindow_drawText(windowscreen, Outcome, NULL);
        sfRenderWindow_display(windowscreen);
    }
    for (int i = 0; i < 4; i++) {
        sfCircleShape_destroy(circles[i]);
    }
    sfText_destroy(Outcome);
    exit(1);
}
void showLoseScreen(sfRenderWindow *windowscreen, sfFont *font){
    sfVector2u windowSize = sfRenderWindow_getSize(windowscreen);
    // creating title in sfml
    sfText *Outcome = sfText_create();
    sfText_setString(Outcome, "You Lost SADDDDDDD");
    sfText_setFont(Outcome, font);
    sfText_setCharacterSize(Outcome, 64);
    sfText_setFillColor(Outcome, sfYellow);
    sfText_setOutlineColor(Outcome, sfBlack);
    sfText_setOutlineThickness(Outcome, 3);
    
    sfFloatRect titleBounds = sfText_getLocalBounds(Outcome);
    //definign the position of title
    sfVector2f titlePoition = {
        (windowSize.x - titleBounds.width) / 2 - titleBounds.left,
        (windowSize.y / 2) - 120
    };
    sfText_setPosition(Outcome, titlePoition);
 //creating sad face
    sfCircleShape *face = sfCircleShape_create();
    sfCircleShape_setRadius(face, 100);
    sfCircleShape_setFillColor(face, sfYellow);
    sfVector2f facePosition = {
        (windowSize.x / 2) - 100,  // center of x axis
        (windowSize.y / 2) - 50    // center of y axis and a little below
    };
    sfCircleShape_setPosition(face, facePosition);

    // Create left eye (small black circle)
    sfCircleShape *left_Eye = sfCircleShape_create();
    sfCircleShape_setRadius(left_Eye, 10);
    sfCircleShape_setFillColor(left_Eye, sfBlack);
    sfVector2f leftEyePosition = {
        facePosition.x + 30,
        facePosition.y + 40
    };
    sfCircleShape_setPosition(left_Eye, leftEyePosition);

    // Create right eye (small black circle)
    sfCircleShape *right_Eye = sfCircleShape_create();
    sfCircleShape_setRadius(right_Eye, 10);
    sfCircleShape_setFillColor(right_Eye, sfBlack);
    sfVector2f rightEyePosition = {
        facePosition.x + 130,
        facePosition.y + 40
    };
    sfCircleShape_setPosition(right_Eye, rightEyePosition);

sfRectangleShape *mouth = sfRectangleShape_create();
sfRectangleShape_setSize(mouth, (sfVector2f){60, 5}); 
sfRectangleShape_setFillColor(mouth, sfBlack);

sfVector2f mouthPosition = {
    facePosition.x + 70,
    facePosition.y + 140
};
sfRectangleShape_setPosition(mouth, mouthPosition);
sfRectangleShape_setRotation(mouth, 20);
    bool proceed = false;
    while (sfRenderWindow_isOpen(windowscreen) && !proceed) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(windowscreen, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(windowscreen);
            if (event.type == sfEvtKeyPressed)
                proceed = true;
        }
       //setting window background color to dark blue
        sfRenderWindow_clear(windowscreen, sfColor_fromRGB(15, 15, 50)); 
        // Drawing cute  sad face
        sfRenderWindow_drawCircleShape(windowscreen, face, NULL);
        sfRenderWindow_drawCircleShape(windowscreen, left_Eye, NULL);
        sfRenderWindow_drawCircleShape(windowscreen, right_Eye, NULL);
        sfRenderWindow_drawRectangleShape(windowscreen, mouth, NULL);
        sfRenderWindow_drawText(windowscreen, Outcome, NULL);
        sfRenderWindow_display(windowscreen);
    }
    sfText_destroy(Outcome);
    sfCircleShape_destroy(face);
    sfCircleShape_destroy(left_Eye);
    sfCircleShape_destroy(right_Eye);
    sfRectangleShape_destroy(mouth);
    exit(1);
}
//function to show the welcome screen
void showWelcomeScreen(sfRenderWindow *windowscreen, sfFont *font) {
    sfVector2u windowSize = sfRenderWindow_getSize(windowscreen);

    // creating title in sfml
    sfText *Gametitle = sfText_create();
    sfText_setString(Gametitle, "Pac Man");
    sfText_setFont(Gametitle, font);
    sfText_setCharacterSize(Gametitle, 64);
    sfText_setFillColor(Gametitle, sfYellow);
    sfText_setOutlineColor(Gametitle, sfBlack);
    sfText_setOutlineThickness(Gametitle, 3);
    
    sfFloatRect titleBounds = sfText_getLocalBounds(Gametitle);
    //definign the position of title
    sfVector2f titlePoition = {
        (windowSize.x - titleBounds.width) / 2 - titleBounds.left,
        (windowSize.y / 2) - 120
    };
    sfText_setPosition(Gametitle, titlePoition);

    // Creating promt text
    sfText *Startprompt = sfText_create();
    sfText_setString(Startprompt, "Press any key to start");
    sfText_setFont(Startprompt, font);
    sfText_setCharacterSize(Startprompt, 32);
    sfText_setFillColor(Startprompt, sfWhite);
    sfText_setOutlineColor(Startprompt, sfBlack);
    sfText_setOutlineThickness(Startprompt, 2);

    sfFloatRect promptBounds = sfText_getLocalBounds(Startprompt);
    sfVector2f promptPos = {
        (windowSize.x - promptBounds.width) / 2 - promptBounds.left,
        titlePoition.y + 100
    };
    sfText_setPosition(Startprompt, promptPos);

    sfClock *clock = sfClock_create();//to handle resonanace timely
    bool proceed = false;
    while (sfRenderWindow_isOpen(windowscreen) && !proceed) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(windowscreen, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(windowscreen);
            if (event.type == sfEvtKeyPressed)
                proceed = true;
        }

        // below 4 lines pulsate or animate and dim and darks the color on screen
        float time = sfTime_asSeconds(sfClock_getElapsedTime(clock));
        sfUint8 alphaResonate = (sfUint8)(128 + 127 * sin(time * 2.0f)); // pulsating alpha
        sfColor promptColor = sfText_getFillColor(Startprompt);
        promptColor.a = alphaResonate;
        sfText_setFillColor(Startprompt, promptColor);
       //setting window background color to dark blue
        sfRenderWindow_clear(windowscreen, sfColor_fromRGB(15, 15, 50)); 
        sfRenderWindow_drawText(windowscreen, Gametitle, NULL);
        sfRenderWindow_drawText(windowscreen, Startprompt, NULL);
        sfRenderWindow_display(windowscreen);
    }

    sfClock_destroy(clock);
    sfText_destroy(Gametitle);
    sfText_destroy(Startprompt);
}

//Function to display the menu screen
void showMenuScreen(sfRenderWindow *windowscreen, sfFont *font) {
    const char *menuOptions[] = {
        "Start Game",
        "Game Guide",
        "Exit"
    };
    const int NoOfItems = 3;
    int Option_selected = 0;

    sfText *menuText[NoOfItems];
    for (int i = 0; i < NoOfItems; i++) {
        menuText[i] = sfText_create();
        sfText_setString(menuText[i], menuOptions[i]);
        sfText_setFont(menuText[i], font);
        sfText_setCharacterSize(menuText[i], 36);
        sfText_setOutlineThickness(menuText[i], 2);
    }

    sfVector2u windowSize = sfRenderWindow_getSize(windowscreen);
    bool proceed = false;
    while (sfRenderWindow_isOpen(windowscreen) && !proceed) {
        sfEvent event;
        while (sfRenderWindow_pollEvent(windowscreen, &event)) {
            if (event.type == sfEvtClosed)
                sfRenderWindow_close(windowscreen);
            if (event.type == sfEvtKeyPressed) {
            //checkign which key is pressed and moving up and down between options to check for ther required option and storing it in selected option and then doing the required task
                if (event.key.code == sfKeyDown)
                    Option_selected = (Option_selected + 1) % NoOfItems;
                if (event.key.code == sfKeyUp)
                    Option_selected = (Option_selected - 1 + NoOfItems) % NoOfItems;
                if (event.key.code == sfKeyReturn) {
                    if (Option_selected == 0) return; // Start game
                    if (Option_selected == 1)
                       showHowToPlayScreen(windowscreen, font);
                    if (Option_selected == 2)
                     sfRenderWindow_close(windowscreen);
                }
            }
        }

        sfRenderWindow_clear(windowscreen, sfColor_fromRGB(15, 15, 45));

        for (int i = 0; i < NoOfItems; i++) {
        //when a particular option is selected then its color and outline is changes 
            sfColor color = (i == Option_selected) ? sfYellow : sfWhite;
            sfColor outline = (i == Option_selected) ? sfRed : sfBlack;
            sfText_setFillColor(menuText[i], color);
            sfText_setOutlineColor(menuText[i], outline);

            sfFloatRect bounds = sfText_getLocalBounds(menuText[i]);
            sfVector2f pos = {
                (windowSize.x - bounds.width) / 2 - bounds.left,
                200 + i * 70
            };
            sfText_setPosition(menuText[i], pos);
            sfRenderWindow_drawText(windowscreen, menuText[i], NULL);
        }

        sfRenderWindow_display(windowscreen);
    }

    for (int i = 0; i < NoOfItems; i++)
        sfText_destroy(menuText[i]);
}
// Map of the gaem where hashtags are showing walls, G for ghosts and P for player and o for power pallets and . for food
char map[MAP_ROWS][MAP_COLS+1] = {
    "#####################",
    "#........#..........#",
    "#.##.###.#.###.###..#",
    "#o#......#......###.#",
    "#.##.#.###  ##.####.#",
    "#....#..#GGGG#......#",
    "#####.# ### #.## ####",
    "#........P.........##",
    "#.##.#.#####.#.# #.##",
    "#o##.....#.....#.#o##",
    "#####################"
};
sfTexture* normalGhostTexture = NULL;
sfTexture* scaredGhostTexture = NULL;
// definign a structure for Ghost 
typedef struct {
    sfVector2f position;
    int direction; // 1: left, 2: right, 3: up, 0: down
    sfSprite* sprite;
    char symbol;
    int id; // ghost index to manage identity/colors if needed
    int isFast;         // 1 if ghost is fast, 0 otherwise
    int hasSpeedBoost;  // 1 if boost is acquired
    sfTime boostStartTime;
    int boostActive;
    Direction currentDir; 
} Ghost;

// choosing a position for the pacman
sfVector2f pacmanPos = {10 * TILE_SIZE + TILE_SIZE/2, 7 * TILE_SIZE + TILE_SIZE/2}; 
int direction = 0;
int lives=3;
int score = 0;
int running = 1;
float powerlesstime=0.0f;
//creating lock fro synchronization
pthread_mutex_t lock;
//making 4 ghosts
Ghost ghosts[GHOST_COUNT];

// Function to check wall collisions
int isWall(float x, float y, float radius) {
    float checks[9][2] = {
        {x, y}, {x - radius, y}, {x + radius, y},
        {x, y - radius}, {x, y + radius},
        {x - radius, y - radius}, {x + radius, y - radius},
        {x - radius, y + radius}, {x + radius, y + radius}
    };
    
    for (int i = 0; i < 9; i++) {
    //calculating the row and col to check for position and then checking if there is a wall at that position
        int col = checks[i][0] / TILE_SIZE;
        int row = checks[i][1] / TILE_SIZE;
        
        if (row < 0 || row >= MAP_ROWS || col < 0 || col >= MAP_COLS)
            return 1;
            
        if (map[row][col] == '#')
            return 1;
    }
    
    return 0;
}

// belwo function simply draws the game
void drawMaze(sfRenderWindow* window) {
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            char tile = map[row][col];
            sfVector2f position = {col * TILE_SIZE, row * TILE_SIZE};

            if (tile == '#') {
                sfRectangleShape* walls= sfRectangleShape_create();
                sfRectangleShape_setSize(walls, (sfVector2f){TILE_SIZE, TILE_SIZE});
                sfRectangleShape_setPosition(walls, position);
                sfRectangleShape_setFillColor(walls, sfColor_fromRGB(0, 0, 255));
                sfRenderWindow_drawRectangleShape(window, walls, NULL);
                sfRectangleShape_destroy(walls);
            } else if (tile == '.') {
                sfCircleShape* food = sfCircleShape_create();
                sfCircleShape_setRadius(food, 3);
                sfCircleShape_setPosition(food, (sfVector2f){position.x + TILE_SIZE/2 - 3, position.y + TILE_SIZE/2 - 3});
                sfCircleShape_setFillColor(food, sfWhite);
                sfRenderWindow_drawCircleShape(window, food, NULL);
                sfCircleShape_destroy(food);
            } else if (tile == 'o') {
                sfCircleShape* powerpellet = sfCircleShape_create();
                sfCircleShape_setRadius(powerpellet, 6);
                sfCircleShape_setPosition(powerpellet, (sfVector2f){position.x + TILE_SIZE/2 - 6, position.y + TILE_SIZE/2 - 6});
                sfCircleShape_setFillColor(powerpellet, sfYellow);
                sfRenderWindow_drawCircleShape(window, powerpellet, NULL);
                sfCircleShape_destroy(powerpellet);
            }
        }
    }
    
}
bool powerful_eatpower=0;
// Drawing ghosts
void drawGhosts(sfRenderWindow* window, Ghost ghosts[], int count) {
    for (int i = 0; i < count; i++) {
        sfSprite* ghost_Sprite = sfSprite_create();

        // choosign whether to display normal or scared ghost
        if (powerful_eatpower)
            sfSprite_setTexture(ghost_Sprite, scaredGhostTexture, sfTrue);
        else
            sfSprite_setTexture(ghost_Sprite, normalGhostTexture, sfTrue);
        sfSprite_setPosition(ghost_Sprite, (sfVector2f){
            ghosts[i].position.x - TILE_SIZE / 2,
            ghosts[i].position.y - TILE_SIZE / 2
        });

        // adjusting the size of pic to match with tile size
        float scaleFactor = (float)TILE_SIZE /2500.0f;
        sfSprite_setScale(ghost_Sprite, (sfVector2f){scaleFactor, scaleFactor});

        sfRenderWindow_drawSprite(window, ghost_Sprite, NULL);
        sfSprite_destroy(ghost_Sprite);
    }
}

void initGhosts(Ghost ghosts[], int count) {
    int ghostIndex = 0;
    for (int row = 0; row < MAP_ROWS; row++) {
        for (int col = 0; col < MAP_COLS; col++) {
            if (map[row][col] == 'G' && ghostIndex < count) {
                ghosts[ghostIndex].position = (sfVector2f){
                    col * TILE_SIZE + TILE_SIZE/2, 
                    row * TILE_SIZE + TILE_SIZE/2
                };
                ghosts[ghostIndex].direction = rand() % 4;
                ghosts[ghostIndex].sprite = sfSprite_create();
                sfSprite_setTexture(ghosts[ghostIndex].sprite, normalGhostTexture, sfTrue);
                sfSprite_setOrigin(ghosts[ghostIndex].sprite, (sfVector2f){TILE_SIZE/2, TILE_SIZE/2});
                ghosts[ghostIndex].symbol = 'G';
                ghosts[ghostIndex].id = ghostIndex;
                ghosts[ghostIndex].isFast = (ghostIndex >= 2) ? 1 : 0;  // First 2 slow (isFast=0), last 2 fast (isFast=1)
                ghosts[ghostIndex].hasSpeedBoost = 0;
                ghosts[ghostIndex].currentDir = NONE;
                ghostIndex++;
            }
        }
    }
}


void ghostturnblue(Ghost ghosts[], int count) {
    for (int i = 0; i < count; i++) {
        sfSprite_setTexture(ghosts[i].sprite, scaredGhostTexture, sfTrue);
    }
}

void ghostturnbluereset(Ghost ghosts[], int count) {
    for (int i = 0; i < count; i++) {
        sfSprite_setTexture(ghosts[i].sprite, normalGhostTexture, sfTrue);
    }
}
//ghost thread function to handle the movement of ghosts
void* ghostThread(void* arg) {
    Ghost* ghost = (Ghost*)arg;
    sfClock* clock = sfClock_create(); // Thread-local clock
    sfClock* boostCycleClock = NULL;
    const float BOOST_DURATION = 10.0f;
    const float BOOST_INTERVAL = 20.0f;

    if (ghost->id == 0) {
        boostCycleClock = sfClock_create(); // Only for Ghost 0
    }

    while (running) {
        sfTime dt = sfClock_restart(clock);
        float stepSizeBase = ghost->isFast ? 35.0f : 25.0f;

        // Speed boost logic for ghost 0
        if (ghost->id == 0) {
            float cycleTime = sfTime_asSeconds(sfClock_getElapsedTime(boostCycleClock));
            ghost->hasSpeedBoost = (cycleTime >= 0 && cycleTime < BOOST_DURATION);
            if (cycleTime >= BOOST_INTERVAL) {
                sfClock_restart(boostCycleClock);
            }
        }

        float stepSize = stepSizeBase * sfTime_asSeconds(dt);
        if (ghost->hasSpeedBoost) stepSize *= 2.0f;

        pthread_mutex_lock(&lock);

        sfVector2f target;

        if (ghost->id < 2) {
            // Ghosts 0 and 1: Chase Pac-Man
            target = pacmanPos;
        } else {
               Direction possibleDirs[4];
    int dirCount = 0;

    int cx = ghost->position.x / TILE_SIZE;
    int cy = ghost->position.y / TILE_SIZE;

    // Only change direction at center of tile
    if ((int)ghost->position.x % TILE_SIZE == TILE_SIZE / 2 &&
        (int)ghost->position.y % TILE_SIZE == TILE_SIZE / 2) {

        // Check possible directions
        if (!isWall((cx) * TILE_SIZE + TILE_SIZE / 2, (cy - 1) * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 3) && ghost->currentDir != DOWN)
            possibleDirs[dirCount++] = UP;
        if (!isWall((cx) * TILE_SIZE + TILE_SIZE / 2, (cy + 1) * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 3) && ghost->currentDir != UP)
            possibleDirs[dirCount++] = DOWN;
        if (!isWall((cx - 1) * TILE_SIZE + TILE_SIZE / 2, (cy) * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 3) && ghost->currentDir != RIGHT)
            possibleDirs[dirCount++] = LEFT;
        if (!isWall((cx + 1) * TILE_SIZE + TILE_SIZE / 2, (cy) * TILE_SIZE + TILE_SIZE / 2, TILE_SIZE / 3) && ghost->currentDir != LEFT)
            possibleDirs[dirCount++] = RIGHT;

        if (dirCount > 0) {
            ghost->currentDir = possibleDirs[rand() % dirCount];
        } else {
            // If stuck, allow turning back
            switch (ghost->currentDir) {
                case UP:    ghost->currentDir = DOWN; break;
                case DOWN:  ghost->currentDir = UP; break;
                case LEFT:  ghost->currentDir = RIGHT; break;
                case RIGHT: ghost->currentDir = LEFT; break;
                default:    break;
            }
        }
    }

    // Move in the chosen direction
    sfVector2f move = ghost->position;
    switch (ghost->currentDir) {
        case UP:    move.y -= stepSize; break;
        case DOWN:  move.y += stepSize; break;
        case LEFT:  move.x -= stepSize; break;
        case RIGHT: move.x += stepSize; break;
        default:    break;
    }

    if (!isWall(move.x, move.y, TILE_SIZE / 3)) {
        ghost->position = move;
    } else {
        ghost->currentDir = NONE; // Stop until a new direction is chosen
    }

    pthread_mutex_unlock(&lock);
    usleep(16000);
    continue;
        }

        sfVector2f nextPos = ghost->position;
        float minDistance = INFINITY;
        int bestDirection = ghost->direction;

        // Try all 4 directions
        for (int dir = 0; dir < 4; dir++) {
            sfVector2f tempPos = ghost->position;
            switch (dir) {
                case 0: tempPos.x += stepSize; break;
                case 1: tempPos.x -= stepSize; break;
                case 2: tempPos.y -= stepSize; break;
                case 3: tempPos.y += stepSize; break;
            }

            if (!isWall(tempPos.x, tempPos.y, TILE_SIZE / 3)) {
                float dx = tempPos.x - target.x;
                float dy = tempPos.y - target.y;
                float distance = dx * dx + dy * dy;

                if (distance < minDistance) {
                    minDistance = distance;
                    bestDirection = dir;
                }
            }
        }

        // Apply best movement
        ghost->direction = bestDirection;
        switch (ghost->direction) {
            case 0: ghost->position.x += stepSize; break;
            case 1: ghost->position.x -= stepSize; break;
            case 2: ghost->position.y -= stepSize; break;
            case 3: ghost->position.y += stepSize; break;
        }

        // Collision check (prevent overshooting into walls)
        if (isWall(ghost->position.x, ghost->position.y, TILE_SIZE / 3)) {
            // Revert movement if we ended up in a wall
            switch (ghost->direction) {
                case 0: ghost->position.x -= stepSize; break;
                case 1: ghost->position.x += stepSize; break;
                case 2: ghost->position.y += stepSize; break;
                case 3: ghost->position.y -= stepSize; break;
            }
            ghost->direction = rand() % 4; // Random new direction on collision
        }

        pthread_mutex_unlock(&lock);
        usleep(16000);
    }

    ghost->hasSpeedBoost = 0;
    sfClock_destroy(clock);
    if (ghost->id == 0 && boostCycleClock) {
        sfClock_destroy(boostCycleClock);
    }

    return NULL;
}
          
// Game engine thread
void* gameEngineThread(void* arg) {
    sfClock* clock = sfClock_create();

    while (running) {
        sfTime dt = sfClock_restart(clock);
        float stepSize = 80 * sfTime_asSeconds(dt);
        sfVector2f nextPos = pacmanPos;
        
        pthread_mutex_lock(&lock);
        
        // Handle Pac-Man movement
        switch (direction) {
            case 0: nextPos.x += stepSize; break;
            case 1: nextPos.x -= stepSize; break;
            case 2: nextPos.y -= stepSize; break;
            case 3: nextPos.y += stepSize; break;
        }

        if (!isWall(nextPos.x, nextPos.y, TILE_SIZE/3)) {
            pacmanPos = nextPos;
        }

        int pacmanCol = pacmanPos.x / TILE_SIZE;
        int pacmanRow = pacmanPos.y / TILE_SIZE;
        
        if (pacmanRow >= 0 && pacmanRow < MAP_ROWS && 
            pacmanCol >= 0 && pacmanCol < MAP_COLS) {
            if (map[pacmanRow][pacmanCol] == '.') {
                score += 1;
               map[pacmanRow][pacmanCol] = ' ';
            } else if (map[pacmanRow][pacmanCol] == 'o') {
                powerful_eatpower=1;
                powerlesstime = 10.0f;  // time till which ghosts stay blue in seconds hahaha
                ghostturnblue(ghosts, 4); 
                score += 5;
               map[pacmanRow][pacmanCol] = ' ';
            }
        }
        
        // checking collision with ghost
        for (int i = 0; i < GHOST_COUNT; i++) {
            float dx = pacmanPos.x - ghosts[i].position.x;
            float dy = pacmanPos.y - ghosts[i].position.y;
            float distance = sqrt(dx*dx + dy*dy);
            
            if (distance < TILE_SIZE/1.5) {
               
                if(lives==0 && powerful_eatpower==0)
                   { running = 0;}
                else if(powerful_eatpower==0)
                   {--lives;//after collision updating pac-man position and pausing for a moment so that player can have some time rather than colliding immediately
                    pacmanPos = (sfVector2f){10 * TILE_SIZE + TILE_SIZE /2, 7  * TILE_SIZE + TILE_SIZE/2};
                   }
                if(powerful_eatpower==1){
                score+=10;//ghost moves back to its house and score is increased by 10
                ghosts[i].position = (sfVector2f){10 * TILE_SIZE + TILE_SIZE / 2, 5 * TILE_SIZE + TILE_SIZE / 2};
                }
                break;
            }
        }
        
        pthread_mutex_unlock(&lock);
        usleep(15000);
    }
    
    sfClock_destroy(clock);
    return NULL;
}

int main() {
 
    srand(time(NULL));
    sfClock* clock = sfClock_create();//creating clock for the timer
    
    // Initialize mutex
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("Mutex initialization failed\n");
        return 1;
    }
    sfMusic* music = sfMusic_createFromFile("pacman background music.ogg");
    if (!music) {
        printf("Failed to load music.\n");
        return 1;  // Handle error
    }

sfMusic_setLoop(music, sfTrue);  // Optional: loop music
sfMusic_play(music);



    // Creating  window
    sfRenderWindow* window = sfRenderWindow_create(
        (sfVideoMode){MAP_COLS * TILE_SIZE, MAP_ROWS * TILE_SIZE, 32},
        "Pac-Man with 4 Ghosts", sfClose, NULL);
    if (!window) return 1;

    // Load ghost textures which are the images
    normalGhostTexture = sfTexture_createFromFile("ghost_red.png", NULL);
    scaredGhostTexture = sfTexture_createFromFile("scared_ghost.png", NULL);

    if (!normalGhostTexture || !scaredGhostTexture) {
        printf("Failed to load ghost textures!\n");
        return 1;
    }
    initGhosts(ghosts, GHOST_COUNT);
    
    // getting font from the file
    sfFont* font = sfFont_createFromFile("Font/PacFont.ttf");
    if (!font) font = sfFont_createFromMemory(NULL, 0); 
    
      showWelcomeScreen(window, font);   
      showMenuScreen(window, font);      

//for score
    sfText* hud = sfText_create();
    sfText_setFont(hud, font);
    sfText_setCharacterSize(hud, 24);
    sfText_setFillColor(hud, sfWhite);
    sfText_setPosition(hud, (sfVector2f){10, 10});
  //for lives  
    sfText* livess = sfText_create();
    sfText_setFont(livess, font);
    sfText_setCharacterSize(livess, 24);
    sfText_setFillColor(livess, sfWhite);
    sfText_setPosition(livess, (sfVector2f){300, 10});
          sfTexture* pacmanTexture = sfTexture_createFromFile("pacman.png", NULL);
      if (!pacmanTexture) {
          printf("Failed to load pacman.png\n");
          return 1;
      }

      sfSprite* pacmanSprite = sfSprite_create();
      sfSprite_setTexture(pacmanSprite, pacmanTexture, sfTrue);
      sfSprite_setScale(pacmanSprite, (sfVector2f){(float)TILE_SIZE / 750, (float)TILE_SIZE / 650}); 

    initSpeedBoosts();
    sem_init(&speedBoosts, 0, 2); 
    // Creating the required threads
    pthread_t threads[GHOST_COUNT + 1];
    pthread_create(&threads[0], NULL, gameEngineThread, NULL);
    for (int i = 0; i < GHOST_COUNT; i++) {
        pthread_create(&threads[i+1], NULL, ghostThread, &ghosts[i]);
    }
    
   int paused = 0;  // Add this at the top

// Main game loop
while (running && sfRenderWindow_isOpen(window)) {
    sfEvent event;

    if (score >= 106)
        showWinScreen(window, font);
    else if (lives == 0)
        showLoseScreen(window, font);

    while (sfRenderWindow_pollEvent(window, &event)) {
        if (event.type == sfEvtClosed)
            running = 0;

        if (event.type == sfEvtKeyPressed) {
            pthread_mutex_lock(&lock);
            switch (event.key.code) {
                case sfKeyRight: direction = 0; break;
                case sfKeyLeft:  direction = 1; break;
                case sfKeyUp:    direction = 2; break;
                case sfKeyDown:  direction = 3; break;
                case sfKeyEscape: running = 0; break;
                case sfKeyP: paused = !paused; break;
                case sfKeyE: exit(0);
               break;
                default: break;
            }
            pthread_mutex_unlock(&lock);
        }
    }

    if (!paused) {
        sfTime time = sfClock_getElapsedTime(clock); 
        float decreasingTime = time.microseconds / 1000000.0f;  
        sfClock_restart(clock);

        if (powerful_eatpower == 1) {
            powerlesstime -= decreasingTime;
            if (powerlesstime <= 0) {
                powerful_eatpower = false;
                ghostturnbluereset(ghosts, 4); 
            }
        }

        // Score and lives display
        char scoreText[50];
        snprintf(scoreText, sizeof(scoreText), "Score: %d", score);
        sfText_setString(hud, scoreText);

        char livesText[50];
        snprintf(livesText, sizeof(livesText), "Lives: %d", lives);
        sfText_setString(livess, livesText);

        // Drawing
        sfRenderWindow_clear(window, sfBlack);
        drawMaze(window);
        drawGhosts(window, ghosts, GHOST_COUNT);

        sfSprite_setPosition(pacmanSprite, (sfVector2f){
            pacmanPos.x - TILE_SIZE / 2,
            pacmanPos.y - TILE_SIZE / 2
        });
        sfRenderWindow_drawSprite(window, pacmanSprite, NULL);
        sfRenderWindow_drawText(window, hud, NULL);
        sfRenderWindow_drawText(window, livess, NULL);
        sfRenderWindow_display(window);
    } else {
        // When paused, show pause message
        sfRenderWindow_clear(window, sfBlack);
        sfText* pauseText = sfText_create();
        sfText_setFont(pauseText, font);
        sfText_setCharacterSize(pauseText, 32);
       sfText_setFillColor(pauseText, sfColor_fromRGBA(191, 162, 219, 255));
        sfText_setString(pauseText, "PAUSED\nPress P to Resume");
        sfText_setPosition(pauseText, (sfVector2f){100, 200});
        sfRenderWindow_drawText(window, pauseText, NULL);
        sfRenderWindow_display(window);
        sfText_destroy(pauseText);
    }
}

    return 0;
}
