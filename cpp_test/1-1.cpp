
#include <limits.h> // for INT_MAX
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <pthread.h>
#include <list>
#include <stack>
#include <vector>
#include <algorithm>
#ifdef WIN32
 #include <windows.h>
 #include <conio.h> // for console I/O
 #define sleep(x) Sleep(1000 * (x))
 #define msleep(x) Sleep(x)
 #define CLEAR_TERM system("CLS");
#else
 #include <unistd.h>
 #define msleep(x) usleep(1000 * (x))
 #define CLEAR_TERM system("clear");
#endif
namespace game {
// using namespace std;
/////////////////////////////////////////////////////////////////////////////
// for debug
#ifdef DEBUG
struct Logger {
 FILE* out;
 Logger(FILE *pf) : out(pf) {}

 void operator()(const char *format, ...) {
  va_list args;
  va_start(args, format);
  vfprintf(out, format, args);
  va_end(args);
 }
};
Logger outLogger(stdout);
Logger errLogger(stderr);
#define log outLogger
#define err errLogger
// void logger(const char *format, ...)
// {
 // va_list args;
 // va_start(args, format);
 // vprintf(format, args);
 // va_end(args);
// }
#else
#define logger(fmt, ...) fmt
#define log logger
#define err logger
#endif
// #define log logger
// #define log outLogger
/////////////////////////////////////////////////////////////////////////////
// key values:
#define K_SPACE 32
#define K_ESC 27
#define K_W   119
#define K_S   115
#define K_A   97
#define K_D   100
// up, down, left, right Pressed 1 key Return 2 value 
#define K_DIR   224  // ignore this 
#define K_UP    72
#define K_DOWN  80
#define K_LEFT  75
#define K_RIGHT 77
// common constants:
#ifndef DELAY 
#define GAME_CYCLE_MS 1000
#else 
#define GAME_CYCLE_MS DELAY
#endif
#define MAX_BODY_LEN  128
#define MAX_FOOD_NUM  8
#define WIDTH  64
#define HEIGHT 24
// char constants:
#define CH_BORDER '#'
#define CH_BLANK  ' '
#define CH_SNAKE  '*'
#define CH_SNAKEH '@'
#define CH_SNAKET '+'
#define CH_FOOD   '$'
#define CH_MINE   '#'

/////////////////////////////////////////////////////////////////////////////
enum Direction
{
 UNKNOW, UP, DOWN, LEFT, RIGHT
};
struct Point
{
 int x;
 int y;

 Point() : x(0), y(0) {}
 Point(int xx, int yy) : x(xx), y(yy) {}

 bool operator==(const Point &rhs) const {
  return x == rhs.x && y == rhs.y;
 }

 Point& operator+=(const Point &rhs) {
  x += rhs.x;
  y += rhs.y;
  return *this;
 }

 Point operator+(const Point &rhs) const {
  Point res(rhs);
  res.x += x;
  res.y += y;
  return res;
 }

#ifdef DEBUG
 void show() {
  log("Point_%p:(%d, %d)\n", this, x, y);
 }
#endif
};
/////////////////////////////////////////////////////////////////////////////
/*
dimension:
 0---x+

 |
 y
 +
*/
Point operator+(const Point &point, const Direction &dir)
{
 Point pt(point);
 switch(dir) {
  case UP: 
   pt.y--; break;
  case DOWN:
   pt.y++; break;
  case LEFT:
   pt.x--; break;
  case RIGHT:
   pt.x++; break;
  default:
   err("ERROR: Point + Direction Error!\n");
   break;
 }
 return pt;
}
/////////////////////////////////////////////////////////////////////////////
class Snake
{
 typedef std::vector<Point> body_type;
 typedef body_type::iterator body_iter;
 typedef body_type::const_iterator body_citer;
 Direction dir;  // 前进方向
 // Point body[MAX_BODY_LEN]; // 身体位置
 body_type body; // 身体位置

public:
 Snake(): dir(UNKNOW) {}
 Snake(Direction d) : dir(d) {}

 void setDir(Direction dir) {
  this->dir = dir;
 }

 void setHead(Point p) {
  log("setHead...\n");
  // p.show();
  if(body.size() == 0) {
   body.insert(body.begin(), p);
  }
 }

 int length() const { return body.size(); }

 Point getNode(int ino) const { return body[ino]; }

 Point getHead() const {
  return *body.begin();
 }

 Point nextHead() const {
  return getHead() + dir;
 }

 bool isOnBody(Point pt) const {
  for( body_citer it = body.begin(); it != body.end(); ++it ) {
   if( *it == pt ) return true;
  }
  return false;
 }

 bool checkDir(Direction newDir) const {
  if( dir == UP    && newDir == DOWN
   || dir == DOWN  && newDir == UP   
   || dir == LEFT  && newDir == RIGHT 
   || dir == RIGHT && newDir == LEFT) 
   return false;
  return true;
 }
 bool selfCollision() const {
  Point h = getHead(); // next time head position
  // if( length() > 1 && isOnBody(h) ) return true;
  body_citer it = body.begin();
  for( ++it; it != body.end(); ++it ) {
   if( *it == h ) return true;
  }
  return false;
 }

