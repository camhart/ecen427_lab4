

void fireAlienMissile();
void moveAliens();
void updateAlienMissiles(char);
void drawAlienBlock(int, int);
void drawAlienMissiles();
void eraseAlienMissile(int);
int detectAlienHit(int, int, int);
int getAlienBlockWidth(int);
int getAlienBlockCorner();
void drawExplosion(int, int);

extern int alienPosX;
extern int alienPosY;
extern char missileType[4];
extern int missileX[4];
extern int missileY[4];
extern int hitAlien;
extern int hitAlienX;
extern int hitAlienY;
