#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <math.h>
#include <time.h>
#include "Magic.h"

#define PI 3.1415926f

/**
 * @section
 * 全局变量
 */

// tick计数器，每帧递增，用于播放逐帧动画（指定要播放哪一帧）
float tick = 0.0f;

/**
 * @section
 * 枚举类型
 */

enum PageSwitch {
	PageSwitch_Standing,
	PageSwitch_Attack,
	PageSwitch_Magic,
	PageSwitch_EnemyRound,
	PageSwitch_Winning,
	PageSwitch_Losing,
	PageSwitch_BackToMain,
	PageSwitch_BackToMap
};

typedef int Blood;

/**
 * @section
 * 结构体
 */

struct Character {
	int x;
	int y;
	int bloodBarOffsetX;
	int bloodBarOffsetY;

	Picture* imgArr;
	int imgArrLen;
	Blood blood;
};

struct WarPageImages {

	// Static Images
	Picture bg;
	Picture bloodBar;
	Picture bloodBar_BottomLayer;
	Picture winTheGame;
	Picture loseTheGame;

	Picture attackButton;
	Picture attackButtonPressed;
	Picture magicButton;
	Picture magicButtonPressed;

	// Me-Related
	Picture meStanding[10];
	Picture meAttack[10];

	// Enemy-Related
	Picture enemyA[10];
	Picture enemyB[10];
	Picture enemyC[10];

	// Magic-Related
	Picture magicAttackA[13];
	Picture magicAttackB[13];
};

/**
 * @section
 * 函数定义
 */

// 实用帮助函数
bool IsMouseInRange(
	int x0, int y0, int x1, int y1
);
bool IsMouseInsidePicture(
	const Picture* picture, int x_, int y_
);
void BatchLoadImage(
	Picture* picArr, int howMany, const char* filenameFormat
);
void LoadWarPageImages(
	WarPageImages& wpImages
);

// 页面函数
bool StarterPage();
void MapPage();
PageSwitch WarPage();

// 战斗页面 子页面函数
PageSwitch WarPage_Standing(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
);
PageSwitch WarPage_Attack(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
);
PageSwitch WarPage_Magic(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
);
PageSwitch WarPage_EnemyAttack(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC, char beingAttackedByWho
);
PageSwitch WarPage_Winning(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
);
PageSwitch WarPage_Losing(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
);

// Animator 一些动画函数
char Scene_ClickEnemy(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
);
void Scene_Walking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	Picture* thingThatWalks, bool stillDrawMeOrNot
);
void Scene_EnemyWalking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	char whichEnemy, bool stillDrawEnemyOrNot
);
void Scene_HittingEnemy(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float thingX, float thingY, char attackWho,
	Picture* thingThatHurts, bool stillDrawMeOrNot, int animationDuration
);
void Scene_BloodUpdate(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, int bloodDrop
);

// Animator 一些动画函数
void Animator_DrawBlood(
	WarPageImages& wpImages, Character& character,
	int customX, int customY
);
void Animator_DrawCharacter(
	WarPageImages& wpImages, Character& character,
	float brightness
);
bool Animator_DrawClickableCharacter(
	WarPageImages& wpImages, Character& character
);
void Animator_LocateWalkToPos(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, float* resultX, float* resultY
);

/**
 * @section
 * 主函数
 */

int main() {
	Magic();
	PageSwitch ps = PageSwitch_BackToMain;

	while (true) {
		if (ps == PageSwitch_BackToMain) {
			bool wannaLogin = StarterPage();
			if (wannaLogin == false) {
				Quit();
				return 0;
			}

			MapPage();
			ps = WarPage();
		}
		else if (ps == PageSwitch_BackToMap) {
			MapPage();
			ps = WarPage();
		}
	}

	Quit();
	return 0;
}

/**
 * @section
 * 实用帮助函数
 */

 // 检测鼠标是否在 (x0, y0) => (x1, y1) 区域内
bool IsMouseInRange(
	int x0, int y0, int x1, int y1
) {
	if (mouseX >= x0) {
		if (mouseX <= x1) {
			if (mouseY >= y0) {
				if (mouseY <= y1) {
					return true;
				}
			}
		}
	}
	return false;
}

// 检测鼠标是否在图片区域内
bool IsMouseInsidePicture(
	const Picture* picture, int x_, int y_
) {
	return IsMouseInRange(x_, y_, picture->width + x_, picture->height + y_);
}

