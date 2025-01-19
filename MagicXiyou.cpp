#define _CRT_SECURE_NO_WARNINGS // 禁用某些编译器警告，通常与安全函数相关

#include <stdio.h>      // 引入标准输入输出库，支持printf等函数
#include <math.h>       // 引入数学库，提供数学计算函数如sin、cos、pow等
#include <time.h>       // 引入时间库，支持时间处理函数，如time()、clock()等
#include "Magic.h"      // 引入自定义头文件 Magic.h，包含图形、声音等相关内容

#define PI 3.1415926f   // 定义常量PI，用于数学计算

/**
 * @section
 * 全局变量
 */

// tick计数器，每帧递增，用于播放逐帧动画（指定要播放哪一帧）
float tick = 0.0f;  // 初始化tick为0.0，用于动画控制

/**
 * @section
 * 枚举类型
 */

enum PageSwitch {
    PageSwitch_Standing,    // 页面切换：战斗站立状态
    PageSwitch_Attack,      // 页面切换：攻击状态
    PageSwitch_Magic,       // 页面切换：魔法状态
    PageSwitch_EnemyRound,  // 页面切换：敌方回合
    PageSwitch_Winning,     // 页面切换：胜利页面
    PageSwitch_Losing,      // 页面切换：失败页面
    PageSwitch_BackToMain,  // 页面切换：返回主页面
    PageSwitch_BackToMap    // 页面切换：返回地图页面
};

typedef int Blood;          // 定义Blood类型为int，用于表示血量

/**
 * @section
 * 结构体
 */

// 定义Character结构体，表示一个角色（玩家或敌人）
struct Character {
    int x;                  // 角色的X坐标
    int y;                  // 角色的Y坐标
    int bloodBarOffsetX;    // 血条X轴偏移量，用于调整血条位置
    int bloodBarOffsetY;    // 血条Y轴偏移量，用于调整血条位置

    Picture* imgArr;        // 角色的图像数组，保存不同状态下的角色图像
    int imgArrLen;          // 角色图像数组的长度（即图像数量）
    Blood blood;            // 角色的血量
};

// 定义WarPageImages结构体，表示战斗页面上的所有图像资源
struct WarPageImages {

    // Static Images（静态图像）
    Picture bg;                     // 背景图像
    Picture bloodBar;               // 血条（上层）
    Picture bloodBar_BottomLayer;   // 血条（下层）
    Picture winTheGame;             // 胜利时显示的图像
    Picture loseTheGame;            // 失败时显示的图像

    Picture attackButton;           // 攻击按钮图像
    Picture attackButtonPressed;    // 按下攻击按钮时的图像
    Picture magicButton;            // 魔法按钮图像
    Picture magicButtonPressed;     // 按下魔法按钮时的图像

    // Me-Related（与玩家相关的图像）
    Picture meStanding[10];         // 玩家站立状态下的图像数组（最多10帧）
    Picture meAttack[10];           // 玩家攻击状态下的图像数组（最多10帧）

    // Enemy-Related（与敌人相关的图像）
    Picture enemyA[10];             // 敌人A的图像数组（最多10帧）
    Picture enemyB[10];             // 敌人B的图像数组（最多10帧）
    Picture enemyC[10];             // 敌人C的图像数组（最多10帧）

    // Magic-Related（与魔法相关的图像）
    Picture magicAttackA[13];       // 魔法A的图像数组（最多13帧）
    Picture magicAttackB[13];       // 魔法B的图像数组（最多13帧）
};

/**
 * @section
 * 函数定义
 */

// 实用帮助函数

// 判断鼠标是否在指定区域内（矩形范围），x0、y0为左上角坐标，x1、y1为右下角坐标
bool IsMouseInRange(
    int x0, int y0, int x1, int y1
);

// 判断鼠标是否在指定图像区域内，picture为图像对象，x_、y_为图像位置
bool IsMouseInsidePicture(
    const Picture* picture, int x_, int y_
);

// 批量加载图像，picArr为图像数组，howMany为图像数量，filenameFormat为图像文件
// 格式
void BatchLoadImage(
    Picture* picArr, int howMany, const char* filenameFormat
);

// 加载所有战斗页面的图像资源，wpImages为WarPageImages结构体实例，包含了所有
// 需要的图像
void LoadWarPageImages(
    WarPageImages& wpImages
);

// 页面函数
// 启动页面，初始化游戏界面
bool StarterPage();
// 地图页面，展示游戏中的地图
void MapPage();
// 战斗页面，返回页面切换状态（通过PageSwitch类型枚举值确定）
PageSwitch WarPage();

// 战斗页面 子页面函数
// 这些函数定义了战斗页面不同阶段的行为，返回的是一个`PageSwitch`类型，表示当前
// 页面的切换状态。

// 战斗页面-站立状态，玩家与敌人对峙的初始状态
PageSwitch WarPage_Standing(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character me,            // 玩家角色
    Character enemyA,        // 敌人A
    Character enemyB,        // 敌人B
    Character enemyC         // 敌人C
);

// 战斗页面-攻击状态，玩家进行攻击时的状态
PageSwitch WarPage_Attack(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC        // 敌人C（引用传递，允许修改）
);

// 战斗页面-魔法状态，玩家使用魔法时的状态
PageSwitch WarPage_Magic(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC        // 敌人C（引用传递，允许修改）
);

// 战斗页面-敌人攻击状态，敌人攻击玩家或其他敌人时的状态
PageSwitch WarPage_EnemyAttack(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    char beingAttackedByWho  // 我被谁攻击了（'A', 'B' 或 'C'）
);

// 战斗页面-胜利状态，玩家获胜后展示的页面
PageSwitch WarPage_Winning(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character me,            // 玩家角色
    Character enemyA,        // 敌人A
    Character enemyB,        // 敌人B
    Character enemyC         // 敌人C
);

// 战斗页面-失败状态，玩家失败后展示的页面
PageSwitch WarPage_Losing(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character me,            // 玩家角色
    Character enemyA,        // 敌人A
    Character enemyB,        // 敌人B
    Character enemyC         // 敌人C
);

// Animator 一些动画函数
// 以下函数用于动画控制，包括点击敌人、角色走动、攻击敌人以及血条更新等动画操作

// 玩家选择点击敌人进行攻击，返回被攻击的敌人（'A', 'B', 或 'C'）
char Scene_ClickEnemy(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC        // 敌人C（引用传递，允许修改）
);

