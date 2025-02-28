/*
* 游戏名：喜欢布丁的大小姐（Remilia Loves Pudding）
* 编译环境：Visual Studio 2019 C++ EasyX
* 操作方法：详见设计说明
* 最后修改：2021/8/26
*/
#include<stdio.h>
#include<graphics.h>
#include<mmsystem.h>
#include<conio.h>
#include<time.h>
#pragma comment(lib,"winmm.lib")
#define WIN_WIDTH 1280
#define WIN_HEIGHT 720
#define CELL_SIZE 35 //基本图像单元
#define ROW (WIN_HEIGHT/CELL_SIZE)
#define COL (WIN_WIDTH/CELL_SIZE)
#define PUD_SPAN 20 //布丁换位秒数
#define MUSH_SPAN 10 //蘑菇换位秒数

enum ATTR
{
	//格子类型
	SPACE,
	BOOM,//炸弹简写
	WALL,
	PUD,//布丁简写
	SUN,
	MUSH,//蘑菇简写
	//游戏难度控制BGM
	HARD,
	MEDIUM,
	EASY,
	//key_ctrl函数返回值
	NORMAL,
	DEAD,
	ESC,
	SUCCESS
};
struct hero
{
	int x;
	int y;
	int width;
	int height;
	int img_x;
	int img_y;
}hero;
struct pudding
{
	int x;
	int y;
}pudding;
struct mushroom
{
	int x;
	int y;
}mushroom;

IMAGE bk;
IMAGE helps[4];
IMAGE settings_bk;
IMAGE booms[2];
IMAGE mush;
IMAGE wall;
IMAGE role;
IMAGE pud;
IMAGE sun;
IMAGE success_bk;
IMAGE fail_bk;

int map[ROW][COL];
int boom_num = 80;
int time_lim = 30;
int life_num = 2;
int sunshine = 1;
int pud_left = 50;
int game_mode = MEDIUM;
int mush_lim;
char cleaner;
bool no_time = FALSE;
bool move_mod = FALSE;
clock_t pud_time_start;
clock_t pud_time_end;
clock_t mush_time_start;
clock_t mush_time_end;
clock_t time_start;
clock_t time_end;

//非游戏界面
void menu_page();
void help();
void settings();
void dead_page();
void success_page();
//游戏界面绘制
void init_game();
void game_draw();
void init_pud();
void init_mush();
//处理键盘操作信息
int key_ctrl(int speed);
void judge_wall();
void judge_sun();
int judge_boom();
int judge_mush();
int judge_pud();

int main()
{
	int state;
	initgraph(WIN_WIDTH, WIN_HEIGHT);
	while (1)
	{
		mush_lim = 0;
		life_num = 2;
		game_mode = MEDIUM;
		int boom_num = 80;
		int time_lim = 30;
		menu_page();
		init_game();
		BeginBatchDraw();//建立二级缓存防屏闪
		time_start = pud_time_start = mush_time_start = clock();
		while (1)
		{
			game_draw();
			FlushBatchDraw();
			state = key_ctrl(1);
			if (state == ESC)
			{
				break;
			}
			else if (state == DEAD)
			{
				EndBatchDraw();
				cleardevice();
				mciSendString("close main", 0, 0, 0);
				dead_page();
				break;
			}
			else if (state == SUCCESS)
			{
				EndBatchDraw();
				cleardevice();
				mciSendString("close main", 0, 0, 0);
				success_page();
				break;
			}
		}
	}
	return 0;
}

void success_page()
{
	mciSendString("open ./audios/success.mp3 alias success", 0, 0, 0);
	mciSendString("play success", 0, 0, 0);
	loadimage(&success_bk, "./images/success.png", WIN_WIDTH,WIN_HEIGHT);
	putimage(0, 0, &success_bk);
	do {//清理之前的缓冲区，下同
		cleaner = _getch();
	} while (cleaner != ' ');
	mciSendString("close success", 0, 0, 0);
	return;
}

