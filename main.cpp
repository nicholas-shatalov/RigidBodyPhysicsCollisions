#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <SFML/System.hpp>
#include <iostream>
#include <cmath>
#include <ctime>
#include <random>
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

        Ball() {}

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

class Texts {
    public:
        sf::Font font;
        sf::Text text;
        std::string name;
        sf::Color color;
        sf::Vector2f position;
        int fontsize;
        int ballNumber;

        Texts() : text(font) {};

        Texts(sf::Font& mainFont, std::string name, int fontsize, sf::Color color, sf::Vector2f position) : text(font) {
            this->font = mainFont;
            this->name = name;
            this->color = color;
            this->position = position;
            this->fontsize = fontsize;
            text.setFont(mainFont); 
            text.setString(name);
            text.setCharacterSize(fontsize);
            text.setFillColor(color);
            text.setPosition(position);
        }

        Texts(sf::Font& mainFont, int fontsize, sf::Color color, sf::Vector2f position, int ballNumber) : text(font) {
            this->font = mainFont;
            this->fontsize = fontsize;
            this->color = color;
            this->position = position;
            this->ballNumber = ballNumber;
            text.setFont(mainFont);
            text.setCharacterSize(fontsize);
            text.setFillColor(color);
            std::string numberString = std::to_string(ballNumber);
            name = "Ball " + numberString;
            text.setString(name);
            float ballNumberFloat = static_cast<float>(ballNumber);
            position = {140, 190 + (ballNumberFloat*60)};
            text.setPosition(position);
        }
};

void addBallButton(bool mouseButtonReleased, clickBoxes &clickbox, float mouse_x, float mouse_y, bool &isSelected, int &textBoxNumber) {
    if((mouse_x >= clickbox.box_x1) && (mouse_x <= clickbox.box_x2) && (mouse_y >= clickbox.box_y1) && (mouse_y <= clickbox.box_y2)) {
        if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            clickbox.box.setFillColor(sf::Color::Black);
            isSelected = false;
        }
        if(mouseButtonReleased) {
            clickbox.box.setFillColor(sf::Color::White);
            textBoxNumber ++;
            if (textBoxNumber == 8) {
                textBoxNumber = 0;
            }
        }
    }
}

int textBoxCheck(bool& isSelected, int textBoxNumber, clickBoxes inputBoxes[], float mouse_x, float mouse_y, bool &sizeClicked,  bool &angleClicked,  bool &massClicked,  bool &veloClicked) {
        //cout << "yuh click" << "\n";
        sizeClicked = false;
        angleClicked = false;
        massClicked = false;
        veloClicked = false;
        isSelected = false;
        
        for(int i = 0; i < textBoxNumber; i++) {
            if((mouse_x >= inputBoxes[i].box_x1) && (mouse_x <= inputBoxes[i].box_x2) && (mouse_y >= inputBoxes[i].box_y1) && (mouse_y <= inputBoxes[i].box_y2)) {
                sizeClicked = true;
                isSelected = true;
                return i;
            }
            if((mouse_x >= inputBoxes[i+7].box_x1) && (mouse_x <= inputBoxes[i+7].box_x2) && (mouse_y >= inputBoxes[i+7].box_y1) && (mouse_y <= inputBoxes[i+7].box_y2)) {
                angleClicked = true;
                isSelected = true;
                return i+7;
            }
            if((mouse_x >= inputBoxes[i+14].box_x1) && (mouse_x <= inputBoxes[i+14].box_x2) && (mouse_y >= inputBoxes[i+14].box_y1) && (mouse_y <= inputBoxes[i+14].box_y2)) {
                massClicked = true;
                isSelected = true;
                return i+14;
            }
            if((mouse_x >= inputBoxes[i+21].box_x1) && (mouse_x <= inputBoxes[i+21].box_x2) && (mouse_y >= inputBoxes[i+21].box_y1) && (mouse_y <= inputBoxes[i+21].box_y2)) {
                veloClicked = true;
                isSelected = true;
                return i+21;
            }
        }
    return 29;
}

