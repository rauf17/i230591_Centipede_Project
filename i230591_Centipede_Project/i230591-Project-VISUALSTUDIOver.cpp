#include <iostream>
#include<ctime>
#include<cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/Window/Event.hpp>

using namespace std;

// Initializing Dimensions.
// resolutionX and resolutionY determine the rendering resolution.
// Don't edit unless required. Use functions on lines 43, 44, 45 for resizing the game window.

const int resolutionX = 960;
const int resolutionY = 960;
const int boxPixelsX = 32;

const int boxPixelsY = 32;
const int gameRows = resolutionX / boxPixelsX;
const int gameColumns = resolutionY / boxPixelsY;

// Initializing GameGrid.

int gameGrid[gameRows][gameColumns] = {};
// The following exist purely for readability.

const int x = 0;
const int y = 1;
const int exists = 2;

//functions for handling player movement
void drawPlayer(sf::RenderWindow &window, float player[], sf::Sprite &playerSprite);
void drawBullet(sf::RenderWindow &window, float bullet[], sf::Sprite &bulletSprite);
void moveBullet(float bullet[], sf::Clock &bulletClock);

//functions for handling mushrooms spawning
void initializeMushrooms(int mushroomPositions[][2], string mushroomStates[] , const int &maxMushrooms);
void drawMushrooms(sf::RenderWindow &window, sf::Texture &mushroomTexture, const int mushroomPositions[][2], int numMushrooms , string mushroomStates[]);
void destroyMushrooms(float bullet[], int mushroomPositions[][2], const int &numMushrooms, string mushroomStates[], int &score);

//functions for handling centipede movement
void initializeCentipede(float centipede[][4], int centipedeSegment, int boxPixelsX, int gameColumns, int gameRows);
void drawCentipede(sf::RenderWindow &window, sf::Sprite &centipedeSprite, sf::Sprite &centipedeHeadSprite, float centipede[][4], int centipedeSegment, int headIndex);
void moveCentipede(float centipede[][4], sf::Clock &centipedeClock, int centipedeSegment, int &headIndex);

//functions for handling collisions
void checkMushroomCollision(float centipede[][4], int centipedeSegment, int mushroomPositions[][2], const int &numMushrooms, string mushroomStates[]);
void checkPlayerCollision(float player[], const float centipede[][4], bool &isplayeralive, const int centipedeSegment,  int &score);
void destroyCentipede(float player[], float bullet[], float centipede[][4], int centipedeSegment, int &headIndex, int &score,  bool &isplayeralive);

//functions for displaying scores
void addscore(int points, int &score);
void showScore(sf::RenderWindow &window, int &score);

//functions for displaying the Game Menu
void drawMenu(sf::RenderWindow &window, sf::Font &font, sf::Text &title, sf::Text &startText, sf::Text &quitText,sf::Text &NameText ,int selectedOption) ;
int displayMenu(sf::RenderWindow &window, sf::Font &font);

