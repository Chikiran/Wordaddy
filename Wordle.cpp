#include "headerer.h"

void wordle_menu() {
    int choice;
    while (true) {
        wordle_header();
        std::vector<std::string> options = { "[1] Play Wordle", "[2] How to play", "[3] Back" };
        output_menu_options(options);
        choice = validate_choice(1, 3, "Option: ");

        switch (choice) {
        case 1:
            centralize_text("Starting Wordle...");
            loading_bar();
            wordle_mode_select();
            break;
        case 2:
            wordle_header();
            wordle_instructions();
        case 3:
            return;
        }
    }
}


void wordle_instructions() {
    wordle_header();
    std::vector<std::string> instructions = {
    "1. Start: Run the program and select 'Play Wordle'.",
    "2. Difficulty: Choose Easy (3-4 letters), Medium (5-6 letters), or Hard (7+ letters).",
    "3. Guess: Guess the word. Blocks represent letters.",
    "4. Input: Type your guess and press Enter. It should match the target word's length.",
    "5. Check: Your guess is checked.",
    "  - Green: Correct letter and position.",
    "  - Yellow: Correct letter, wrong position.",
    "  - White: Incorrect letter.",
    "6. Options: During guessing, enter ‘D’ to change difficulty, or ‘N’ for a new word.",
    "7. Win: Game continues until you guess correctly."
    };
    output_menu_options(instructions);
    system("pause");
}

void wordle_mode_select() {
    while (true) {
        wordle_header();
        std::vector<std::string> options = { "[1] Singleplayer", "[2] vs AI", "[3] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");

        switch (choice) {
        case 1:
            centralize_text("Starting a Singleplayer session...");
            loading_bar();
            wordle_singleplayer();
            break;
        case 2:
            centralize_text("Starting a vs AI session...");
            loading_bar();
            wordle_vs_ai();
            break;
        case 3:
            return;
        }
    }
}

std::string get_random_word(std::pair<int, int> wordLength) {
    std::ifstream file(wordListFile);
    std::string line;
    std::vector<std::string> words;

    while (std::getline(file, line)) {
        if (line.length() >= wordLength.first && line.length() <= wordLength.second) {
            words.push_back(line);
        }
    }

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> range(0, words.size() - 1);

    int randomIndex = range(engine);

    to_upper(words[randomIndex]);
    return words[randomIndex];
}


void wordle_singleplayer() {
    wordle_header();
    Difficulty difficulty = get_word_difficulty();

    while (true) {
        play_wordle_round(difficulty);

        std::vector<std::string> options = { "[1] Play again", "[2] Change Difficulty", "[3] Exit" };
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");

        switch (choice) {
        case 1:
            wordle_header();
            break;
        case 2:
            cnb_header();
            difficulty = get_word_difficulty();
            break;
        case 3:
            return;
        }
    }
    
}

void play_wordle_round(Difficulty difficulty) {
    std::pair<int, int> wordLength = get_word_length(difficulty);
    std::string targetWord = get_random_word(wordLength);

    wordle_header();
    centralize_text("The target word has been selected, Goodluck!");
    centralize_text("[D] Change difficulty | [N] New word | [B] Back");
    std::cout << "\n";
    int guessCounter = 1;


    while (true) {
        std::string userGuess = wordle_user_guess(guessCounter, targetWord, difficulty, wordLength);
        if (userGuess == targetWord) {
            std::string tmp = "You're correct! The Word is " + targetWord;
            centralize_text(tmp);
            break;
        }
        else if (userGuess == "") {
            return;
        }
        centralize_text(" ", targetWord.length() + 1, false);
        std::vector<std::pair<char, std::string>> result = check_wordle_guess(userGuess, targetWord);
        print_colored_text(result);
        std::cout << "\n";
        guessCounter++;
    }
}