 bool changeDir(Direction newDir) {
  if( checkDir(newDir) ) {
   setDir(newDir);
   return true;
  }
  else {
   return false;
  }
 }

 void move() {
  if( ! selfCollision() ) {
   Point nh = nextHead();
   body.insert(body.begin(), nh);
   body.erase(body.end());
  }
  else {
   err("ERROR: move failed! direction incorrect!\n");
  }
 }

 void growth() {
  Point nh = nextHead();
  body.insert(body.begin(), nh);
 }
 // void putTo(Point axis) {
  // for(body_iter it=body.begin(); it!=body.end(); ++it) {
   // *it += axis;
  // }
 // }

#ifdef DEBUG
 void show() {
  log("Snake_%p:\n", this);
  log("{\n");
  log("  dir: %d,\n", dir);
  log("  body: [");
  for(body_citer it = body.begin(); it != body.end(); ++it) {
   log("(%d, %d), ", it->x, it->y);
  }
  log("]\n}\n");
 }
#endif
};

struct PlayGround 
{
 int width;
 int height;
 bool border;

 PlayGround() : width(0), height(0), border(true) {}
 PlayGround(int w, int h, bool b) : width(w), height(h), border(b) {  }

 // x -- width,  y -- height
 bool inArea(int x, int y) {
  if( border ) {
   if( x < 1 || x >= width-1  ) return false;
   if( y < 1 || y >= height-1 ) return false;
  }
  else { // no border
   if( x < 0 || x >= width  ) return false;
   if( y < 0 || y >= height ) return false;   
  }
  return true;
 }

 bool inArea(Point p) { return inArea(p.x, p.y); }

#ifdef DEBUG
 void show() {
  log("PlayGround_%p:\n", this);
  log("{\n");
  log(" width: %d,\n", width);
  log(" height: %d,\n", height);
  log(" border: %d,\n", border);
  log("}\n");
 }
#endif 
};
enum GameState {
 GS_UNKNOW,
 GS_START,
 GS_PAUSE,
 GS_OVER,
 GS_EXIT
};
class Game 
{
 Snake *snake;
 PlayGround *ground;
 char buffer[HEIGHT][WIDTH+2];
 int  foodCount;
 Point *foodBuffer[MAX_FOOD_NUM];

 int time;
 GameState state;
 std::stack<GameState> gsStack;
public:
 Game() : snake(new Snake(RIGHT)),
  ground(new PlayGround(WIDTH, HEIGHT, true)), foodCount(0) { init(); }

 ~Game() { 
  if(ground) delete ground; 
  if(snake) delete snake;
 }

 void setState(GameState gs) { state = gs; }

 GameState getState() const { return state; }

 void pause() {
  if( state != GS_PAUSE ) {
   state = GS_PAUSE;
   gsStack.push(state);
   log("state: %d, statck.size(): %d\n", state, gsStack.size());
  }
  else {
   state = gsStack.top();
   gsStack.pop();
  }
 }

 void init() {
  memset(buffer, 0, sizeof(buffer));  
  memset(foodBuffer, 0, sizeof(foodBuffer));
  Point ph(2, ground->height/3); // init head pos
  snake->setHead(ph);
  time = 0;
 }

 void syncGround() { // ground => buffer
  for(int x=0; x<ground->width; x++) {
   for(int y=0; y<ground->height; y++) { 
    if( ground->border 
    && ( y == 0 || y == HEIGHT-1
      || x == 0 || x == WIDTH-1  )
    ){
     buffer[y][x] = CH_BORDER;
    }
    else buffer[y][x] = CH_BLANK;
   }
  }
 }

 void syncSnake() { // snake => buffer
  Point head = snake->getNode(0);
  buffer[head.y][head.x] = CH_SNAKEH;
  for(int i=1; i<snake->length()-1; i++) {
   Point p = snake->getNode(i);
   buffer[p.y][p.x] = CH_SNAKE;
  }
  if(snake->length() > 1) {
   Point tail = snake->getNode(snake->length()-1);
   buffer[tail.y][tail.x] = CH_SNAKET;
  }
 }

 void syncFood() { // foodBuffer => buffer
  for(int i=0; i<MAX_FOOD_NUM; i++) {
   Point *p = foodBuffer[i];
   if(NULL != p) {
    buffer[p->y][p->x] = CH_FOOD;
   }
  }
 }

 void draw() { // buffer => console  
  // 0. clear last buffer
  memset(buffer, 0, sizeof(buffer));

  // 1. sync PlayGround
  syncGround();

  // 2. sync Snake
  syncSnake();

  // 3. draw food
  syncFood();

  // 4. draw to console
  for(int i=0; i<HEIGHT; i++) {
   puts(buffer[i]);
  }
 }
 bool checkPos(Point p) const {
  // check for border
  if( ! ground->inArea(p) ) return false;

  // check for snake
  if( snake->isOnBody(p) ) return false;

  // check for foods
  for(int i=0; i<MAX_FOOD_NUM; i++) { 
   if(NULL != foodBuffer[i] && p == *foodBuffer[i]) {
    return false;
   }
  }

  return true;
 }