int main()
{
    srand(time(0));
   
	// Declaring RenderWindow.

    sf::RenderWindow window(sf::VideoMode(resolutionX, resolutionY), "Centipede", sf::Style::Close | sf::Style::Titlebar);
    // Used to resize your window if it's too big or too small. Use according to your needs.
	window.setSize(sf::Vector2u(960, 960)); // Recommended for 1366x768 (768p) displays.

	// window.setSize(sf::Vector2u(1280, 1280)); // Recommended for 2560x1440 (1440p) displays.
	// window.setSize(sf::Vector2u(1920, 1920)); // Recommended for 3840x2160 (4k) displays.
	
	// Used to position your window on every launch. Use according to your needs.
	window.setPosition(sf::Vector2i(100, 0));

	// Initializing Background Music.
	sf::Music bgMusic;
	bgMusic.openFromFile("Music/field_of_hopes.ogg");
	bgMusic.play();
	bgMusic.setVolume(50);

	// Initializing Background.
	sf::Texture backgroundTexture;
	sf::Sprite backgroundSprite;
	backgroundTexture.loadFromFile("Textures/game2.jpg");
	backgroundSprite.setTexture(backgroundTexture);
	backgroundSprite.setColor(sf::Color(255, 255, 255, 255 * 0.75)); // Reduces Opacity to 75%

	// Initializing Player and Player Sprites.

	float player[2] = {};
	player[x] = (gameColumns / 2) * boxPixelsX;
	player[y] = ((gameColumns * 3 / 4) * boxPixelsY)+ (6*boxPixelsY);
	sf::Texture playerTexture;
	sf::Sprite playerSprite;
	playerTexture.loadFromFile("Textures/player.png");
	playerSprite.setTexture(playerTexture);
	playerSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	// Initializing Bullet and Bullet Sprites.
  	float bullet[3] = {};
 	sf::Clock bulletClock;
        sf::Texture bulletTexture;
        sf::Sprite bulletSprite;
   
        bulletTexture.loadFromFile("Textures/bullet.png");
        bulletSprite.setTexture(bulletTexture);
        bulletSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
    
        sf::Texture mushroomTexture;
        if (!mushroomTexture.loadFromFile("Textures/mushroom.png")) {
            cout << "Error: Mushroom texture failed to load!" << endl;
            return EXIT_FAILURE;
        }

        
   	const int maxMushrooms = 30; 
   	const int maxMushroomStates = 4; 			// Number of possible mushroom states

   	int mushroomPositions[maxMushrooms][2]; 		// 2D array for storing mushroom positions
   	string mushroomStates[maxMushrooms];    		// Array for declaring mushroom states

    constexpr int centipedeSegment = 12;
	float centipede[centipedeSegment][4] = {}; 
        
        //initializing the centipede sprite and setting up the texture
        sf::Texture centipedeTexture;
        sf::Sprite centipedeSprite;
       	sf::Clock centipedeClock;
       	
    	centipedeTexture.loadFromFile("Textures/c_body_left_walk.png");
    	centipedeSprite.setTexture(centipedeTexture);
    	centipedeSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
	
        //initializing the centipede head sprite and setting up the texture
	sf::Texture centipedeHeadTexture;
        sf::Sprite centipedeHeadSprite;
              	
    	centipedeHeadTexture.loadFromFile("Textures/c_head_left_walk.png");
    	centipedeHeadSprite.setTexture(centipedeHeadTexture);
    	centipedeHeadSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));

	int centipede_direction = -1; 						//setting the centipede direction towards left(-1)
	int headIndex = 0;							//setting centipede head at 0 index
   
        initializeMushrooms( mushroomPositions, mushroomStates , maxMushrooms);
        initializeCentipede(centipede,centipedeSegment,boxPixelsX, gameColumns, gameRows);
        
  	bool isplayeralive = true;						//setting player alive
  	
  	int score = 0;
  	int points = 0;
  	
  	sf::Font menuFont;
       if (!menuFont.loadFromFile("Font/BlazeCircuitRegular-xRvxj.ttf"))
        {
            return EXIT_FAILURE;
        }

        sf::Text menuStartText("Start Game", menuFont, 30);
        menuStartText.setFillColor(sf::Color::White);
        menuStartText.setPosition(300, 200);

        sf::Text menuQuitText("Quit", menuFont, 30);
        menuQuitText.setFillColor(sf::Color::White);
        menuQuitText.setPosition(300, 300);
        
        sf::Text menuNameText("Abdul Rauf | 23I-0591", menuFont, 30);
        menuNameText.setFillColor(sf::Color::White);
        menuNameText.setPosition(100, 300);

	 bool inMenu = true;
  	 sf::Texture menuBackgroundTexture;
	 sf::Sprite menuBackgroundSprite;

	if (!menuBackgroundTexture.loadFromFile("Textures/background2.jpg")) 
	{
   		cout<< "Failed to load menu background image!" << endl;
    		return EXIT_FAILURE;
	}
  	menuBackgroundSprite.setTexture(menuBackgroundTexture);
  	
    	while(window.isOpen() && isplayeralive) 						//starting the game window and it will run as long as the player is alive
    	{

	   if (inMenu) 
	   {
       		 window.clear();
	         window.draw(menuBackgroundSprite);

        	int selectedOption = displayMenu(window, menuFont);
            if (selectedOption == 0)
            {							
                inMenu = false;									  // Start the game and close the menu
            }
            else if (selectedOption == 1)
            {
                window.close();									 // Close the game and close the menu
                break;
            }
        }
	    else
           
            {
		window.draw(backgroundSprite);							// drawing background on the screen
		drawPlayer(window, player, playerSprite);					//setting player sprite
		
		  if (bullet[exists])
       		  { 
            		moveBullet(bullet, bulletClock);
            		drawBullet(window, bullet, bulletSprite);
             	  }

		sf::Event e;
		while (window.pollEvent(e)) 										// event function to handle player movement and bullet firing
		{		
			if (e.type == sf::Event::Closed) 
			{
				return 0;
			}
			if (e.type == sf::Event::KeyPressed) 
			{
				if(e.key.code == sf::Keyboard :: Left && player[x] > 0)
				{
					player[x] -= boxPixelsX;
				}
				if(e.key.code == sf::Keyboard :: Right && player[x] < (resolutionX - boxPixelsX))
				{
					player[x] += boxPixelsX;
				}
				
				if(e.key.code == sf::Keyboard :: Up && player[y] > (gameRows-5) * boxPixelsY)			//excluding 5 rows as the player area
				{
					player[y] -= boxPixelsY;
				}
				
				if(e.key.code == sf::Keyboard :: Down && player[y] < (resolutionY - boxPixelsX))
				{
					player[y] += boxPixelsY;
				}
				
				if(e.key.code == sf::Keyboard :: Space)
				{
					if(!bullet[exists])
					{
					bullet[x] = player[x];									//initializing the bullet at the coordinates of the player
					bullet[y] = player[y];	
					bullet[exists] = true;									//generating bullet
					}				
				}
			}		
			
			
		}
		
	drawMushrooms(window, mushroomTexture, mushroomPositions, maxMushrooms,  mushroomStates);				//calling the function to draw mushrooms on the screen
        destroyMushrooms(bullet, mushroomPositions, maxMushrooms , mushroomStates, score);					//calling the function to handle mushroom destruction by bullet
        
 
	drawCentipede(window, centipedeSprite, centipedeHeadSprite, centipede, centipedeSegment, headIndex);			//calling the function to draw centipede on the screen
	moveCentipede(centipede, centipedeClock, centipedeSegment, headIndex);							//calling the function to move the centipede across the screen
	
	
	checkMushroomCollision( centipede, centipedeSegment,  mushroomPositions, maxMushrooms,mushroomStates);			//calling the function to check for the collisions between mushroom and centipede
	checkPlayerCollision(player,centipede, isplayeralive, centipedeSegment, score);						//calling the function to check for the collisions between centipede and the player
	destroyCentipede(player, bullet, centipede, centipedeSegment, headIndex, score, isplayeralive);			//calling the function to check for the collisions between bullet and centipede
	
	
 	showScore(window, score);												//calling the function to display score
        window.display();									
        window.clear();
    	
    
   		}
   	}
 return EXIT_SUCCESS;
}

