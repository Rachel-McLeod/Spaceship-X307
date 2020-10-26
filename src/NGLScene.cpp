#include <QMouseEvent>
#include <QGuiApplication>

#include "NGLScene.h"
#include "Actor.h"
#include "Player.h"
#include "Block.h"
#include <ngl/ShaderLib.h>
#include <ngl/NGLStream.h>
#include <ngl/NGLInit.h>
#include <ngl/Util.h>
#include <ngl/VAOPrimitives.h>
#include <math.h>
#include <iostream>



NGLScene::NGLScene()
{
  // re-size the widget to that of the parent (in this case the GLFrame passed in on construction)
  setTitle("First NGL");
}


NGLScene::~NGLScene()
{
  std::cout<<"Shutting down NGL, removing VAO's and Shaders\n";
}

void NGLScene::loadMatrixToShader(ngl::Transformation &_tx)
{
    auto shader = ngl::ShaderLib::instance();
    shader->use("ColourShader");
    shader->setUniform("MVP", m_project*m_view*_tx.getMatrix());
}

void NGLScene::resizeGL(int _w , int _h)
{
  m_win.width  = static_cast<int>( _w * devicePixelRatio() );
  m_win.height = static_cast<int>( _h * devicePixelRatio() );
  m_project = ngl::perspective(45.0f, static_cast<float>(_w)/_h, 0.5f, 20.0f);

}


void NGLScene::initializeGL()
{
  // we need to initialise the NGL lib which will load all of the OpenGL functions, this must
  // be done once we have a valid GL context but before we call any GL commands. If we dont do
  // this everything will crash
  ngl::NGLInit::instance();
  glClearColor(0.0f, 0.0f, 0.0f, 1.0f);			   // Grey Background
  // enable depth testing for drawing
  glEnable(GL_DEPTH_TEST);
  // enable multisampling for smoother drawing
  glEnable(GL_MULTISAMPLE);

  ngl::VAOPrimitives::instance()->createLineGrid("bounds", 20, 30, 1);//creating the grid for the level. Done here because it doesn't change at all in the course of the program.
  


  auto shader = ngl::ShaderLib::instance();
  shader->loadShader("ColourShader", "shaders/VertexShader.glsl", "shaders/FragmentShader.glsl");
  shader->loadShader("TextureShader", "shaders/TextureVertexShader.glsl", "shaders/TextureFragmentShader.glsl");
  shader->linkProgramObject("TextureShader");
  
  m_BlockTimer = new QTimer;        //creating the various timers needed for gameplay
  m_BlockTimer->start(rand()%80+40);
  m_HitTimer = new QTimer;
  m_DeathRayTimer = new QTimer;
  m_InvincibilityTimer = new QTimer;


  //create stars
  int i = 0;
  for (i = 0;i < 15;i++)        // makes a list of "stars", the actors that are decorative, randomising their position and colour
  {
      m_StarList.push_back(std::make_unique<Actor>(((rand() % 20) - 10, -1.0, (rand() % 10) - 5, 1.0f)));
      m_StarList[i]->m_position.m_x = (rand() % 20) - 10;
      m_StarList[i]->m_position.m_z = (rand() % 20) - 10;
      m_StarList[i]->m_colour.m_x = ((rand() % 100))/100.0;
      m_StarList[i]->m_width = (rand() % 8 + 4.0) / 300.0;
  }

  ngl::Vec3 eye(-4.0f, 12.0f, 0.0f);        //setting up the "camera"
  m_view = ngl::lookAt(eye, { 0.0,-2.0,0.0, }, ngl::Vec3::up());
  std::cout << m_view << "\n";
  
  m_player = std::make_unique<Player>((-5.0, 0.0, 0.0));        //creates the player
  //m_player->m_position = m_player->m_playerStartPos;
  m_BlockList.push_back(std::make_unique<Block>((0.0f, 1.0f, 1.0f, 1.0f)));
  

  startTimer(10);
}

void NGLScene::timerEvent(QTimerEvent*)  //Tick
{
    update();//call paintGL
}