// 批量按特定文件名格式，读取图片，实现动画效果
// 参数 *picArr：传入图片数组，用于存储图片
// 参数 howMany：有几张图片要加载。例如：输入15，则加载数从0到14
// 参数 filenameFormat：图片名的字符串。例如：".\\BMP\\Image%d.bmp"；串中%d会被自动替换
void BatchLoadImage(
	Picture* picArr, int howMany, const char* filenameFormat
) {

	// 用于字符串%d替换的缓冲区
	char buffer[100] = { 0 };

	// 批量按格式，从文件中读取图片
	for (int i = 0; i < howMany; i++) {
		sprintf(buffer, filenameFormat, i);
		picArr[i].Load(buffer);
	}
}

void LoadWarPageImages(
	WarPageImages& wpImages
) {
	// Static Images
	wpImages.bg.Load(".\\BMP\\Images\\WarPageBackground.bmp");
	wpImages.bloodBar.Load(".\\BMP\\Images\\BloodBar.bmp");
	wpImages.bloodBar_BottomLayer.Load(".\\BMP\\Images\\BloodBar_BottomLayer.bmp");
	wpImages.winTheGame.Load(".\\BMP\\Images\\WinTheGame.bmp");
	wpImages.loseTheGame.Load(".\\BMP\\Images\\LoseTheGame.bmp");

	wpImages.attackButton.Load(".\\BMP\\Buttons\\Attack.bmp");
	wpImages.attackButtonPressed.Load(".\\BMP\\Buttons\\Attack_Pressed.bmp");
	wpImages.magicButton.Load(".\\BMP\\Buttons\\Magic.bmp");
	wpImages.magicButtonPressed.Load(".\\BMP\\Buttons\\Magic_Pressed.bmp");

	// Me-Related
	BatchLoadImage(
		wpImages.meStanding, 10,
		".\\BMP\\Animation\\\Me_StandingAnimation\\%02d.bmp"
	);
	BatchLoadImage(
		wpImages.meAttack, 10,
		".\\BMP\\Animation\\\Me_AttackAnimation\\%02d.bmp"
	);

	// Enemy-Related
	BatchLoadImage(
		wpImages.enemyA, 10,
		".\\BMP\\Animation\\\EnemyA\\%02d.bmp"
	);
	BatchLoadImage(
		wpImages.enemyB, 10,
		".\\BMP\\Animation\\\EnemyB\\%02d.bmp"
	);
	BatchLoadImage(
		wpImages.enemyC, 10,
		".\\BMP\\Animation\\\EnemyC\\%02d.bmp"
	);

	// Magic-Related
	BatchLoadImage(
		wpImages.magicAttackA, 13,
		".\\BMP\\Animation\\\MagicAttackA\\%02d.bmp"
	);
	BatchLoadImage(
		wpImages.magicAttackB, 13,
		".\\BMP\\Animation\\\MagicAttackB\\%02d.bmp"
	);
}

/**
 * @section
 * 页面函数
 */

 // 页面1：开始界面
 // 返回值 bool：true代表允许登录游戏，false代表请求退出游戏