std::string wordle_user_guess(int guessCounter, std::string& targetWord, Difficulty& difficulty, std::pair<int, int>& wordLength) {
    std::string userGuess;
    const char emptyBlock = 177;  // Light shade block

    while (true) {
        // adds light blocks as indicator for user input
        std::string guessTmp = "Guess " + std::to_string(guessCounter) + ": ";
        for (int i = 0; i < targetWord.length(); i++) {
            guessTmp += emptyBlock;
        }
        centralize_text(guessTmp, targetWord.length()/2, false);
        move_cursor(targetWord.length());
        std::cin >> userGuess;
        to_upper(userGuess);


        // Check if the user wants to generate a new word or change difficulty
        if (userGuess == "N") {
            targetWord = get_random_word(wordLength);
            wordle_header();
            std::string nwStr = "A new word has been generated! It has " + std::to_string(targetWord.length()) + " letters. Goodluck!";
            centralize_text(nwStr);
            centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
            continue;
        }
        else if (userGuess == "D") {
            wordle_header();
            difficulty = get_word_difficulty();
            std::pair<int, int> wordLength = get_word_length(difficulty);
            targetWord = get_random_word(wordLength);
            wordle_header();
            std::string diffStr = "Difficulty changed! The new word has " + std::to_string(targetWord.length()) + " letters. Goodluck!";
            centralize_text(diffStr);
            centralize_text("[D] Change difficulty | [N] New word | [B] Back");

            continue;
        }
        else if (userGuess == "B") {
            return "";
        }

        // Check the guess length
        if (userGuess.length() != targetWord.length()) {
            std::string igStr = "Invalid guess! The target word has " + std::to_string(targetWord.length()) + " letters.";
            centralize_text(igStr);
            std::cout << "\n";
            //system("pause");
            continue;
        }
        // Check if the guess contains only letters
        bool allAlpha = true;
        for (char c : userGuess) {
            if (!isalpha (c)) {
                allAlpha = false;
                break;
            }
        }

        if (!allAlpha) {
            centralize_text("Invalid guess! Please enter only letters.");
            std::cout << "\n";
            //system("pause");
            continue;

        }
        return userGuess;
    }
}



std::vector<std::pair<char, std::string>> check_wordle_guess(std::string userGuess, std::string targetWord) {
    std::vector<std::pair<char, std::string>> result;
    std::vector<bool> matched(targetWord.size(), false);  // This'll keep track of matched letters

    for (int i = 0; i < userGuess.length(); i++) {
        if (i < targetWord.length() && userGuess[i] == targetWord[i]) {
            result.push_back(std::make_pair(userGuess[i], "Correct"));
            matched[i] = true;  // Flag the matched letter so that it doesn't repeat
        }
        else {
            bool isMisplaced = false;
            for (int j = 0; j < targetWord.length(); j++) {
                if (userGuess[i] == targetWord[j] && !matched[j]) {
                    result.push_back(std::make_pair(userGuess[i], "Misplaced"));
                    matched[j] = true;
                    isMisplaced = true;
                    break;
                }
            }
            if (!isMisplaced) {
                result.push_back(std::make_pair(userGuess[i], "Wrong"));
            }
        }
    }
    return result;
}





