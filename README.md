# Spaceship-X307

Research

My original gameplay design for the project was to create a game similar to the Binding of Isaac, in which the player moves along two axes, and can shoot at enemies in all directions, with the objective of either clearing a room. The first iteration of the project was a 2D space in which the player could move in all directions with equal speed and acceleration, but this made movement feel boring. Having to defend from all sides also meant that the camera would have to be almost directly above the arena so that the player doesn’t have blind spots, which would be less visually appealing.


I decided to change the player movement to simulate traveling on largely one axis, focusing on movement from side to side. I looked at the gameplay from both Space Invaders and Bug on a Wire, which both use side to side movement in different ways. Space Invaders has the enemies move downwards, towards the player, and the player can shoot them by firing directly upwards. Bug on a Wire, however, has gameplay centring dodging rather than attacking, as the player has no attack and instead must move out of the way of the approaching enemies. I wanted to include elements of both of these in my game, to come up with gameplay that is not entirely derivative of either game.

My next phase of gameplay planning consisted of primitives that would move down the screen for the player to dodge. This also allowed me to design a level where the difficulty could increase gradually, making the game much more fun to play for a longer time as it will continue to be challenging.

With this change in the level format, a single firing direction was all that would be needed, which would mean that the controls for the player would be much more simple to use. This makes the game quicker to learn and more enjoyable. It also allowed me to make the method of calculating the player’s direction much more simple, as it currently uses the player’s speed left or right and adds it to the player’s rotation, which is more efficient than having to calculate a direction vector.

Implementation


The game uses the NGLScene class from NGL, which draws a window and contains the function PaintGL, which is called repeatedly on a timer. This is used for the main game loop, as it is called repeatedly as the program is run. It contains a switch statement which decides whether the game is currently playing, or which menu screen should be drawn, based on the flag set in the variable m_MenuType.

The game takes input in the form of key press events, which are used to switch between menus and gameplay, and control the player. This includes keys to show the “how to play” menu, to start the game, toggle a pause menu, and restart. The “game over” case is controlled by the player’s health, which changes m_MenuType to the corresponding flag. The menus themselves are a plane which is orthographically projected, and as needed the appropriate texture for each specific menu is applied.

The game is primarily made up of the interactions between “actors”, a class which contains the basic information common to the player, obstacles, and stars, such as position and size. It also contains the function which draws the actor, depending on a string which indicates what needs to be drawn. The player is most complex, as it consists of two primitives. The cone for the spaceship’s body is drawn first, as its colour and size are constant. The rocket flame is then calculated by either incrementing or decrementing a value based on a Boolean, which is reversed when the flame value reaches its minimum or maximum values. The flame value is then used to vary the flame’s colour and size. Drawing the obstacles and stars are much more simple as they only have to create a sphere of the right size at the correct location.

The “Player” class inherits from Actor, and contains variables for the player’s start position, health, and invincibility, as well as functions to set the player’s position and health. The “Block” class also inherits from Actor, and contains the function to calculate the block’s position, which is done very differently from the player’s.

How to Play

The player is controlled by the W, A, S, and D keys. Move around the arena avoiding the obstacles for as long as possible. The game includes two abilities, a death ray (used with the space bar) which destroys all obstacles directly above the player, and dodge (activated by the shift key) which grants temporary invincibility. The P key also toggles pausing the game.

Results

The game overall is successful, with a visually simplistic yet appealing result that behaves reliably and is fun to play.

The game could be improved visually with more complex shaders, as currently the game only uses flat colours, and more complex models and textures for the obstacles and player would add visual interest.

The gameplay could be extended by adding variation in the size of obstacles, and more variation in player abilities could make the game more interesting. Perhaps the most important addition, however, would be the addition of a score display either during play or at the game over screen, rather than only printing the score to the terminal.

Another improvement would be to use a more accurate timer for the cooldowns, potentially via a function that counts how many times the game loop has been called since the start of the “timer”, rather than using a QTimer.