void drawPlayer(sf::RenderWindow &window, float player[], sf::Sprite &playerSprite)
{
    playerSprite.setPosition(player[x], player[y]);
    window.draw(playerSprite);
}

void moveBullet(float bullet[], sf::Clock &bulletClock)
{
    if (bulletClock.getElapsedTime().asMilliseconds() < 15)
        return;

    bulletClock.restart();
    bullet[y] -= 32;
    
    if (bullet[y] < -32)
    bullet[exists] = false;
}
void drawBullet(sf::RenderWindow &window, float bullet[], sf::Sprite &bulletSprite)
{
    bulletSprite.setPosition(bullet[x], bullet[y]);
    window.draw(bulletSprite);

}

// Function to initialize mushroom positions and states
void initializeMushrooms(int mushroomPositions[][2], string mushroomStates[] , const int &maxMushrooms)
{
 	for (int i = 0; i < maxMushrooms; ++i)
	{
    		mushroomPositions[i][0] = (rand() % (gameColumns - 2) + 1) * boxPixelsX;		// exclude the first and last columns (i did this just to make my centipede movement smooth)
    		mushroomPositions[i][1] = rand() % gameRows * boxPixelsY;				// use gameRows as the range for y
		mushroomStates[i] = "normal_healthy";
        }

}