// 场景中角色行走的动画，玩家或敌人从某个点移动到目标点
void Scene_Walking(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    float startX,            // 起始X坐标
    float startY,            // 起始Y坐标
    float targetX,           // 目标X坐标
    float targetY,           // 目标Y坐标
    Picture* thingThatWalks, // 走动的物体图像数组
    bool stillDrawMeOrNot    // 是否继续绘制玩家角色，控制是否绘制角色或血条
);

// 敌人从一个点走到目标点的动画，敌人A、B、C的行走动画
void Scene_EnemyWalking(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    float startX,            // 起始X坐标
    float startY,            // 起始Y坐标
    float targetX,           // 目标X坐标
    float targetY,           // 目标Y坐标
    char whichEnemy,         // 哪个敌人在行动，'A'、'B'或'C'
    bool stillDrawEnemyOrNot // 是否继续绘制敌人角色，控制是否绘制角色或血条
);

// 角色攻击敌人时的动画，显示攻击动作及敌人受伤
void Scene_HittingEnemy(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& me,           // 玩家角色（引用传递，允许修改）
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    float thingX,            // 伤害特效的X坐标
    float thingY,            // 伤害特效的Y坐标
    char attackWho,          // 被攻击的敌人（'A', 'B' 或 'C'）
    Picture* thingThatHurts, // 伤害特效图像数组
    bool stillDrawMeOrNot,   // 是否继续绘制玩家角色，控制是否绘制角色或血条
    int animationDuration    // 动画持续时间
);

// 更新敌人血量，造成伤害后血条减少
void Scene_BloodUpdate(
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    char attackWho,          // 攻击的目标（'A', 'B' 或 'C'）
    int bloodDrop            // 造成的伤害值
);

// Animator 一些动画函数
// 以下是角色动画的相关函数，包括绘制角色、绘制血条等

// 绘制角色的血条，显示血量情况
void Animator_DrawBlood(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& character,    // 角色对象
    int customX,             // 自定义X坐标
    int customY              // 自定义Y坐标
);

// 绘制角色，控制角色图像的亮度，通常用于绘制当前状态的角色图像
void Animator_DrawCharacter(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& character,    // 角色对象
    float brightness         // 亮度值，影响图像的显示效果
);

// 绘制可以被点击的角色，若鼠标点击到该角色，则返回`true`
bool Animator_DrawClickableCharacter(
    WarPageImages& wpImages, // 包含所有战斗页面图像的结构体
    Character& character     // 角色对象
);

// 根据攻击目标的位置，计算角色的目标位置（用于绘制角色走向攻击目标的动画）
void Animator_LocateWalkToPos(
    Character& enemyA,       // 敌人A（引用传递，允许修改）
    Character& enemyB,       // 敌人B（引用传递，允许修改）
    Character& enemyC,       // 敌人C（引用传递，允许修改）
    char attackWho,          // 攻击的目标（'A', 'B' 或 'C'）
    float* resultX,          // 计算出来的目标X坐标（通过引用传递）
    float* resultY           // 计算出来的目标Y坐标（通过引用传递）
);

/**
 * @section
 * 主函数
 */

int main() {
    // 调用 Magic 函数，初始化 Magic 图形框架
    Magic();
    
    // 设置初始页面为“返回主界面”
    PageSwitch ps = PageSwitch_BackToMain;

    // 启动游戏的主循环
    while (true) {
        // 如果当前页面为“返回主界面”
        if (ps == PageSwitch_BackToMain) {
            // 显示启动页面并获取用户是否愿意登录
            bool wannaLogin = StarterPage();
            // 如果用户不愿意登录，则退出游戏
            if (wannaLogin == false) {
                Quit(); // 调用退出函数
                return 0; // 返回 0 表示程序结束
            }

            // 显示地图页面
            MapPage();
            // 调用战斗页面函数，进入战斗并获取页面状态
            ps = WarPage();
        }
        // 如果当前页面为“返回到地图”，显示地图页面并切换到战斗页面
        else if (ps == PageSwitch_BackToMap) {
            MapPage(); // 显示地图页面
            ps = WarPage(); // 切换到战斗页面
        }
    }

    Quit(); // 游戏结束后调用退出函数
    return 0; // 程序结束
}

/**
 * @section
 * 实用帮助函数
 */

// 检测鼠标是否在 (x0, y0) => (x1, y1) 区域内
bool IsMouseInRange(
    int x0, int y0, int x1, int y1
) {
    // 判断鼠标的X坐标是否在指定范围内
    if (mouseX >= x0) {
        if (mouseX <= x1) {
            // 判断鼠标的Y坐标是否在指定范围内
            if (mouseY >= y0) {
                if (mouseY <= y1) {
                    return true; // 鼠标在范围内，返回true
                }
            }
        }
    }
    return false; // 鼠标不在范围内，返回false
}

// 检测鼠标是否在图片区域内
bool IsMouseInsidePicture(
    const Picture* picture, int x_, int y_
) {
    // 判断鼠标是否在图片的矩形区域内
    return IsMouseInRange(
		x_, y_, picture->width + x_, picture->height + y_
	);
}

// 批量按特定文件名格式，读取图片，实现动画效果
// 参数 *picArr：传入图片数组，用于存储图片
// 参数 howMany：有几张图片要加载。例如：输入15，则加载数从0到14
// 参数 filenameFormat：图片名的字符串。例如：".\\BMP\\Image%d.bmp"；
// 串中%d会被自动替换
void BatchLoadImage(
    Picture* picArr, int howMany, const char* filenameFormat
) {
    // 用于字符串%d替换的缓冲区
    char buffer[100] = { 0 };

    // 批量按格式，从文件中读取图片
    for (int i = 0; i < howMany; i++) {
        // 使用sprintf格式化文件名
        sprintf(buffer, filenameFormat, i);
        // 加载图片
        picArr[i].Load(buffer);
    }
}