bool StarterPage() {

	// 加载音乐文件
	MagicMusic("open .\\Music\\FreshFocus_Clipped.wav on_channel 0");
	MagicMusic("play channel 0 times -1");

	// 函数的返回值，true代表允许登录游戏，false代表请求退出游戏
	bool returnValue;

	// 设置锁帧60FPS
	fpsLockRate = 60;

	// 加载图片资源（starter为背景图，logo为梦幻西游Logo图）
	Picture starter, logo;
	starter.Load(".\\BMP\\Images\\StarterPage.bmp");
	logo.Load(".\\BMP\\Images\\Logo.bmp");

	// 加载图片资源（loginButton为登录游戏，exitButton为退出游戏，Pressed后缀是黑白化版本）
	Picture loginButton, loginButtonPressed, exitButton, exitButtonPressed;
	loginButton.Load(".\\BMP\\Buttons\\LoginButton.bmp");
	loginButtonPressed.Load(".\\BMP\\Buttons\\LoginButton_Pressed.bmp");
	exitButton.Load(".\\BMP\\Buttons\\ExitButton.bmp");
	exitButtonPressed.Load(".\\BMP\\Buttons\\ExitButton_Pressed.bmp");

	// 设定图片资源XY坐标
	int starterX = 0, starterY = 0;
	int logoX = 165, logoY = 25;
	int loginButtonX = 250, loginButtonY = 315;
	int exitButtonX = 250, exitButtonY = 440;

	// Logo图像的缩放动画：控制参数
	// @ speed缩放速度，minSize最小尺寸，portion映射系数
	// @ 正弦函数sin从[-1, +1]变化，portion负责把[-1, +1]映射到[0.75f, 1.0f]
	float speed = 4.0f;
	float minSize = 0.75f;
	float portion = (1 - minSize) * 0.5f;

	// 游戏循环
	bool continueLoop = true;
	while (continueLoop) {

		// Logo图像的缩放动画：每帧参数
		// @ timeState时间状态，sinState正弦状态，ratioState缩放比例
		// timeState获取进程运行至今的毫秒数；乘以0.001f * speed减慢速度，防止变化过快
		// sinState用来获取正弦sin函数的值，用于缩放；乘以portion来进行映射
		// ratioState是最终计算结果（缩放比例），代表图片要缩放为原来的多少倍，例如0.85倍
		float timeState = clock() * 0.001f * speed;
		float sinState = sin(timeState) * portion;
		float ratioState = minSize + portion + sinState;

		// 绘制背景图和Logo图（带缩放效果的Logo图）
		starter.Draw(starterX, starterY);
		logo.DrawZoomCentered(logoX, logoY, ratioState);

		// 检测鼠标是否Hover在“登录游戏”按钮上方
		if (IsMouseInsidePicture(&loginButton, loginButtonX, loginButtonY)) {
			// 如果在，按钮灰色
			loginButtonPressed.Draw(loginButtonX, loginButtonY);
			// 如果不仅Hover，还按下了左键
			if (isLeftClick == true) {
				// 那么依然绘制彩色按钮
				loginButton.Draw(loginButtonX, loginButtonY);
				// 执行完这次游戏循环，就退出循环（为何不直接return？因为还没Show()。）
				continueLoop = false;
				// 设定函数返回值为true，代表允许登录游戏
				returnValue = true;
			}
		}
		else {
			// 如果不在，按钮彩色
			loginButton.Draw(loginButtonX, loginButtonY);
		}

		// 检测鼠标是否Hover在“退出游戏”按钮上方
		if (IsMouseInsidePicture(&exitButton, exitButtonX, exitButtonY)) {
			// 如果在，按钮灰色
			exitButtonPressed.Draw(exitButtonX, exitButtonY);
			// 如果不仅Hover，还按下了左键
			if (isLeftClick == true) {
				// 那么依然绘制彩色按钮
				exitButton.Draw(exitButtonX, exitButtonY);
				// 执行完这次游戏循环，就退出循环（为何不直接return？因为还没Show()。）
				continueLoop = false;
				// 设定函数返回值为false，代表请求退出游戏
				returnValue = false;
			}
		}
		else {
			// 如果不在，按钮彩色
			exitButton.Draw(exitButtonX, exitButtonY);
		}

		// 更新帧缓冲，将画面推送到前台（双缓冲机制）
		Show();
	}

	// 关闭音乐文件
	MagicMusic("stop channel 0");
	MagicMusic("close channel 1");

	// 返回函数的返回值，true代表允许登录游戏，false代表请求退出游戏
	return returnValue;
}

