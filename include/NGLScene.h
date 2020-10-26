#ifndef NGLSCENE_H_
#define NGLSCENE_H_
#include <ngl/Transformation.h>
#include <ngl/Vec3.h>
#include <ngl/Mat4.h>
#include "WindowParams.h"
#include "Actor.h"
#include "Player.h"
#include "Block.h"
// this must be included after NGL includes else we get a clash with gl libs
#include <QOpenGLWindow>
#include <ngl/Obj.h>


//----------------------------------------------------------------------------------------------------------------------
/// @file NGLScene.h
/// @brief this class inherits from the Qt OpenGLWindow and allows us to use NGL to draw OpenGL
/// @author Jonathan Macey
/// @version 1.0
/// @date 10/9/13
/// Revision History :
/// This is an initial version used for the new NGL6 / Qt 5 demos
/// @class NGLScene
/// @brief our main glwindow widget for NGL applications all drawing elements are
/// put in this file
//----------------------------------------------------------------------------------------------------------------------

class NGLScene : public QOpenGLWindow
{
  public:
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief ctor for our NGL drawing class
    /// @param [in] parent the parent window to the class
    //----------------------------------------------------------------------------------------------------------------------
    NGLScene();
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief dtor must close down ngl and release OpenGL resources
    //----------------------------------------------------------------------------------------------------------------------
    ~NGLScene() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief the initialize class is called once when the window is created and we have a valid GL context
    /// use this to setup any default GL stuff
    //----------------------------------------------------------------------------------------------------------------------
    void initializeGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we want to draw the scene
    //----------------------------------------------------------------------------------------------------------------------
    void paintGL() override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this is called everytime we resize the window
    //----------------------------------------------------------------------------------------------------------------------
    void resizeGL(int _w, int _h) override;

private:

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief Qt Event called when a key is pressed
    /// @param [in] _event the Qt event to query for size etc
    //----------------------------------------------------------------------------------------------------------------------
    void keyPressEvent(QKeyEvent *_event) override;
    void keyReleaseEvent(QKeyEvent* _event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called every time a mouse is moved
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseMoveEvent (QMouseEvent * _event ) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is pressed
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mousePressEvent ( QMouseEvent *_event) override;
    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse button is released
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void mouseReleaseEvent ( QMouseEvent *_event ) override;

    //----------------------------------------------------------------------------------------------------------------------
    /// @brief this method is called everytime the mouse wheel is moved
    /// inherited from QObject and overridden here.
    /// @param _event the Qt Event structure
    //----------------------------------------------------------------------------------------------------------------------
    void wheelEvent( QWheelEvent *_event) override;

    void loadMatrixToShader(ngl::Transformation &_tx);

    void PlayerPosition();

    void NGLScene::timerEvent(QTimerEvent*) override; //added

    void DrawPlayerHealth(int health);

    void Reset();

    void DrawStars(int i);

    void DeathRay();
    
    GLfloat m_ClearColour = 0.0; // red variable for the background

    //Actor m_player;
    std::unique_ptr<Player> m_player;
    ngl::Real _PositionX = 0.0;
    ngl::Real _PositionZ = 0.0;

    void timerEvent(QTimerEvent);
    QTimer* m_BlockTimer;       //time between new blocks spawning
    QTimer* m_HitTimer;         //time invincible after hitting a block
    QTimer* m_DeathRayTimer;    //time between firing the deathray
    QTimer* m_InvincibilityTimer;    //time between using the dodge ability
    std::vector<std::unique_ptr<Block>> m_BlockList;
    std::vector<std::unique_ptr<Actor>> m_StarList;
    bool CollisionDetect();
    void DrawPlane(std::string type);
    bool TextureLoaded = FALSE;
    std::string m_image;

    GLuint m_textureName;

    float SpeedX = 0;
    float SpeedZ = 0;

    int Score = 0;
    int Difficulty = 0;
    
    bool hit = FALSE;
    bool prevHit = FALSE;
    bool m_deathray = FALSE;
    bool m_DeathRayTimerComplete = TRUE;
    bool m_InvincibilityTimerComplete = TRUE;
    float m_deathrayColour = 1.0f;

    bool pause = TRUE;

    enum MenuType {Play, Pause, Title, GameOver, HowTo};
    MenuType m_MenuType = NGLScene::Title;

    //KeyPress variables
    bool m_MoveForward = FALSE;
    bool m_MoveBackward = FALSE;
    bool m_MoveRight = FALSE;
    bool m_MoveLeft = FALSE;
   

    //projection matrix
    ngl::Mat4 m_project;
    //view matrix
    ngl::Mat4 m_view;
    //map bounds
    ngl::Vec2 m_bounds = { 4.0,4.0 };

    /// @brief windows parameters for mouse control etc.
    WinParams m_win;
    /// position for our model
    ngl::Vec3 m_modelPos;

};



#endif