// 加载战斗页面的图像资源
void LoadWarPageImages(
    WarPageImages& wpImages
) {
    // 静态图像资源
	// 加载背景图
    wpImages.bg.Load(
		".\\BMP\\Images\\WarPageBackground.bmp"
	);
	// 加载血条图
    wpImages.bloodBar.Load(
		".\\BMP\\Images\\BloodBar.bmp"
	);
	// 加载血条底层图
    wpImages.bloodBar_BottomLayer.Load(
		".\\BMP\\Images\\BloodBar_BottomLayer.bmp"
	);
	// 加载胜利图
    wpImages.winTheGame.Load(
		".\\BMP\\Images\\WinTheGame.bmp"
	);
	// 加载失败图
    wpImages.loseTheGame.Load(
		".\\BMP\\Images\\LoseTheGame.bmp"
	);

    // 按钮图像资源
	// 加载攻击按钮图
    wpImages.attackButton.Load(
		".\\BMP\\Buttons\\Attack.bmp"
	);
	// 加载攻击按钮按下状态图
    wpImages.attackButtonPressed.Load(
		".\\BMP\\Buttons\\Attack_Pressed.bmp"
	);
	// 加载魔法按钮图
    wpImages.magicButton.Load(
		".\\BMP\\Buttons\\Magic.bmp"
	);
	// 加载魔法按钮按下状态图
    wpImages.magicButtonPressed.Load(
		".\\BMP\\Buttons\\Magic_Pressed.bmp"
	);

    // 玩家相关图像（动画）
	// 批量加载玩家站立动画
    BatchLoadImage(
        wpImages.meStanding, 10,
        ".\\BMP\\Animation\\Me_StandingAnimation\\%02d.bmp"
    );
	// 批量加载玩家攻击动画
    BatchLoadImage(
        wpImages.meAttack, 10,
        ".\\BMP\\Animation\\Me_AttackAnimation\\%02d.bmp"
    );

    // 敌人相关图像（动画）
	// 批量加载敌人A的动画
    BatchLoadImage(
        wpImages.enemyA, 10,
        ".\\BMP\\Animation\\EnemyA\\%02d.bmp"
    );
	// 批量加载敌人B的动画
    BatchLoadImage(
        wpImages.enemyB, 10,
        ".\\BMP\\Animation\\EnemyB\\%02d.bmp"
    );
	// 批量加载敌人C的动画
    BatchLoadImage(
        wpImages.enemyC, 10,
        ".\\BMP\\Animation\\EnemyC\\%02d.bmp"
    );

    // 魔法相关图像（动画）
	// 批量加载魔法攻击A的动画
    BatchLoadImage(
        wpImages.magicAttackA, 13,
        ".\\BMP\\Animation\\MagicAttackA\\%02d.bmp"
    );
	// 批量加载魔法攻击B的动画
    BatchLoadImage(
        wpImages.magicAttackB, 13,
        ".\\BMP\\Animation\\MagicAttackB\\%02d.bmp"
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

	// 加载图片资源
	// loginButton为登录游戏，exitButton为退出游戏，Pressed后缀是黑白化版本
	Picture loginButton, loginButtonPressed;
	Picture exitButton, exitButtonPressed;
	loginButton.Load(
		".\\BMP\\Buttons\\LoginButton.bmp"
	);
	loginButtonPressed.Load(
		".\\BMP\\Buttons\\LoginButton_Pressed.bmp"
	);
	exitButton.Load(
		".\\BMP\\Buttons\\ExitButton.bmp"
	);
	exitButtonPressed.Load(
		".\\BMP\\Buttons\\ExitButton_Pressed.bmp"
	);

	// 设定图片资源XY坐标
	int starterX = 0, starterY = 0;
	int logoX = 165, logoY = 25;
	int loginButtonX = 250, loginButtonY = 315;
	int exitButtonX = 250, exitButtonY = 440;

	// Logo图像的缩放动画：控制参数
	// @ speed缩放速度，minSize最小尺寸，portion映射系数
	// @ 正弦函数sin从[-1, +1]变化，
	// @ portion负责把[-1, +1]映射到[0.75f, 1.0f]
	float speed = 4.0f;
	float minSize = 0.75f;
	float portion = (1 - minSize) * 0.5f;

	// 游戏循环
	bool continueLoop = true;
	while (continueLoop) {

		// Logo图像的缩放动画：每帧参数
		// @ timeState时间状态，sinState正弦状态，ratioState缩放比例
		// timeState获取进程运行至今的毫秒数；
		//   乘以0.001f * speed减慢速度，防止变化过快
		// sinState用来获取正弦sin函数的值，用于缩放；
		//   乘以portion来进行映射
		// ratioState是最终计算结果（缩放比例），代表图片要缩放为原来
		//   的多少倍，例如0.85倍
		float timeState = clock() * 0.001f * speed;
		float sinState = sin(timeState) * portion;
		float ratioState = minSize + portion + sinState;

		// 绘制背景图和Logo图（带缩放效果的Logo图）
		starter.Draw(starterX, starterY);
		logo.DrawZoomCentered(logoX, logoY, ratioState);

		// 检测鼠标是否Hover在“登录游戏”按钮上方
		if (
			IsMouseInsidePicture(
				&loginButton, loginButtonX, loginButtonY
			)
		) {
			// 如果在，按钮灰色
			loginButtonPressed.Draw(loginButtonX, loginButtonY);
			// 如果不仅Hover，还按下了左键
			if (isLeftClick == true) {
				// 那么依然绘制彩色按钮
				loginButton.Draw(loginButtonX, loginButtonY);
				// 执行完这次游戏循环，就退出循环
				// 为何不直接return？因为还没Show()
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
		if (
			IsMouseInsidePicture(
				&exitButton, exitButtonX, exitButtonY
			)
		) {
			// 如果在，按钮灰色
			exitButtonPressed.Draw(exitButtonX, exitButtonY);
			// 如果不仅Hover，还按下了左键
			if (isLeftClick == true) {
				// 那么依然绘制彩色按钮
				exitButton.Draw(exitButtonX, exitButtonY);
				// 执行完这次游戏循环，就退出循环
				// 为何不直接return？因为还没Show()
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
	MagicText(
		L"draw text channel 0 content \"使用箭头方向键，控制人物移动\" "
		L"x 11 y 11 textColor 0 0 0 bgTransparent true"
	);
	MagicText(
		L"draw text channel 1 content \"使用箭头方向键，控制人物移动\" "
		L"x 10 y 10 textColor 255 255 255 bgTransparent true"
	);

	// 设定锁帧15FPS
	// 如果帧率太快，地图移动很平滑流畅，但人物动画帧率低，反而有穿帮感
	fpsLockRate = 15;

	// 加载地图素材
	Picture map;
	map.Load(".\\BMP\\Images\\Map.bmp");

	// 加载人物逐帧动画素材（上下左右移动各5帧，共20帧）
	// ld lu rd ru是缩写，l代表Left，r代表Right，u代表Up，d代表Down
	Picture ldTexture[5], luTexture[5], rdTexture[5], ruTexture[5];
	BatchLoadImage(
		ldTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\ld%d.bmp"
	);
	BatchLoadImage(
		luTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\lu%d.bmp"
	);
	BatchLoadImage(
		rdTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\rd%d.bmp"
	);
	BatchLoadImage(
		ruTexture, 5, ".\\BMP\\Animation\\\Me_InsideMap\\ru%d.bmp"
	);

	// 图片指针，用于指向最后一次播放的图片；用来实现动画结束后，停在最后一帧
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
		if (
			mapY > -270.0f && mapY < -150.0f &&
			mapX > -1200.0f && mapX < -760.0f
		) {
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
    // 设置每秒帧数为60，控制游戏的刷新率
    fpsLockRate = 60;
    
    // 初始化战斗页面的下一个页面为“返回主界面”
    PageSwitch nextPage = PageSwitch_BackToMain;

    // 加载音频文件，使用 MagicMusic 函数打开音频文件
	// 加载战斗背景音乐
    MagicMusic("open .\\Music\\CursedIntro_Clipped.wav on_channel 0");
	// 加载攻击音效
    MagicMusic("open .\\Music\\AttackSound.wav on_channel 1");
	// 加载魔法音效
    MagicMusic("open .\\Music\\MagicSound.wav on_channel 2");

    // 播放背景音乐，循环播放
    MagicMusic("play channel 0 times -1");

    // 加载战斗页面所需的图像资源
    WarPageImages wpImages;
	// 调用 LoadWarPageImages 函数加载图像资源
    LoadWarPageImages(wpImages);

    // 定义角色对象及其初始位置、血条偏移、动画帧数组和初始血量
    Character me = {
        100, 225, -35, -20,  // 玩家初始位置和血条偏移
        (Picture*)&wpImages.meStanding,  // 动画的图片数组
		10,  // 表示图片数组有10帧
        100  // 玩家初始血量
    };

    // 定义敌人A的初始位置、血条偏移、动画帧数组和初始血量
    Character enemyA = {
        550, 100, 0, -5,  // 敌人A的初始位置和血条偏移
        (Picture*)&wpImages.enemyA, 10,  // 敌人A的动画帧数组
        100  // 敌人A的初始血量
    };

    // 定义敌人B的初始位置、血条偏移、动画帧数组和初始血量
    Character enemyB = {
        600, 225, 20, 0,  // 敌人B的初始位置和血条偏移
        (Picture*)&wpImages.enemyB, 10,  // 敌人B的动画帧数组
        100  // 敌人B的初始血量
    };

    // 定义敌人C的初始位置、血条偏移、动画帧数组和初始血量
    Character enemyC = {
        550, 350, -20, -10,  // 敌人C的初始位置和血条偏移
        (Picture*)&wpImages.enemyC, 10,  // 敌人C的动画帧数组
        100  // 敌人C的初始血量
    };

    // 定义当前页面状态，初始状态为“站立”
    PageSwitch state = PageSwitch_Standing;

    // 进入页面的主循环，控制页面的切换和动画
    bool continueLoop = true;
    while (continueLoop) {
        switch (state) {
        // 如果当前状态为“站立”
        case PageSwitch_Standing:
            // 切换到“战斗站立”页面，并更新当前状态
            state = WarPage_Standing(
				wpImages, me, enemyA, enemyB, enemyC
			);
            break;
        // 如果当前状态为“攻击”
        case PageSwitch_Attack:
            // 切换到“攻击”页面，并更新当前状态
            state = WarPage_Attack(
				wpImages, me, enemyA, enemyB, enemyC
			);
            break;
        // 如果当前状态为“魔法”
        case PageSwitch_Magic:
            // 切换到“魔法”页面，并更新当前状态
            state = WarPage_Magic(
				wpImages, me, enemyA, enemyB, enemyC
			);
            break;
        // 如果当前状态为“敌方回合”
        case PageSwitch_EnemyRound:
            // 检查敌人和玩家的血量，如果敌人和玩家都存活，执行敌人攻击
            if (enemyA.blood > 0 && me.blood > 0) {
				// 敌人A攻击
                WarPage_EnemyAttack(
					wpImages, me, enemyA, enemyB, enemyC, 'A'
				);
            }
            if (enemyB.blood > 0 && me.blood > 0) {
				// 敌人B攻击
                WarPage_EnemyAttack(
					wpImages, me, enemyA, enemyB, enemyC, 'B'
				);
            }
            if (enemyC.blood > 0 && me.blood > 0) {
				// 敌人C攻击
                WarPage_EnemyAttack(
					wpImages, me, enemyA, enemyB, enemyC, 'C'
				);
            }
            // 回到“站立”页面
            state = PageSwitch_Standing;
            break;
        // 如果当前状态为“胜利”
        case PageSwitch_Winning:
            // 切换到“胜利”页面，并更新当前状态
            state = WarPage_Winning(
				wpImages, me, enemyA, enemyB, enemyC
			);
            break;
        // 如果当前状态为“失败”
        case PageSwitch_Losing:
            // 切换到“失败”页面，并更新当前状态
            state = WarPage_Losing(
				wpImages, me, enemyA, enemyB, enemyC
			);
            break;
        // 如果当前状态为“返回主界面”
        case PageSwitch_BackToMain:
			// 退出循环，返回主界面
            continueLoop = false;
			// 设置下一个页面为主界面
            nextPage = PageSwitch_BackToMain;
            break;
        // 如果当前状态为“返回地图”
        case PageSwitch_BackToMap:
			// 退出循环，返回地图
            continueLoop = false;
			// 设置下一个页面为地图页面
            nextPage = PageSwitch_BackToMap;
            break;
        }
    }

    // 关闭和停止音频
    MagicMusic("stop channel 0");  // 停止背景音乐
    MagicMusic("stop channel 1");  // 停止攻击音效
    MagicMusic("stop channel 2");  // 停止魔法音效

    // 关闭音频通道
    MagicMusic("close channel 0");
    MagicMusic("close channel 1");
    MagicMusic("close channel 2");

    // 返回下一个页面（主界面或地图）
    return nextPage;
}

// 页面3 子页面1：战斗界面 站立状态
PageSwitch WarPage_Standing(
    WarPageImages& wpImages, Character me, Character enemyA,
    Character enemyB, Character enemyC
) {
    // 删除文本通道0和通道1的文字
    MagicText(L"delete text channel 0");
    MagicText(L"delete text channel 1");
	// 设置文本通道0和通道1的字体
    MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
    MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
    // 显示引导文字
    MagicText(
		L"draw text channel 0 content \"点击按钮，以发动技能\" "
		L"x 11 y 11 textColor 0 0 0 bgTransparent true"
	);
    MagicText(
		L"draw text channel 1 content \"点击按钮，以发动技能\" "
		L"x 10 y 10 textColor 255 255 255 bgTransparent true"
	);

    bool continueLoop = true;
    PageSwitch nextState = PageSwitch_Standing;

    while (continueLoop) {
        wpImages.bg.Draw(0, 0); // 绘制背景

        // 绘制所有角色
        Animator_DrawCharacter(wpImages, me, 1.0f);
        Animator_DrawCharacter(wpImages, enemyA, 1.0f);
        Animator_DrawCharacter(wpImages, enemyB, 1.0f);
        Animator_DrawCharacter(wpImages, enemyC, 1.0f);

        // 攻击按钮
        if (
			IsMouseInsidePicture(
				&wpImages.attackButton, 630, 500
			)
		) {
			// 绘制按钮按下状态
			wpImages.attackButtonPressed.Draw(630, 500);
			// 如果按下按钮
            if (isLeftClick == true) {
                continueLoop = false;
				// 转到攻击页面
                nextState = PageSwitch_Attack;
            }
        } else {
			// 绘制攻击按钮
            wpImages.attackButton.Draw(630, 500);
        }

        // 魔法按钮
        if (
			IsMouseInsidePicture(
				&wpImages.magicButton, 700, 500
			)
		) {
			// 绘制按钮按下状态
            wpImages.magicButtonPressed.Draw(700, 500);
			// 如果按下按钮
            if (isLeftClick == true) {
                continueLoop = false;
				// 转到魔法页面
                nextState = PageSwitch_Magic;
            }
        } else {
			// 绘制魔法按钮
            wpImages.magicButton.Draw(700, 500);
        }

        tick += 0.15f; // 增加tick，控制动画
        if (tick >= 10.0f) {
            tick = 0.0f; // 重置tick
        }

        Show(); // 显示屏幕内容

        // 如果敌人A、B、C都死亡，则胜利
        if (
			enemyA.blood <= 0 &&
			enemyB.blood <= 0 &&
			enemyC.blood <= 0
		) {
            continueLoop = false;
			// 转到胜利页面
            nextState = PageSwitch_Winning;
        }

        // 如果自己死亡，则失败
        if (me.blood <= 0) {
            continueLoop = false;
			// 转到失败页面
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
    // 删除文本通道0和通道1的文字
    MagicText(L"delete text channel 0");
    MagicText(L"delete text channel 1");
	// 设置文本通道0和通道1的字体
    MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
    MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	// 显示引导文字
    MagicText(
		L"draw text channel 0 content \"选择要发动攻击的敌人\" "
		L"x 11 y 11 textColor 0 0 0 bgTransparent true"
	);
    MagicText(
		L"draw text channel 1 content \"选择要发动攻击的敌人\" "
		L"x 10 y 10 textColor 255 255 255 bgTransparent true"
	);

    // 点击选择敌人
    char attackWho = Scene_ClickEnemy(
		wpImages, me, enemyA, enemyB, enemyC
	);

    // 设置走路目标
    float startX, startY;
    float targetX, targetY;
    startX = me.x, startY = me.y;
    Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC,
		attackWho, &targetX, &targetY
	);

    // 走路动画
    Scene_Walking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, me.imgArr, false
	);

    // 攻击动画
    MagicMusic("stop channel 1");
    MagicMusic("play channel 1 times 0");
    Scene_HittingEnemy(
		wpImages, me, enemyA, enemyB, enemyC,
		targetX, targetY, attackWho, wpImages.meAttack, false,
		me.imgArrLen
	);

    // 血量更新
    Scene_BloodUpdate(
		enemyA, enemyB, enemyC, attackWho, 40
	);

    // 设置返回目标
    targetX = me.x, targetY = me.y;
    Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC, attackWho, &startX, &startY
	);

    // 返回走路动画
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
    // 删除文本通道0和通道1的文字
    MagicText(L"delete text channel 0");
    MagicText(L"delete text channel 1");
	// 设置文本通道0和通道1的字体
    MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
    MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	// 显示引导文字
    MagicText(
		L"draw text channel 0 content \"选择要发动魔法的敌人\" "
		L"x 11 y 11 textColor 0 0 0 bgTransparent true"
	);
    MagicText(
		L"draw text channel 1 content \"选择要发动魔法的敌人\" "
		L"x 10 y 10 textColor 255 255 255 bgTransparent true"
	);

    // 点击选择敌人
    char attackWho = Scene_ClickEnemy(
		wpImages, me, enemyA, enemyB, enemyC
	);

    // 设置走路目标
    float startX, startY;
    float targetX, targetY;
    startX = me.x, startY = me.y;
    Animator_LocateWalkToPos(
		enemyA, enemyB, enemyC,
		attackWho, &targetX, &targetY
	);

    // 走路动画
    MagicMusic("stop channel 2");
    MagicMusic("play channel 2 times 0");
    Scene_Walking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, wpImages.magicAttackB, true
	);

    // 魔法攻击动画
    Scene_HittingEnemy(
		wpImages, me, enemyA, enemyB, enemyC,
		targetX, targetY, attackWho, wpImages.magicAttackB, true, 13
	);

    // 血量更新
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
	// 删除文本通道0和通道1的文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");
	// 设置文本通道0和通道1的字体
	MagicText(L"set font channel 0 fontSize 20 fontFamily \"黑体\"");
	MagicText(L"set font channel 1 fontSize 20 fontFamily \"黑体\"");
	// 显示引导文字
	MagicText(
		L"draw text channel 0 content \"正在遭受敌方攻击！\" "
		L"x 11 y 11 textColor 0 0 0 bgTransparent true"
	);
	MagicText(
		L"draw text channel 1 content \"正在遭受敌方攻击！\" "
		L"x 10 y 10 textColor 255 255 255 bgTransparent true"
	);

	// 设置敌人攻击的目标位置
	float meBorder = 25.0f; // 定义角色边界的偏移量
	Picture* enemyImgArr = nullptr; // 用来存储敌人图像数组的指针

	// 定义起始位置和目标位置
	float startX, startY;
	// 目标位置设置在玩家角色的位置右边偏移25
	float targetX = me.x + meBorder, targetY = me.y;

	// 根据被攻击的敌人来选择相应的敌人图像数组和起始位置
	switch (beingAttackedByWho) {
	case 'A':
		enemyImgArr = enemyA.imgArr; // 选择敌人A的图像数组
		startX = enemyA.x, startY = enemyA.y; // 设置敌人A的起始位置
		break;
	case 'B':
		enemyImgArr = enemyB.imgArr; // 选择敌人B的图像数组
		startX = enemyB.x, startY = enemyB.y; // 设置敌人B的起始位置
		break;
	case 'C':
		enemyImgArr = enemyC.imgArr; // 选择敌人C的图像数组
		startX = enemyC.x, startY = enemyC.y; // 设置敌人C的起始位置
		break;
	}

	// 敌人行走动画，模拟敌人从起始位置走到目标位置
	Scene_EnemyWalking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, beingAttackedByWho, false
	);

	// 停止音乐通道1，并重新用通道1播放攻击音效
	MagicMusic("stop channel 1");
	MagicMusic("play channel 1 times 0");

	// 攻击动画
	for (int i = 0; i < 60; i++) {
		// 绘制角色的攻击动画，使用0.2的亮度
		me.imgArr[i % me.imgArrLen].DrawBrightness(me.x, me.y, 0.2f);
		// 刷新屏幕，显示更新后的内容
		Show();
	}

	// 血量更新
	// 如果纯粹用攻击消灭所有敌人，愚蠢打法，按ABCABC轮流打，会挨打21次
	// 如果纯粹用攻击消灭所有敌人，聪明打法，先把A打拔下再打其他，会挨打15次
	// 如果纯粹用魔法消灭所有敌人，愚蠢打法，按ABCABC轮流打，会挨打12次
	// 如果纯粹用魔法消灭所有敌人，聪明打法，先把A打拔下再打其他，会挨打9次
	// 如果用聪明Demo打法，A攻B魔C攻，A魔B攻C魔，会挨打12次
	// 如果用愚蠢Demo打法，A攻A魔，B攻B魔，C攻C魔，会挨打9次
	// 于是决定把通关算为挨打10次，每次扣10血，这样有成功和失败的情况
	me.blood -= 10; // 每次敌人攻击后，玩家的血量减少10

	// 设置回走的目标位置
	startX = me.x + meBorder, startY = me.y;
	switch (beingAttackedByWho) {
	case 'A':
		// 回走的目标是敌人A的位置
		targetX = enemyA.x, targetY = enemyA.y;
		break;
	case 'B':
		// 回走的目标是敌人B的位置
		targetX = enemyB.x, targetY = enemyB.y;
		break;
	case 'C':
		// 回走的目标是敌人C的位置
		targetX = enemyC.x, targetY = enemyC.y;
		break;
	}

	// 敌人回走动画，模拟敌人从目标位置回到原位置
	Scene_EnemyWalking(
		wpImages, me, enemyA, enemyB, enemyC,
		startX, startY, targetX, targetY, beingAttackedByWho,
		false
	);

	// 返回到“站立”页面，战斗回合结束
	return PageSwitch_Standing;
}

// 页面3 子页面5：战斗界面 回合胜利
PageSwitch WarPage_Winning(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
) {

	// 删除文本通道0和通道1的引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");

	// 初始化下一个页面状态，返回到主页面
	PageSwitch nextState = PageSwitch_BackToMain;

	// 循环控制变量
	bool continueLoop = true;

	// 胜利Logo的亮度初始值
	float winningLogoBrightness = 0.2f;

	// 循环动画，直到胜利Logo的亮度达到最大
	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 绘制我方角色和所有敌方角色
		Animator_DrawCharacter(wpImages, me, 1.0f); // 绘制我方角色
		Animator_DrawCharacter(wpImages, enemyA, 1.0f); // 绘制敌人A
		Animator_DrawCharacter(wpImages, enemyB, 1.0f); // 绘制敌人B
		Animator_DrawCharacter(wpImages, enemyC, 1.0f); // 绘制敌人C

		// 绘制胜利的Logo，并根据亮度进行渐变显示
		wpImages.winTheGame.DrawBrightness(175, 240, winningLogoBrightness);

		// 更新动画帧
		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f; // 每10帧重置tick
		}

		// 增加胜利Logo的亮度，使其逐渐变亮
		winningLogoBrightness += 0.008f;

		// 当亮度达到最大值时，结束循环
		if (winningLogoBrightness >= 1.0f) {
			continueLoop = false;
		}

		// 刷新屏幕，显示更新后的内容
		Show();
	}

	// 返回下一个状态，通常为回到主页面
	return nextState;
}

// 页面3 子页面6：战斗界面 回合败北
PageSwitch WarPage_Losing(
	WarPageImages& wpImages, Character me, Character enemyA,
	Character enemyB, Character enemyC
) {

	// 删除文本通道0和通道1的引导文字
	MagicText(L"delete text channel 0");
	MagicText(L"delete text channel 1");

	// 初始化下一个页面状态，返回到地图页面
	PageSwitch nextState = PageSwitch_BackToMap;

	// 循环控制变量
	bool continueLoop = true;

	// 败北Logo的亮度初始值
	float losingLogoBrightness = 1.0f;

	// 循环动画，直到败北Logo的亮度变暗
	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 绘制我方角色和所有敌方角色
		Animator_DrawCharacter(wpImages, me, 1.0f);
		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		// 绘制败北的Logo，并根据亮度进行渐变显示
		wpImages.loseTheGame.DrawBrightness(
			175, 240, losingLogoBrightness
		);

		// 更新动画帧
		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f; // 每10帧重置tick
		}

		// 减少败北Logo的亮度，使其逐渐变暗
		losingLogoBrightness -= 0.008f;

		// 当亮度低于0.2时，结束循环
		if (losingLogoBrightness <= 0.2f) {
			continueLoop = false;
		}

		// 刷新屏幕，显示更新后的内容
		Show();
	}

	// 返回下一个状态，通常为回到地图页面
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

	// 循环控制变量，继续执行直到点击到敌人
	bool continueLoop = true;
	char attackWho = ' '; // 初始化攻击目标为空字符

	// 开始循环，直到用户点击一个敌人
	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 绘制我方角色（亮度设为0.5f）
		Animator_DrawCharacter(wpImages, me, 0.5f);

		// 绘制敌人A并检查是否点击了敌人A
		if (
			Animator_DrawClickableCharacter(wpImages, enemyA)
			== true
		) {
			continueLoop = false;  // 如果点击了敌人A，停止循环
			attackWho = 'A';  // 记录攻击目标为敌人A
		}

		// 绘制敌人B并检查是否点击了敌人B
		if (
			Animator_DrawClickableCharacter(wpImages, enemyB)
			== true
		) {
			continueLoop = false;  // 如果点击了敌人B，停止循环
			attackWho = 'B';  // 记录攻击目标为敌人B
		}

		// 绘制敌人C并检查是否点击了敌人C
		if (
			Animator_DrawClickableCharacter(wpImages, enemyC)
			== true
		) {
			continueLoop = false;  // 如果点击了敌人C，停止循环
			attackWho = 'C';  // 记录攻击目标为敌人C
		}

		// 更新动画帧
		tick += 0.15f;
		if (tick >= 10.0f) {
			tick = 0.0f; // 每10帧重置tick
		}

		// 刷新屏幕，显示更新后的内容
		Show();
	}

	// 返回玩家点击的攻击目标（'A'、'B'、'C'）
	return attackWho;
}

