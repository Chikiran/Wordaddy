#include "headerer.h"

void cnb_menu() {
    while (true) {
        cnb_header();
        std::vector<std::string> options = { "[1] Play Cows and Bulls", "[2] How to play", "[3] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");

        switch (choice) {
        case 1:
            cnb_mode_select();
            break;
        case 2:
            cnb_instructions();
            continue;
        case 3:
            return;
        }
    }
}

void cnb_instructions() {
    cnb_header();
    std::vector<std::string> instructions = {
    "1. Start: Choose difficulty level, determining word length.",
    "2. Guess: Attempt to find the randomly selected word.",
    "3. Bulls: Post-guess, number of bulls (correct letters in right positions) is given.",
    "4. Cows: Number of cows (correct letters in wrong positions) is also provided.",
    "5. Continue: Keep guessing until word is found. Incorrect guesses increase counter.",
    "6. Win: After correct guess, play again, change difficulty, or exit."
    };
    output_menu_options(instructions);
    system("pause");
}

void cnb_mode_select() {
    while (true) {
        cnb_header();
        std::vector<std::string> options = { "[1] Singleplayer", "[2] vs AI", "[3] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");

        switch (choice) {
        case 1:
            centralize_text("Starting a Singleplayer session...");
            loading_bar();
            cnb_singleplayer();
            break;
        case 2:
            centralize_text("Starting a vs AI session...");
            loading_bar();
            cnb_vs_ai();
            break;
        case 3:
            return;
        }
    }
}

void play_cnb_round(Difficulty difficulty) {
    std::pair<int, int> wordLength = get_word_length(difficulty);
    std::string targetWord = get_random_password(wordLength);

    cnb_header();
    std::string tmp = "The target password has been selected. It has " + std::to_string(targetWord.length()) + " digits. Goodluck!";
    centralize_text(tmp);
    centralize_text("[D] Change difficulty | [N] New word | [B] Back");
    std::cout << "\n";
    int guessCounter = 1;
    while (true) {
        std::string userGuess = cnb_user_guess(guessCounter, targetWord, difficulty, wordLength);
        if (userGuess == targetWord) {
            std::string tmp = "You're correct! The password is " + targetWord;
            centralize_text(tmp);
            break;
        }
        else if (userGuess == "") {
            return;
        }
        std::pair<int, int> cowsAndBulls = get_cows_and_bulls(userGuess, targetWord);
        std::string cowsAndBullsStr = "Cows: " + std::to_string(cowsAndBulls.first) + " | Bulls: " + std::to_string(cowsAndBulls.second);
        centralize_text(cowsAndBullsStr);
        guessCounter++;
    }
}

