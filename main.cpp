/***********************************************************************************
 * Project: ZeroCool_RayCaster                                                     *
 * Description: Implementation of the DDA (Digital Differential Analyzer)          *
 *              algorithm for Raycasting in 2D space.                              *
 * Author: Mohammed Fawaz (aka ZeroCool)                                           *
 *                                                                                 *
 * GitHub:    https://github.com/zerocool-m12                                      *
 * Email:     zerocool_m12@hotmail.com                                             *
 * Instagram: @zerocool_m12                                                        *
 * X (Twitter): @zerocool_m12                                                      *
 *                                                                                 *
 * License: This project is for educational purposes. It may be used, modified,    *     
 *          and redistributed freely without any legal constraints.                *
 *          Attribution is appreciated but not mandatory. No warranties provided.  *
 *                                                                                 *
 * The DDA algorithm, often used for line drawing in computer graphics, is         *
 * adapted here for raycasting, a core technique in early 2D games like            *
 * Wolfenstein 3D.                                                                 *
 ***********************************************************************************/
 


#include<SFML/Graphics.hpp>
#include<cmath>
#include<vector>
#include<iostream>
#define MAX_CELL 128  // maximum length of the ray 
#define SCREEN_W 1500
#define SCREEN_H 1000
constexpr float MOV_SPEED = 8.f;
constexpr float TUR_SPEED = 150.f;
constexpr float PI = 3.14;
class Map{
  private:
    float cellsize;
    std::vector<std::vector<int>> grid;
    
  public:
    Map(const float & x, const std::vector<std::vector<int>> & y) : cellsize(x), grid(y) {}
    void draw(sf::RenderWindow & window){
      sf::RectangleShape cell (sf::Vector2f (cellsize * 0.95, cellsize * 0.95));  //setting up the cell size
      for(int i = 0; i < grid.size(); i++){
        for(int j = 0; j < grid[i].size(); j++){
          if(grid[i][j] == 0) cell.setFillColor(sf::Color(0,255,0)); // if the cell is road then it is green, if it is wall then it is gray
          else cell.setFillColor (sf::Color(70,70,70));
          cell.setPosition(sf::Vector2f (j,i) * cellsize + sf::Vector2f(cellsize * 0.025, cellsize * 0.025));
          window.draw(cell);
        }
      }
    }
    const float getSize() const {return cellsize;}
    const auto & getMap() const {return grid;}
    
    void toggleCell(const sf::Vector2i& mousePos) {  // a function to create walls and roads on the map
      int gridX = mousePos.x / cellsize;
      int gridY = mousePos.y / cellsize;
    
      if (gridX > 0 and gridX < grid[0].size() - 1 and 
          gridY > 0 and gridY < grid.size() - 1) {
          grid[gridY][gridX] = 1 - grid[gridY][gridX];  // if the position is wall then it becomes road and vice versa
      }
    }
};
class Player{
  public:
    sf::Vector2f position;
    float angle;
    sf::Vector2f direction;
    void draw(sf::RenderWindow & window, const Map & map){
      sf::CircleShape player (5.f);
      player.setFillColor(sf::Color(0,0,255));
      player.setPosition(position * map.getSize());
      player.setOrigin(sf::Vector2f (player.getRadius(), player.getRadius()));
      window.draw(player);
    }
    void update(const float & deltaTime , const Map & map){  // handling the events of the player
      sf::Vector2i next_pos, back_pos;
      double radian = angle * PI / 180.;
      direction.x = cos(radian);
      direction.y = sin(radian);
      next_pos.x = static_cast<int>(position.x + direction.x * deltaTime * MOV_SPEED);  // calculating the next cell position according to the direction of the player
      next_pos.y = static_cast<int>(position.y + direction.y * deltaTime * MOV_SPEED);
      back_pos.x = static_cast<int>(position.x - direction.x * deltaTime * MOV_SPEED);  // calculating the back cell position
      back_pos.y = static_cast<int>(position.y - direction.y * deltaTime * MOV_SPEED);
      if(next_pos.x >= 0 and next_pos.x < map.getMap()[0].size() and next_pos.y >= 0 and next_pos.y < map.getMap().size()){  // walls collision detection for forward movement
        if(map.getMap()[next_pos.y][next_pos.x] == 0){
          if(sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){  // moving forward using Up arrow
            position.x += direction.x * MOV_SPEED * deltaTime; 
            position.y += direction.y * MOV_SPEED * deltaTime;
          }
         }
        }
      if(back_pos.x >= 0 and back_pos.x < map.getMap()[0].size() and back_pos.y >= 0 and back_pos.y < map.getMap().size() and map.getMap()[back_pos.y][back_pos.x] == 0){ // walls collision detection for backward movement
          if(sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){ // moving backward using Down arrow
            position.x -= direction.x * MOV_SPEED * deltaTime;
            position.y -= direction.y * MOV_SPEED * deltaTime;
           }
          }
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Left))  // changing the direction using Left arrow
        angle -= TUR_SPEED * deltaTime;
      if(sf::Keyboard::isKeyPressed(sf::Keyboard::Right))  // changing the direction of the player to the right
        angle += TUR_SPEED * deltaTime;
    }
};