void wordle_vs_ai_round(Difficulty aiDifficulty, Difficulty wordDifficulty) {
    std::pair<int, int> wordLength = get_word_length(wordDifficulty);
    std::string userTargetWord = get_random_word(wordLength);
    std::string aiTargetWord = get_random_word(wordLength);
    
    wordle_header();
    std::string tmp = "Your word has been selected! It has " + std::to_string(userTargetWord.length()) + " letters. Goodluck!";
    centralize_text(tmp);
    centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
    std::cout << "\n";
    //std::cout << userTargetWord << std::endl;

    int guessCounter = 1;
    std::vector<std::pair<char, std::string>> userResult;
    std::vector<std::pair<char, std::string>> aiResult;
    std::string blacklistedLetters = "";
    std::vector<std::string> listWords;
    while (true) {
        // AI's guess
        std::string aiGuess = get_wordle_ai_guess(aiDifficulty, aiTargetWord, aiResult, blacklistedLetters, listWords);
        aiResult = check_wordle_guess(aiGuess, aiTargetWord);
        for (auto& pair : aiResult) {
            if (pair.second == "Wrong") {
                if (blacklistedLetters.find(pair.first) == std::string::npos) {
                    blacklistedLetters += pair.first;
                }
            }
        }
        std::string userGuess = wordle_vs_ai_guess(guessCounter, userTargetWord, wordDifficulty, wordLength, aiGuess);

        if (userGuess == "N" || userGuess == "D") {
            if (userGuess == "N") {
                userTargetWord = get_random_password(wordLength);;
            }
            else {
                cnb_header();
                wordDifficulty = get_word_difficulty();
                wordLength = get_word_length(wordDifficulty);
                userTargetWord = get_random_password(wordLength);
            }
            cnb_header();
            tmp = "A new word has been generated! It has " + std::to_string(userTargetWord.length()) + " letters. Goodluck!";
            centralize_text(tmp);
            centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
            continue;
        }
        else if (userGuess == "B") {
            return;
        }

        // Check guesses
        wordle_ai_check_guess(userGuess, aiGuess, userTargetWord, aiTargetWord);
        std::string drawStr = R"del(
         ___  ___    ___      ___ 
        |   \| _ \  /_\ \    / / |
        | |) |   / / _ \ \/\/ /|_|
        |___/|_|_\/_/ \_\_/\_/ (_)

        )del";

        std::string winStr[] = {
            R"( __      _____ _  _ _  _ ___ ___ _ )",
            R"( \ \    / /_ _| \| | \| | __| _ \ |)",
            R"(  \ \/\/ / | || .` | .` | _||   /_|)",
            R"(   \_/\_/ |___|_|\_|_|\_|___|_|_(_))"
        };


        if (userGuess == userTargetWord && aiGuess == aiTargetWord) {
            centralize_text(drawStr);
        }
        else if (userGuess == userTargetWord) {
            for (int i = 0; i < 4; i++) {
                print_split_prompt(winStr[i], "", true);
                std::cout << "\n";
            }
            std::cout << "\n";
            break;
        }
        else if (aiGuess == aiTargetWord) {
            for (int i = 0; i < 4; i++) {
                print_split_prompt("", winStr[i], true);
                std::cout << "\n";
            }
            std::cout << "\n";
            break;
        }

        guessCounter++;
    }
}


void wordle_vs_ai() {
    wordle_header();
    Difficulty aiDifficulty = get_wordle_ai_difficulty();
    while (true) {
        wordle_header();
        Difficulty wordDifficulty = get_word_difficulty();
        wordle_vs_ai_round(aiDifficulty, wordDifficulty);

        std::vector<std::string> options = { "[1] Play again", "[2] Change Difficulty", "[3] Change AI Difficulty", "[4] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 4, "Option: ");
        std::string str;
        switch (choice) {
        case 1:
            break;
        case 2:
            wordle_header();
            wordDifficulty = get_word_difficulty();
            break;
        case 3:
            wordle_header();
            aiDifficulty = get_wordle_ai_difficulty();
            break;
        case 4:
            return;
        }
    }
}

std::string wordle_vs_ai_guess(int guessCounter, const std::string& userTargetWord, Difficulty wordDifficulty, std::pair<int, int> wordLength, const std::string& aiGuess) {
    std::string userGuess;
    while (true) {
        std::string tmp = "Guess " + std::to_string(guessCounter) + ": ";
        for (int i = 0; i < userTargetWord.length(); i++) {
			tmp += 177;
		}
        std::string aitmp = "AI Guess " + std::to_string(guessCounter) + ": " + aiGuess;

        print_split_prompt(tmp, aitmp, true, 10, 10);
        move_cursor(userTargetWord.length());
        std::getline(std::cin, userGuess);
        to_upper(userGuess);

        if (userGuess == "N" || userGuess == "D") {
            return userGuess;
        }
        else if (userGuess == "B") {
            return "B";
        }

        // Check the guess length
        if (userGuess.length() != userTargetWord.length()) {
            std::string igStr = "Invalid guess! The target word has " + std::to_string(userTargetWord.length()) + " letters.";
            print_split_prompt(igStr, "", true, 1, 1);
            std::cout << "\n";
            print_split_prompt("", "", true, 1, 1);
            std::cout << "\n";
            continue;
        }

        // Check if the guess contains only alphabets
        bool allAlpha = true;
        for (char c : userGuess) {
            if (!isalpha(c)) {
                allAlpha = false;
                break;
            }
        }

        if (!allAlpha) {
            print_split_prompt("Invalid guess! Please enter only letters.", "", true, 1, 1);
            std::cout << "\n";
            print_split_prompt("", "", true, 1, 1);
            std::cout << "\n";
            continue;
        }
        return userGuess;
    }
}
void print_colored_text(std::vector<std::pair<char, std::string>> result) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    for (auto& pair : result) {
        if (pair.second == "Correct") {
            SetConsoleTextAttribute(hConsole, 2);  // Green
        }
        else if (pair.second == "Misplaced") {
            SetConsoleTextAttribute(hConsole, 14);  // Yellow
        }
        else {  // Wrong
            SetConsoleTextAttribute(hConsole, 7);  // White
        }
        std::cout << pair.first;
    }

    SetConsoleTextAttribute(hConsole, 7);  // Reset text color to white
}