void Scene_Walking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	Picture* thingThatWalks, bool stillDrawMeOrNot
) {

	// 初始化当前位置为起始位置
	float nowX = startX, nowY = startY;

	// 设置步伐的速度
	float speedX = 4.0f;
	float speedY = fabs(nowY - targetY) /
		fabs(nowX - targetX) * speedX;

	// 循环控制变量，用于动画的更新
	bool continueLoop = true;
	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 根据是否需要绘制我方角色的站立状态，绘制角色和血条
		if (stillDrawMeOrNot == true) {
			// 绘制站立的我方角色
			Animator_DrawCharacter(wpImages, me, 1.0f);
		}
		else {
			// 如果不是绘制站立的我方，而是走动中的我方，则需要移动血条
			Animator_DrawBlood(wpImages, me, nowX, nowY);
		}

		// 绘制敌方角色
		Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		Animator_DrawCharacter(wpImages, enemyC, 1.0f);

		// 绘制正在走动的物体（可能是敌人或者其他物体）
		thingThatWalks[0].Draw(int(nowX), int(nowY));

		// 计算角色当前位置相对于目标位置的状态
		float posState = (nowX - startX) / (targetX - startX);
		// 使用正弦函数计算当前位置的比例，产生平滑的动画效果
		float sinState = sin(posState * PI);
		// 通过sinState调整移动比例，使得角色的移动更为流畅
		float ratioState = 0.3f + sinState * 3.0f;

		// 根据目标位置更新当前的位置，速度会受到比例的影响
		if (nowX < targetX) nowX += speedX * ratioState;
		if (nowX > targetX) nowX -= speedX * ratioState;
		if (nowY < targetY) nowY += speedY * ratioState;
		if (nowY > targetY) nowY -= speedY * ratioState;

		// 判断是否到达目标位置，如果到达则停止循环
		if (
			fabs(nowX - targetX) < 3.5f &&
			fabs(nowY - targetY) < 7.5f
		) {
			continueLoop = false;
		}

		// 更新屏幕显示
		Show();
	}
}