class ray {  // the ray object
  public:
    sf::Vector2f hitPosition;
    float distance;
    bool hit;
    bool isVertical;
};

ray cast_ray(sf::Vector2f & start, const sf::Vector2f & dir, const Map & map){  // a function for calculating the casted ray

  sf::Vector2f deltaDist, sideDist;
  sf::Vector2i step, mapPos {start};
  deltaDist.x = std::abs(1.f / dir.x);
  deltaDist.y = std::abs(1.f / dir.y);
  bool hit = false;
  bool isVertical = false;
  if(dir.x < 0.0){
    step.x = -1;
    sideDist.x = (-mapPos.x + start.x) * deltaDist.x;
  }else{
    step.x = 1;
    sideDist.x = (mapPos.x - start.x + 1.f) * deltaDist.x;
  }
  if(dir.y < 0.0){
    step.y = -1;
    sideDist.y = (-mapPos.y + start.y) * deltaDist.y;
  }else{
    step.y = 1;
    sideDist.y = (mapPos.y - start.y + 1.f) * deltaDist.y;
  }
  for ( int i = 0; not hit and i < MAX_CELL; i++){
    if(sideDist.x < sideDist.y){
      sideDist.x += deltaDist.x;
      mapPos.x += step.x;
      isVertical = false;
    }else{
      sideDist.y += deltaDist.y;
      mapPos.y += step.y;
      isVertical = true;
    }
    if(mapPos.x >= 0 and mapPos.x < map.getMap()[0].size() and mapPos.y >= 0 and mapPos.y < map.getMap().size())  // collision of the ray with the walls
      hit = map.getMap()[mapPos.y][mapPos.x];
  } 
  float distance = isVertical? sideDist.y - deltaDist.y : sideDist.x - deltaDist.x;
  return ray {start + distance * dir, distance, hit, isVertical};
}

class render{
  public:
  void draw (sf::RenderWindow & window, const Player & player, const Map & map){  // implementing the DDA algorithm on the map
    sf::Vector2f rayDir, rayPos {player.position}, direction, plane;
    float aspect = SCREEN_W / static_cast<float>(SCREEN_H);
    double radian = player.angle * PI / 180.;
    direction.x = cos(radian);
    direction.y = sin(radian);
    plane.x = -direction.y * aspect * 0.5f;
    plane.y = direction.x * aspect * 0.5f;
    sf::VertexArray walls {sf::Lines};
    for(int i = 0; i < SCREEN_W; i++){
      float cameraX = i * 2.f / static_cast<float>(SCREEN_W) - 1.f;
      rayDir.x = direction.x + plane.x * cameraX;
      rayDir.y = direction.y + plane.y * cameraX;
      ray ray = cast_ray(rayPos,rayDir,map);
      walls.append ( sf::Vertex ( player.position * map.getSize()));
      walls.append ( sf::Vertex ( sf::Vector2f (ray.hitPosition * map.getSize())));
    }
    window.draw(walls);
  }
  
