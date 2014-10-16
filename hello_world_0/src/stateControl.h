#ifndef STATE_CONTROL_H__
#define STATE_CONTROL_H__

#define ALIEN_BLOCK_UPDATE 53 //prime number, makes it work better cause it never updates alien blocks
#define BOSS_SCORE_UPDATE 200
#define TANK_DEATH_UPDATE 200
#define BULLET_UPDATE 5
#define TANK_BULLET_UPDATE 3
#define BOSS_UPDATE 3
#define EXPLOSION_UPDATE 6
#define BOSS_APPEAR 1000
#define ALIEN_BULLET_FIRE_PROBABILITY 250

void makeChange(int);
void drawTankLives(int);

extern char alienState;
extern int gameOver;
extern int tankLives;


#endif /* STATE_CONTROL_H__ */
