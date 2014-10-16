#define SCORE_START_POSITION_X1 24
#define SCORE_STOP_POSITION_X1 52
#define SCORE_START_POSITION_X2 53
#define SCORE_STOP_POSITION_X2 81
#define LIVES_START_POSITION_X1 400
#define LIVES_STOP_POSITION_X1 430
#define LIVES_START_POSITION_X2 431
#define LIVES_STOP_POSITION_X2 450
#define NUMBER_START_POS_X 96
#define NUMBER_WIDTH 12
#define TOP_ROW 6
#define BOTTOM_ROW 15
#define TEXT_HEIGHT 10

extern int score;
extern int one[TEXT_HEIGHT];
extern int two[TEXT_HEIGHT];
extern int three[TEXT_HEIGHT];
extern int zero[TEXT_HEIGHT];
extern int five[TEXT_HEIGHT];

void updateScore(int);
void paintScreeRed();