// 页面2：地图界面
void MapPage() {

	// 加载音乐文件
	MagicMusic("open .\\Music\\StudyAndRelax_Clipped.wav on_channel 0");
	MagicMusic("open .\\Music\\FootSound.wav on_channel 1");

	MagicMusic("play channel 0 times -1");
	MagicMusic("play channel 1 times -1");
	MagicMusic("pause channel 1");

	// 显示引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"draw text channel 0 content \"使用箭头方向键，控制人物移动\" x 11 y 11 textColor 0 0 0 bgTransparent true");
	MagicText(L"draw text channel 1 content \"使用箭头方向键，控制人物移动\" x 10 y 10 textColor 255 255 255 bgTransparent true");

	// 设定锁帧15FPS（如果帧率太快，地图移动很平滑流畅，但人物动画帧率低，反而不好，有穿帮感）
	fpsLockRate = 15;

	// 加载地图素材
	Picture map;
	map.Load(".\\BMP\\Images\\Map.bmp");

	// 加载人物逐帧动画素材（上下左右移动各5帧，共20帧）
	// ld lu rd ru是缩写，l代表Left，r代表Right，u代表Up，d代表Down
	Picture ldTexture[5], luTexture[5], rdTexture[5], ruTexture[5];
	BatchLoadImage(ldTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\ld%d.bmp");
	BatchLoadImage(luTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\lu%d.bmp");
	BatchLoadImage(rdTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\rd%d.bmp");
	BatchLoadImage(ruTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\ru%d.bmp");

	// 图片指针，用于指向最后一次播放的图片（用来实现：动画结束后，停留在最后一张图片）
	// 初始值就使用&luTexture[0]，代表没播放动画之前，人物默认朝向Left-Up
	Picture* latestTexture = &ruTexture[0];

	// 地图XY坐标，用于稍后控制地图移动（使用float是为了支持细粒度动画）
	float mapX = -353.0f;
	float mapY = -641.0f;

	// 地图移动速度，每帧4.5像素
	float speed = 0.75f * 6.0f;

	// 重置tick计数器，从头播放帧动画
	tick = 0.0f;

	// 游戏循环
	bool continueLoop = true;
	while (continueLoop) {

		// 每帧开始时，首先绘制地图
		map.Draw(int(mapX), int(mapY));

		// 如果按下箭头上键，就往Left-Up方向移动
		if (keyboard[VK_UP]) {
			// 继续播放脚步声
			MagicMusic("resume channel 1");
			// 移动地图坐标
			mapX += speed * 2.0f;
			mapY += speed * 1.0f;
			// 播放Left-Up动画
			luTexture[int(tick)].Draw(200, 400);
			// 将最后一帧播放的图片，存入指针中
			latestTexture = &luTexture[int(tick)];
		}

		// 如果按下箭头下键，就往Right-Down方向移动
		else if (keyboard[VK_DOWN]) {
			// 继续播放脚步声
			MagicMusic("resume channel 1");
			// 移动地图坐标
			mapX -= speed * 2.0f;
			mapY -= speed * 1.0f;
			// 播放Right-Down动画
			rdTexture[int(tick)].Draw(200, 400);
			// 将最后一帧播放的图片，存入指针中
			latestTexture = &rdTexture[int(tick)];
		}

		// 如果按下箭头左键，就往Left-Down方向移动
		else if (keyboard[VK_LEFT]) {
			// 继续播放脚步声
			MagicMusic("resume channel 1");
			// 移动地图坐标
			mapX += speed * 2.0f;
			mapY -= speed * 1.0f;
			// 播放Left-Down动画
			ldTexture[int(tick)].Draw(200, 400);
			// 将最后一帧播放的图片，存入指针中
			latestTexture = &ldTexture[int(tick)];
		}

		// 如果按下箭头右键，就往Right-Up方向移动
		else if (keyboard[VK_RIGHT]) {
			// 继续播放脚步声
			MagicMusic("resume channel 1");
			// 移动地图坐标
			mapX -= speed * 2.0f;
			mapY += speed * 1.0f;
			// 播放Right-Up动画
			ruTexture[int(tick)].Draw(200, 400);
			// 将最后一帧播放的图片，存入指针中
			latestTexture = &ruTexture[int(tick)];
		}

		// 如果按下箭头上键，就往Left-Up方向移动
		else {
			// 暂停播放脚步声
			MagicMusic("pause channel 1");
			// 利用指针，显示之前最后一帧播放的图片
			// 为什么不显示其他图片？因为不显示最后一帧图片的话，会穿帮
			latestTexture->Draw(200, 400);
		}

		// 限制地图范围，防止超出图片边界
		// 地图的大小：(2000, 1500)；屏幕的大小：(800, 600)
		// X限制：[-1200, 0]，1200 = 2000 - 800，恰好保证永远不超出屏幕
		// Y限制：[ -900, 0]， 900 = 1500 - 600，恰好保证永远不超出屏幕
		mapX = clamp(-(map.width - G_SCREEN_WIDTH), mapX, 0);
		mapY = clamp(-(map.height - G_SCREEN_HEIGHT), mapY, 0);

		// 如果进入宫殿，则退出游戏循环（退出Map页面，进入战斗页面）
		if (mapY > -270.0f && mapY < -150.0f && mapX > -1200.0f && mapX < -760.0f) {
			continueLoop = false;
		}

		// 更新tick计数器，每帧递增，用于播放逐帧动画
		tick += 1.0f;
		if (tick >= 5.0f) {
			tick = 0.0f;
		}

		// 更新帧缓冲，将画面推送到前台（双缓冲机制）
		Show();
	}

	// 关闭音乐文件
	MagicMusic("stop channel 0");
	MagicMusic("stop channel 1");

	MagicMusic("close channel 0");
	MagicMusic("close channel 1");
}

// 页面3：战斗界面
PageSwitch WarPage() {
	fpsLockRate = 60;
	PageSwitch nextPage = PageSwitch_BackToMain;

	// 加载音乐文件
	MagicMusic("open .\\Music\\CursedIntro_Clipped.wav on_channel 0");
	MagicMusic("open .\\Music\\AttackSound.wav on_channel 1");
	MagicMusic("open .\\Music\\MagicSound.wav on_channel 2");

	// 播放战斗背景音
	MagicMusic("play channel 0 times -1");

	// 加载资源
	WarPageImages wpImages;
	LoadWarPageImages(wpImages);

	// 定义角色位置、血条位置、角色逐帧动画、血量
	Character me = {
		100, 225, -35, -20,
		(Picture*)&wpImages.meStanding, 10,
		100
	};

	Character enemyA = {
		550, 100, 0, -5,
		(Picture*)&wpImages.enemyA, 10,
		100
	};

	Character enemyB = {
		600, 225, 20, 0,
		(Picture*)&wpImages.enemyB, 10,
		100
	};

	Character enemyC = {
		550, 350, -20, -10,
		(Picture*)&wpImages.enemyC, 10,
		100
	};

	// Draw Procedure
	PageSwitch state = PageSwitch_Standing;

	// Page Loop
	bool continueLoop = true;
	while (continueLoop) {
		switch (state) {
		case PageSwitch_Standing:
			state = WarPage_Standing(wpImages, me, enemyA, enemyB, enemyC);
			break;
		case PageSwitch_Attack:
			state = WarPage_Attack(wpImages, me, enemyA, enemyB, enemyC);
			break;
		case PageSwitch_Magic:
			state = WarPage_Magic(wpImages, me, enemyA, enemyB, enemyC);
			break;
		case PageSwitch_EnemyRound:
			if (enemyA.blood > 0 && me.blood > 0) {
				WarPage_EnemyAttack(wpImages, me, enemyA, enemyB, enemyC, 'A');
			}
			if (enemyB.blood > 0 && me.blood > 0) {
				WarPage_EnemyAttack(wpImages, me, enemyA, enemyB, enemyC, 'B');
			}
			if (enemyC.blood > 0 && me.blood > 0) {
				WarPage_EnemyAttack(wpImages, me, enemyA, enemyB, enemyC, 'C');
			}
			state = PageSwitch_Standing;
			break;
		case PageSwitch_Winning:
			state = WarPage_Winning(wpImages, me, enemyA, enemyB, enemyC);
			break;
		case PageSwitch_Losing:
			state = WarPage_Losing(wpImages, me, enemyA, enemyB, enemyC);
			break;
		case PageSwitch_BackToMain:
			continueLoop = false;
			nextPage = PageSwitch_BackToMain;
			break;
		case PageSwitch_BackToMap:
			continueLoop = false;
			nextPage = PageSwitch_BackToMap;
			break;
		}
	}

	// 关闭音乐文件
	MagicMusic("stop channel 0");
	MagicMusic("stop channel 1");
	MagicMusic("stop channel 2");

	MagicMusic("close channel 0");
	MagicMusic("close channel 1");
	MagicMusic("close channel 2");

	return nextPage;
}

// 页面3 子页面1：战斗界面 站立状态
PageSwitch WarPage_Standing(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
) {

	// 显示引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"draw text channel 0 content \"点击按钮，以发动技能\" x 11 y 11 textColor 0 0 0 bgTransparent true");
	MagicText(L"draw text channel 1 content \"点击按钮，以发动技能\" x 10 y 10 textColor 255 255 255 bgTransparent true");

	bool continueLoop = true;
	PageSwitch nextState = PageSwitch_Standing;

	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		Animator_DrawCharacter(wpImages, me, 1.0f);
		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		// Attack Button
		if (IsMouseInsidePicture(&wpImages.attackButton, 630, 500)) {
			wpImages.attackButtonPressed.Draw(630, 500);
			if (isLeftClick == true) {
				continueLoop = false;
				nextState = PageSwitch_Attack;
			}
		}
		else {
			wpImages.attackButton.Draw(630, 500);
		}

		// Magic Button
		if (IsMouseInsidePicture(&wpImages.magicButton, 700, 500)) {
			wpImages.magicButtonPressed.Draw(700, 500);
			if (isLeftClick == true) {
				continueLoop = false;
				nextState = PageSwitch_Magic;
			}
		}
		else {
			wpImages.magicButton.Draw(700, 500);
		}

		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f;
		}

		Show();

		// If All Enemies Died, Then WIN!
		if (
			enemyA.blood <= 0 &&
			enemyB.blood <= 0 &&
			enemyC.blood <= 0
			) {
			continueLoop = false;
			nextState = PageSwitch_Winning;
		}

		// If I Died, Then LOSE!
		if (me.blood <= 0) {
			continueLoop = false;
			nextState = PageSwitch_Losing;
		}
	}

	return nextState;
}

// 页面3 子页面2：战斗界面 近身攻击
PageSwitch WarPage_Attack(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
) {

	// 显示引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"draw text channel 0 content \"选择要发动攻击的敌人\" x 11 y 11 textColor 0 0 0 bgTransparent true");
	MagicText(L"draw text channel 1 content \"选择要发动攻击的敌人\" x 10 y 10 textColor 255 255 255 bgTransparent true");

	// Clicking Scene
	char attackWho = Scene_ClickEnemy(wpImages, me, enemyA, enemyB, enemyC);

	// Set Walking Target
	float startX, startY;
	float targetX, targetY;
	startX = me.x, startY = me.y;
	Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC,
		attackWho, &targetX, &targetY
	);

	// Walking Animation
	Scene_Walking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, me.imgArr, false
	);

	// Attack Animation
	MagicMusic("stop channel 1");
	MagicMusic("play channel 1 times 0");
	Scene_HittingEnemy(
		wpImages, me, enemyA, enemyB, enemyC,
		targetX, targetY, attackWho, wpImages.meAttack, false, me.imgArrLen
	);

	// Blood Update
	Scene_BloodUpdate(
		enemyA, enemyB, enemyC, attackWho, 40
	);

	// Set Walk-Back Target
	targetX = me.x, targetY = me.y;
	Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC, attackWho, &startX, &startY
	);

	// Walk-Back Animation
	Scene_Walking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, me.imgArr, false
	);

	return PageSwitch_EnemyRound;
}

