#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <vector>
using namespace std;


class Ball {
    public:
        float radius;
        float velocity;
        float x_velocity;
        float y_velocity;
        float angle;
        float mass;
        float x;
        float y;
        float contactang;
        float contactxpos;
        float contactypos;
        sf::CircleShape shape;

        Ball(float radius, float x, float y, float velocity, float angle, float mass) {
            this->radius = radius;
            this->x = x;
            this->y = y;
            this->velocity = velocity;
            this->angle = angle;
            this->mass = mass;
            x_velocity = velocity * cos(angle);
            y_velocity = velocity * sin(angle);
            shape.setRadius(radius);
            shape.setFillColor(sf::Color::Green);
            shape.setOrigin({shape.getRadius(), shape.getRadius()});
            shape.setPosition({x,y});
        }

        void BallMovement() {
            x += x_velocity;
            y += y_velocity;
            //cout << "Contact Xpos: " << contactxpos << "\n";
            //cout << "Contact ypos: " << contactypos << "\n";
            shape.setPosition({x,y});
        }

        void WallCollision() {
            if (x <= (radius)) {
                x_velocity = -x_velocity;
                angle = acos(x_velocity/velocity);
                x = radius + 1;
            }
            if (x >= (1280 - radius)) {
                x_velocity = -x_velocity;
                angle = acos(x_velocity/velocity);
                x = 1280 - 1 - radius;
            }
            if (y >= (720 - radius)) {
                y_velocity = -y_velocity;
                angle = asin(y_velocity/velocity);
                y = 720 - 1 - radius;
            }
            if (y <= (radius)) {
                y_velocity = -y_velocity;
                angle = asin(y_velocity/velocity);
                y = radius + 1;
            }
        }

        bool CheckCollision(Ball other) {
            float x_dist = abs(x - other.x);
            float y_dist = abs(y - other.y);
            float distance = sqrt((pow(x_dist, 2))+(pow(y_dist, 2)));
            if(distance - (radius + other.radius) <= 0) {
                contactxpos = x;
                contactypos = y;
                other.contactxpos = other.x;
                other.contactypos = other.y;
                cout << "Contact Xpos: " << contactxpos << "\n";
                cout << "Contact ypos: " << contactypos << "\n";
                cout << "other.Contact Xpos: " << other.contactxpos << "\n";
                cout << "other.Contact ypos: " << other.contactypos << "\n";

                return true;
            }
            else {
                return false;
            }
        }
        
        void ContactAngle(Ball other){
            float x_dist = abs(x - other.x);
            float y_dist = abs(y - other.y);
            contactang = atan(y_dist/x_dist);

            cout << "CONTACTANG:" << contactang << "\n";

        }

        float DotProduct(Ball other) {
            return (x_velocity - other.x_velocity)*(x - other.x) + (y_velocity - other.y_velocity)*(y - other.y);
        }

        void BallBounce(Ball& other) {
            float magnitude1 = sqrt(pow((x - other.x), 2) + pow((y - other.y), 2));
            float magnitude2 = sqrt(pow((other.x - x), 2) + pow((other.y - y), 2));
            float x_temp = x_velocity - ((2*(other.mass))/(mass + other.mass))*(((x_velocity - other.x_velocity)*(x - other.x) + (y_velocity - other.y_velocity)*(y - other.y))/(pow(magnitude1, 2)))*(x - other.x);
            float y_temp = y_velocity - ((2*(other.mass))/(mass + other.mass))*(((x_velocity - other.x_velocity)*(x - other.x) + (y_velocity - other.y_velocity)*(y - other.y))/(pow(magnitude1, 2)))*(y - other.y);
            float otherx_temp = other.x_velocity - ((2*(mass))/(mass + other.mass))*(((other.x_velocity - x_velocity)*(other.x - x) + (other.y_velocity - y_velocity)*(other.y - y))/(pow(magnitude2, 2)))*(other.x - x);
            float othery_temp = other.y_velocity - ((2*(mass))/(mass + other.mass))*(((other.x_velocity - x_velocity)*(other.x - x) + (other.y_velocity - y_velocity)*(other.y - y))/(pow(magnitude2, 2)))*(other.y - y);
            x_velocity = x_temp;
            y_velocity = y_temp;
            other.x_velocity = otherx_temp;
            other.y_velocity = othery_temp;
        }
    