Difficulty get_wordle_ai_difficulty() {
    centralize_text("Choose AI difficulty: ");
    std::vector<std::string> options = { "[1] Easy: Makes random guesses", "[2] Medium: Avoids previous incorrect letters", "[3] Hard: Maintains correct letters", "[4] Insane: Has a dictionary of words", "[5] Souls-like: Do you even want to try?" };
    output_menu_options(options);

    int choice = validate_choice(1, 5, "Option: ");

    Difficulty difficulty[5] = { Difficulty::EASY, Difficulty::MEDIUM, Difficulty::HARD, Difficulty::INSANE, Difficulty::SOULSLIKE };
    return difficulty[choice - 1];
}

std::string get_wordle_ai_guess(Difficulty aiDifficulty, std::string aiTargetWord, std::vector<std::pair<char,std::string>>& aiResult, std::string blacklistedLetters, std::vector<std::string> listWords ){
    std::string aiGuess;
	switch (aiDifficulty) {
	case Difficulty::EASY:
		aiGuess = wordle_easy_ai(aiTargetWord.length());
		break;
	case Difficulty::MEDIUM:
		aiGuess = wordle_medium_ai(aiTargetWord.length(), blacklistedLetters);
		break;
	case Difficulty::HARD:
        aiGuess = wordle_hard_ai(aiTargetWord.length(), blacklistedLetters, aiResult);
		break;
	case Difficulty::INSANE:
        aiGuess = wordle_insane_ai(aiTargetWord.length(), aiResult, blacklistedLetters);
        break;
	case Difficulty::SOULSLIKE:
		aiGuess = wordle_soulslike_ai(aiTargetWord);
		break;
	}
	return aiGuess;
}

void print_wordle_guess(std::vector<std::pair<char, std::string>> leftResult, std::vector<std::pair<char, std::string>> rightResult, bool separator, int leftOffset, int rightOffset) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    int halfWidth = consoleWidth / 2;

    // Print the left prompt with colors
    int leftPadding = halfWidth - leftResult.size() - leftOffset;
    for (int i = 0; i < leftPadding; i++) {
        std::cout << ' ';
    }
    for (auto& pair : leftResult) {
        if (pair.second == "Correct") {
            SetConsoleTextAttribute(hConsole, 2);  // Green
        }
        else if (pair.second == "Misplaced") {
            SetConsoleTextAttribute(hConsole, 14);  // Yellow
        }
        else {  // Wrong
            SetConsoleTextAttribute(hConsole, 7);  // White
        }
        std::cout << pair.first;
    }

    // Print spaces to move the cursor to the middle of the console
    std::cout << std::string(halfWidth - leftPadding - leftResult.size(), ' ');
    SetConsoleTextAttribute(hConsole, 7);
    // Print the separator if needed
    if (separator) {
        std::cout << '|';
    }

    // Print the right prompt with colors
    int rightPadding = rightOffset;  // Calculate right padding
    for (int i = 0; i < rightPadding; i++) {
        std::cout << ' ';  // Print padding before right result
    }
    for (auto& pair : rightResult) {
        if (pair.second == "Correct") {
            SetConsoleTextAttribute(hConsole, 2);  // Green
        }
        else if (pair.second == "Misplaced") {
            SetConsoleTextAttribute(hConsole, 14);  // Yellow
        }
        else {  // Wrong
            SetConsoleTextAttribute(hConsole, 7);  // White
        }
        std::cout << pair.first;
    }


    SetConsoleTextAttribute(hConsole, 7);
}