void Scene_EnemyWalking(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float startX, float startY, float targetX, float targetY,
	char whichEnemy, bool stillDrawEnemyOrNot
) {

	// 根据攻击的敌人选择目标敌人
	Character* enemyPtr = nullptr;
	switch (whichEnemy) {
	case 'A':
		enemyPtr = &enemyA;  // 选择敌人A
		break;
	case 'B':
		enemyPtr = &enemyB;  // 选择敌人B
		break;
	case 'C':
		enemyPtr = &enemyC;  // 选择敌人C
		break;
	}

	// 初始化敌人当前位置
	float nowX = startX, nowY = startY;
	// 计算敌人移动的速度
	float speedX = 4.0f;
	float speedY = fabs(nowY - targetY) /
		fabs(nowX - targetX) * speedX;

	// 循环控制变量，用于动画的更新
	bool continueLoop = true;
	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 根据是否需要绘制敌人角色的站立状态，绘制敌人和血条
		if (stillDrawEnemyOrNot == true) {
			// 绘制站立的敌人角色
			Animator_DrawCharacter(wpImages, *enemyPtr, 1.0f);
		}
		else {
			// 如果不是绘制敌人站立，而是走动中的敌人，则需要移动血条
			Animator_DrawBlood(wpImages, *enemyPtr, nowX, nowY);
		}

		// 绘制我方角色
		Animator_DrawCharacter(wpImages, me, 1.0f);

		// 绘制剩余的敌人角色（不包括当前正在移动的敌人）
		if (enemyPtr != &enemyA) {
			Animator_DrawCharacter(wpImages, enemyA, 1.0f);
		}
		if (enemyPtr != &enemyB) {
			Animator_DrawCharacter(wpImages, enemyB, 1.0f);
		}
		if (enemyPtr != &enemyC) {
			Animator_DrawCharacter(wpImages, enemyC, 1.0f);
		}

		// 绘制正在走动的敌人角色
		enemyPtr->imgArr[0].Draw(int(nowX), int(nowY));

		// 计算角色当前位置相对于目标位置的状态
		float posState = (nowX - startX) / (targetX - startX);
		// 使用正弦函数计算当前位置的比例，产生平滑的动画效果
		float sinState = sin(posState * PI);
		// 通过sinState调整移动比例，使得角色的移动更为流畅
		float ratioState = 0.3f + sinState * 3.0f;

		// 根据目标位置更新当前的位置，速度会受到比例的影响
		if (nowX < targetX) nowX += speedX * ratioState;
		if (nowX > targetX) nowX -= speedX * ratioState;
		if (nowY < targetY) nowY += speedY * ratioState;
		if (nowY > targetY) nowY -= speedY * ratioState;

		// 判断是否到达目标位置，如果到达则停止循环
		if (
			fabs(nowX - targetX) < 3.5f &&
			fabs(nowY - targetY) < 7.5f
		) {
			continueLoop = false;
		}

		// 更新屏幕显示
		Show();
	}
}