        float CollisionSearch(Ball ballone, Ball balltwo, float time, float step, float iterationNum, int precision) {
            if (iterationNum = precision) {
                return time;
            }
            ballone.x = ballone.x + ballone.x_velocity * time;
            ballone.y = ballone.y + ballone.y_velocity * time;
            balltwo.x = balltwo.x + balltwo.x_velocity * time;
            balltwo.y = balltwo.y + balltwo.y_velocity * time;
            if(ballone.CheckCollision(balltwo)) {
                ballone.x = ballone.x - ballone.x_velocity * time;
                ballone.y = ballone.y - ballone.y_velocity * time;
                balltwo.x = balltwo.x - balltwo.x_velocity * time;
                balltwo.y = balltwo.y - balltwo.y_velocity * time;
                return CollisionSearch(ballone, balltwo, time - step, step/2, iterationNum+1, precision);
            }
            else {
                ballone.x = ballone.x - ballone.x_velocity * time;
                ballone.y = ballone.y - ballone.y_velocity * time;
                balltwo.x = balltwo.x - balltwo.x_velocity * time;
                balltwo.y = balltwo.y - balltwo.y_velocity * time;
                return CollisionSearch(ballone, balltwo, time + step, step/2, iterationNum+1, precision);
            }
        }

        void CollisionAdjust(Ball &other) {
            Ball ballone(*this);
            Ball balltwo(other);
            x = x - x_velocity;
            y = y - y_velocity;
            other.x = other.x - other.x_velocity;
            other.y = other.y - other.y_velocity;
            float collisionTime = CollisionSearch(ballone, balltwo, 0.5, 0.25, 0, 10);
            x = x - x_velocity*(1-collisionTime);
            y = y - y_velocity*(1-collisionTime);
            other.x = other.x - other.x_velocity*(1-collisionTime);
            other.y = other.y - other.y_velocity*(1-collisionTime);
            this->BallBounce(other);

        }
};

class clickBoxes {
    public:
        float box_x1;
        float box_x2;
        float box_y1;
        float box_y2;
        sf::RectangleShape box;
        clickBoxes() {
        }
        clickBoxes(sf::Vector2f position, sf::Vector2f size, sf::Color color, bool outline) {
            this->box_x1 = box_x1;
            this->box_x2 = box_x2;
            this->box_y1 = box_y1;
            this->box_y2 = box_y2;
            box.setFillColor(color);
            box.setSize(size);
            box.setPosition(position);
            if(outline) {
                box.setOutlineThickness(2.f);
                box.setOutlineColor(sf::Color::White);
            }
            box_x1 = box.getPosition().x;
            box_x2 = box_x1 + box.getSize().x;
            box_y1 = box.getPosition().y;
            box_y2 = box_y1 + box.getSize().y;

        }
};

void Texts(sf::Text &text, std::string name, int fontsize, sf::Color color, sf::Vector2f position) {
        text.setString(name);
        text.setCharacterSize(fontsize);
        text.setFillColor(color);
        text.setPosition(position);
}

sf::Text TextBoxes(sf::Font font, int number, float ballnumber) {
    sf::Vector2f setposition = {140, 190 + (ballnumber*60)};
    sf::Text textbox(font);
    textbox.setFillColor(sf::Color::White);
    std::string numberString = std::to_string(number);
    textbox.setString("Ball " + numberString);
    textbox.setPosition(setposition);
    return textbox;
}

int userInput(int intString, int limit, bool isSelected, std::string& input) {
    int stringsize = input.size();
    int intInput;
    if(isSelected && stringsize < limit) {
        input.append(std::to_string(intString));
        intInput = std::stoi(input);
        return intInput;
    }
    else {
        return intInput;
    }
}