void NGLScene::paintGL()
{
  // clear the screen and depth buffer
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glViewport(0,0,m_win.width,m_win.height);


  switch (m_MenuType)
  {
  case MenuType::Play:
  {
      auto shader = ngl::ShaderLib::instance();
      shader->use("ColourShader");
      glClearColor(m_ClearColour,0.0,0.0,1.0);

      PlayerPosition();     //Calculates the player's position

      if (m_BlockTimer->remainingTime() == 0 && m_BlockList.size() < 10.0 + Difficulty)     //increases the number of obstacles until there's 10, and then increases as difficulty increases
      {
          m_BlockList.push_back(std::make_unique<Block>((0.0f, 1.0f, 1.0f, 1.0f)));
      }

      shader->setUniform("colour", 0.6f, 0.6f, 0.6f);
      int i;
      for (i = 0;i < m_BlockList.size();i++)        //each obstacle is moved and drawn
      {
          Score = m_BlockList[i]->BlockMove(Score);
          m_BlockList[i]->Draw("MVP", m_project, m_view, "Block", SpeedZ);
      }

      prevHit = hit;                //stores whether this is the beginning of a new collision
      hit = CollisionDetect();


      if (!prevHit && hit)          //if this is the start of a new hit
      {
          m_HitTimer->start(45);        //timer sets invincibility after getting hit
      }

      if (m_HitTimer->remainingTime() <= 1)     //resets invincibility when the timer runs out
      {
          m_player->invincibility = FALSE;
      }

      if (m_InvincibilityTimer->remainingTime() <= 1)
      {
          m_InvincibilityTimerComplete = TRUE;
      }

      if (m_DeathRayTimer->remainingTime() <= 0)
      {
          m_DeathRayTimerComplete = TRUE;
      }

      if (!prevHit && hit && !m_player->invincibility)      //damage is only taken at the beginning of a hit, if the player is not invincible
      {
          m_ClearColour = 0.4;                              //the backgroud colour is set to red to indicate taking damage
          m_player->TakeDamage();
          m_player->invincibility = TRUE;                   // the player becomes invincible for a short time, to allow the player to recover
      }

      if (m_ClearColour > 0.0)                              //the background colour gets closer to the default black after a hit
      {
          m_ClearColour -= 0.01;
      }
      
      if (m_player->m_health <= -1)                         //if the player dies, the game over case is set
      {
          TextureLoaded = FALSE;
          glClearColor(0.0, 0.0, 0.0, 1.0);
          m_MenuType = MenuType::GameOver;
          std::cout << "Game Over! Score : " << Score << "\n";
          break;
      }

      if (m_deathray)                                       // draws the death ray
      {
          ngl::Mat4 PositionMat;
          PositionMat.translate(m_player->m_position.m_x, m_player->m_position.m_y -0.05, m_player->m_position.m_z); //uses the player's position to start at
          PositionMat.rotateY(-90);
          shader->setUniform("colour", m_deathrayColour, m_deathrayColour, m_deathrayColour);
          shader->setUniform("MVP", m_project * m_view * PositionMat);
          ngl::VAOPrimitives::instance()->createCylinder("DeathRay", 0.04, 20, 4, 1);
          ngl::VAOPrimitives::instance()->draw("DeathRay");
          if (m_deathrayColour > 0.5)                       //the death ray's colour is used to indicate when it should stop being drawn, at which point it is set to false
          {
              m_deathrayColour -= 0.02;
          }
          else
          {
              m_deathrayColour = 1.0;
              m_deathray = FALSE;
          }
      }
      
      for (i = 0;i < m_StarList.size();i++)                 //steps through the list of stars and gradually fades brighter and dimmer
      {
          if (m_StarList[i]->m_colour.m_x <= 6.0f || m_StarList[i]->m_colour.m_x >= 0.99f)
          {
              m_StarList[i]->Increasing = !m_StarList[i]->Increasing;
          }
          if (m_StarList[i]->Increasing)
          {
              m_StarList[i]->m_colour.m_x += 0.01;
          }
          else
          {
              m_StarList[i]->m_colour.m_x -= 0.01;
          }
          m_StarList[i]->m_colour.m_y = m_StarList[i]->m_colour.m_x;
          m_StarList[i]->m_colour.m_z = m_StarList[i]->m_colour.m_x;
              
          shader->setUniform("colour", m_StarList[i]->m_colour.m_x, m_StarList[i]->m_colour.m_x, m_StarList[i]->m_colour.m_x);
          DrawStars(i);
      }
      

      DrawPlayerHealth(m_player->m_health);                 //draws the amount of health left on the screen

      ngl::Transformation tx;
      tx.setScale(0.5f, 1.5f, 0.5f);
      loadMatrixToShader(tx);
      shader->setUniform("colour", 0.5f, 0.5f, 0.5f);
      ngl::VAOPrimitives::instance()->draw("bounds");       //draws the edges of where the player can travel
      if (m_player->invincibility)                          //sets the colour of the player based on invincibility to inform the player
      {
          shader->setUniform("colour", 0.5f, 0.5f, 0.5f);
      }
      else
      {
          shader->setUniform("colour", 0.8f, 0.8f, 0.8f);
      }
      m_player->Draw("MVP", m_project, m_view, "Player", SpeedZ);

      Difficulty = Score / 50;                              //the difficulty increases much more slowly than the score, and is used to create extra obstacles
      break;
  }
  case MenuType::Pause:         //pause menu texture is drawn
  {
      DrawPlane("Pause");
  }
  break;
  case MenuType::Title:         //title screen texture is drawn
  {
      DrawPlane("Title");
  }
  break;
  case MenuType::HowTo:         //"how to play" texture is drawn
  {
      DrawPlane("HowTo");
  }
  break;
  case MenuType::GameOver:      //game over menu texture is drawn
  {
      DrawPlane("GameOver");
  }
  break;
  }
}