// 页面3 子页面3：战斗界面 魔法攻击
PageSwitch WarPage_Magic(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
) {

	// 显示引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"draw text channel 0 content \"选择要发动魔法的敌人\" x 11 y 11 textColor 0 0 0 bgTransparent true");
	MagicText(L"draw text channel 1 content \"选择要发动魔法的敌人\" x 10 y 10 textColor 255 255 255 bgTransparent true");

	// Clicking Scene
	char attackWho = Scene_ClickEnemy(wpImages, me, enemyA, enemyB, enemyC);

	// Set Walking Target
	float startX, startY;
	float targetX, targetY;
	startX = me.x, startY = me.y;
	Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC,
		attackWho, &targetX, &targetY
	);

	// Walking Animation
	MagicMusic("stop channel 2");
	MagicMusic("play channel 2 times 0");
	Scene_Walking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, wpImages.magicAttackB, true
	);

	// Magic Animation
	Scene_HittingEnemy(
		wpImages, me, enemyA, enemyB, enemyC,
		targetX, targetY, attackWho, wpImages.magicAttackB, true, 13
	);

	// Blood Update
	Scene_BloodUpdate(
		enemyA, enemyB, enemyC, attackWho, 60
	);

	return PageSwitch_EnemyRound;
}

// 页面3 子页面4：战斗界面 敌方攻击
PageSwitch WarPage_EnemyAttack(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC, char beingAttackedByWho
) {
	// 显示引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"draw text channel 0 content \"正在遭受敌方攻击！\" x 11 y 11 textColor 0 0 0 bgTransparent true");
	MagicText(L"draw text channel 1 content \"正在遭受敌方攻击！\" x 10 y 10 textColor 255 255 255 bgTransparent true");

	// Set Walking Target
	float meBorder = 25.0f;
	Picture* enemyImgArr = nullptr;

	float startX, startY;
	float targetX = me.x + meBorder, targetY = me.y;

	switch (beingAttackedByWho) {
	case 'A':
		enemyImgArr = enemyA.imgArr;
		startX = enemyA.x, startY = enemyA.y;
		break;
	case 'B':
		enemyImgArr = enemyB.imgArr;
		startX = enemyB.x, startY = enemyB.y;
		break;
	case 'C':
		enemyImgArr = enemyC.imgArr;
		startX = enemyC.x, startY = enemyC.y;
		break;
	}

	// Walking Animation
	Scene_EnemyWalking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, beingAttackedByWho, false
	);

	// Attack Animation
	MagicMusic("stop channel 1");
	MagicMusic("play channel 1 times 0");
	for (int i = 0; i < 60; i++) {
		me.imgArr[i % me.imgArrLen].DrawBrightness(me.x, me.y, 0.2f);
		Show();
	}

	// Blood Update
	// 如果纯粹用攻击消灭所有敌人，愚蠢打法，按ABCABC轮流打，会挨打21次
	// 如果纯粹用攻击消灭所有敌人，聪明打法，先把A打拔下再打其他，会挨打15次
	// 如果纯粹用魔法消灭所有敌人，愚蠢打法，按ABCABC轮流打，会挨打12次
	// 如果纯粹用魔法消灭所有敌人，聪明打法，先把A打拔下再打其他，会挨打9次
	// 如果用聪明Demo打法，A攻B魔C攻，A魔B攻C魔，会挨打12次
	// 如果用愚蠢Demo打法，A攻A魔，B攻B魔，C攻C魔，会挨打9次
	// 于是决定把通关算为挨打10次，每次扣10血，这样有成功和失败的情况
	me.blood -= 10;

	// Set Walk-Back Target
	startX = me.x + meBorder, startY = me.y;
	switch (beingAttackedByWho) {
	case 'A':
		targetX = enemyA.x, targetY = enemyA.y;
		break;
	case 'B':
		targetX = enemyB.x, targetY = enemyB.y;
		break;
	case 'C':
		targetX = enemyC.x, targetY = enemyC.y;
		break;
	}

	// Walk-Back Animation
	Scene_EnemyWalking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, beingAttackedByWho, false
	);

	return PageSwitch_Standing;
}