void Scene_HittingEnemy(
	WarPageImages& wpImages, Character& me, Character& enemyA,
	Character& enemyB, Character& enemyC,
	float thingX, float thingY, char attackWho,
	Picture* thingThatHurts, bool stillDrawMeOrNot,
	int animationDuration
) {
	// 初始化动画循环控制变量
	bool continueLoop = true;
	// 初始化攻击动画的时长计时器
	float attackTick = 0;

	while (continueLoop) {
		// 绘制背景
		wpImages.bg.Draw(0, 0);

		// 根据是否绘制站立的我方角色来判断是否移动血条
		if (stillDrawMeOrNot == true) {
			// 绘制站立的我方角色
			Animator_DrawCharacter(wpImages, me, 1.0f);
		}
		else {
			// 如果是绘制走动中的我方角色，则需要移动血条
			Animator_DrawBlood(wpImages, me, thingX, thingY);
		}

		// 根据`attackWho`选择不同的敌人进行绘制，并设置敌人被攻击时的显示状态
		switch (attackWho) {
		case 'A':
			// 绘制敌人A为攻击目标，敌人B和C为正常状态
			Animator_DrawCharacter(wpImages, enemyA, 2.0f);
			Animator_DrawCharacter(wpImages, enemyB, 1.0f);
			Animator_DrawCharacter(wpImages, enemyC, 1.0f);
			break;
		case 'B':
			// 绘制敌人B为攻击目标，敌人A和C为正常状态
			Animator_DrawCharacter(wpImages, enemyA, 1.0f);
			Animator_DrawCharacter(wpImages, enemyB, 2.0f);
			Animator_DrawCharacter(wpImages, enemyC, 1.0f);
			break;
		case 'C':
			// 绘制敌人C为攻击目标，敌人A和B为正常状态
			Animator_DrawCharacter(wpImages, enemyA, 1.0f);
			Animator_DrawCharacter(wpImages, enemyB, 1.0f);
			Animator_DrawCharacter(wpImages, enemyC, 2.0f);
			break;
		}

		// 绘制伤害特效，thingThatHurts 的当前帧绘制在敌人上方
		thingThatHurts[int(attackTick)].Draw(int(thingX), int(thingY));

		// 增加攻击动画的计时器
		attackTick += 0.15f;

		// 如果动画时长超过指定的 animationDuration，则结束动画循环
		if (attackTick > (float)animationDuration) {
			continueLoop = false;
		}

		// 更新屏幕显示
		Show();
	}
}