void textMove(sf::Text& inputText, std::string &input, clickBoxes inputBoxes[], int i, Texts textSave[]) {
    if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
        float currentClickedBox_x = inputBoxes[i].box_x1;
        float currentClickedBox_y = inputBoxes[i].box_y1; 
        int previousindex;
        if(i != previousindex && i != 29) {
            input = textSave[i].text.getString();
        }
        previousindex = i;
        inputText.setString(input + "_");
        inputText.setPosition({currentClickedBox_x, currentClickedBox_y});
        //cout << "yuh text" << "\n";
    }
    return;
}

void textStay(Texts textSave[], std::string input, clickBoxes inputBoxes[], int i) {
    textSave[i].text.setString(input);
    textSave[i].text.setPosition({inputBoxes[i].box_x1, inputBoxes[i].box_y1});
}

void userInput(int intString, int limit, bool isSelected, std::string& input) {
    int stringsize = input.size();
    if(isSelected && stringsize < limit) {
        input.append(std::to_string(intString));
        return;
    }
    else {
        return;
    }
}

void keypressedToString(const sf::Event::TextEntered& textEntered, bool& isSelected, sf::Text& inputText, sf::String& unicodeConvertString, std::string& input) {
    if(textEntered.unicode < 128) {
        if(isSelected) {    
            if(textEntered.unicode == 8 && input.size() != 0) {
                input.pop_back();
                inputText.setString(input + "_");
            }
            if (textEntered.unicode >= 48 && textEntered.unicode <= 57) {
                unicodeConvertString = textEntered.unicode;
                std::string nonIntString = unicodeConvertString;
                int intString = std::stoi(nonIntString);
                userInput(intString, 3, true, input);
                inputText.setString(input + "_");
            }

        }
    }
    return;
}


void saveInputs(std::vector<int>& ballSize, std::vector<float>& ballAngle, std::vector<int>& ballMass, std::vector<int>& ballInitialVelocity, bool isSelected, int index, std::string input) {
    cout << "isSelected " << isSelected << "\n";
    cout << "Index " << index << "\n";
    if(input.size() == 0) {
        input = "0";
    }
    if(isSelected && (index < 7)) {
        ballSize[index] = std::stoi(input);
        cout << ballSize[index] << "\n";
    }
    if(isSelected && (index >= 7) && (index < 14)) {
        ballAngle[index-7] = (std::stoi(input))*(M_PI/180);
        cout << ballAngle[index-7] << "\n";
    }
    if(isSelected && (index >= 14) && (index < 21)) {
        ballMass[index-14] = std::stoi(input);
        cout << ballMass[index-14] << "\n";
    }
    if(isSelected && (index >= 21) && (index < 28)) {
        ballInitialVelocity[index-21] = std::stoi(input);
        cout << ballInitialVelocity[index-21] << "\n";
    }
    return;
}

void menuSwitch(bool& menu, bool& escapeWasPressed, bool& generate) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Escape)) {
        if (!escapeWasPressed) 
        {
            menu = !menu;   
            generate = true;
            escapeWasPressed = true;
        }
    }
    else {
        escapeWasPressed = false;
    }
}