// function to draw mushrooms at random places on the screen
void drawMushrooms(sf::RenderWindow &window, sf::Texture &mushroomTexture, const int mushroomPositions[][2], int numMushrooms, string mushroomStates[])
{
    sf::Sprite mushroomSprite;
    mushroomSprite.setTexture(mushroomTexture);

    const int leftColumn = 1;           // exclude the first column
    const int rightColumn = gameColumns - 2; // exclude the last column

    for (int i = 0; i < numMushrooms; ++i)
    {
        // check if the mushroom is within the desired columns
        if (mushroomPositions[i][0] > leftColumn * boxPixelsX && mushroomPositions[i][0] < rightColumn * boxPixelsX )
        {																// setting the texture rectangle based on mushroom state
            if (mushroomStates[i] == "normal_healthy")
            {
                mushroomSprite.setTextureRect(sf::IntRect(0, 0, boxPixelsX, boxPixelsY));
            }
            else if (mushroomStates[i] == "normal_damaged")
            {
                mushroomSprite.setTextureRect(sf::IntRect(boxPixelsX, 0, boxPixelsX, boxPixelsY));
            }
            else if (mushroomStates[i] == "poisonous_healthy")
            {
                mushroomSprite.setTextureRect(sf::IntRect(0, boxPixelsY, boxPixelsX, boxPixelsY));
            }
            else if (mushroomStates[i] == "poisonous_damaged")
            {
                mushroomSprite.setTextureRect(sf::IntRect(boxPixelsX, boxPixelsY, boxPixelsX, boxPixelsY));
            }
            else
            {
                continue;
            }

            mushroomSprite.setPosition(mushroomPositions[i][0], mushroomPositions[i][1]);
            window.draw(mushroomSprite);												// draw the mushroom on the window
        }
    }
}


// function to check for collisions between bullets and mushrooms, and update mushroom states and score accordingly
void destroyMushrooms(float bullet[], int mushroomPositions[][2], const int &numMushrooms, string mushroomStates[], int &score)
{
    for (int i = 0; i < numMushrooms; i++)
    {											 // check if the bullet position matches the mushroom position and the bullet exists and disappear the bullet
    					

        if (mushroomPositions[i][0] == bullet[x] && mushroomPositions[i][1] == bullet[y] && bullet[exists] == true && (mushroomStates[i] == "normal_healthy" || mushroomStates[i] == "poisonous_healthy" || mushroomStates[i] == "normal_damaged" || mushroomStates[i] == "poisonous_damaged"))
        {
	 
            bullet[exists] = false;
            if (mushroomStates[i] == "normal_healthy")										
            {												 
                mushroomStates[i] = "normal_damaged";						 // update mushroom state from normal_healthy to normal_damaged
            }	
            else if (mushroomStates[i] == "poisonous_healthy")
            {											  
                mushroomStates[i] = "poisonous_damaged";					// update mushroom state from poisonous_healthy to poisonous_damaged
            }
            else
            {
                mushroomStates[i] = "destroyed";
                addscore(1, score);
            }
        }
        
    }
}

// Function to initialize the centipede at a random position 
void initializeCentipede(float centipede[][4], int centipedeSegment, int boxPixelsX, int gameColumns, int gameRows)
{																	
    int headX = rand() % (gameColumns - centipedeSegment) * boxPixelsX + boxPixelsX;
    int headY = rand() % 1 * boxPixelsY; 							 // the centipede spawns at a random place within the first row

    // Loop through each segment of the centipede
    for (int i = 0; i < centipedeSegment; ++i)
    {
        centipede[i][x] = headX + i * boxPixelsX;						 // Set the x-coordinate of the current segment according to the x-coordinate of the head
        centipede[i][y] = headY;								 // Set the y-coordinate of the current segment to the y-coordinate of the head
        centipede[i][3] = -1;  									//reverse the direction
    }
}