// 页面3 子页面5：战斗界面 回合胜利
PageSwitch WarPage_Winning(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
) {

	// 删除引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");

	PageSwitch nextState = PageSwitch_BackToMain;

	bool continueLoop = true;
	float winningLogoBrightness = 0.2f;
	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		Animator_DrawCharacter(wpImages, me, 1.0f);
		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		wpImages.winTheGame.DrawBrightness(175, 240, winningLogoBrightness);

		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f;
		}

		winningLogoBrightness += 0.008f;
		if (winningLogoBrightness >= 1.0f) {
			continueLoop = false;
		}

		Show();
	}

	return nextState;
}

// 页面3 子页面6：战斗界面 回合败北
PageSwitch WarPage_Losing(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
) {

	// 删除引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");

	PageSwitch nextState = PageSwitch_BackToMap;

	bool continueLoop = true;
	float losingLogoBrightness = 1.0f;
	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		Animator_DrawCharacter(wpImages, me, 1.0f);
		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		wpImages.loseTheGame.DrawBrightness(175, 240, losingLogoBrightness);

		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f;
		}

		losingLogoBrightness -= 0.008f;
		if (losingLogoBrightness <= 0.2f) {
			continueLoop = false;
		}

		Show();
	}

	return nextState;
}

/**
 * @section
 * Scenes 一些场景函数
 */