int clickCheck(int textBoxNumber, clickBoxes &clickbox, clickBoxes sizeboxes[], clickBoxes angleboxes[], clickBoxes massboxes[], clickBoxes veloboxes[], float mouse_x, float mouse_y, bool &sizeClicked,  bool &angleClicked,  bool &massClicked,  bool &veloClicked, std::string &input) {
    /*if((mouse_x >= clickbox.box_x1) && (mouse_x <= clickbox.box_x2) && (mouse_y >= clickbox.box_y1) && (mouse_y <= clickbox.box_y2)) {
        //clickbox clicked
    }*/
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        for(int i = 0; i < textBoxNumber; i++) {
            if((mouse_x >= sizeboxes[i].box_x1) && (mouse_x <= sizeboxes[i].box_x2) && (mouse_y >= sizeboxes[i].box_y1) && (mouse_y <= sizeboxes[i].box_y2)) {
                if(!sizeClicked) {
                    input = "";
                }
                sizeClicked = true;
                cout << "Size clicked TRUE" << "\n";
                return i;
            }
            if((mouse_x >= angleboxes[i].box_x1) && (mouse_x <= angleboxes[i].box_x2) && (mouse_y >= angleboxes[i].box_y1) && (mouse_y <= angleboxes[i].box_y2)) {
                if(!angleClicked) {
                    input = "";
                }
                sizeClicked = false;
                angleClicked = true;
                //cout << "Angle clicked TRUE" << "\n";
                return i;
            }
            if((mouse_x >= massboxes[i].box_x1) && (mouse_x <= massboxes[i].box_x2) && (mouse_y >= massboxes[i].box_y1) && (mouse_y <= massboxes[i].box_y2)) {
                if(!massClicked) {
                    input = "";
                }
                sizeClicked = false;
                angleClicked = false;
                massClicked = true;
                //cout << "Mass clicked TRUE" << "\n";
                return i;
            }
            if((mouse_x >= veloboxes[i].box_x1) && (mouse_x <= veloboxes[i].box_x2) && (mouse_y >= veloboxes[i].box_y1) && (mouse_y <= veloboxes[i].box_y2)) {
                if(!veloClicked) {
                    input = "";
                }
                sizeClicked = false;
                angleClicked = false;
                massClicked = false;
                veloClicked = true;
                //cout << "Velo clicked TRUE" << "\n";
                return i;
            }
        }
        cout << "Size clicked FALSE" << "\n";
        sizeClicked = false;
        angleClicked = false;
        massClicked = false;
        veloClicked = false;
        return 8;
        }
    return 8;
}