void NGLScene::DeathRay()                   //calculates the effect of the death ray - what obstacles it destroys
{
    int i;
    if (m_DeathRayTimerComplete)            //this only executes if the death ray has not been fired too recently
    {
        for (i = 0; i < m_BlockList.size(); i++)            //deletes a block from the list if it's above the player
        {
            if (m_BlockList[i]->m_position.m_z + m_BlockList[i]->m_width > m_player->m_position.m_z &&
                m_BlockList[i]->m_position.m_z - m_BlockList[i]->m_width < m_player->m_position.m_z)        //if a block is directly above the player, it is deleted
            {
                m_BlockList.erase(m_BlockList.begin() + i);
                i--;        //check the space that was just deleted in the list
            }
        }
        m_deathray = TRUE;
    }
}

void NGLScene::DrawStars(int i)
{

    m_StarList[i]->Draw("MVP", m_project, m_view, "Star", SpeedZ);
    m_StarList[i]->m_position.m_x = m_StarList[i]->m_position.m_x - (0.08 + Score / 4000.0)/20;		//gets faster as the score increases
        

    if (m_StarList[i]->m_position.m_x < -15)	//resets position in x when the block reaches the bottom of the screen
    {
        m_StarList[i]->m_position.m_x = 7;
        Score++;
        m_StarList[i]->m_position.m_z = ((rand() % 10) - 5);

    }
}

void NGLScene::Reset()      //resets everything for a new game to start
{
    hit = FALSE;
    prevHit = FALSE;
    m_ClearColour = 0.0;
    m_DeathRayTimerComplete = TRUE;
    m_InvincibilityTimerComplete = TRUE;
    m_player->m_health = 3;
    m_player->invincibility = FALSE;
    m_player->m_position = m_player->m_playerStartPos;
    m_BlockList.clear();
    Difficulty = 0;
    Score = 0;
}

bool NGLScene::CollisionDetect()       //collision detection uses rectangles around objects
{
    bool collision = FALSE;
    int i;
    for (i = 0;i < m_BlockList.size(); i++)
    {
        if (((m_player->m_position.m_z ) > (m_BlockList[i]->m_position.m_z - m_BlockList[i]->m_width))
            && ((m_player->m_position.m_z ) < m_BlockList[i]->m_position.m_z + m_BlockList[i]->m_width)
            && ((m_player->m_position.m_x ) > (m_BlockList[i]->m_position.m_x - m_BlockList[i]->m_width))
            && ((m_player->m_position.m_x - m_player->m_height) < (m_BlockList[i]->m_position.m_x + m_BlockList[i]->m_width)))
        {
            collision = TRUE;
            m_BlockList.erase(m_BlockList.begin() + i);         //on collision, the block is removed
            i--;                                                //similarly to the death ray function, i is decremented to recheck the place in the list that was just vacated
        }
    }
    return collision;                                           //returns a boolean of whether a collision took place this tick
}