 void genFood() { // gen food => foodBuffer
  log("Food generate...\n");
  if( foodCount < MAX_FOOD_NUM ) {
   int x, y;
   do {
    x = rand() % WIDTH;
    y = rand() % HEIGHT;
   }while( ! checkPos(Point(x, y)) );

   for(int i=0; i<MAX_FOOD_NUM; i++) {
    if( NULL == foodBuffer[i] ) {
     foodBuffer[i] = new Point(x, y);
     break;
    }
   }

   foodCount++;
   // foodInfo();
  }
 }

 void update() { // move snake once
  ++time;
  Point nh = snake->nextHead();

  // check for eating food
  bool willEat = false;
  int  foodidx = -1;
  for(int i=0; i<MAX_FOOD_NUM; i++) {
   if(foodBuffer[i] && *foodBuffer[i] == nh ) {
    willEat = true;
    foodidx = i;
    break;
   }
  }

  if( willEat ) { // snake growth and food delete.
   snake->growth();

   // food delete.
   delete foodBuffer[foodidx];
   foodBuffer[foodidx] = NULL;
   foodCount--;

   // new food.
   genFood();
  }
  else snake->move();

  // check for wall collision
  if( ground->border ) {
   if( nh.x == 0 || nh.x == WIDTH-1
    || nh.y == 0 || nh.y == HEIGHT-1 ) {
    state = GS_OVER;
   }
  }
  else {
   log("UNDEFINE...");
   exit(-1);
  }

  // check for slef colision
  if( snake->selfCollision() ) {
   state = GS_OVER;
   return;
  }
 }
 void snakeTrun(Direction d) {
  snake->changeDir(d);
 }
 void info() {
  printf("Greedy Snake! length: %d  time:%d\n", snake->length(), time);
 }

#ifdef DEBUG
 void foodInfo() {
  log("foodInfo: {\n");
  log(" foodCount: %d,\n", foodCount);
  log(" foodBuffer: [");
  for(int i=0; i<MAX_FOOD_NUM; i++) {
   Point *p = foodBuffer[i];
   if( p ) log("(%d, %d)", p->x, p->y);
   else log("null");
   log("%s", i != MAX_FOOD_NUM-1 ? ", " : "");
  }
  log("]\n}\n");
 }
 void show() {
  snake->show();
  ground->show();
  log("buffer:\n");
 #if 0
  for(int i=0; i<HEIGHT; i++) {
   for(int j=0; j<WIDTH; j++) {
    log("%02X ", buffer[i][j]);
   }
   log("\n");
  }
 #endif
 }
#endif
};
/////////////////////////////////////////////////////////////////////////////
volatile int keyPressed = INT_MAX;  // 按键缓冲（只记录一次）
volatile bool isDirKey = false;
Game *pGame = NULL;
pthread_t keyLisener;
pthread_mutex_t keyBufferLock;
void* keyListenFun(void *args)
{
 while(1) {
  keyPressed = getch(); // getch will bolck this thread.
  // log("Pressed: %d\n", keyPressed);
 }
 return NULL;
}
void init() 
{
 pGame = new Game();
 // srand( time(NULL) );
 pthread_mutex_init(&keyBufferLock, NULL); 
 pthread_create(&keyLisener, NULL, keyListenFun, NULL); // 创建按键侦听线程
}
void cleanup()
{
 pthread_cancel(keyLisener);
 pthread_mutex_destroy(&keyBufferLock);
 delete pGame;
}
};
/////////////////////////////////////////////////////////////////////////////
int main(int argc, char *argv[])
{
 using namespace game;
 init();

 // pGame->show();
 for(int i=0; i<MAX_FOOD_NUM-1; i++) 
  pGame->genFood();
 pGame->draw();

 while(1) {
  if( GS_PAUSE != pGame->getState() ) {
   CLEAR_TERM

   switch(keyPressed) {
    case K_SPACE: pGame->setState(GS_PAUSE); break;

    case K_UP:
    case K_W: pGame->snakeTrun(UP); break;

    case K_DOWN:
    case K_S: pGame->snakeTrun(DOWN); break;

    case K_LEFT:
    case K_A: pGame->snakeTrun(LEFT); break;

    case K_RIGHT:
    case K_D: pGame->snakeTrun(RIGHT); break;

    case K_ESC: pGame->setState(GS_EXIT); break;
   }

   pGame->info();

   pGame->update();

   pGame->draw();
  }
  else {
   switch(keyPressed) {
    case K_SPACE: pGame->setState(GS_START); break;
    case K_ESC: pGame->setState(GS_EXIT); break;
   }
  }

  if( pGame->getState() == GS_OVER ) {
   puts("game over!");
   break;
  }
  else if( pGame->getState() == GS_EXIT ) {
   puts("exit!");
   break;
  }

  keyPressed = INT_MAX;
  msleep(GAME_CYCLE_MS);
 }

 cleanup();
 return 0;
}