int main()
{
    bool pause = true;
    bool spaceWasPressed = false;
    bool menu = true;
    bool escapeWasPressed = false;
    bool isSelected = false;
    bool sizeClicked = false;
    bool angleClicked = false;
    bool massClicked = false;
    bool veloClicked = false;
    vector<Ball> balls;
    balls.emplace_back(90, 100, 100, 5, M_PI/2, 9);
    balls.emplace_back(30, 500, 500, 3, M_PI/3, 1);
    balls.emplace_back(30, 800, 300, 3, M_PI/4, 1);
    balls.emplace_back(30, 600, 300, 10, M_PI/4, 1);
    //balls.emplace_back(30, 300, 300, 3, M_PI/4, 1);
    //balls.emplace_back(30, 1000, 300, 3, M_PI/4, 1);
    //balls.emplace_back(30, 800, 600, 3, M_PI/4, 1);
    //balls.emplace_back(30, 1000, 500, 3, M_PI/4, 1);
    //balls.emplace_back(30, 500, 700, 3, M_PI/4, 1);
    //balls.emplace_back(30, 100, 600, 3, M_PI/4, 1);
    //balls.emplace_back(30, 300, 300, 3, M_PI/4, 1);
    //balls.emplace_back(30, 1100, 600, 3, M_PI/4, 1);

    
    

    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Physics Collisions", sf::Style::Titlebar);
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    sf::Font font("C:\\Users\\nicho\\Desktop\\VSCode Projects\\SFML\\SFML-3.0.0\\bin\\Fonts\\roboto.ttf");

    sf::Text menutext(font);
    Texts(menutext, "Ball Collisions", 80, sf::Color::White, {410,80});

    sf::Text addball(font);
    Texts(addball, "Click to add ball", 30, sf::Color::Black, {60, 180});

    sf::String myString;
    sf::Text inputText(font);
    Texts(inputText, myString, 30, sf::Color::White, {420, 180});

    clickBoxes clickbox({50, 180}, {236, 40}, sf::Color::White, false);
    clickBoxes sizeboxes[7];
    clickBoxes angleboxes[7];
    clickBoxes massboxes[7];
    clickBoxes veloboxes[7];
    for(int i = 0; i < 7; i++) {
        float iFloat = static_cast<float>(i);
        sizeboxes[i] = clickBoxes({350, 250 + iFloat*60}, {150, 35}, sf::Color::Transparent, true);
        angleboxes[i] = clickBoxes({550, 250 + iFloat*60}, {150, 35}, sf::Color::Transparent, true);
        massboxes[i] = clickBoxes({750, 250 + iFloat*60}, {150, 35}, sf::Color::Transparent, true);
        veloboxes[i] = clickBoxes({950, 250 + iFloat*60}, {150, 35}, sf::Color::Transparent, true);
    }

    float currentClickedBox_x;
    float currentClickedBox_y;

    float mouse_x;
    float mouse_y;

    int textBoxNumber = 0;

    std::string input;
    std::vector<int> ballSize;
    std::vector<int> ballAngle;
    std::vector<int> ballMass;
    std::vector<int> ballInitialVelocity;
    std::vector<sf::Vector2f> ballInitialPosition;


    while (window.isOpen())
    {
        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if(menu) {
                int index;
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    inputText.setString(input + "_");
                    index = clickCheck(textBoxNumber, clickbox, sizeboxes, angleboxes, massboxes, veloboxes, mouse_x, mouse_y, sizeClicked, angleClicked, massClicked, veloClicked, input); 
                }
                inputText.setPosition({currentClickedBox_x, currentClickedBox_y});
                if(sizeClicked) {
                    currentClickedBox_x = sizeboxes[index].box_x1;
                    currentClickedBox_y = sizeboxes[index].box_y1;      
                    if(event->is<sf::Event::MouseButtonReleased>()) {
                        isSelected = true;
                    }
                }
                else if(angleClicked) {
                    currentClickedBox_x = angleboxes[index].box_x1;
                    currentClickedBox_y = angleboxes[index].box_y1;      
                    if(event->is<sf::Event::MouseButtonReleased>()) {
                        isSelected = true;
                    }
                }
                else if(massClicked) {
                    currentClickedBox_x = massboxes[index].box_x1;
                    currentClickedBox_y = massboxes[index].box_y1;      
                    if(event->is<sf::Event::MouseButtonReleased>()) {
                        isSelected = true;
                    }
                }
                else if(veloClicked) {
                    currentClickedBox_x = veloboxes[index].box_x1;
                    currentClickedBox_y = veloboxes[index].box_y1;      
                    if(event->is<sf::Event::MouseButtonReleased>()) {
                        isSelected = true;
                    }
                }
                else if((mouse_x >= clickbox.box_x1) && (mouse_x <= clickbox.box_x2) && (mouse_y >= clickbox.box_y1) && (mouse_y <= clickbox.box_y2)) {
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        clickbox.box.setFillColor(sf::Color::Black);
                        isSelected = false;

                    }
                    else {
                        clickbox.box.setFillColor(sf::Color::White);
                    }
                    if(event->is<sf::Event::MouseButtonReleased>()) {
                        textBoxNumber ++;
                        if (textBoxNumber == 8) {
                            textBoxNumber = 0;
                        }
                        //cout << textBoxNumber << "\n";
                    }
                }
                else {
                    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                        isSelected = false;
                    }
                }

                if(event->is<sf::Event::TextEntered>()) {
                    const auto* textEntered = event->getIf<sf::Event::TextEntered>();
                    if(textEntered->unicode < 128) {
                        if(isSelected && (sizeClicked || angleClicked || massClicked || veloClicked)) {    
                            if(textEntered->unicode == 8 && input.size() != 0) {
                                input.pop_back();
                                inputText.setString(input + "_");
                            }
                            if (textEntered->unicode >= 48 && textEntered->unicode <= 57) {
                                myString = textEntered->unicode;
                                std::string testString = myString;
                                int intString = std::stoi(testString);
                                userInput(intString, 6, true, input);
                                inputText.setString(input + "_");
                            }
                            if(sizeClicked) {
                                ballSize[index] = std::stoi(input);
                            }
                            if(angleClicked) {
                                ballAngle[index] = std::stoi(input);
                            }
                            if(massClicked) {
                                ballMass[index] = std::stoi(input);
                            }
                            if(veloClicked) {
                                ballInitialVelocity[index] = std::stoi(input);
                            }    
                        }
                    }
                }
            }
        }

        window.clear();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape))
        {
            if (!escapeWasPressed) 
            {
                menu = !menu;   
                escapeWasPressed = true;
            }
        }
        else
        {
            escapeWasPressed = false;
        }

        if(menu) {
            window.draw(menutext);
            window.draw(clickbox.box);
            window.draw(addball);
            if(isSelected) {
                window.draw(inputText);
            }
            if(textBoxNumber != 0) {
                for(int i = 0; i < textBoxNumber; i++) {
                    window.draw(TextBoxes(font, i+1, i+1));
                    window.draw(sizeboxes[i].box);
                    window.draw(angleboxes[i].box);
                    window.draw(massboxes[i].box);
                    window.draw(veloboxes[i].box);
                }
            }
        }

        else {
            for(int j = 0; j < balls.size(); j++) {
                if(!pause) { 
                    balls[j].BallMovement();
                    balls[j].WallCollision();
                }
                for(int i = 0; i < balls.size(); i++) {
                    if(i != j) {
                        if(balls[j].CheckCollision(balls[i])) {
                            balls[j].ContactAngle(balls[i]);
                            balls[j].CollisionAdjust(balls[i]);
                        }
                    }
                }
                window.draw(balls[j].shape);
            }
        }

        window.display();

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Space))
        {
            if (!spaceWasPressed) 
            {
                pause = !pause;   
                spaceWasPressed = true;
            }
        }
        else
        {
            spaceWasPressed = false;
        }

    }
}