void dead_page()
{
	
	mciSendString("open ./audios/fail.mp3 alias fail", 0, 0, 0);
	mciSendString("play fail", 0, 0, 0);
	loadimage(&success_bk, "./images/fail.png", WIN_WIDTH, WIN_HEIGHT);
	putimage(0, 0, &success_bk);
	do {
		cleaner = _getch();
	} while (cleaner != ' ');
	mciSendString("close fail", 0, 0, 0);
	return;
}

void judge_sun()
{
	mciSendString("open ./audios/sun.mp3 alias sun", 0, 0, 0);
	mciSendString("play sun", 0, 0, 0);
	hero.img_x = 2;
	hero.img_y = 3;
	putimage(hero.x, hero.y, hero.width, hero.height, &role, hero.img_x * hero.width, hero.img_y * hero.height);
	FlushBatchDraw();//由于开启缓存，不写这一步，效果不会显示，下同
	Sleep(1000);
	mciSendString("close sun", 0, 0, 0);
}

void judge_wall()
{
	mciSendString("open ./audios/wall.mp3 alias wall", 0, 0, 0);
	mciSendString("play wall", 0, 0, 0);
	hero.img_x = 2;
	hero.img_y = 3;
	putimage(hero.x, hero.y, hero.width, hero.height, &role, hero.img_x * hero.width, hero.img_y * hero.height);
	FlushBatchDraw();
	Sleep(1000);
	mciSendString("close wall", 0, 0, 0);
}

