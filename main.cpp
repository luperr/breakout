#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>
#include <cmath>

using namespace std;
using namespace sf;

using FrameTime = float;

constexpr int windowWidth{800}, windowHeight{600};
constexpr float ballRadius{10.f}, ballVelocity{6.f};
constexpr float paddleWidth{60.f}, paddleHeight{20.f}, paddleVelocity{6.f};
constexpr float blockHeight{20.f}, blockWidth{60.f};
constexpr int countBlockX{11}, countBlockY{4}; 

struct Ball{
	CircleShape shape;
	Vector2f velocity{-ballVelocity, -ballVelocity};
	Ball(float mX, float mY){
		shape.setPosition(mX, mY);
		shape.setRadius(ballRadius);
		shape.setFillColor(Color::Red);
		shape.setOrigin(ballRadius, ballRadius);
	}
	
	void update(){
		shape.move(velocity); 
			if(left() < 0) velocity.x = ballVelocity;
		else if(right() > windowWidth) velocity.x = -ballVelocity;
		
		if(top() < 0) velocity.y = ballVelocity;
		else if(bottom() > windowHeight) velocity.y = -ballVelocity;	
	}

	float x() 		const noexcept {return shape.getPosition().x;}
	float y() 		const noexcept {return shape.getPosition().y;}
	float left() 	const noexcept {return x() - shape.getRadius();}
	float right() 	const noexcept {return x() + shape.getRadius();}
	float top() 	const noexcept {return y() - shape.getRadius();}
	float bottom() 	const noexcept {return y() + shape.getRadius();}
};

struct Rectangle{
	RectangleShape shape;
	float x() 		const noexcept {return shape.getPosition().x;}
	float y() 		const noexcept {return shape.getPosition().y;}
	float left()	const noexcept {return x() - shape.getSize().x/2.f;}
	float right() 	const noexcept {return x() + shape.getSize().x/2.f;}
	float top() 	const noexcept {return y() - shape.getSize().y/2.f;}
	float bottom() 	const noexcept {return y() + shape.getSize().y/2.f;}
	
};

struct Paddle : public Rectangle{
	Vector2f velocity;
	

	Paddle(float mX, float mY){
		shape.setPosition(mX, mY);
		shape.setSize({paddleWidth, paddleHeight});
		shape.setFillColor(Color::Red);
		shape.setOrigin(paddleWidth/2.f, paddleHeight/2.f);
	}

	void update(){
		shape.move(velocity);

		if(Keyboard::isKeyPressed(Keyboard::Key::Left) && left() > 0)
			velocity.x = -paddleVelocity;
		else if(Keyboard::isKeyPressed(Keyboard::Key::Right) && right() < windowWidth)
			velocity.x = paddleVelocity;
		else velocity.x = 0;
	}
};

struct Brick : public Rectangle{
	bool destroyed {false};

	Brick(float mX, float mY){
		shape.setPosition(mX, mY);
		shape.setSize({blockWidth, blockHeight});
		shape.setFillColor(Color::Yellow);
		shape.setOrigin(blockWidth/2.f, blockHeight/2.f);
	}
};

template<class T1, class T2> bool isIntersecting(T1& mA, T2&mB){
	return mA.right() >= mB.left() && mA.left() <= mB.right() &&
			mA.bottom() >= mB.top() && mA.top() <= mB.bottom();
}

void testCollision(Paddle& mPaddle, Ball& mBall){
	if(!isIntersecting(mPaddle, mBall)) return;

	mBall.velocity.y = -ballVelocity;

	if(mBall.x() < mPaddle.x())
		mBall.velocity.x = -ballVelocity;
	else
		mBall.velocity.x = ballVelocity;

}

void testCollision(Brick& mBrick, Ball& mBall)
{
    if(!isIntersecting(mBrick, mBall)) return;

    mBrick.destroyed = true;

    float overlapLeft{mBall.right() - mBrick.left()};
    float overlapRight{mBrick.right() - mBall.left()};
    float overlapTop{mBall.bottom() - mBrick.top()};
    float overlapBottom{mBrick.bottom() - mBall.top()};

    bool ballFromLeft(abs(overlapLeft) < abs(overlapRight));

    bool ballFromTop(abs(overlapTop) < abs(overlapBottom));

    float minOverlapX{ballFromLeft ? overlapLeft : overlapRight};
    float minOverlapY{ballFromTop ? overlapTop : overlapBottom};

    if(abs(minOverlapX) < abs(minOverlapY))
        mBall.velocity.x = ballFromLeft ? -ballVelocity : ballVelocity;
    else
		mBall.velocity.y = ballFromTop ? -ballVelocity : ballVelocity;
}

int main(){
	Ball ball{windowWidth/2, windowHeight/2};
	Paddle paddle {windowWidth/2, windowHeight-50};
	vector<Brick> bricks;

	for(int iX{0}; iX < countBlockX; iX++)
		for(int iY{0}; iY <countBlockY; iY++)
			bricks.emplace_back((iX + 1) * (blockWidth + 3) + 22, (iY + 2) * (blockHeight +3));

	RenderWindow window{{windowWidth, windowHeight}, "Breakout"};
	window.setFramerateLimit(60);


	while(true){
		window.clear(Color::Black);

		Event event;
		while(window.pollEvent(event)){
			if(event.type == Event::Closed){
				window.close();
				break;
			}
		}
		
		if(Keyboard::isKeyPressed(Keyboard::Key::Escape)) break;
		
		ball.update();
		paddle.update();
		testCollision(paddle, ball);
		for(auto& brick : bricks) testCollision(brick, ball);

		bricks.erase(remove_if(begin(bricks), end(bricks),
            [](const Brick& mBrick){
                return mBrick.destroyed;
            }),
		end(bricks));

		window.draw(paddle.shape);
		window.draw(ball.shape);
		for(auto& brick : bricks)
			window.draw(brick.shape);
		window.display();
	}

	return 0;
}