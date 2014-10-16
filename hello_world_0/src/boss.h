

void initializeBoss();
void moveBoss();
void drawBoss();
int detectBossHit(int, int, int, int);

extern int bossExists;
extern int bossPosX; //boss starting point
extern int bossTopRow;	//boss top Y position
extern int bossBottomRow;	//boss bottom Y position
extern int bossDir;	 //right (direction boss is moving)
extern int bossExists;
void killBoss();
