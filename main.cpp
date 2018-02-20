	 /*
	Author: Scott Russell
	Last Updated: 02/08/2018
	Description: CSCI 1720 Spring 2018 Proj. 2 starter

	Modified by:  <Spencer Peters>
*/

#include <iostream>
#include <fstream>
#include <sstream>
#include <cctype>
#include <string>
#include <cstring>
#include <ctime>
#include <cstdlib>	// to access srand, rand, and system
#include <cmath>
#include <stdlib.h>
#include <time.h>

using namespace std;

// global constants
// number of words in a puzzle
const int NUMBER_OF_WORDS = 4;
// maximum number of underlined/significant letters from a single word that will be used
// to make final puzzle answer.  This is the maximum number of positions per word in line
// of input file
const int MAX_LETTER_FROM_WORD = 4;

// the following are used to store the three lines at the end of the file related
// to the solution answer in a single array of strings of size NUM_SOLUTION_PARTS
const size_t NUM_ANSWER_PARTS = 3;
const size_t ANS_CONTEXT_INDEX = 0;
const size_t ANS_PROMPT_INDEX = 1;
const size_t ANS_SOLUTION_INDEX = 2;



// User interface constants
const char EOL = '\n';	// end of line character
const char ANSWER_BLANK = '#';		// feel free to change to something else
const char UNGUESSED_MARK = 'X';
const string VOWELS="AEIOU";

// prototypes
void loadPuzzleData(ifstream&, string words[], int positions[][MAX_LETTER_FROM_WORD],
	int numPositions[], string answerInfo[]);

void displayWordList(const string wordList[]);

void displayAnswerInfo(string solutionLetters, string solutionTemplate,
	const string answerInfo[]);

bool solveAnagrams(const string words[], const string markedWords[]);
bool solvePuzzle(bool quitEarly, string solutionLetters, const string answerInfo[]);

void anagramWords(const string words[], string anagrams[]);

void upcaseWords(const string wordlist[], const int positions[][MAX_LETTER_FROM_WORD],
	const int numPositions[], string markedWords[], const string answerInfo[]);

string makeAnagram(string source);
string blankLetters(string source, char blank = ANSWER_BLANK);

string makeSolutionLetterString(const string wordlist[],
	const int positions[][MAX_LETTER_FROM_WORD], const int numPositions[], string markedWords[]);
bool isVowel(char);

// Use to verify data loaded correctly - not used in actual puzzle solving
void displayPuzzle(const string words[], const int letterPositions[][MAX_LETTER_FROM_WORD],
	const int numLettersUsed[], const string answerInfo[]);


int main(int agrc, char* args[])
{
	// seed the random number generator using time
	srand(time(NULL));
	// prompt for puzzle data file name unless given as command line parameter...
	string sourceFileName = "czech.txt";
	ifstream inFileStream;
	bool playAgain = false;
    bool openFile = false;

	do {    // repeat process of getting puzzle name, loading and solving until they're done

        // 1. ADD YOUR CODE HERE

        // repeatedly prompt for a name until given file name can be opened
        while(openFile == false)
        {
            // add option to use default file name if none entered?
            cout << "Please enter name of file to load puzzle from: ";
            getline(cin, sourceFileName);
            cout << endl;
            inFileStream.open(sourceFileName.c_str());
            if(!inFileStream.is_open())
            {
                cout << "The file has failed to open or does not exist" << endl;
            }
            else
            {
                cout << "Your file has successfully been loaded. Enjoy!" << endl;
                openFile = true;
            }

            // open specified file and verify opened successfully
            //inFileStream.open(sourceFileName.c_str());
        }

		// variables to store assorted puzzle data.  Could package into a struct...
		// list of words to anagram/anagram answers
		string words[NUMBER_OF_WORDS];
		// same list of words with positions in letterPositions changed to uppercase
		string markedWords[NUMBER_OF_WORDS];

		// the list of significant postions in each word that are used to make
		// puzzle's final answer and number of significant positions for each word.
		int letterPositions[NUMBER_OF_WORDS][MAX_LETTER_FROM_WORD],
			numLettersUsed[NUMBER_OF_WORDS];

		// The context/people's utterances, i.e. the lead-in to the answer and
		// final puzzle answer made by arranging letters from anagrammed words.
		string answerInfo[NUM_ANSWER_PARTS];

		// load puzzle data from file
		loadPuzzleData(inFileStream, words, letterPositions, numLettersUsed, answerInfo);

		// close input file
		inFileStream.close();

		// verify data loaded from file correctly
		// use this BEFORE you start passing this data to your anagram function to ensure
		// all data loaded correctly
		/****  comment out next two lines in final version  ****/
		// call function to display the contents of the 4 arrays
		//displayPuzzle(words, letterPositions, numLettersUsed, answerInfo);
		//cout << endl;

		// I. Keep everything from here to end of loop commented out until
		// you verify all data is correctly loaded.

		// start puzzle solving process
		// generate uppercase version of words in word list
		upcaseWords(words, letterPositions, numLettersUsed, markedWords, answerInfo);
		// Use the statement below to check that upcaseWords is woking correctly
		// without having to play puzzle; it should be commented out in final version
		// unless you want user to be able to cheat!
		//displayWordList(markedWords);

		// II. Move the start of multi-line comment to here AFTER completing
		// upcaseStringPositions function

		// solve anagrams ...
		bool quitEarly = !solveAnagrams(words, markedWords);

		// III. Move the start of multi-line comment to here AFTER completing
		// makeAnagram and upcaseStringPositions

		// Regardless of whether user has already quit...
		// make string of letters to use in solution
		string solutionLetters =
			makeSolutionLetterString(words, letterPositions, numLettersUsed, markedWords);

		// IV. Move the start of multi-line comment to here AFTER
		//	makeSolutionLetterString completed

		// solve the puzzle or show solution if already gave up
		// repeat for a new puzzle if desired
		playAgain = solvePuzzle(quitEarly, solutionLetters, answerInfo);

	// V. remove multi-line comment completely once blankLetters function done...
	}	while (playAgain);	// end of game repeat loop

	cout << "Thanks for playing and keep jumbling!" << endl;
	return 0;
}