void Scene_BloodUpdate(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, int bloodDrop
) {
	// 定义指向角色血量的指针变量
	int* ptr;
	// 根据`attackWho`决定受伤害的敌人，并减少其血量
	switch (attackWho) {
	case 'A':
		// 敌人A受伤，减少血量
		enemyA.blood -= bloodDrop;
		break;
	case 'B':
		// 敌人B受伤，减少血量
		enemyB.blood -= bloodDrop;
		break;
	case 'C':
		// 敌人C受伤，减少血量
		enemyC.blood -= bloodDrop;
		break;
	}
}

/**
 * @section
 * Animator 一些动画函数
 */

// 绘制角色的血条
void Animator_DrawBlood(
	WarPageImages& wpImages, Character& character,
	int customX = -1, int customY = -1
) {
	// 设置血条的X坐标，如果没有传入自定义坐标，则使用角色的X坐标
	int bloodX = customX == -1 ? character.x : customX;
	// 设置血条的Y坐标，如果没有传入自定义坐标，则使用角色的Y坐标
	int bloodY = customY == -1 ? character.y : customY;

	// 绘制血条的底层（黑色底层）
	wpImages.bloodBar_BottomLayer.Draw(
		bloodX + character.bloodBarOffsetX,
		bloodY + character.bloodBarOffsetY
	);

	// 绘制血条的上层（红色上层），根据角色的血量显示红色部分
	wpImages.bloodBar.DrawClip(
		bloodX + character.bloodBarOffsetX,
		bloodY + character.bloodBarOffsetY,
		0.0f, (float)character.blood, 0.0f, 100.0f
	);
}