void cnb_singleplayer() {
    cnb_header();
    Difficulty difficulty = get_word_difficulty();
    while (true) {
        play_cnb_round(difficulty);

        std::vector<std::string> options = { "[1] Play again", "[2] Change Difficulty", "[3] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");
        std::string str;
        switch (choice) {
        case 1:
            cnb_header();
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

std::string cnb_user_guess(int guessCounter, std::string& targetWord, Difficulty& difficulty, std::pair<int, int>& wordLength) {
    std::string userGuess;

    while (true) {
        std::string tmp = "Guess " + std::to_string(guessCounter) + ": ";
        centralize_text(tmp, targetWord.length()/2, false);
        std::getline(std::cin, userGuess);
        to_upper(userGuess);

        // Check if the user wants to generate a new word or change difficulty
        if (userGuess == "N") {
            targetWord = get_random_password(wordLength);
            cnb_header();
            std::string nwStr = "A new password has been generated! It has " + std::to_string(targetWord.length()) + " digits. Goodluck!";
            centralize_text(nwStr);
            centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
            continue;
        }
        else if (userGuess == "D") {
            cnb_header();
            difficulty = get_word_difficulty();
            std::pair<int, int> wordLength = get_word_length(difficulty);
            targetWord = get_random_password(wordLength);
            cnb_header();
            std::string diffStr = "Difficulty changed! The new password has " + std::to_string(targetWord.length()) + " digits. Goodluck!";
            centralize_text(diffStr);
            centralize_text("[D] Change difficulty | [N] New word | [B] Back");

            continue;
        }
        else if (userGuess == "B") {
            return "";
        }

        // Check the guess length
        if (userGuess.length() != targetWord.length()) {
            std::string igStr = "Invalid guess! The target password has " + std::to_string(targetWord.length()) + " digits.";
            centralize_text(igStr);
            std::cout << "\n";
            //system("pause");
            continue;
        }
        
        // Check if the guess contains only letters
        bool allNums = true;
        for (char c : userGuess) {
            if (!isdigit(c)) {
                allNums = false;
                break;
            }
        }

        if (!allNums) {
            centralize_text("Invalid guess! Please enter only numbers.");
            std::cout << "\n";
            //system("pause");
            continue;

        }
        return userGuess;
    }
}

std::pair<int, int> get_cows_and_bulls(const std::string& guess, const std::string& targetWord) {
    int bulls = 0;
    int cows = 0;
    std::vector<int> target_frequency(10, 0);
    std::vector<int> guess_frequency(10, 0);

    for (int i = 0; i < guess.size(); ++i) {
        int target_digit = targetWord[i] - '0';
        int guess_digit = guess[i] - '0';

        if (target_digit == guess_digit) {
            ++bulls;
        }
        else {
            ++target_frequency[target_digit];
            ++guess_frequency[guess_digit];
        }
    }

    for (int i = 0; i < 10; ++i) {
        cows += std::min(target_frequency[i], guess_frequency[i]);
    }

    return std::make_pair(cows, bulls);
}

void cnb_ai_check_guess(const std::string& userGuess, const std::string& aiGuess, const std::string& userTargetWord, const std::string& aiTargetWord,std::set<char>& aiIncorrectLetters) {
    std::pair<int, int> userCowsAndBulls = get_cows_and_bulls(userGuess, userTargetWord);
    std::pair<int, int> aiCowsAndBulls = get_cows_and_bulls(aiGuess, aiTargetWord);

    if (aiCowsAndBulls.first == 0 && aiCowsAndBulls.second == 0) {
        for (char letter : aiGuess) {
            aiIncorrectLetters.insert(letter);
        }
    }

    std::string userCnB = "Cows: " + std::to_string(userCowsAndBulls.first) + " | Bulls: " + std::to_string(userCowsAndBulls.second);
    std::string aiCnB = "Cows: " + std::to_string(aiCowsAndBulls.first) + " | Bulls: " + std::to_string(aiCowsAndBulls.second);
    print_split_prompt(userCnB, aiCnB, true, 3, 10);
    std::cout << "\n";
}

std::string get_cnb_ai_guess(Difficulty aiDifficulty, const std::string& aiTargetWord, const std::set<char>& aiIncorrectLetters, std::pair<int,int> aiCowsAndBulls, std::vector<std::string>& pastGuesses, std::string& blacklistedNumbers, std::vector<std::string>& all_answers, std::string& last_guess) {
    std::string aiGuess;
    switch (aiDifficulty) {
    case Difficulty::EASY:
        aiGuess = cnb_easy_ai(aiTargetWord.length());
        break;
    case Difficulty::MEDIUM:
        aiGuess = cnb_medium_ai(aiTargetWord.length(), pastGuesses);
        break;
    case Difficulty::HARD:
        aiGuess = cnb_hard_ai(aiTargetWord.length(), aiCowsAndBulls, pastGuesses, blacklistedNumbers);
        break;
    case Difficulty::INSANE:
        aiGuess = cnb_insane_ai(aiTargetWord.length(), aiCowsAndBulls,all_answers, last_guess);
        break;
    case Difficulty::SOULSLIKE:
        aiGuess = cnb_soulslike_ai(aiTargetWord);
    }
    return aiGuess;
}

std::string cnb_vs_ai_guess(int guessCounter, const std::string& userTargetWord, Difficulty wordDifficulty, std::pair<int, int> wordLength, const std::string& aiGuess) {
    std::string userGuess;
    while (true) {
        std::string tmp = "Guess " + std::to_string(guessCounter) + ": ";
        std::string aitmp = "AI Guess " + std::to_string(guessCounter) + ": " + aiGuess;

        print_split_prompt(tmp, aitmp, true, 10 + userTargetWord.length(), 10);

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
            std::string igStr = "Invalid guess! The target password has " + std::to_string(userTargetWord.length()) + " digits.";
            print_split_prompt(igStr, "", true, 1, 1);
            std::cout << "\n";
            print_split_prompt("", "", true, 1, 1);
            std::cout << "\n";
            continue;
        }

        // Check if the guess contains only alphabets
        bool allNums = true;
        for (char c : userGuess) {
            if (!isdigit(c)) {
                allNums = false;
                break;
            }
        }

        if (!allNums) {
            print_split_prompt("Invalid guess! Please enter only numbers.", "", true, 1, 1);
            std::cout << "\n";
            print_split_prompt("", "", true, 1, 1);
            std::cout << "\n";
            continue;
        }
        return userGuess;
    }
}

void cnb_vs_ai_round(Difficulty aiDifficulty, Difficulty wordDifficulty) {
    std::pair<int, int> wordLength = get_word_length(wordDifficulty);
    std::string userTargetWord = get_random_password(wordLength);
    std::string aiTargetWord = get_random_password(wordLength);

    cnb_header();
    std::string tmp = "The password has been selected. It has " + std::to_string(userTargetWord.length()) + " digits. Goodluck!";
    centralize_text(tmp);
    centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
    std::cout << "\n";
    //std::cout << userTargetWord << std::endl;

    std::set<char> aiIncorrectLetters;
    int guessCounter = 1;
    std::pair<int, int> aiCowsAndBulls;
    std::vector<std::string> pastGuesses;
    std::string blacklistedNumbers;
    std::vector<std::string> all_answers;
    std::string aiGuess, last_guess;

    while (true) {
        // AI's guess
        std::string aiGuess = get_cnb_ai_guess(aiDifficulty, aiTargetWord, aiIncorrectLetters,aiCowsAndBulls, pastGuesses, blacklistedNumbers, all_answers, last_guess);
        aiCowsAndBulls = get_cows_and_bulls(aiGuess, aiTargetWord);
        // User's guess
        std::string userGuess = cnb_vs_ai_guess(guessCounter, userTargetWord, wordDifficulty, wordLength, aiGuess);

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
            tmp = "A new password has been generated! It has " + std::to_string(userTargetWord.length()) + " digits. Goodluck!";
            centralize_text(tmp);
            centralize_text("[D] Change difficulty | [N] New word | [B] Stop game");
            continue;
        }
        else if (userGuess == "B") {
            return;
        }

        // Check guesses
        cnb_ai_check_guess(userGuess, aiGuess, userTargetWord, aiTargetWord, aiIncorrectLetters);
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

void cnb_vs_ai() {
    cnb_header();
    Difficulty aiDifficulty = get_cnb_ai_difficulty();
    while (true) {
        cnb_header();
        Difficulty wordDifficulty = get_word_difficulty();
        cnb_vs_ai_round(aiDifficulty, wordDifficulty);

        std::vector<std::string> options = { "[1] Play again", "[2] Change Difficulty", "[3] Change AI Difficulty", "[4] Back" };
        output_menu_options(options);
        int choice = validate_choice(1, 4, "Option: ");
        std::string str;
        switch (choice) {
        case 1:
            break;
        case 2:
            cnb_header();
            wordDifficulty = get_word_difficulty();
            break;
        case 3:
            cnb_header();
            aiDifficulty = get_cnb_ai_difficulty();
            break;
        case 4:
            return;
        }
    }
}

Difficulty get_cnb_ai_difficulty() {
    centralize_text("Choose AI difficulty: ");
    std::vector<std::string> options = { "[1] Easy: Makes random guesses", "[2] Medium: Avoids previous guesses", "[3] Hard: Improves overtime", "[4] Insane: Learns and improves quicker", "[5] Souls-like: Do you even want to try?"};
    output_menu_options(options);

    int choice = validate_choice(1, 5, "Option: ");

    Difficulty difficulty[5] = { Difficulty::EASY, Difficulty::MEDIUM, Difficulty::HARD, Difficulty::INSANE, Difficulty::SOULSLIKE };
    return difficulty[choice - 1];
}

std::string get_random_password(std::pair<int, int> wordLength) {
    std::vector<char> numbers{ '0', '1', '2', '3', '4', '5', '6', '7', '8', '9' };
    // Create a random device and a default random engine
    std::random_device rd;
    std::default_random_engine engine(rd());

    // Shuffle the numbers
    std::shuffle(numbers.begin(), numbers.end(), engine);

    // Generate a random length within the specified range
    std::uniform_int_distribution<int> lengthDist(wordLength.first, wordLength.second);
    int length = lengthDist(engine);

    // Convert the first 'length' numbers to a string
    std::string password(numbers.begin(), numbers.begin() + length);

    return password;
}

std::string cnb_easy_ai(int wordLength) {
    std::string numbers = "0123456789";

    std::random_device rd;
    std::default_random_engine engine(rd());

    std::shuffle(numbers.begin(), numbers.end(), engine);

    std::string guess = numbers.substr(0, wordLength);

    return guess;
}

std::string cnb_medium_ai(int wordLength, std::vector<std::string>& pastGuesses) {
    std::string numbers = "0123456789";

    std::random_device rd;
    std::default_random_engine engine(rd());

    std::string guess;
    do {
        std::shuffle(numbers.begin(), numbers.end(), engine);

        guess = numbers.substr(0, wordLength);
    } while (std::find(pastGuesses.begin(), pastGuesses.end(), guess) != pastGuesses.end());

    // add the new guess to the past guesses
    pastGuesses.push_back(guess);

    return guess;
}

std::string cnb_hard_ai(int wordLength, std::pair<int, int> cowsAndBulls, std::vector<std::string>& pastGuesses, std::string& blacklistedNumbers) {
    std::string numbers = "0123456789";

    // Create a random device and a default random engine
    std::random_device rd;
    std::default_random_engine engine(rd());

    std::string guess;
    do {
        // Shuffle the numbers
        std::shuffle(numbers.begin(), numbers.end(), engine);

        // Take the first 'wordLength' numbers as the guess
        guess = numbers.substr(0, wordLength);
    } while (std::find(pastGuesses.begin(), pastGuesses.end(), guess) != pastGuesses.end());

    // Add the new guess to the past guesses
    pastGuesses.push_back(guess);

    // If the guess had no cows or bulls, add the numbers to the blacklist
    if (cowsAndBulls.first == 0 && cowsAndBulls.second == 0) {
        blacklistedNumbers += guess;
    }

    // If a cow is detected, replace the first half of the guess with new, non-blacklisted numbers
    if (cowsAndBulls.first > 0) {
        for (int i = 0; i < guess.size() / 2; ++i) {
            char newNumber;
            do {
                newNumber = '0' + rd() % 10;
            } while (blacklistedNumbers.find(newNumber) != std::string::npos);
            guess[i] = newNumber;
        }
        // Shuffle the numbers within their container
        std::shuffle(guess.begin(), guess.end(), engine);
    }

    // Remove blacklisted numbers from the string
    for (int i = 0; i < blacklistedNumbers.size(); ++i) {
        char num = blacklistedNumbers[i];
        numbers.erase(std::remove(numbers.begin(), numbers.end(), num), numbers.end());
    }

    return guess;
}

struct pair_hash {
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p) const {
        auto h1 = std::hash<T1>{}(p.first);
        auto h2 = std::hash<T2>{}(p.second);

        return h1 ^ h2;
    }
};

void generate_all_answers(std::vector<std::string>& all_answers, std::string answer = "", std::string digits = "0123456789") {
    if (answer.size() == 4) {
        all_answers.push_back(answer);
    }
    else {
        for (int i = 0; i < digits.size(); ++i) {
            char digit = digits[i];
            std::string remaining_digits = digits.substr(0, i) + digits.substr(i + 1);
            generate_all_answers(all_answers, answer + digit, remaining_digits);
        }
    }
}

std::pair<int, int> count_bulls_and_cows(const std::string& guess, const std::string& other_guess) {
    int bulls = 0;
    int cows = 0;
    for (int i = 0; i < guess.size(); ++i) {
        if (guess[i] == other_guess[i]) {
            ++bulls;
        }
        else if (other_guess.find(guess[i]) != std::string::npos) {
            ++cows;
        }
    }
    return std::make_pair(cows, bulls);
}

std::string cnb_insane_ai(int wordLength, std::pair<int, int> cowsAndBulls, std::vector<std::string>& all_answers, std::string& last_guess) {

    if (all_answers.empty()) {
        generate_all_answers(all_answers);
        std::random_shuffle(all_answers.begin(), all_answers.end());
    }

    if (!last_guess.empty()) {
        std::vector<std::string> consistent_answers;
        for (const auto& answer : all_answers) {
            if (count_bulls_and_cows(last_guess, answer) == cowsAndBulls) {
                consistent_answers.push_back(answer);
            }
        }
        all_answers = consistent_answers;
    }

    std::string next_guess = all_answers.front();
    int min_max_remaining = all_answers.size();
    for (const auto& guess : all_answers) {
        std::unordered_map<std::pair<int, int>, int, pair_hash> score_map;
        for (const auto& answer : all_answers) {
            std::pair<int, int> score = count_bulls_and_cows(guess, answer);
            ++score_map[score];
        }
        int max_score = 0;
        for (const auto& pair : score_map) {
            max_score = std::max(max_score, pair.second);
        }
        if (max_score < min_max_remaining) {
            min_max_remaining = max_score;
            next_guess = guess;
        }
    }

    last_guess = next_guess;
    return last_guess;
}

std::string cnb_soulslike_ai(std::string aiTargetWord){
    return aiTargetWord;
}