/* Returns true if all anagrams are correctly solved by the user and false if the user
	elects to quit before solving all of them.
*/
bool solveAnagrams(const string words[], const string markedWords[]) {
	string anagrams[NUMBER_OF_WORDS], wordGuess, discard;
	int wordNumber, correctWordCount = 0;
	bool giveUp = false;
	int i = 0;
	bool wordsCor[NUMBER_OF_WORDS] = {false};
	int check = 0;
	string holdWords[4];
	int count = 0;
	// NOTE: change giveUp to false when you want to play.  Change to false if you just
	// want everything to display without you providing any guesses/input

	// make anagrams of puzzle words
	anagramWords(words, anagrams);


	do {
		cout << endl;
		displayWordList(anagrams);
		cout << endl;

		// prompt user for word to guess - can the user quit here too?
		// ensure guess number in valid range
		do {
			cout << "Please enter number of word to guess and guess separated by one or more spaces"
				 << endl << ", e.g. 2 hello (enter 0 to give up): ";
			cin >> wordNumber;
			// check if gave up in which case will be no word to read
			if (wordNumber != 0) {
				cin >> wordGuess;
			} else {
				cout << "Giving up.  Unguessed words marked with an " << UNGUESSED_MARK << endl;
				giveUp = true;
			}
		} while (!giveUp && (wordNumber < 1 || wordNumber > NUMBER_OF_WORDS));

		// skip remainder of code in this loop if user gave up and exit the loop at the top
		if (!giveUp) {
			// read and check correctness of their guess
			wordNumber--;    // adjust downward to use as array index

			// if guess correct replace anagram of guessed word with upcase version of the
			// unscrambled word
			if (wordsCor[wordNumber] != true)
			{

			if (wordGuess == words[wordNumber]) {
				cout << "Correct guess for anagram " << anagrams[wordNumber] << endl;
				anagrams[wordNumber] = markedWords[wordNumber];
				holdWords[check] = markedWords[wordNumber];
				check++;
				wordsCor[wordNumber] = true;
				correctWordCount++;
			} else {
				cout << "Incorrect guess for anagram " << anagrams[wordNumber] << endl;
			}
		} else{cout << "Stop cheating" << endl;}

	}


	} while (!giveUp && correctWordCount < NUMBER_OF_WORDS);

	// clear newline after final response
	getline(cin, discard);
	for(int i = 0; i < NUMBER_OF_WORDS; i++)
	{
		if(wordsCor[i] == true)
		{
			cout << words[i] << "[-]" << endl;
		}
		else
		{
			cout << words[i] << "[X]" << endl;
		}

	}


	// 2. ADD YOUR CODE HERE
	// if user gave up set unsolved anagrams to markedWords add visual clue, e.g. add
	// UNGUESSED_MARK or show both anagram and solution, for anagrams user didn't sol


	return !giveUp;
}