int judge_pud()
{
	mciSendString("close pud", 0, 0, 0);
	mciSendString("open ./audios/set_ok.mp3 alias pud", 0, 0, 0);
	mciSendString("play pud", 0, 0, 0);
	pud_time_start = pud_time_end;
	map[pudding.x][pudding.y] = SPACE;
	init_pud();
	pud_left--;
	if (pud_left <= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int judge_mush()
{
	life_num++;
	mciSendString("close eat_mush", 0, 0, 0);
	mciSendString("open ./audios/eat_mush.mp3 alias eat_mush", 0, 0, 0);
	mciSendString("play eat_mush", 0, 0, 0);
	map[mushroom.x][mushroom.y] = SPACE;
	init_mush();
	mush_lim++;
	if (mush_lim >= 4)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

int judge_boom(int x, int y)
{
	putimage(y * CELL_SIZE, x * CELL_SIZE, &booms[1]);
	mciSendString("open ./audios/boom.mp3 alias boom", 0, 0, 0);
	mciSendString("play boom", 0, 0, 0);
	hero.img_x = 2;
	hero.img_y = 3;
	putimage(hero.x, hero.y, hero.width, hero.height, &role, hero.img_x * hero.width, hero.img_y * hero.height);
	FlushBatchDraw();
	Sleep(1000);
	map[x][y] = SPACE;
	mciSendString("close boom",0,0,0);
	life_num--;
	if (life_num <= 0)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}



void init_mush()
{
	while (1)
	{
		int x = rand() % ROW;
		int y = rand() % COL;
		if (map[x][y] == SPACE)
		{
			mushroom.x = x;
			mushroom.y = y;
			map[x][y] = MUSH;
			break;
		}
	}
	return;
}

void init_pud()
{
	while (1)
	{
		int x = rand() % ROW;
		int y = rand() % COL;
		if (map[x][y] == SPACE)
		{
			pudding.x = x;
			pudding.y = y;
			map[x][y] = PUD;
			break;
		}
	}
	return;
}

int key_ctrl(int speed)//这个函数写得好像很失败，但是无能的我找不到其他办法了
{
	hero.img_x = 0, hero.img_y = 0;
	if (GetAsyncKeyState('W')|| GetAsyncKeyState(VK_UP))
	{
		hero.img_y = 1;
		if (hero.img_x != 1 && move_mod==FALSE)
		{
			hero.img_x = 1;
			move_mod = TRUE;
		}
		else
		{
			hero.img_x = 0;
			move_mod = FALSE;
		}
		if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == SPACE &&
			map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == SPACE)
			hero.y -= speed;
		else if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == PUD ||
			map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == PUD)
		{
			if (judge_pud())
			{
				return SUCCESS;
			}
		}
		else if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == MUSH ||
			map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == MUSH)
		{
			if (judge_mush())
			{
				return DEAD;
			}
		}
		else if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y - speed) / CELL_SIZE, hero.x / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y - speed) / CELL_SIZE, (hero.x + hero.width) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == WALL ||
			map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == WALL)
		{
			judge_wall();
			return DEAD;
		}
		else if (map[(hero.y - speed) / CELL_SIZE][hero.x / CELL_SIZE] == SUN ||
			map[(hero.y - speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == SUN)
		{
			judge_sun();
			return DEAD;
		}
	}
	else if (GetAsyncKeyState('S')|| GetAsyncKeyState(VK_DOWN))
	{
		hero.img_y = 2;
		if (hero.img_x != 1 && move_mod == FALSE)
		{
			hero.img_x = 1;
			move_mod = TRUE;
		}
		else
		{
			hero.img_x = 0;
			move_mod = FALSE;
		}
		if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == SPACE &&
			map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == SPACE)
			hero.y += speed;
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == PUD ||
			map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == PUD)
		{
			if (judge_pud())
			{
				return SUCCESS;
			}
		}
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y + hero.height + speed) / CELL_SIZE, hero.x / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y + hero.height + speed) / CELL_SIZE, (hero.x + hero.width) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == WALL ||
			map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == WALL)
		{
			judge_wall();
			return DEAD;
		}
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == SUN ||
			map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == SUN)
		{
			judge_sun();
			return DEAD;
		}
		else if (map[(hero.y + hero.height + speed) / CELL_SIZE][hero.x / CELL_SIZE] == MUSH ||
			map[(hero.y + hero.height + speed) / CELL_SIZE][(hero.x + hero.width) / CELL_SIZE] == MUSH)
		{
			if (judge_mush())
			{
				return DEAD;
			}
		}
	}
	else if (GetAsyncKeyState('A')|| GetAsyncKeyState(VK_LEFT))
	{
		if (!(hero.img_x == 1 && hero.img_y == 0)&&move_mod==FALSE)
		{
			hero.img_x = 1;
			hero.img_y = 0;
			move_mod = TRUE;
		}
		else
		{
			hero.img_x = 0;
			hero.img_y = 3;
			move_mod = FALSE;
		}
		if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == SPACE &&
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == SPACE)
			hero.x -= speed;
		else if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y) / CELL_SIZE, (hero.x - speed) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y + hero.height) / CELL_SIZE, (hero.x - speed) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == MUSH ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == MUSH)
		{
			if (judge_mush())
			{
				return DEAD;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == PUD ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == PUD)
		{
			if (judge_pud())
			{
				return SUCCESS;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == WALL ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == WALL)
		{
			judge_wall();
				return DEAD;
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == SUN ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x - speed) / CELL_SIZE] == SUN)
		{
			judge_sun();
			return DEAD;
		}
	}
	else if (GetAsyncKeyState('D')|| GetAsyncKeyState(VK_RIGHT))
	{
		if (!(hero.img_x == 2 && hero.img_y == 0)&&move_mod==FALSE)
		{
			hero.img_x = 2;
			hero.img_y = 0;
			move_mod = TRUE;
		}
		else
		{
			hero.img_x = 1;
			hero.img_y = 3;
			move_mod = FALSE;
		}
		if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == SPACE &&
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == SPACE)
			hero.x += speed;
		else if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == PUD ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == PUD)
		{
			if (judge_pud())
			{
				return SUCCESS;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y) / CELL_SIZE, (hero.x + hero.width + speed) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == BOOM)
		{
			if (judge_boom((hero.y + hero.height) / CELL_SIZE, (hero.x + hero.width + speed) / CELL_SIZE))
			{
				return DEAD;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == MUSH ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == MUSH)
		{
			if (judge_mush())
			{
				return DEAD;
			}
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == WALL ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == WALL)
		{
			judge_wall();
				return DEAD;
		}
		else if (map[(hero.y) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == SUN ||
			map[(hero.y + hero.height) / CELL_SIZE][(hero.x + hero.width + speed) / CELL_SIZE] == SUN)
		{
			judge_sun();
			return DEAD;
		}
	}
	else if (GetKeyState(VK_ESCAPE) & 0x8000)//返回主界面
	{
		cleaner = _getch(); 
		EndBatchDraw();
		cleardevice();
		mciSendString("close main", 0, 0, 0);
		return ESC;
	}
	else if (GetKeyState(VK_HOME) & 0x8000)//保留的debug阶段作弊按键
	{
	return SUCCESS;
    }
	return 1;
}