void NGLScene::DrawPlane(std::string type)                      // draws the textured planes for the menu screens
{
    auto shader = ngl::ShaderLib::instance();

    if (!TextureLoaded)
    {
        TextureLoaded = TRUE;
       // shader->use("TextureShader");                           //changes the active shader
        
        ngl::Texture texture("image/" + type + ".png");         //uses the string passed to the function to find the filename for the appropriate texture
        m_textureName = texture.setTextureGL();
    }

    ngl::Transformation tx;
    tx.setScale(0.5f, 1.5f, 0.5f);
    loadMatrixToShader(tx);

    GLenum m_polyMode;
    m_polyMode = GL_FILL;
    glBindTexture(GL_TEXTURE_2D, m_textureName);
    glPolygonMode(GL_FRONT_AND_BACK, m_polyMode);


    shader->use("TextureShader");                           //changes the active shader
    ngl::VAOPrimitives::instance()->createTrianglePlane("plane", 120, 80, 1, 1, 100);
    ngl::Mat4 m_orthProject = ngl::ortho(-60.0f, 70.0f, 50.0f, -50.0f, 0.1f, 150.0f);       //projects the texture othographically
    ngl::Mat4 transform;
    transform.rotateY(-90);
    shader->setUniform("MVP", m_orthProject * m_view *transform );
    ngl::VAOPrimitives::instance()->draw("plane");
}

void NGLScene::DrawPlayerHealth(int health)                                         //draws the amount of lives the player has left
{
    if (health > 0)                                                                 //uses nested if statements for optimisation
    {
        ngl::Mat4 PositionMat;
        PositionMat.rotateY(90);
        PositionMat.translate(5.0, 0.0, -8.0); //converts the vector to a matrix
        auto shader = ngl::ShaderLib::instance();
        shader->setUniform("colour", 0.8f, 0.8f, 0.8f);
        ngl::VAOPrimitives::instance()->createCone("Actor", 0.1, 0.4, 4, 2);
        shader->setUniform("MVP", m_project * m_view * PositionMat);
        ngl::VAOPrimitives::instance()->draw("Actor");

        if (health > 1)
        {
            PositionMat.translate(5.0, 0.0, -7.5); //converts the vector to a matrix
            ngl::VAOPrimitives::instance()->createCone("Actor", 0.1, 0.4, 4, 2);
            shader->setUniform("MVP", m_project * m_view * PositionMat);
            ngl::VAOPrimitives::instance()->draw("Actor");

            if (health > 2)
            {
                PositionMat.translate(5.0, 0.0, -7.0); //converts the vector to a matrix
                ngl::VAOPrimitives::instance()->createCone("Actor", 0.1, 0.4, 4, 2);
                shader->setUniform("MVP", m_project * m_view * PositionMat);
                ngl::VAOPrimitives::instance()->draw("Actor");
            }
        }
    }
}

void NGLScene::PlayerPosition()
{
    float Acceleration = 0.1;
    float MaxSpeed = 3.0;
    float MaxDistance = 50.0; 

    if (m_MoveForward && (!m_MoveBackward))     //if the forwards key is pressed at the same time as backwards, the player doesn't accelerate
    {
        if (SpeedX < MaxSpeed)                  //speed here should perhaps more accurately be called velocity, as it has direction
        {
            SpeedX+=Acceleration/4;             // Acceleration is much slower for moving forwards, to improve playability
        }
        else
        {
            SpeedX = MaxSpeed;
        }
         _PositionX+=SpeedX;

    }

    if (m_MoveBackward && (!m_MoveForward))
    {
        if (SpeedX > -1 * MaxSpeed)
        {
            SpeedX-=Acceleration*2;     // Acceleration backwards is increased slightly
        }
        else
        {
            SpeedX = -1* MaxSpeed;
        }
          _PositionX+=SpeedX;
    }
    if (m_MoveRight && (!m_MoveLeft))
    {
        if (SpeedZ < MaxSpeed)
        {
            SpeedZ += Acceleration;
        }
        else
        {
            SpeedZ = MaxSpeed;
        }
         _PositionZ+=SpeedZ;
    }

    if (m_MoveLeft && (!m_MoveRight))
    {
        if (SpeedZ > -1 * MaxSpeed)
        {
            SpeedZ -= Acceleration;
        }
        else
        {
            SpeedZ = -1 * MaxSpeed;
        }
         _PositionZ+=SpeedZ;
    }

    if (_PositionZ < -(1 * MaxDistance))        //sets the player boundary side to side
    {
        _PositionZ = -1 * MaxDistance;
        SpeedZ = -1 * SpeedZ - Acceleration*2;  //reversing the speed makes the sides "bouncy"
    }
    if (_PositionZ > MaxDistance)
    {
        _PositionZ = MaxDistance;
        SpeedZ = -1 * SpeedZ + Acceleration*2;
    }
    if (_PositionX < (-1 * MaxDistance))        //sets the boundry up and down
    {
        _PositionX = -1 * MaxDistance;
        SpeedX = Acceleration;
    }
    if (_PositionX > MaxDistance)
    {
        _PositionX = MaxDistance;
        SpeedX = -1 * Acceleration;
    }

    if ((m_MoveRight && m_MoveLeft)||(m_MoveForward && m_MoveBackward))         //Decreases the speed when opposing arrow keys are pressed
    {
        SpeedX = SpeedX * (1 - Acceleration);
        SpeedZ = SpeedZ * (1 - Acceleration);
    }


                      

    if (m_MoveForward || m_MoveBackward || m_MoveRight || m_MoveLeft)           //if any of the movement keys are pressed, ie. if the player is currently moving
    {
        m_player->SetPosition((_PositionX / 10), (_PositionZ / 10));            //updates the player position                    
    }
    if (!m_MoveForward && !m_MoveBackward)                                      //sets the speed to 0 if neither of the relevant keys are pressed so that the controls don't feel "slippery"
    {
        SpeedX = 0;
    }
    if (!m_MoveRight && !m_MoveLeft)
    {
        SpeedZ = 0;
    }
}