/* Allow user to solve final riddle part of the puzzle.  If they gave up previously as
	indicated by quitEarly flag or give up during final part show the answer.  Returns true
	if user would like to play again and attempt to solve new puzzle and false if not.
*/
bool solvePuzzle(bool quitEarly, string solutionLetters, const string answerInfo[]) {
	if (!quitEarly) {
		cout << "All anagrams solved.  Onto the final challenge ..." << endl;
	}

	// show solution letters and remainder of puzzle regardless of early quit or not
	string solutionTemplate = blankLetters(answerInfo[ANS_SOLUTION_INDEX], ANSWER_BLANK);
	displayAnswerInfo(solutionLetters, solutionTemplate, answerInfo);

	// Solve the puzzle if user hasn't previously given up
	string solutionGuess = "jumble!";
	// keep prompting user until proper completion of answer provided or signal quit
	while (!quitEarly && solutionGuess != answerInfo[ANS_SOLUTION_INDEX])
	{
		cout << endl << "Please enter you answer guess including spaces and punctuation"
			<< endl << "or press ENTER to quit: ";
		getline(cin, solutionGuess);
		// if just return set quit early flag to true
		quitEarly = solutionGuess.length() == 0;
	}

	// show different final message depending on whether user quit or
	// succeessfully completed final question/challenge
	if (quitEarly) {
		cout << "The correct answer is: " << answerInfo[ANS_SOLUTION_INDEX]
			<< "\nBetter luck next time!" << endl;
	}
	else {
		cout << "Congratulations, you got it!" << endl;
	}

	// ask to play again - move to separate function?
	cout << endl << "Care to try another puzzle? (Y or ENTER/N): ";
	string repeatAnswer;
	getline(cin,repeatAnswer);

	if (repeatAnswer.length() == 0 || toupper(repeatAnswer[0]) == 'Y') {
		return true;
	}
	else
		return false;
}



/*
	Display given list of words preceded by word number.
*/
void displayWordList(const string wordList[]) {
	for (int i = 0; i < NUMBER_OF_WORDS; i++) {
		// read number of letters used from this word
		cout << i+1 << ":\t" << wordList[i] << endl;
	}
}

/*
	Displays letters user will use to solve final challenge, template of words to make with
	those letters and the context and clue for the puzzle to solve.  Feel free to change
	the layout manner of presentation to your liking...
*/
void displayAnswerInfo(string solutionLetters, string solutionTemplate,
	const string answerInfo[]) {
	cout << endl;
	cout << "Context:\t" << answerInfo[ANS_CONTEXT_INDEX] << endl;
	cout << "Setup:\t" << answerInfo[ANS_PROMPT_INDEX] << "--" << endl;
	cout << "Answer:\t" << solutionTemplate
		<<  endl << endl;
	cout << "Letters:\t" << solutionLetters << endl;
}

/**** Functions you must complete are BELOW HERE ******/

/* Loads all the data for a puzzle from the given already opened inFileStream.
	Data will be loaded into various array parameters.  inFileStream will remain open
	on exiting the function.

	INPUTS:
	------
	inStream - reference to already open stream to read from
	wordList - array into which the first NUMBER_OF_WORDS words from inStream will be read
	positions - NUMBER_OF_WORDS by MAX_LETTER_FROM_WORD 2-D array into which the significant
		positions for each word will be read.  Note positions read from file count from 1.
	numPositions - since variable number positions bewteen 1 and MAX_LETTER_FROM_WORD
		may be read for each word, actual number positions read stored in this array
	answerInfo - holds the NUM_ANSWER_PARTS lines related to the solution/answer.
*/
void loadPuzzleData(ifstream &inStream, string wordList[],
	int positions[][MAX_LETTER_FROM_WORD], int numPositions[], string answerInfo[])
{
	int i = 0;
    int count = 0;
    int number = 0;
	int holder = 0;
    for(i = 0; i < NUMBER_OF_WORDS; i++)
    {
        getline(inStream, wordList[i]);
    }

    for(i = 0; i < NUMBER_OF_WORDS; i++)
    {
        count = 0;
        while(inStream.peek() != EOL && count < MAX_LETTER_FROM_WORD)
        {
			inStream >> holder;
			//Holder - 1 makes it so the it is the position in the array
             positions[i][count] = holder-1;
             count++;
        }
       //Ignores the NULL char
        inStream.ignore();
        numPositions[i] = count;
    }
    while(number < NUM_ANSWER_PARTS)
    {
        getline(inStream, answerInfo[number]);
        number++;
    }
}