void game_draw()
{
	setbkcolor(BLACK);
	cleardevice();
	time_end = pud_time_end = mush_time_end = clock();
	if ((pud_time_end - pud_time_start) / CLK_TCK >= PUD_SPAN)//时间间隔到了则重置位置，下同
	{
		pud_time_start = pud_time_end;
		map[pudding.x][pudding.y] = SPACE;
		init_pud();
	}
	if ((mush_time_end - mush_time_start) / CLK_TCK >= MUSH_SPAN)
	{
		mush_lim = 0;
		mush_time_start = mush_time_end;
		map[mushroom.x][mushroom.y] = SPACE;
		init_mush();
	}
	if ((time_end - time_start) / CLK_TCK >= 30&&no_time==FALSE)//太阳区块为纯白色，随着时间推移从上往下，一层一层增多
	{
		time_start = time_end;
		for (int i = 1; i < COL - 1; i++)
		{
			if (map[sunshine][i] == SPACE)
			{
				map[sunshine][i] = SUN;
			}
		}
		sunshine++;
	}
	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			switch (map[i][j])
			{
			case WALL:
				putimage(j * CELL_SIZE, i * CELL_SIZE,&wall);
				break;
			case BOOM:
				putimage(j * CELL_SIZE, i * CELL_SIZE, &booms[0]);
				break;
			case MUSH:
				putimage(j * CELL_SIZE, i * CELL_SIZE, &mush);
				break;
			case PUD:
				putimage(j * CELL_SIZE, i * CELL_SIZE, &pud);
				break;
			case SUN:
				putimage(j * CELL_SIZE, i * CELL_SIZE, &sun);
				break;
			}
		}
	}
	putimage(hero.x, hero.y, hero.width, hero.height, &role, hero.img_x * hero.width, hero.img_y * hero.height);
}

void init_game()
{
	hero.width = 20;
	hero.height = 29;
	hero.img_x = hero.img_y = 0;
	loadimage(&wall, "./images/wall.jpg", CELL_SIZE, CELL_SIZE);
	loadimage(&booms[0], "./images/boom0.png",CELL_SIZE,CELL_SIZE);
	loadimage(&booms[1], "./images/boom1.png", CELL_SIZE, CELL_SIZE);
	loadimage(&mush, "./images/mush.png", CELL_SIZE, CELL_SIZE);
	loadimage(&role, "./images/role.png", 3*hero.width, 4*hero.height);
	loadimage(&pud, "./images/pud.png", CELL_SIZE, CELL_SIZE);
	loadimage(&sun, "./images/sun.png", CELL_SIZE, CELL_SIZE);

	srand(GetTickCount());//获取变化的随机值

	for (int i = 0; i < ROW; i++)
	{
		for (int j = 0; j < COL; j++)
		{
			map[i][j] = SPACE;
		}
	}

	for (int i = 0; i < ROW; i++)
	{
		map[i][0] = map[i][COL - 1] = WALL;
	}
	for (int i = 0; i < COL; i++)
	{
		map[0][i] = map[ROW - 1][i] = WALL;
	}
	for (int i = 0; i < boom_num; i++)
	{
		int r = rand() % ROW;
		int c = rand() % COL;
		if (map[r][c] == SPACE)
		{
			map[r][c] = BOOM;
		}
	}
	while (1)
	{
		int x = rand() % ROW;
		int y = rand() % COL;
		if (map[x][y] == SPACE)
		{
			hero.x = y * CELL_SIZE;
			hero.y = x * CELL_SIZE;
			break;
		}
	}
	init_mush();
	init_pud();
}

