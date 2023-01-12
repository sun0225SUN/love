#include<graphics.h>
#include<conio.h>
#include<time.h>
#include<math.h>
#include<stdlib.h>


//爱心点结构体
struct Point {
	double x, y;     //坐标
	COLORREF color;  //颜色
};

//颜色数组
COLORREF colors[7] = { RGB(255,32,83),RGB(252,222,250) ,RGB(255,0,0) ,RGB(255,0,0) ,RGB(255,2,2) ,RGB(255,0,8) ,RGB(255,5,5) };
//COLORREF colors[7] = { RGB(55,132,83),RGB(252,222,250) ,RGB(25,120,130) ,RGB(25,230,40) ,RGB(25,24,112) ,RGB(255,230,128) ,RGB(25,5,215) };


const int xScreen = 1200;				  //屏幕宽度
const int yScreen = 800;				  //屏幕高度
const double PI = 3.1426535159;         //圆周率
const double e = 2.71828;				  //自然数e
const double averag_distance = 0.162;    //弧度以0.01增长时，原始参数方程每个点的平均距离
const int quantity = 506;				  //一个完整爱心所需点的数量
const int circles = 210;				  //组成爱心主体的爱心个数（每个爱心会乘以不同系数）
const int frames = 20;					  //爱心扩张一次的帧数
Point  origin_points[quantity];			  //创建一个保存原始爱心数据的数组
Point  points[circles * quantity];      //创建一个保存所有爱心数据的数组
IMAGE images[frames];					  //创建图片数组

//坐标转换函数
double screen_x(double x)
{
	x += xScreen / 2;
	return x;
}
//坐标转换函数
double screen_y(double y)
{
	y = -y + yScreen / 2;
	return y;
}

//创建x1-x2的随机数的函数
int creat_random(int x1, int x2)
{
	if (x2 > x1)
		return  rand() % (x2 - x1 + 1) + x1;
	else
		return 0;
}