// Function to move the centipede based on its current direction
void moveCentipede(float centipede[][4], sf::Clock &centipedeClock, int centipedeSegment, int &headIndex)
{
    if (centipedeClock.getElapsedTime().asMilliseconds() < 50)
    {
        return;
    }
    centipedeClock.restart();

    for (int i = 0; i < centipedeSegment; i++)
    {
        centipede[i][x] += centipede[i][3] * boxPixelsX;  //move horizontally
													 // check if the current segment reaches the left or right boundary
        if (centipede[i][x] >= ((gameColumns * boxPixelsX) - boxPixelsX) || centipede[i][x] <= 0)
        {
            centipede[i][y] += boxPixelsY;  								// moving centipede down if it reached the boundary
            centipede[i][3] = -centipede[i][3];  							// reverse direction
        }
    
    }

   												 	// Update head index based on the direction
    headIndex = (centipede[0][3] != 0) ? 0 : centipedeSegment - 1;

}

// Function to draw the centipede on the game window
void drawCentipede(sf::RenderWindow &window, sf::Sprite &centipedeSprite, sf::Sprite &centipedeHeadSprite, float centipede[][4], int centipedeSegment, int headIndex)
{
    for (int i = 0; i < centipedeSegment; ++i)
    {
        if (i == headIndex)										  // checking if the current segment is the head
        {
            centipedeHeadSprite.setPosition(centipede[i][x], centipede[i][y]);				  // if segment is head. Draw the head sprite
            window.draw(centipedeHeadSprite);
        }
        else
        {
            centipedeSprite.setPosition(centipede[i][x], centipede[i][y]);				  //if segment is normal. Draw the body sprite
            window.draw(centipedeSprite);
        }
    }
}

//function to check the collision between the centipede and the mushroom and change the direction of centipede acoordingly.
void checkMushroomCollision(float centipede[][4], int centipedeSegment, int mushroomPositions[][2], const int &numMushrooms, string mushroomStates[])
{
    for (int i = 0; i < centipedeSegment; ++i)
    {
        for (int j = 0; j < numMushrooms; ++j)
        {
            if ((centipede[i][x]) == mushroomPositions[j][x] && (centipede[i][y]) == mushroomPositions[j][y])                  // Check if the centipede segment is in the same position as any mushroom
            {
            	if (centipede[i][3] == -1)
		{
    			centipede[i][3] = 1;				 // Reverse the direction
		}
		
		if (centipede[i][3] == 1)
		{
    			centipede[i][3] = -1; 				// Reverse the direction
		}

            	              
            }
        }
    }
}  

//function to check collision between the player and the centipede
void checkPlayerCollision(float player[], const float centipede[][4], bool &isplayeralive, const int centipedeSegment, int &score)
{

    // Load the game over sound (before loop to avoid loading it multiple times)
    sf::Music SoundBuffer ;
    bool soundLoaded = false;

    if (!soundLoaded) {
        if (!SoundBuffer.openFromFile("Music/gameovervoice.mp3")) { // Replace with your own path and filename
            cout << "Failed to load game over sound!" << endl;
        }
        else {
            SoundBuffer.setVolume(100); // Set volume (0 to 100)
            soundLoaded = true;
        }
    }

	 for (int i = 0; i < centipedeSegment; ++i)
         { 
		if(player[x] == centipede[i][x] && player[y] == centipede[i][y])			//checking if the coordinates of the player and the centipede match
    		{
    	   									
    	   		
                // Play sound
                if (soundLoaded)
                    SoundBuffer.play();

                sf::sleep(sf::seconds(2));

                isplayeralive = false;  //killing player

		cout<<"*******************************************************************"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*    ************          *         *              * *********   *"<<endl;
    		cout<<"*    *                    * *        * *           ** *           *"<<endl;
    		cout<<"*    *                   *   *       *   *       *  * *           *"<<endl;
    		cout<<"*    *                  *     *      *     *   *    * *           *"<<endl;
            cout<<"*    *                 *       *     *       *      * *********   *"<<endl; 
            cout<<"*    *   ********     ***********    *              * *           *"<<endl;
    		cout<<"*    *          *    *           *   *              * *           *"<<endl;
    		cout<<"*    *          *   *             *  *              * *           *"<<endl;
    		cout<<"*    ************  *               * *              * *********   *"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*      **********  *               * *********   ********    *    *"<<endl;
    		cout<<"*     *          *  *             *  *           *       *   *    *"<<endl;
    		cout<<"*    *            *  *           *   *           *       *   *    *"<<endl;
    		cout<<"*    *            *   *         *    *           *       *   *    *"<<endl;
    		cout<<"*    *            *    *       *     *********   ********    *    *"<<endl;
    		cout<<"*    *            *     *     *      *           *    *      *    *"<<endl;
    		cout<<"*    *            *      *   *       *           *     *     *    *"<<endl;
    		cout<<"*     *          *        * *        *           *      *   ***   *"<<endl;
    		cout<<"*      **********          *         *********   *       *  ***   *"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*                                                                 *"<<endl;
    		cout<<"*******************************************************************"<<endl;
    	
		 cout<<"*******************************************************************"<<endl;
    		 cout<<"*          Your Score:                                            *"<<endl;
    		 cout<<"*                       "<<score<<"                                         *"<<endl;
    		 cout<<"*******************************************************************"<<endl;
    	    	}
	}
}