char Scene_ClickEnemy(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC
) {

	bool continueLoop = true;
	char attackWho = ' ';

	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		// Me
		Animator_DrawCharacter(wpImages, me, 0.5f);

		// Enemy A
		if (Animator_DrawClickableCharacter(wpImages, enemyA) == true) {
			continueLoop = false;
			attackWho = 'A';
		}

		// Enemy B
		if (Animator_DrawClickableCharacter(wpImages, enemyB) == true) {
			continueLoop = false;
			attackWho = 'B';
		}

		// Enemy C
		if (Animator_DrawClickableCharacter(wpImages, enemyC) == true) {
			continueLoop = false;
			attackWho = 'C';
		}

		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f;
		}

		Show();
	}

	return attackWho;
}

void Scene_Walking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	Picture* thingThatWalks, bool stillDrawMeOrNot
) {

	float nowX = startX, nowY = startY;
	float speedX = 4.0f, speedY = fabs(nowY - targetY) / fabs(nowX - targetX) * speedX;

	bool continueLoop = true;
	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		if (stillDrawMeOrNot == true) {
			Animator_DrawCharacter(wpImages, me, 1.0f);
		}
		else {
			// If not drawing me stading, but drawing me walking,
			// blood bar needs to be moved as well.
			Animator_DrawBlood(wpImages, me, nowX, nowY);
		}

		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		thingThatWalks[0].Draw(int(nowX), int(nowY));

		float posState = (nowX - startX) / (targetX - startX);
		float sinState = sin(posState * PI);
		float ratioState = 0.3f + sinState * 3.0f;

		if (nowX < targetX) nowX += speedX * ratioState;
		if (nowX > targetX) nowX -= speedX * ratioState;
		if (nowY < targetY) nowY += speedY * ratioState;
		if (nowY > targetY) nowY -= speedY * ratioState;

		if (fabs(nowX - targetX) < 3.5f && fabs(nowY - targetY) < 7.5f) {
			continueLoop = false;
		}

		Show();
	}
}

void Scene_EnemyWalking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	char whichEnemy, bool stillDrawEnemyOrNot
) {

	Character* enemyPtr = nullptr;
	switch (whichEnemy) {
	case 'A':
		enemyPtr = &enemyA;
		break;
	case 'B':
		enemyPtr = &enemyB;
		break;
	case 'C':
		enemyPtr = &enemyC;
		break;
	}

	float nowX = startX, nowY = startY;
	float speedX = 4.0f, speedY = fabs(nowY - targetY) / fabs(nowX - targetX) * speedX;

	bool continueLoop = true;
	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		if (stillDrawEnemyOrNot == true) {
			Animator_DrawCharacter(wpImages,*enemyPtr, 1.0f);
		}
		else {
			// If not drawing me stading, but drawing me walking,
			// blood bar needs to be moved as well.
			Animator_DrawBlood(wpImages, *enemyPtr, nowX, nowY);
		}

		Animator_DrawCharacter(wpImages, me, 1.0f);
		if (enemyPtr != &enemyA) Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		if (enemyPtr != &enemyB) Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		if (enemyPtr != &enemyC) Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		enemyPtr->imgArr[0].Draw(int(nowX), int(nowY));

		float posState = (nowX - startX) / (targetX - startX);
		float sinState = sin(posState * PI);
		float ratioState = 0.3f + sinState * 3.0f;

		if (nowX < targetX) nowX += speedX * ratioState;
		if (nowX > targetX) nowX -= speedX * ratioState;
		if (nowY < targetY) nowY += speedY * ratioState;
		if (nowY > targetY) nowY -= speedY * ratioState;

		if (fabs(nowX - targetX) < 3.5f && fabs(nowY - targetY) < 7.5f) {
			continueLoop = false;
		}

		Show();
	}
}