/*
	Display puzzle for user in form for solving.  This is used only as a quick way to check
	that data loaded correctly.  It takes the same parameters as loadPuzzleData
*/
void displayPuzzle(const string words[],
	const int letterPositions[][MAX_LETTER_FROM_WORD], const int numPositions[],
	const string answerInfo[])
{
	int i = 0;
    int count = 0;
    int number = 0;
    for(i = 0; i < NUMBER_OF_WORDS; i++)
    {
        cout << words[i] << endl;
    }

    for(i = 0; i < NUMBER_OF_WORDS; i++)
    {
        count = 0;
        while(count < numPositions[i])
        {
			//To makes it rematch the data files positions
            cout << letterPositions[i][count]+1;
            count++;
        }
        cout << endl;
    }
    while(number < NUM_ANSWER_PARTS)
    {
        cout << answerInfo[number] << endl;
        number++;
    }

	cout << "(Display list of anagrammed/solved words here.)" << endl;

	cout << "Context: " << answerInfo[ANS_CONTEXT_INDEX] << endl;
	cout << "Clue: " << answerInfo[ANS_PROMPT_INDEX] << endl;
	cout << "Ans: " << blankLetters(answerInfo[ANS_SOLUTION_INDEX], ANSWER_BLANK)
		<<  endl;
}

/* Changes to upper case the indicated positions of all words in wordList and stores
	results in markedWords.  Both arrays are assumed to be of size NUMBER_OF_WORDS.

	INPUTS:
	wordlist - string to transform
	positionsList - list of postions in the string to upcase with ? denoting the first pos.
		in source
	numPositions - the number of entries beginning with the 0 entry in positions to use
	markedWords - will contain the mixed case versions of words in word list at end.
*/
void upcaseWords(const string wordlist[], const int positions[][MAX_LETTER_FROM_WORD],
	const int numPositions[], string markedWords[], const string answerInfo[])
{
	string upper;
	for(int i = 0; i < NUMBER_OF_WORDS; i++)
	{
		upper = wordlist[i];
		for(int x = 0; x < numPositions[i]; x++)
		{
			upper.at(positions[i][x]) = toupper(wordlist[i].at(positions[i][x]));
		}
		markedWords[i] = upper;
	}
}


/* Makes an anagram of each word in words and saves it in anagrams.  Both arrays should
	have size NUMBER_OF_WORDS.
*/
void anagramWords(const string words[], string anagrams[])
{
	for (int i = 0; i < NUMBER_OF_WORDS; i++) {
		anagrams[i] = makeAnagram(words[i]);
	}
}


/* Generates and returns a random permutation/anagram of given input string source
	INPUTS:
		source - the string to make a permuted copy of
*/
string makeAnagram(string source) {
	string newScram;
	int length = 0;
	int ran = 0;
	length = source.size();
	for(int i = 0; i < length; i++)
	{
		ran = rand()%(length-i);
		newScram += source[ran];
		source.erase(ran,1);
	}

	source = newScram;
	return source;
}


/* Returns a string of all lowercase letters from the specified positions in each word.
	Format the string so that the list of all consonants appears first
	separated by a space or other character and followed by the list of all vowels.
*/
string makeSolutionLetterString(const string wordList[],
	const int positions[][MAX_LETTER_FROM_WORD], const int numPositions[], string markedWords[])
{
	string upper;
	for(int i = 0; i < NUMBER_OF_WORDS; i++)
	{
		upper = wordList[i];
		for(int x = 0; x < numPositions[i]; x++)
		{
			upper.at(positions[i][x]) = toupper(wordList[i].at(positions[i][x]));
		}
	}
	return "";
}


/*	Returns a new string of the same length as the source string with all lowercase
	and uppercase alphabetic characters replaced by the given blankChar.
*/
string blankLetters(string source, char blankChar) {
	blankChar = '#';

	for(int i = 0; i < source.size(); i++)
	{
		if(source[i] <= 122 && source[i] >= 97 || source[i] <= 90 && source[i] >= 65)
		{
			source[i] = blankChar;
		}
	}
	return source;
}


