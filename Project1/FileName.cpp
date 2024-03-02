//SFML的头文件
#include <SFML/Graphics.hpp>
#include <vector>
#include<iostream>
//音乐播放的头文件
#include <Windows.h>
#include <mmsystem.h>
#pragma comment (lib,"winmm.lib")

/*
目标：
    修复撞到身体程序就会崩溃的bug
    修改蛇身增长的算法
    增加吃到水果的音效处理
    增加游戏趣味性
    现实目前得分
    优化显示程序
*/

/*
2024/02/29 完成对源代码的理解

*/
const int size = 16;
const int windowWidth = 60;
const int windowHeight = 40;

int dir;
int score;
struct SnakeSegment {
    int x, y;

    SnakeSegment(int xx, int yy) : x(xx), y(yy) {}

    bool operator == (const SnakeSegment& other) {           //进行运算符的重载，==运算符用来比较两者的坐标是否相同
        return this->x == other.x && this->y == other.y;
    }
};

sf::Vector2i fruit;

std::vector<SnakeSegment> snake;

void init() {
    snake.clear();   
    snake.push_back(SnakeSegment(1, 1));
    snake.push_back(SnakeSegment(2, 1));
    snake.push_back(SnakeSegment(3, 1));

    fruit.x = rand() % (windowWidth - 1);
    fruit.y = rand() % (windowHeight - 1);              //随机产生食物

    dir = 2;
    score = 0;             //增加分数
}

void renderBorder(sf::RenderWindow& window) {//绘制边界
    sf::RectangleShape bd(sf::Vector2f(size, size));
    bd.setFillColor(sf::Color(122, 122, 122));

    // 绘制上下边界
    for (int i = 0; i < windowWidth; i++) {
        bd.setPosition(i * size, 0);
        window.draw(bd);

        bd.setPosition(i * size, (windowHeight - 1) * size);
        window.draw(bd);
    }

    // 绘制左右边界
    for (int i = 1; i < windowHeight - 1; i++) {
        bd.setPosition(0, i * size);
        window.draw(bd);

        bd.setPosition((windowWidth - 1) * size, i * size);
        window.draw(bd);
    }
}

void renderSnake(sf::RenderWindow& window) {
    sf::RectangleShape rec(sf::Vector2f(size - 1, size - 1));
    int n = snake.size();

    for (int i = 0; i < n - 1; i++) {
        rec.setPosition(snake[i].x * size, snake[i].y * size);
        rec.setFillColor(sf::Color::Green);
        window.draw(rec);
    }
    rec.setPosition(snake[n - 1].x * size, snake[n - 1].y * size);
    rec.setFillColor(sf::Color::Yellow);
    window.draw(rec);
}

void render(sf::RenderWindow& window) {
    sf::RectangleShape rec(sf::Vector2f(size - 1, size - 1));//绘制果实的颜色大小
    rec.setFillColor(sf::Color::Red);
    rec.setPosition(fruit.x * size, fruit.y * size);
    window.draw(rec);

    renderBorder(window);
    renderSnake(window);
}

void update() {
    int n = snake.size();
    //移动问题
    for (int i = 0; i < n - 1; i++) //行走的坐标变换逻辑
        snake[i] = snake[i + 1]; 
    if (dir == 0) snake[n - 1].x--; if (dir == 1) snake[n - 1].y++;//改变头部坐标
    if (dir == 2) snake[n - 1].x++; if (dir == 3) snake[n - 1].y--;

    //查找身体坐标是否包含头部坐标
    if (std::find(snake.begin(), snake.end() - 1, snake[n - 1]) != snake.end() - 1)//撞到身体
        init();//重新开始
    //长度增加问题
    if (snake[n - 1].x == fruit.x && snake[n - 1].y == fruit.y) {
        fruit.x = rand() % (windowWidth - 2) + 1;
        fruit.y = rand() % (windowHeight - 2) + 1;
        snake.push_back(snake[n - 1]);// 可以进行改进，行走snake.size次才增加长度
        score++;
        if (score % 5 != 0) {
            PlaySound(L"growup.wav", NULL, SND_ASYNC);
        }
           
        else {
            PlaySound(L"upgrade.wav", NULL, SND_ASYNC);
        }
            
    }

    //穿墙处理
    if (snake[n - 1].x >= windowWidth - 1) snake[n - 1].x = 1;
    if (snake[n - 1].x < 1) snake[n - 1].x = windowWidth - 2;

    if (snake[n - 1].y >= windowHeight - 1) snake[n - 1].y = 1;
    if (snake[n - 1].y < 1) snake[n - 1].y = windowHeight - 2;

}


int main(int argc, char const* argv[])
{
    sf::RenderWindow window(sf::VideoMode(windowWidth * size, windowHeight * size), "Snake Game"); //定义窗口的大小，以及名字

    sf::Clock clock;
    float dtTime = 0;
    float delay = 0.25; //控制刷新速度
    srand(1000009);              //播种由函数rand使用的随机数发生器

    init();

    while (window.isOpen()) {
        sf::Event event;          //初始化事件

        dtTime += clock.restart().asSeconds();   //启动计时器使用秒计时

        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left) && dir != 2) {  //简单的方向控制逻辑
                dir = 0;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && dir != 3) {
                dir = 1;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right) && dir != 0) {
                dir = 2;
            }
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && dir != 1) {
                dir = 3;
            }
        }

        if (dtTime > delay) {  //进过delay的时间进行地图的刷新
            dtTime = 0;
            update();
        }
        if (!(score % 5)) {    
            delay -= 0.1;
        }
        window.clear(sf::Color::Black);
        render(window);
        window.display();
    }
    return 0;
}