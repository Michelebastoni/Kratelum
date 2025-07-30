<img width="1232" height="706" alt="EditorSplash" src="https://github.com/user-attachments/assets/3cf4f3f8-a054-4a75-bacc-1d149dc46fce" />


The last academic project lasting six months, composed of a team of 20 people including 10 designers, 4 programmers, 3 concept artists, and 3 3D artists.

During this project, we encountered some problems that forced us to rethink the direction and concept of the game.
Despite the unexpected challenges and the limited time remaining, we worked hard to ensure that the game would be entertaining and fun.
During development, I gained experience in team management and communication with various departments.

# About the game

Kratelum is an arcade arena shooter set in an alternate Metal WWI, where exaggeration by weapons, power ups, and trap activation leads to a high score.
- Goal of the game: kill all enemies in the arena.
- Game Over: Character HP reaches = 0.
- Camera: isometric.
- Level Structure: each Level is an arena.

## [***Trailer***](https://youtu.be/ZAgwMNOgDZw) 

## [***Download***]() 

# My Role

I worked on various core mechanics, including:
- [Characters Base Class](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#characters-base-class)
- [Player controller](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#player-controller)
- [Component implementations](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#components-implementation)
- [Score implementation](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#score-implementation)
- [Save system](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#save-system)
- [UI](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#ui)

# Characters Base Class 
[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/CharacterLogic/GenericCharacter.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/CharacterLogic/GenericCharacter.cpp)

## Overview

The base class from which the player and the AI ​​will inherit.
Being the base class it has the generic functions and logic that both the player and the AI ​​have, such as:
- Movement
- Rotation
- States (alive or dead, can move?, can rotate?)
  
The character uses different components such as:
- [Integrity Component] to determinate the character state
- Interaction Component to be able to interact with “Interactable” object

> [!NOTE]
> I didn't implement Interaction Component, "Interactable" Obejct too, so it's not gonna be its description on this file.
> You can find the components i worked on [Here](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#components-implementation).
> 
> Interaction Scripts if you wanna go in deep [.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/InteractionSystem/Components/InteractionComponent.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/InteractionSystem/Components/InteractionComponent.cpp)

Furthermore, The character’s base class has a Weapon class variable which can be used if equipped.

## Player

[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/CharacterLogic/PlayerCharacter.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/CharacterLogic/PlayerCharacter.cpp)

the player inherits from the character's base class, in addition to the basic things he owns:
[Overflow Component](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#overflow-component) to trigger the “Overflow” status

# Player Controller

[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/Controllers/GenericPlayerController.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/Controllers/GenericPlayerController.cpp)

## Overview
The player controller have to possess a [Generic Character](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#characters-base-class) in which it then manages all the actions such as:
 - Shooting
 - Reloading
 - Interactions
 - Dashing

## Actions

Player controller has an array of **actions** that the player can perform.

These actions derive from the [ActionBase](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/Controllers/Actions/ActionBase.h) class in which through the action input they allow the player to move, shoot, reload, dash, pause...

### Initialize
The base action has a ***setUp*** function that initializes the relevant action by taking the controller reference and subscribing to the bind event via the controller's EnhancedInputComponent.

### Perform
Once the action is initialized, once the player presses the correct input, the action's ***perform*** function will be activated, in which the actual reference action (shoot, reload, dash) occurs depending on the action and the button pressed by the user.

### ShutDown
In the end, the action also has a ***shutDown*** function that is called by the controller in EndPlay. This function is intended to "clean up" any data left over during gameplay of the relevant action.
For example, unbinding additional subscriptions.


# Components Implementation

In the project, I worked on some core components:
- [Integrity Component](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#integrity-component)
- [Overflow Component](https://github.com/Michelebastoni/Kratelum/edit/main/README.md#overflow-component)

## Integrity Component
[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/IntegritySystem/Integrity.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/IntegritySystem/Integrity.cpp)

### Overview
The Integrity System is designed to monitor the integrity of the assigned actor and display it on the HUD for the player to see.
The Integrity monitors the integrity of the assigned actor. It has functions to decrease or increase itself, but not more than the maximum indicated.
The Integrity System also grants immunity to all or some damage and the recovery block based on the actor settings.

#### Immortality logic
Integrity immortality is measured using an array named ***ImmortalityContainer*** that fills and empties depending on the game dynamics. If the array contains at least one element, it means the actor is immortal and will not receive damage.

## Overflow Component
[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/OverflowSystem/Overflow.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/OverflowSystem/Overflow.cpp)

### Overview
The overflow represents the bar the player has available to launch their special attack.
This component manages the addition and removal of overflows, launching useful events that connect to other game systems such as the UI.

# Score Implementation
[.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/RankSystem/ScoreHandler.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/RankSystem/ScoreHandler.cpp)

### Overview
The scoring system is designed to give the player a sense of accomplishment and satisfaction as they destroy endless hordes. The score handler is an actor placed on the map and updates all the values corresponding to the score, such as the score multiplier and the combo timer, via a subsystem. The score handler also provides a widget to which it is connected so that it updates accordingly.

# Save System

## Overview
Save system has the purpose of saving and loading data with the creation of different ***save Objects*** for each system that requires data to save.
Then when the data is required, create the relative save object, and you can get data from this save object.
- ***ScoreSaveSystem*** [.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/SaveSystem/ScoreSaveSystem.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/SaveSystem/ScoreSaveSystem.cpp)
- ***VolumeSaveSystem*** [.h](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/SaveSystem/VolumeSaveSystem.h) / [.cpp](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Private/SaveSystem/VolumeSaveSystem.cpp)

# UI

## Overview
The interactable screens all derive from  [***GameScreen***](https://github.com/Michelebastoni/Kratelum/blob/main/Source/TBO02/Public/Widgets/GameScreen.h) base class in which the overrides for the controller via pad input are made, some buttons are shared plus some overridable functions for the use of specific screens