void pauseSwitch(bool& spaceWasPressed, bool& pause) {
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

bool randomPositionChecker(float x_rand1, float x_rand2, float y_rand1, float y_rand2, int ballSize1, int ballSize2) {
    float x_temp;
    float y_temp;
    float randdistance;
    x_temp = x_rand1 - x_rand2;
    y_temp = y_rand1 - y_rand2;
    randdistance = sqrt(pow(x_temp, 2) + pow(y_temp, 2));
    return randdistance < (ballSize1 + ballSize2);
}

void randomPositionGenerator(std::vector<float>& initialPositionX, std::vector<float>& initialPositionY, std::vector<int> ballSize) {
    bool validPosition;
    int attemps = 0;
    float x_rand_array[7];
    float y_rand_array[7];
    std::mt19937 rng(static_cast<unsigned>(std::time(nullptr)));
    for(int i = 0; i < 7; i++) {
        std::uniform_real_distribution<float> x_rand(1 + ballSize[i], 1279 - ballSize[i]);
        std::uniform_real_distribution<float> y_rand(1 + ballSize[i], 719 - ballSize[i]);
        x_rand_array[i] = x_rand(rng);
        y_rand_array[i] = y_rand(rng);
        cout << x_rand_array[i] << "\n";
        cout << y_rand_array[i] << "\n";
        //cout << "positions succcesfully generated" << "\n";
    }
    for(int i = 0; i < 7; i++) {
        for(int j = 0; j < 7; j++) {
            if(i == j) {continue;}
            validPosition = randomPositionChecker(x_rand_array[i], x_rand_array[j], y_rand_array[i], y_rand_array[j], ballSize[i], ballSize[j]);
            cout << "Position check " << validPosition << "\n";
            while(validPosition && attemps < 11) {
                std::uniform_real_distribution<float> x_rand(1 + ballSize[j], 1279 - ballSize[j]);
                std::uniform_real_distribution<float> y_rand(1 + ballSize[j], 719 - ballSize[j]);
                x_rand_array[j] = x_rand(rng);
                y_rand_array[j] = y_rand(rng);
                /*cout << "index i " << i << "\n";
                cout << "index j " << j << "\n";
                cout << x_rand_array[j] << "\n";
                cout << y_rand_array[j] << "\n";*/
                cout << "positions succcesfully regenerated" << "\n";
                cout << "Position check 2 " << validPosition << "\n";
                validPosition = randomPositionChecker(x_rand_array[i], x_rand_array[j], y_rand_array[i], y_rand_array[j], ballSize[i], ballSize[j]);
                attemps++;
            }
            //cout << "positions succcesfully checked" << "\n";
            attemps = 0;
        }
    }
    for(int i = 0; i < 7; i++) {
        cout << "positions succcesfully updated" << "\n";
        initialPositionX[i] = x_rand_array[i];
        initialPositionY[i] = y_rand_array[i];
        cout << initialPositionX[i] << "\n";
        cout << initialPositionY[i] << "\n";
    }
}

int main()
{
    //cout << "yuh start" << "\n";
    sf::RenderWindow window(sf::VideoMode({1280, 720}), "Physics Collisions", sf::Style::Titlebar);
    window.setFramerateLimit(120);
    window.setKeyRepeatEnabled(false);

    sf::Font mainFont;
    if(!mainFont.openFromFile("C:\\Users\\nicho\\Desktop\\VSCode Projects\\SFML\\SFML-3.0.0\\bin\\Fonts\\roboto.ttf")) {
        throw std::runtime_error("Failed to load font");
    }

    vector<Ball> balls(7);
    //balls.emplace_back(90, 100, 100, 5, M_PI/2, 9);
    //balls.emplace_back(30, 500, 500, 5, M_PI/4, 3);


    Texts menutext(mainFont, "Ball Collisions", 80, sf::Color::White, {410,80});
    Texts addball(mainFont, "Click to add ball", 30, sf::Color::Black, {60, 180});
    sf::String unicodeConvertString;
    Texts inputText(mainFont, unicodeConvertString, 30, sf::Color::White, {410,80});
    Texts textSave[28];
    for(int i = 0; i < 28; i++) {
        textSave[i] = Texts(mainFont, "", 30, sf::Color::White, {0,0});
    }
    Texts inputLabelSize(mainFont, "Size", 20, sf::Color::White, {350, 215});
    Texts inputLabelAngle(mainFont, "Angle", 20, sf::Color::White, {550, 215});
    Texts inputLabelMass(mainFont, "Mass", 20, sf::Color::White, {750, 215});
    Texts inputLabelVelo(mainFont, "Initial Velocity", 20, sf::Color::White, {950, 215});
    Texts ballTextLabels[7];
    for(int i = 0; i < 7; i++) {
        ballTextLabels[i] = Texts(mainFont, 30, sf::Color::White, {140, 190}, i+1);
    }

    //cout << "yuh ckeckpoint 1" << "\n";
    clickBoxes clickbox({50, 180}, {236, 40}, sf::Color::White, false);

    //sizeboxes are indexed 0-6
    //angleboxes are indexed 7-13
    //massboxes are indexed 14-20
    //veloboxes are indexed 21-27
    clickBoxes inputBoxes[28];
    for(int i = 0; i < 28; i++) {
        float iFloat;
        iFloat = static_cast<float>(i);
        if(i >= 0 && i < 7) {
            inputBoxes[i] = clickBoxes({350, 250 + iFloat*60}, {150, 35}, sf::Color::Transparent, true);
        }
        if(i >= 7 && i < 14) {
            inputBoxes[i] = clickBoxes({550, 250 + (iFloat-7)*60}, {150, 35}, sf::Color::Transparent, true);
        }
        if(i >= 14 && i < 21) {
            inputBoxes[i] = clickBoxes({750, 250 + (iFloat-14)*60}, {150, 35}, sf::Color::Transparent, true);
        }
        if(i >= 21 && i < 28) {
            inputBoxes[i] = clickBoxes({950, 250 + (iFloat-21)*60}, {150, 35}, sf::Color::Transparent, true);
        }
    }

    bool pause = true;
    bool spaceWasPressed = false;
    bool menu = true;
    bool escapeWasPressed = false;
    bool generate = false;
    bool isSelected = false;
    bool sizeClicked = false;
    bool angleClicked = false;
    bool massClicked = false;
    bool veloClicked = false;
    
    float currentClickedBox_x;
    float currentClickedBox_y;

    float mouse_x;
    float mouse_y;

    int textBoxNumber = 0;
    int index; 

    std::string input;
    std::vector<int> ballSize(7);
    std::vector<float> ballAngle(7);
    std::vector<int> ballMass(7);
    std::vector<int> ballInitialVelocity(7);
    std::vector<float> ballInitialPositionX(7);
    std::vector<float> ballInitialPositionY(7);


    while (window.isOpen())
    {
        mouse_x = sf::Mouse::getPosition(window).x;
        mouse_y = sf::Mouse::getPosition(window).y;
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if(menu) {
                if(sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
                    index = textBoxCheck(isSelected, textBoxNumber, inputBoxes, mouse_x, mouse_y, sizeClicked, angleClicked, massClicked, veloClicked);
                }
                bool mouseButtonReleased = event->is<sf::Event::MouseButtonReleased>();
                addBallButton(mouseButtonReleased, clickbox, mouse_x, mouse_y, isSelected, textBoxNumber);
                textMove(inputText.text, input, inputBoxes, index, textSave);
                if(auto textEntered = event->getIf<sf::Event::TextEntered>()) {
                    keypressedToString(*textEntered, isSelected, inputText.text, unicodeConvertString, input);
                    saveInputs(ballSize, ballAngle, ballMass, ballInitialVelocity, isSelected, index, input);
                    textStay(textSave, input, inputBoxes, index);
                }
            }
        }

        window.clear();
        menuSwitch(menu, escapeWasPressed, generate);
        pauseSwitch(spaceWasPressed, pause);

        if(menu) {
            window.draw(menutext.text);
            window.draw(clickbox.box);
            window.draw(addball.text);
            if(isSelected) {
                window.draw(inputText.text);
            }
            if(textBoxNumber != 0) {
                window.draw(inputLabelSize.text);
                window.draw(inputLabelAngle.text);
                window.draw(inputLabelMass.text);
                window.draw(inputLabelVelo.text);
                for(int i = 0; i < textBoxNumber; i++) {
                    window.draw(ballTextLabels[i].text);
                    window.draw(inputBoxes[i].box);
                    window.draw(inputBoxes[i+7].box);
                    window.draw(inputBoxes[i+14].box);
                    window.draw(inputBoxes[i+21].box);
                    if(index != i) {
                        window.draw(textSave[i].text);
                    }
                    if(index != i+7) {
                        window.draw(textSave[i+7].text);
                    }
                    if(index != i+14) {
                        window.draw(textSave[i+14].text);
                    }
                    if(index != i+21) {
                        window.draw(textSave[i+21].text);
                    }
                }
            }
        }

        else {
            if(generate) {
                randomPositionGenerator(ballInitialPositionX, ballInitialPositionY, ballSize);
                for(int i = 0; i < textBoxNumber; i++) {
                    balls[i] = Ball(ballSize[i], ballInitialPositionX[i], ballInitialPositionY[i], ballInitialVelocity[i], ballAngle[i], ballMass[i]);
                    generate = false;
                }
            }
            
            
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

    }
}