  void draw_3d ( sf::RenderWindow & window, const Player & player, const Map & map){  // Redndering for 3d view
    sf::Vector2f direction, rayDir, rayPos {player.position},plane;
    double radian = player.angle * PI / 180.;
    direction.x = cos(radian);
    direction.y = sin(radian);
    float aspect = SCREEN_W / static_cast<float>(SCREEN_H);
    plane.x = -direction.y * aspect * 0.5f;
    plane.y = direction.x * aspect * 0.5f;
    sf::RectangleShape sky (sf::Vector2f (SCREEN_W,SCREEN_H / 2.f));
    sky.setFillColor(sf::Color(150,210,235));
    window.draw(sky);
    sf::RectangleShape floor (sf::Vector2f ( SCREEN_W, SCREEN_H / 2.f));  // drawing the floor
    floor.setPosition(sf::Vector2f ( 0.0f, SCREEN_H / 2.f));
    floor.setFillColor(sf::Color(70,70,70));
    window.draw(floor);
    sf::VertexArray walls {sf::Lines};
      
    for(int i = 0; i < SCREEN_W; i ++){
    
      float cameraX = i * 2.f / static_cast<float>(SCREEN_W) - 1.f;
      rayDir.x = direction.x + plane.x * cameraX;
      rayDir.y = direction.y + plane.y * cameraX;
      ray ray = cast_ray (rayPos, rayDir, map);
      if(ray.hit){
        float wallheight = SCREEN_H / ray.distance;
        float wallstart = (-wallheight + static_cast<float>(SCREEN_H)) / 2.f;
        float wallend = (wallheight + static_cast<float>(SCREEN_H)) / 2.f;
        float brightness = 1.f - (ray.distance / static_cast<float>(MAX_CELL));
        if(ray.isVertical) brightness *= 0.7f;  // handling the color difference on the vertical walls
        sf::Color color(brightness * 190, brightness * 100, brightness * 80);
        walls.append ( sf::Vertex ( sf::Vector2f ( static_cast<float>(i),wallstart),color));
        walls.append ( sf::Vertex ( sf::Vector2f ( static_cast<float>(i), wallend), color));
        
      }
    }
    window.draw(walls);
  }
};
int main(){
  std::vector<std::vector<int>> grid = {  // the map grid
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 1, 1, 0, 1, 1, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
  };
  sf::RenderWindow window (sf::VideoMode(SCREEN_W,SCREEN_H),"ZeroCool - Raycasting",sf::Style::Close);
  Map map(10.f,grid); // creating the map object with 10.f cell size  
  Player player;
  player.position = sf::Vector2f (8,10);
  render render;
  sf::Clock g_time;
  while(window.isOpen()){
    float deltaTime = g_time.restart().asSeconds();  // calculating the delta time
    sf::Event event;
    while(window.pollEvent(event))
      if(event.type == sf::Event::Closed) window.close();  // handling the close button event
      else if(event.type == sf::Event::MouseButtonPressed) {  // handling mouse left click event to add / remove walls on the map
                if(event.mouseButton.button == sf::Mouse::Left) {
                    sf::Vector2i mousePos = sf::Mouse::getPosition(window);
                    map.toggleCell(mousePos);
				}
	  }
    window.clear();
    player.update(deltaTime,map);  // update the movement of the player
    render.draw_3d(window,player,map);  // the 3D rendering the map
    map.draw(window); // drawing the map
    player.draw(window,map);  // drawing the player
    render.draw(window,player,map);  // rendering the map
    window.display();
    }
  return EXIT_SUCCESS;
  }