void help()
{
	loadimage(&helps[0], "./images/help1.png", WIN_WIDTH, WIN_HEIGHT);
	loadimage(&helps[1], "./images/help2.png", WIN_WIDTH, WIN_HEIGHT);
	loadimage(&helps[2], "./images/help3.png", WIN_WIDTH, WIN_HEIGHT);
	loadimage(&helps[3], "./images/help4.png", WIN_WIDTH, WIN_HEIGHT);
	putimage(0, 0, &helps[0]);
	cleaner=_getch();
	cleardevice();
	putimage(0, 0, &helps[1]);
	cleaner = _getch();
	cleardevice();
	putimage(0, 0, &helps[2]);
	cleaner = _getch();
	cleardevice();
	putimage(0, 0, &helps[3]);
	cleaner = _getch();
	cleardevice();
	mciSendString("close begin", 0, 0, 0);
	menu_page();
}

void settings()
{
	loadimage(&settings_bk, "./images/settings.png", WIN_WIDTH, WIN_HEIGHT);\
	putimage(0, 0, &settings_bk);
	char op;
	op = _getch();
	while (op != ' ')
	{
		mciSendString("open ./audios/set_ok.mp3 alias ok", 0, 0, 0);
		switch (op) 
		{
		case 'q':
			boom_num = 150;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'w':
			boom_num = 80;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'e':
			boom_num = 50;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'a':
			time_lim = 20;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 's':
			time_lim = 30;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'd':
			no_time = TRUE;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'z':
			life_num = 1;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'x':
			life_num = 2;
			mciSendString("play ok", 0, 0, 0);
			break;
		case 'c':
			life_num = 3;
			mciSendString("play ok", 0, 0, 0);
			break;
		default:
			break;
		}
		op = _getch();
		mciSendString("close ok", 0, 0, 0);
	}
	if (life_num == 1 && time_lim == 20 && boom_num == 150)
	{
		game_mode = HARD;
	}
	else if (life_num == 3 && no_time == TRUE && boom_num == 50)
	{
		game_mode = EASY;
	}
	else
	{
		game_mode = MEDIUM;
	}
	cleardevice();
	mciSendString("close begin", 0, 0, 0);
	menu_page();
}

void menu_page()
{
	loadimage(&bk,"./images/bkp.png",WIN_WIDTH,WIN_HEIGHT);
	putimage(0, 0, &bk);
	mciSendString("open ./audios/bgm1.mp3 alias begin", 0, 0, 0);
	mciSendString("play begin repeat", 0, 0, 0);
	while (1)
	{
		if (GetKeyState('A') & 0x8000)
		{
			Sleep(500);
			cleaner = _getch();
			mciSendString("close begin", 0, 0, 0);
			switch (game_mode)
			{
			case MEDIUM:
				mciSendString("open ./audios/main.mp3 alias main", 0, 0, 0);
				mciSendString("play main repeat", 0, 0, 0);
				break;
			case HARD:
				mciSendString("open ./audios/hard_main.mp3 alias main", 0, 0, 0);
				mciSendString("play main repeat", 0, 0, 0);
				break;
			case EASY:
				mciSendString("open ./audios/easy_main.mp3 alias main", 0, 0, 0);
				mciSendString("play main repeat", 0, 0, 0);
				break;
			default:
				break;
			}
			cleardevice();
			return;
		}
		else if (GetKeyState('B') & 0x8000)
		{
			cleaner = _getch();
			cleardevice();
			help();
		}
		else if (GetKeyState('C') & 0x8000)
		{
			cleaner = _getch();
			cleardevice();
			settings();
		}
	}
}