//function to check the collision between the centipede and the player and then split the centipede
void destroyCentipede(float player[], float bullet[], float centipede[][4], int centipedeSegment, int &headIndex, int &score,  bool &isplayeralive)
{
    for (int i = 0; i < centipedeSegment; ++i)
    {  
        if (bullet[x] >= centipede[i][x] && bullet[x] < centipede[i][x] + boxPixelsX &&					//checking if the coordinates match
            bullet[y] >= centipede[i][y] && bullet[y] < centipede[i][y] + boxPixelsY)
        {
              // Bullet hit the centipede segment			
            bullet[exists] = false;  											//Disappearing bullet
             addscore(10, score);
             
              if (i == headIndex)
            {
                isplayeralive = false;  										// kill player
                cout<<"\n\nYou Hit the Centipede Head!"<<endl<<endl<<endl;
                
                cout<<"***********************************************************"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"*    *          *     **********     *           *        *"<<endl;
    		cout<<"*     *        *     *          *    *           *        *"<<endl;
    		cout<<"*      *      *     *            *   *           *        *"<<endl;
    		cout<<"*       *    *      *            *   *           *        *"<<endl;
    		cout<<"*        *  *       *            *   *           *        *"<<endl;
                cout<<"*         **        *            *   *           *        *"<<endl; 
                cout<<"*         *         *            *   *           *        *"<<endl;
    		cout<<"*        *           *          *     *         *         *"<<endl;
    		cout<<"*       *             **********        ********          *"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"*   *           *    **********       **        *   *     *"<<endl;
    		cout<<"*   *           *   *          *      * *       *   *     *"<<endl;
    		cout<<"*   *           *  *            *     *  *      *   *     *"<<endl;
    		cout<<"*   *     *     *  *            *     *   *     *   *     *"<<endl;
    		cout<<"*   *    * *    *  *            *     *    *    *   *     *"<<endl;
    		cout<<"*   *   *   *   *  *            *     *     *   *   *     *"<<endl;
    		cout<<"*   *  *     *  *  *            *     *      *  *   *     *"<<endl;
    		cout<<"*   * *       * *   *          *      *       * *  ***    *"<<endl;
    		cout<<"*   **         **    **********       *        **  ***    *"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"*                                                         *"<<endl;
    		cout<<"***********************************************************"<<endl;
    		
    		
    		 cout<<"***********************************************************"<<endl;
    		 cout<<"*          Your Score:                                    *"<<endl;
    		 cout<<"*                       "<<score<<"                              *"<<endl;
    		 cout<<"***********************************************************"<<endl;
    	
    	break;												 //breaking out of the loop to end the game
            }
           
            int babySegmentCount = centipedeSegment - i;						 // Split the centipede into two parts at the hit segment
            if (babySegmentCount > 1)
            {
               												 // Create a new  centipede with the hit segment as its head
                float** babyCentipede = new float* [babySegmentCount];
                for (int j = 0; j < babySegmentCount; ++j) {
                    babyCentipede[j] = new float[4];
                    babyCentipede[j][0] = centipede[i + j][x];
                    babyCentipede[j][1] = centipede[i + j][y];
                    babyCentipede[j][3] = -centipede[i + j][3]; // Reverse the direction
                }

               
                centipedeSegment = i;  									// Update the original centipede
                headIndex = centipedeSegment - 1; 							// Set head index to the last segment of the  centipede

                for (int j = 0; j < babySegmentCount; ++j)						// Add the new centipede after the  centipede to split it
                {
                    centipede[centipedeSegment + j][0] = babyCentipede[j][0];
                    centipede[centipedeSegment + j][1] = babyCentipede[j][1];
                    centipede[centipedeSegment + j][3] = babyCentipede[j][3];
                }


                // Free allocated memory
                for (int j = 0; j < babySegmentCount; ++j) {
                    delete[] babyCentipede[j];
                }
                delete[] babyCentipede;
            }
            else
            {
                centipede[i][3] *= -1;									// If the centipede has only one segment, just reverse its direction
            }
           addscore(10, score);


        }
    }
}