//创建爱心扩张一次的全部数据，并绘制成20张图片保存
// 1 用参数方程计算出一个爱心的所有坐标并保存在 origin_points 中
// 2 重复对 origin_points 的所有坐标乘上不同的系数获得一个完整的爱心坐标数据，并保存在 points 中
// 3 通过一些数学逻辑计算 points 中所有点扩张后的坐标并绘制，并覆盖掉原来的数据（循环20次）
// 4 计算圆的外层那些闪动的点，不保存这些点的数据（循环20次）
void creat_data()
{

	int index = 0;

	//保存相邻的坐标信息以便用于计算距离
	double x1 = 0, y1 = 0, x2 = 0, y2 = 0;
	for (double radian = 0.1; radian <= 2 * PI; radian += 0.005)
	{
		//爱心的参数方程
		x2 = 16 * pow(sin(radian), 3);
		y2 = 13 * cos(radian) - 5 * cos(2 * radian) - 2 * cos(3 * radian) - cos(4 * radian);

		//计算两点之间的距离 开根号（(x1-x2)平方 + （y1-y1）平方）
		double distance = sqrt(pow(x2 - x1, 2) + pow(y2 - y1, 2));

		//只有当两点之间的距离大于平均距离才保存这个点，否则跳过这个点
		if (distance > averag_distance)
		{
			//x1和y1保留当前数据
			//x2和y2将在下一次迭代获得下一个点的坐标
			x1 = x2, y1 = y2;
			origin_points[index].x = x2;
			origin_points[index++].y = y2;
		}
	}

	index = 0;
	for (double size = 0.1; size <= 20; size += 0.1)
	{
		//用sigmoid函数计算当前系数的成功概率
		//用个例子说明一下，假设有100个点成功概率为 90%，那么就可能会有90个点经过筛选保留下来
		//					假设有100个点成功概率为 20%，那么就可能会有20个点经过筛选保留下来
		double success_p = 1 / (1 + pow(e, 8 - size / 2));

		//遍历所有原始数据
		for (int i = 0; i < quantity; ++i)
		{
			//用概率进行筛选
			if (success_p > creat_random(0, 100) / 100.0)
			{
				//从颜色数组随机获得一个颜色
				points[index].color = colors[creat_random(0, 6)];

				//对原始数据乘上系数保存在points中
				points[index].x = size * origin_points[i].x + creat_random(-4, 4);
				points[index++].y = size * origin_points[i].y + creat_random(-4, 4);
			}
		}
	}

	//index当前值就是points中保存了结构体的数量
	int points_size = index;

	for (int frame = 0; frame < frames; ++frame)
	{
		//初始化每张图片宽xScreen，高yScreen
		images[frame] = IMAGE(xScreen, yScreen);

		//把第frame张图像设为当前工作图片
		SetWorkingImage(&images[frame]);

		//计算爱心跳动的坐标
		for (index = 0; index < points_size; ++index)
		{

			double x = points[index].x, y = points[index].y;                              //把当前值赋值给x和y
			double distance = sqrt(pow(x, 2) + pow(y, 2));										//计算当前点与原点的距离
			double diatance_increase = -0.0009 * distance * distance + 0.35714 * distance + 5;	//把当前距离代入方程获得该点的增长距离

			//根据增长距离计算x轴方向的增长距离 x_increase = diatance_increase * cos（当前角度）
			//cos（当前角度）= x / distance
			double x_increase = diatance_increase * x / distance / frames;
			//根据增长距离计算x轴方向的增长距离 x_increase = diatance_increase * sin（当前角度）
			//sin（当前角度）= y / distance
			double y_increase = diatance_increase * y / distance / frames;

			//因为以上计算得到的是一整个过程的增长距离，而整个过程持续20帧，因此要除20


			//用新的数据覆盖原来的数据
			points[index].x += x_increase;
			points[index].y += y_increase;

			//提取当前点的颜色设置为绘画颜色
			setfillcolor(points[index].color);
			//注意，因为以上所有坐标是基于数学坐标的
			//因此绘制到屏幕是就要转换为屏幕坐标
			solidcircle(screen_x(points[index].x), screen_y(points[index].y), 1);
		}

		//产生外围闪动的点
		for (double size = 17; size < 23; size += 0.3)
		{
			for (index = 0; index < quantity; ++index)
			{
				//当系数大于等于20，通过概率为百分之四十，当系数小于20，通过概率为百分之五
				//20作为关键值是因为爱心主体的最大系数就是20
				if ((creat_random(0, 100) / 100.0 > 0.6 && size >= 20) || (size < 20 && creat_random(0, 100) / 100.0 > 0.95))
				{
					double x, y;
					if (size >= 20)
					{
						//用frame的平方的正负值作为上下限并加减15产生随机数
						//用frame的平方的好处是frame越大，外围闪动的点运动范围越大
						x = origin_points[index].x * size + creat_random(-frame * frame / 5 - 15, frame * frame / 5 + 15);
						y = origin_points[index].y * size + creat_random(-frame * frame / 5 - 15, frame * frame / 5 + 15);
					}
					else
					{
						//对于系数小于20的处理与爱心点一样
						x = origin_points[index].x * size + creat_random(-5, 5);
						y = origin_points[index].y * size + creat_random(-5, 5);
					}


					//随机获取颜色并设置为当前绘图颜色
					setfillcolor(colors[creat_random(0, 6)]);
					//把数学坐标转换为屏幕坐标再进行绘制
					solidcircle(screen_x(x), screen_y(y), 1);
					//需要注意的是，我并没有保存这些点，因为这些点不需要前一帧的坐标数据
					//只需要当前系数就可绘制出来，因此没 必要保存
				}
			}
		}
	}
}

int main()
{
	initgraph(xScreen, yScreen);  //创建屏幕
	BeginBatchDraw();			  //开始批量绘图
	srand(time(0));				  //初始化随机种子
	creat_data();				  //调用函数产生20张图片
	SetWorkingImage();			  //调用函数把工作图像恢复为窗口，没有添加参数默认为窗口
	//因为接下是用窗口播放图片，因此要把绘图效果设置为窗口

	bool extend = true, shrink = false;
	for (int frame = 0; !_kbhit();)     //退出条件为检测到按键信息
	{
		putimage(0, 0, &images[frame]); //播放第frame张图片
		FlushBatchDraw();				//刷新批量绘图
		Sleep(20);						//延时20毫秒
		cleardevice();					//清除屏幕，用来播放下一帧图片


		//注意 creat data 产生的只是爱心扩张的20张图片，并没有产生爱心收缩的图片
		//但是把扩张的图片倒着播放就产生的收缩的效果
		//所以下面这个 if else 语句就是决定图片是正常播放还是倒着播放

		if (extend)  //扩张时， ++frame，正常播放
			frame == 19 ? (shrink = true, extend = false) : ++frame;
		else         //收缩时， --frame，倒着播放
			frame == 0 ? (shrink = false, extend = true) : --frame;
	}

	EndBatchDraw(); //关闭批量绘图
	closegraph();	//关闭绘图窗口
	return 0;		//结束程序
}