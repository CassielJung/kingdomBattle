# Multiplayer Card Game: Kingdoms Battle
### L2C_Team39 Members
- Andre Santiago Correia
- Cassiel Jung
- Nick Zhang
- Mike He

## Project Topic
Developing real-time multiplayer card game using *cloud, user interface(game and AI), hardware* that supports multiplayer mode between two people and single-player mode between a person and AI.

![image](https://user-images.githubusercontent.com/69894554/162331210-1848859a-3568-4246-a924-9c5b8c95b27b.png)

## Project Goal
Since our final product is a video game, the game should meet following goals:
- **ease of use**; intuitive game rules
- **accessibility**; users should be able to access the game everywhere through internet
- **innovative and strategic game logic**; game should be interesting to play and not similar to pre-existing games
- **fast-paced**; if game is taking for a long time user would lose their concentration

## Features Implemented
**Multiplayer Mode**: Multiple games can be run synchronously. For example, if we have 4 people running the game, there will be 2 rooms with 2 people each.
![image](https://user-images.githubusercontent.com/69894554/162332666-ee28c63f-9b6d-4b62-b050-c10e0941628a.png)

**Singleplayer Mode**: Multiple games can be run synchronously. When 4 people joined to play singleplayer mode with AI at the same time, server will create 4 separate room for them so that they can play just like the image below.
![image](https://user-images.githubusercontent.com/69894554/162333109-90437082-9111-4b77-947d-d7125249f5d8.png)


## Contribution
### [Hardware_Nick](https://github.com/UBC-CPEN391/l2c-39/tree/main/hardware)
- Designed and implemented game UI
- Developed VGA video output driver in verilog
    - Fully hardware-accelrated graphics
    - Textures stored in on-chip memory
- Developed PS2 Keyboard input driver
### [Cloud_Andre](https://github.com/UBC-CPEN391/l2c-39/tree/main/cloud)
- Developed a back-end server in Python
    - Handles multiple client connections
    - Create separate rooms for multiplayer and singlplayer modes
- Developed client code in Python and converted to C
    - Sends/Receives message
    - the game runs here
### [Game_Cassiel](https://github.com/UBC-CPEN391/l2c-39/tree/main/gameFiles)
- Implemented game logic in C
- Merge game logic with client code
    - run the code on linux
    - synchronize two player's screen when running multiplayer mode
- Merge game code with VGA and Keyboard drivers
### [AI_Mike](https://github.com/UBC-CPEN391/l2c-39/tree/main/AI)
- Implemented first draft of game logic with Cassiel
    - Construct dataset by simulating above code 10,000 to 50,000 times
- Implemented AI logic which use dataset as a lookup table to determine the next move
    - Checked conditions when the dataset does not support the situation
- Merge the game and the AI to both client and server component
- AI has winning probability of 60% roughly

## Acknowledgement
The HPS-to-FPGA memory-mapped interface, PIO, and On-chip memory IP cores were obtained from Altera 
in Quartus. Other than that, every code was written by team members.