void addscore(int points, int &score)
{
    score += points;											//scoring
}


void showScore(sf::RenderWindow &window, int &score)
{
    sf::Font font;
    if (!font.loadFromFile("Font/BlazeCircuitRegular-xRvxj.ttf")) 
    {
       return;
    }

    sf::Text text;
    text.setFont(font);
    text.setString("Score: " + std::to_string(score));
    text.setCharacterSize(20);
    text.setFillColor(sf::Color::Red);
    text.setOutlineColor(sf::Color::Black);
   
    text.setPosition(1.f, 1.f);									// Positioning the text at the top of the screen
    window.draw(text);										// Draw the text to the window
}


void drawMenu(sf::RenderWindow &window, sf::Font &font, sf::Text &startText, sf::Text &quitText,sf::Text &NameText ,int selectedOption) 
{

    // Reset both options to default color
    startText.setFillColor(sf::Color::White);
    quitText.setFillColor(sf::Color::White);
    startText.setOutlineColor(sf::Color::Transparent);
    quitText.setOutlineColor(sf::Color::Transparent);

    // Highlight the selected option
    if (selectedOption == 0)
    {
        startText.setFillColor(sf::Color::Yellow);
        startText.setOutlineColor(sf::Color::Blue);
    }
    else
    {
        window.display();
        quitText.setFillColor(sf::Color::Yellow);
        quitText.setOutlineColor(sf::Color::Blue);
    }

    window.draw(startText);
    window.draw(quitText);
    window.draw(NameText);
    window.display();
}

int displayMenu(sf::RenderWindow& window, sf::Font& font)
{
    sf::Text menuStartText("Start Game", font, 40);
    menuStartText.setFillColor(sf::Color::White);
    menuStartText.setOutlineThickness(3);
    menuStartText.setPosition(350, 750);

    sf::Text menuQuitText("Quit", font, 40);
    menuQuitText.setFillColor(sf::Color::White);
    menuQuitText.setOutlineThickness(3);
    menuQuitText.setPosition(400, 800);

    sf::Text menuNameText("Abdul Rauf | 23I-0591", font, 30);
    menuNameText.setFillColor(sf::Color::Red);
    menuNameText.setPosition(250, 900);

    int selectedOption = 0;
    drawMenu(window, font, menuStartText, menuQuitText, menuNameText, selectedOption);

    sf::Event menuEvent;
    while (window.waitEvent(menuEvent))
    {
        if (menuEvent.type == sf::Event::KeyPressed)
        {
            if (menuEvent.key.code == sf::Keyboard::Down)
            {
                selectedOption = (selectedOption + 1) % 2;
            }
            else if (menuEvent.key.code == sf::Keyboard::Up)
            {
                selectedOption = (selectedOption - 1 + 2) % 2;
            }
            else if (menuEvent.key.code == sf::Keyboard::Enter)
            {
                return selectedOption;
            }

            drawMenu(window, font, menuStartText, menuQuitText, menuNameText, selectedOption);
        }
    }
    return -1;
}

