// 绘制角色及其血条
void Animator_DrawCharacter(
	WarPageImages& wpImages, Character& character,
	float brightness
) {
	// 只有当角色的血量大于0时才绘制角色
	if (character.blood > 0) {
		// 绘制角色的血条
		Animator_DrawBlood(wpImages, character);
		// 绘制角色的图像，并设置亮度
		character.imgArr[int(tick)].DrawBrightness(
			character.x, character.y, brightness
		);
	}
}

// 绘制可点击的角色，并检测是否被点击
bool Animator_DrawClickableCharacter(
	WarPageImages& wpImages, Character& character
) {
	// 初始化点击状态
	bool isClickingMe = false;

	// 只有当角色的血量大于0时才绘制角色
	if (character.blood > 0) {
		// 绘制角色的血条
		Animator_DrawBlood(wpImages, character);
		// 检测鼠标是否在角色图像范围内
		if (
			IsMouseInsidePicture(
				&character.imgArr[int(tick)],
				character.x, character.y
			)
		) {
			// 如果鼠标在角色上，绘制角色的图像并亮度为1
			character.imgArr[int(tick)].DrawBrightness(
				character.x, character.y, 1.0f
			);
			// 检测左键是否被点击
			if (isLeftClick == true) {
				// 如果左键点击了角色，设置点击状态为true
				isClickingMe = true;
			}
		}
		else {
			// 如果鼠标不在角色范围内，绘制角色的图像并降低亮度
			character.imgArr[int(tick)].DrawBrightness(
				character.x, character.y, 0.5f
			);
		}
	}

	// 返回是否点击了角色
	return isClickingMe;
}

// 计算目标敌人所在的位置，通常用于敌人移动到指定位置
void Animator_LocateWalkToPos(
	Character& enemyA, Character& enemyB, Character& enemyC,
	char attackWho, float* resultX, float* resultY
) {
	// 设置一个边界值，用于控制角色与目标之间的距离
	float border = 25.0f;

	// 根据`attackWho`选择不同的敌人，并计算其位置
	switch (attackWho) {
	case 'A':
		// 敌人A的位置减去边界值
		*resultX = enemyA.x - border, *resultY = enemyA.y;
		break;
	case 'B':
		// 敌人B的位置减去边界值
		*resultX = enemyB.x - border, *resultY = enemyB.y;
		break;
	case 'C':
		// 敌人C的位置减去边界值
		*resultX = enemyC.x - border, *resultY = enemyC.y;
		break;
	}
}