void Scene_HittingEnemy(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float thingX, float thingY, char attackWho,
	Picture* thingThatHurts, bool stillDrawMeOrNot, int animationDuration
) {
	bool continueLoop = true;
	float attackTick = 0;

	while (continueLoop) {
		wpImages.bg.Draw(0, 0);

		if (stillDrawMeOrNot == true) {
			Animator_DrawCharacter(wpImages, me, 1.0f);
		}
		else {
			// If not drawing me stading, but drawing me walking,
			// blood bar needs to be moved as well.
			Animator_DrawBlood(wpImages, me, thingX, thingY);
		}

		switch (attackWho) {
		case 'A':
			Animator_DrawCharacter(wpImages, enemyA, 2.0f);
			Animator_DrawCharacter(wpImages, enemyB, 1.0f);
			Animator_DrawCharacter(wpImages, enemyC, 1.0f);
			break;
		case 'B':
			Animator_DrawCharacter(wpImages, enemyA, 1.0f);
			Animator_DrawCharacter(wpImages, enemyB, 2.0f);
			Animator_DrawCharacter(wpImages, enemyC, 1.0f);
			break;
		case 'C':
			Animator_DrawCharacter(wpImages, enemyA, 1.0f);
			Animator_DrawCharacter(wpImages, enemyB, 1.0f);
			Animator_DrawCharacter(wpImages, enemyC, 2.0f);
			break;
		}

		// 图层绘制顺序，ThingThatHurts要在Enemy上方
		thingThatHurts[int(attackTick)].Draw(int(thingX), int(thingY));

		attackTick += 0.15f;
		if (attackTick > (float)animationDuration) {
			continueLoop = false;
		}

		Show();
	}
}

void Scene_BloodUpdate(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, int bloodDrop
) {
	int* ptr;
	switch (attackWho) {
	case 'A':
		enemyA.blood -= bloodDrop;
		break;
	case 'B':
		enemyB.blood -= bloodDrop;
		break;
	case 'C':
		enemyC.blood -= bloodDrop;
		break;
	}
}

/**
 * @section
 * Animator 一些动画函数
 */

void Animator_DrawBlood(
	WarPageImages& wpImages, Character& character,
	int customX = -1, int customY = -1
) {
	int bloodX = customX == -1 ? character.x : customX;
	int bloodY = customY == -1 ? character.y : customY;

	// Draw Blood Bar (Black BottomLayer)
	wpImages.bloodBar_BottomLayer.Draw(
		bloodX + character.bloodBarOffsetX, bloodY + character.bloodBarOffsetY
	);

	// Draw Blood Bar (Red UpperLayer)
	wpImages.bloodBar.DrawClip(
		bloodX + character.bloodBarOffsetX, bloodY + character.bloodBarOffsetY,
		0.0f, (float)character.blood, 0.0f, 100.0f
	);
}

void Animator_DrawCharacter(
	WarPageImages& wpImages, Character& character,
	float brightness
) {
	if (character.blood > 0) {
		Animator_DrawBlood(wpImages, character);
		character.imgArr[int(tick)].DrawBrightness(
			character.x, character.y, brightness
		);
	}
}

bool Animator_DrawClickableCharacter(
	WarPageImages& wpImages, Character& character
) {
	bool isClickingMe = false;

	if (character.blood > 0) {
		Animator_DrawBlood(wpImages, character);
		if (
			IsMouseInsidePicture(
				&character.imgArr[int(tick)], character.x, character.y
			)
		) {
			character.imgArr[int(tick)].DrawBrightness(
				character.x, character.y, 1.0f
			);
			if (isLeftClick == true) {
				isClickingMe = true;
			}
		}
		else {
			character.imgArr[int(tick)].DrawBrightness(
				character.x, character.y, 0.5f
			);
		}
	}

	return isClickingMe;
}

void Animator_LocateWalkToPos(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, float* resultX, float* resultY
) {
	float border = 25.0f;

	switch (attackWho) {
	case 'A':
		*resultX = enemyA.x - border, *resultY = enemyA.y;
		break;
	case 'B':
		*resultX = enemyB.x - border, *resultY = enemyB.y;
		break;
	case 'C':
		*resultX = enemyC.x - border, *resultY = enemyC.y;
		break;
	}
}