//----------------------------------------------------------------------------------------------------------------------

void NGLScene::keyPressEvent(QKeyEvent *_event)
{
  // this method is called every time the main window recives a key event.
  // we then switch on the key value and set the camera in the GLWindow
  switch (_event->key())
  {
  // escape key to quite
  case Qt::Key_Escape : QGuiApplication::exit(EXIT_SUCCESS); break;
  case Qt::Key_L :
      m_win.spinXFace=0;
      m_win.spinYFace=0;
      m_modelPos.set(ngl::Vec3::zero());
  case Qt::Key_W :
      m_MoveForward = TRUE;
      break;
  case Qt::Key_S :
      m_MoveBackward = TRUE;
      break;
  case Qt::Key_D :
      m_MoveRight = TRUE;
      break;
  case Qt::Key_A :
      m_MoveLeft = TRUE;
      break;
  case Qt::Key_P:                                         //toggles pause
      if (m_MenuType == MenuType::Play)
      {
          TextureLoaded = FALSE;
          m_MenuType = MenuType::Pause;
      }
      else if (m_MenuType == MenuType::Pause)
      {
          m_MenuType = MenuType::Play;
      }
      break;
  case Qt::Key_Space:                                       //starts the game if at the game over or title menu, and fires the death ray if not
      if (m_MenuType == MenuType::Title)
      {
          m_MenuType = MenuType::Play;
      }
      else if (m_MenuType == MenuType::GameOver)
      {
          Reset();
          m_MenuType = MenuType::Play;
      }
      else if (m_MenuType == MenuType::Play && m_DeathRayTimerComplete)
      {
          DeathRay();
          m_DeathRayTimer->start(60);
          m_DeathRayTimerComplete = FALSE;
      }
      break;
  case Qt::Key_Shift:                                       //shift either shows the how to play texture, or makes the player temporarily invincible
      if (m_MenuType == MenuType::Title)
      {
          TextureLoaded = FALSE;
          m_MenuType = MenuType::HowTo;
      }
      else if (m_InvincibilityTimerComplete)
      {
          m_player->invincibility = TRUE;
          m_HitTimer->start(45);                            //the time that the player will be invincible for
          m_InvincibilityTimerComplete = FALSE;
          m_InvincibilityTimer->start(80);                  //the time until the invincibility can be activated again
      }
      
      break;
  default : break;
  }
  // finally update the GLWindow and re-draw

    update();
}

void NGLScene::keyReleaseEvent(QKeyEvent* _event)
{
    // this method is called every time the main window recives a key event.
    // we then switch on the key value and set the camera in the GLWindow
    switch (_event->key())
    {
    case Qt::Key_W:
        m_MoveForward = FALSE;
        break;
    case Qt::Key_S:
        m_MoveBackward = FALSE;
        break;
    case Qt::Key_D:
        m_MoveRight = FALSE;
        break;
    case Qt::Key_A:
        m_MoveLeft = FALSE;
        break;
    case Qt::Key_Shift:                         //releasing the shift key hides the how to play screen
        if (m_MenuType == MenuType::HowTo)
        {
            TextureLoaded = FALSE;
            m_MenuType = MenuType::Title;
        }
        break;
    default: break;
    }
    // finally update the GLWindow and re-draw

    update();
}

//----------------------------------------------------------------------------------------------------------------------

