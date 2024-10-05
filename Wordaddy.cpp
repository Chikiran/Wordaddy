#include "headerer.h"

int main() {
    while (true) {
        wordaddy_header();
        menu_header();
        centralize_text("Welcome to Wordaddy!");
        std::vector<std::string> options = {"[1] Cows and Bulls", "[2] Wordle", "[3] Exit"};
        output_menu_options(options);
        int choice = validate_choice(1, 3, "Option: ");
        
        switch (choice) {
        case 1: cnb_menu();
			break;
        case 2: wordle_menu();
			break;
        case 3: return 0;
        }
    }
}


Difficulty get_word_difficulty() {
    centralize_text("Choose Word difficulty:");
    std::vector<std::string> options = { "[1] Easy (3-4)", "[2] Medium (5-6)", "[3] Hard (7+)" };
    output_menu_options(options);

    int choice = validate_choice(1, 3, "Option: ");

    Difficulty difficulty[3] = { Difficulty::EASY, Difficulty::MEDIUM, Difficulty::HARD };
    return difficulty[choice - 1];
}


std::pair<int, int> get_word_length(Difficulty difficulty) {
    switch (difficulty) {
    case Difficulty::EASY:
        return std::make_pair(3, 4);
    case Difficulty::MEDIUM:
        return std::make_pair(5, 6);
    case Difficulty::HARD:
        return std::make_pair(7, 10);
    }
}


int calculate_padding(int textLength, int div) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    return (consoleWidth - textLength) / div;
}


void loading_bar() {
    const char block = 219;  // Full block
    const char emptyBlock = 177;  // Light shade block

    std::random_device rd;
    std::default_random_engine engine(rd());
    std::uniform_int_distribution<int> dist_duration(100000, 300000);
    std::uniform_int_distribution<int> dist_counts(10, 20);
    std::uniform_int_distribution<int> dist_addition(2, 14);
    int duration = dist_duration(engine);
    int counts = dist_counts(engine);

    const int barLength = 60;
    int currentLength = 0;

    std::string loadingBar(barLength, emptyBlock);  // Initialize the loading bar with empty blocks

    for (int i = 0; i < counts && currentLength < barLength; i++) {
        std::this_thread::sleep_for(std::chrono::microseconds(duration));
        int addition = dist_addition(engine);
        for (int j = 0; j < addition && currentLength < barLength; j++) {
            loadingBar[currentLength] = block;
            ++currentLength;
        }
        // Calculate the necessary padding to center the loading bar
        int padding = calculate_padding(loadingBar.length());
        std::cout << "\r" << std::string(padding, ' ') << loadingBar << std::flush;
    }
}

//Centralizes argument text in the console window
void centralize_text(std::string text, int offset, bool newline) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD max_size = GetLargestConsoleWindowSize(hConsole); // measures the console window's size

    std::istringstream iss(text);
    std::string line;
    while (std::getline(iss, line)) {
        if (max_size.X > line.size()) {
            int newpos = ((max_size.X - line.size()) / 2) - offset;
            for (int i = 0; i < newpos; i++) std::cout << " ";
        }
        std::cout << line;
        if (newline) {
            std::cout << "\n";
        }
    }
}


int validate_choice(int minNum, int maxNum, std::string prompt) {

    long long number;  // Can hold higher values than int 
    while (true) {
        std::cout << "\n";
        centralize_text(prompt, 5, false);
        std::cin >> number;
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        if (std::cin.fail() || number < std::numeric_limits<int>::min() || number > std::numeric_limits<int>::max()) {
            std::cin.clear();
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            centralize_text("Invalid input! Please enter a number within the range of int.");
        }
        else if (number < minNum || number > maxNum) {
            std::string tmp = "Invalid input! Please enter a number between " + std::to_string(minNum) + " and " + std::to_string(maxNum) + ".\n";
            centralize_text(tmp);
        }
        else {
            return static_cast<int>(number);  // If the input is valid, return the number
        }
    }
}


void to_upper(std::string& string) {
    for (char& c : string) {
        c = std::toupper(c);
    }
}


void output_menu_options(const std::vector<std::string>& menuOptions) {
    // Find the length of the longest option
    size_t maxLength = 0;
    for (const auto& option : menuOptions) {
        maxLength = std::max(maxLength, option.length());
    }

    // Output the options with proper alignment
    for (const auto& option : menuOptions) {
        std::string optionText = option + std::string(maxLength - option.length(), ' ');
        centralize_text(optionText);
    }
}


