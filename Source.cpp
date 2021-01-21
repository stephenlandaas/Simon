// Program: Simon
// =================================================== 
// Programmer: Stephen Landaas
// Creation Date: January 2021
// ===================================================

#include <iostream>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <cctype>
#include <vector>
#include <fstream>
#include "Windows.h"

typedef unsigned short int USHORT;

void startGame(bool &gameRunning, std::vector<char> &simonInputs, USHORT &round);
void displaySimonInputs(std::vector<char> &simonInputs);
void displayColor(char ch, int additionalTime); // additionalTime is increased to a nonzero digit to allow more time for Simon's pattern to display
bool userTurn(std::vector<char> &simonInputs, USHORT &round);
void checkHighScore(USHORT round);

int main() {
	std::vector<char> simonInputs = {}; // Vector that stores the list of inputs for Simon to run, grows each turn.
	char inputOptions[4] = {'r', 'g', 'b', 'y'}; // Each char represents a color code (Red, Green, Blue, and Yellow)
	USHORT round = 1; // Current round that the user is on.
	bool gameRunning = false; // Flag for whether or not the game is running
	srand(time(NULL));

	startGame(gameRunning, simonInputs, round); // Open mini-menu, reset inputs, reset round.

	while (gameRunning) {
		simonInputs.push_back(inputOptions[rand() % 4]); // Add a random character (r,g,b, or y) to Simon's list of current inputs.
		std::cout << "\n\n\n\t\t\t\t\t\t    - ROUND " << round << " -" << std::endl;
		Sleep(1500);
		system("cls");
		displaySimonInputs(simonInputs); // Display the colors Simon has pressed based off of his current inputs
		gameRunning = userTurn(simonInputs, round); // Allow the user to enter their own color inputs

		// If gameRunning is set to false prematurely, it means the user lost the game. 
		if (gameRunning == false) {
			system("cls");
			Sleep(100);
			system("Color 0c");
			std::cout << "\n\n\n\t\t\t\t\t\t    YOU LOSE!!!";
			Sleep(3000);
			system("Color 0f");
			checkHighScore(round);
			startGame(gameRunning, simonInputs, round); // Restart the game (prompt the menu for the user to play again).
		}
	}

	return 0;
}



// Function that opens the menu, asks the user if they wish to play or exit, and resets simon's input list and the round status.
void startGame(bool &gameRunning, std::vector<char> &simonInputs, USHORT &round) {
	char option = '\0'; // Option that controls whether the user wants to play again (P) or exit the game (E)
	bool validInput = false; // Flag for determining whether or not the user's choice is valid

	while (!validInput) {
		system("cls");
		std::cout << "\n\t\tCONTROLS:  'R' to toggle Red | 'G' to toggle Green | 'B' to toggle Blue | 'Y' to toggle Yellow\n\n\n\n\n";
		std::cout << " > Please enter 'P' if you'd like to play/play again, or press 'E' to exit:  ";
		std::cin.get(option);
		
		option = toupper(option);
		validInput = true;

		// Flushing the buffer...
		std::cin.ignore(1000, '\n');

		if (option == 'P') {
			// Clear the screen, run the game again, reset the round status and empty Simon's current list of inputs.
			system("cls");
			gameRunning = true;
			simonInputs.resize(0); // Empty Simon's list of inputs and restore to default state.
			round = 1;
		}
		else if (option == 'E') {
			// Exit the game, set gameRunning = false to end the loop in main().
			gameRunning = false; 
		}
		else {
			// Mark the user's input as incorrect and prompt them to try again.
			validInput = false;
			std::cout << "Wrong input! Try again!\n";
			Sleep(1500);
		}
	}
}



// Run through Simon's input vector and display each color accordingly.
void displaySimonInputs(std::vector<char> &simonInputs) {
	for (int i = 0; i < simonInputs.size(); i++) {
		displayColor(simonInputs[i], 185);
		Sleep(150);
	}
}



// Helper function that will receive a color inputted by the user or Simon, and make the color flash on the screen for a designated amount of time.
void displayColor(char ch, int additionalTime) {
	switch (ch) {
		case 'r': // RED
			system("Color 4f");
			Beep(1000, 130 + additionalTime);
			break;
		case 'g': // GREEN
			system("Color 2f");
			Beep(500, 130 + additionalTime);
			break;
		case 'b': // BLUE
			system("Color 1f");
			Beep(300, 130 + additionalTime);
			break;
		case 'y': // YELLOW
			system("Color 6f");
			Beep(1500, 130 + additionalTime);
			break;
		default:
			exit(1); // Error
	}

	// Return the screen to black after making the color appear.
	system("Color 0f");
}



// Run the user's input cycle, 
bool userTurn(std::vector<char> &simonInputs, USHORT &round) {
	char userInput = '\0'; // The user's selection (Red, Green, Blue, or Yellow)
	int index = 0; // Current index of simon's inputs, used to compare the user's inputs against Simon's list of inputs on the fly.

	// While all of Simon's inputs haven't been guessed by the user yet
	while (index < simonInputs.size()) {
		userInput = char(tolower(_getch()));

		if ((userInput == 'r') || (userInput == 'g') || (userInput == 'b') || (userInput == 'y')) {
			displayColor(userInput, 150); // Display the proper color
			if (simonInputs[index] != userInput) { 
				return false; // Exit the game if the user's input at the given index is not equal to Simon's input (they guessed a color wrong)
			}
			else {
				index++; // Otherwise, add 1 to the index and move to the next of Simon's inputs for the user to guess.
			}
		}

		FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
	}

	round++; // Increment the round, the user survived this one.

	return true; // Return true, meaning that the game is still running
}



// Check the user's current high score stored in HIGH-SCORE.bin and overwrite it if necessary.
void checkHighScore(USHORT round) {
	std::fstream highScoreFile("HIGH-SCORE.bin", std::ios::in | std::ios::binary);
	USHORT currentHighScore = 0; // The current high score stored in HIGH-SCORE.bin

	if (highScoreFile.fail()) {
		std::cout << "Unable to read/write high scores at this time.";
		Sleep(2000);
		return;
	}

	// Read in the high score from the file and store in currentHighScore
	highScoreFile.read(reinterpret_cast<char*>(&currentHighScore), sizeof(currentHighScore));
	highScoreFile.close(); 

	// If the current round (current score) is greater than what's in the file, we'll overwrite it.
	if (round >= currentHighScore) {
		highScoreFile.open("HIGH-SCORE.bin", std::ios::out | std::ios::binary);
		highScoreFile.write(reinterpret_cast<char*>(&round), sizeof(round)); // Overwrite the score already in the file.
		highScoreFile.close();
		system("cls");

		// Do a celebratory flash of green text to indicate that the user has beaten the stored high score.
		system("Color 0a");
		std::cout << "\n\n\n\t\t\t\t\t\tNew High Score of " << round << '!';
		Sleep(3000);
		system("Color 0f"); // Reset the text color back to normal.
	}
}