void wordle_ai_check_guess(const std::string& userGuess, const std::string& aiGuess, const std::string& userTargetWord, const std::string& aiTargetWord) {
    std::vector<std::pair<char, std::string>> userResult = check_wordle_guess(userGuess, userTargetWord);
    std::vector<std::pair<char, std::string>> aiResult = check_wordle_guess(aiGuess, aiTargetWord);

    print_wordle_guess(userResult, aiResult, true, 10, 10);
    std::cout<< "\n";
    
}


std::string wordle_easy_ai(int length) {
    std::string random_string;
    for (int i = 0; i < length; i++) {
        char random_char = 'A' + rand() % 26;
        random_string += random_char;
    }
    return random_string;
}


std::string wordle_medium_ai(int length, std::string blacklistedLetters) {
    std::string guess;
    for (int i = 0; i < length; i++) {
        char random_char;
        do {
            random_char = 'A' + rand() % 26;
        } while (blacklistedLetters.find(random_char) != std::string::npos);
        guess += random_char;
    }
    return guess;
}


std::string wordle_hard_ai(int length, std::string blacklistedLetters, std::vector<std::pair<char, std::string>>& aiResult) {
    std::string guess(length, ' ');  // Initialize an empty guess of the correct length

    // Maintain the correct letters in their positions
    for (int i = 0; i < aiResult.size(); i++) {
        if (aiResult[i].second == "Correct") {
            guess[i] = aiResult[i].first;
        }
    }

    // Try to reuse misplaced letters
    for (int i = 0; i < aiResult.size(); i++) {
        if (aiResult[i].second == "Misplaced" && guess[i] == ' ') {
            guess[i] = aiResult[i].first;
        }
    }

    // Generate random letters for the remaining positions
    for (int i = 0; i < length; i++) {
        if (guess[i] == ' ') {  
            char random_char;
            do {
                random_char = 'A' + rand() % 26;
            } while (blacklistedLetters.find(random_char) != std::string::npos);
            guess[i] = random_char;
        }
    }

    return guess;
}




std::string wordle_insane_ai(int length, std::vector<std::pair<char, std::string>>& aiResult, std::string blacklistedLetters) {
    std::vector<std::string> possible_words;
    std::vector<std::string> filtered_words;
    std::string word;

    std::ifstream file("words.txt");

    if (!file) {
        std::cerr << "Unable to open file words.txt";
        exit(1);   // call system to stop
    }

    while (file >> word) {
        // Only consider words of the correct length
        if (word.length() == length) {
            possible_words.push_back(word);
        }
    }

    file.close();

    // Filter the words to only include those that match the current guess
    for (const std::string& word : possible_words) {
        bool matches = true;
        for (int i = 0; i < aiResult.size(); i++) {
            if (aiResult[i].second == "Correct" && word[i] != aiResult[i].first) {
                matches = false;
                break;
            }
            if (aiResult[i].second == "Wrong" && word[i] == aiResult[i].first) {
                matches = false;
                break;
            }
            if (aiResult[i].second == "Misplaced") {
                bool misplacedExists = false;
                for (int j = 0; j < word.length(); j++) {
                    if (j != i && word[j] == aiResult[i].first) {
                        misplacedExists = true;
                        break;
                    }
                }
                if (!misplacedExists) {
                    matches = false;
                    break;
                }
            }
        }
        // Check if the word contains any blacklisted letters
        for (char c : blacklistedLetters) {
            if (word.find(c) != std::string::npos) {
                matches = false;
                break;
            }
        }
        if (matches) {
            filtered_words.push_back(word);
        }
    }

    if (!filtered_words.empty()) {
        int random_index = rand() % filtered_words.size();
        return filtered_words[random_index];
    }

    return wordle_hard_ai(length, blacklistedLetters, aiResult);
}




std::string wordle_soulslike_ai(std::string aiTargetWord) {
    return aiTargetWord;
}