void print_split_prompt(const std::string& leftPrompt, const std::string& rightPrompt, bool separator, int leftOffset, int rightOffset) {
    const int chunkSize = 100;  // Number of spaces to print at a time
    // get the width of the console window
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    int consoleWidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;

    // Calculate the width of each half of the screen
    int halfWidth = consoleWidth / 2;

    // Calculate the padding needed to place each prompt at the specified offset from the middle
    int leftPadding = halfWidth - leftPrompt.length() - leftOffset;
    int rightPadding = halfWidth + rightOffset;

    for (int i = 0; i < leftPadding; i += chunkSize) {
        std::cout << std::string(std::min(chunkSize, leftPadding - i), ' ');
    }
    std::cout << leftPrompt;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD leftPromptEndPos = csbi.dwCursorPosition;

    std::cout << std::string(halfWidth - leftPadding - leftPrompt.length(), ' ');

    if (separator) {
        std::cout << '|';
    }

    // Print spaces to move the cursor to the start of the right half of the console
    for (int i = 0; i < rightPadding - halfWidth; i += chunkSize) {
        std::cout << std::string(std::min(chunkSize, rightPadding - halfWidth - i), ' ');
    }

    std::cout << rightPrompt;

    SetConsoleCursorPosition(hConsole, leftPromptEndPos);
}


void move_cursor(int spaces) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    GetConsoleScreenBufferInfo(hConsole, &csbi);
    COORD newCoord;
    newCoord.X = csbi.dwCursorPosition.X - spaces;
    newCoord.Y = csbi.dwCursorPosition.Y;

    SetConsoleCursorPosition(hConsole, newCoord);
}


void wordaddy_header() {
    system("cls");
    std::string str =  R"del(                                                                                                                       
      _____                   _____         _____        _____         _____        _____        _____    _____      _____ 
     |\    \   _____     ____|\    \    ___|\    \   ___|\    \    ___|\    \   ___|\    \   ___|\    \  |\    \    /    /|
     | |    | /    /|   /     /\    \  |    |\    \ |    |\    \  /    /\    \ |    |\    \ |    |\    \ | \    \  /    / |
     \/     / |    ||  /     /  \    \ |    | |    ||    | |    ||    |  |    ||    | |    ||    | |    ||  \____\/    /  /
     /     /_  \   \/ |     |    |    ||    |/____/ |    | |    ||    |__|    ||    | |    ||    | |    | \ |    /    /  / 
    |     // \  \   \ |     |    |    ||    |\    \ |    | |    ||    .--.    ||    | |    ||    | |    |  \|___/    /  /  
    |    |/   \ |    ||\     \  /    /||    | |    ||    | |    ||    |  |    ||    | |    ||    | |    |      /    /  /   
    |\ ___/\   \|   /|| \_____\/____/ ||____| |____||____|/____/||____|  |____||____|/____/||____|/____/|     /____/  /    
    | |   | \______/ | \ |    ||    | /|    | |    ||    /    | ||    |  |    ||    /    | ||    /    | |    |`    | /     
     \|___|/\ |    | |  \|____||____|/ |____| |____||____|____|/ |____|  |____||____|____|/ |____|____|/     |_____|/      
        \(   \|____|/      \(    )/      \(     )/    \(    )/     \(      )/    \(    )/     \(    )/          )/         
         '      )/          '    '        '     '      '    '       '      '      '    '       '    '           '          
    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~)del";
    centralize_text(str);
}


void cnb_header() {
    wordaddy_header();
    std::string str = R"del(
  ____                     ___     ____        _ _     
 / ___|_____      _____   ( _ )   | __ ) _   _| | |___ 
| |   / _ \ \ /\ / / __|  / _ \/\ |  _ \| | | | | / __|
| |__| (_) \ V  V /\__ \ | (_>  < | |_) | |_| | | \__ \
 \____\___/ \_/\_/ |___/  \___/\/ |____/ \__,_|_|_|___/

)del";
    centralize_text(str);
}


void wordle_header() {
    wordaddy_header();
    std::string str = R"del(
__        __            _ _      
\ \      / /__  _ __ __| | | ___ 
 \ \ /\ / / _ \| '__/ _` | |/ _ \
  \ V  V / (_) | | | (_| | |  __/
   \_/\_/ \___/|_|  \__,_|_|\___|

)del";
    centralize_text(str);
}


void menu_header() {
    std::string str = R"del(
 __  __ _____ _   _ _   _ 
|  \/  | ____| \ | | | | |
| |\/| |  _| |  \| | | | |
| |  | | |___| |\  | |_| |
|_|  |_|_____|_| \_|\___/ 

)del";
    centralize_text(str);
}