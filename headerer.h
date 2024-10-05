#pragma once
#include <iostream>
#include <limits>
#include <fstream>
#include <random>
#include <chrono>
#include <vector>
#include <string>
#include <thread>
#include <chrono>
#include <sstream>
#include <set>
#include <algorithm>
#include <map>
#include <unordered_map>
#define NOMINMAX
#include <windows.h>


const std::string wordListFile = "words.txt";
enum class Difficulty { EASY, MEDIUM, HARD, INSANE, SOULSLIKE};
enum class ScreenHalf { LEFT, RIGHT };

// Wordaddy.cpp
Difficulty get_word_difficulty();
Difficulty get_word_difficulty();
std::pair<int, int> get_word_length(Difficulty difficulty);
int calculate_padding(int textLength, int div = 2);
void loading_bar();
void centralize_text(std::string text, int offset = 0, bool newline = true);
int validate_choice(int minNum, int maxNum, std::string prompt);
void to_upper(std::string& string);
void output_menu_options(const std::vector<std::string>& menuOptions);
void print_split_prompt(const std::string& leftPrompt, const std::string& rightPrompt, bool separator = false, int leftOffset = 1, int rightOffset = 1);
void wordaddy_header();
void move_cursor(int spaces);
void cnb_header();
void wordle_header();
void menu_header();

// CNB.cpp
void cnb_menu();
void cnb_instructions();
void cnb_mode_select();
void cnb_singleplayer();
void play_cnb_round(Difficulty difficulty);
std::string cnb_user_guess(int guessCounter, std::string& targetWord, Difficulty& difficulty, std::pair<int, int>& wordLength);
std::pair<int, int> get_cows_and_bulls(const std::string& guess, const std::string& targetWord);
void cnb_ai_check_guess(const std::string& userGuess, const std::string& aiGuess, const std::string& userTargetWord, const std::string& aiTargetWord, std::set<char>& aiIncorrectLetters);
std::string get_cnb_ai_guess(Difficulty aiDifficulty, const std::string& aiTargetWord, const std::set<char>& aiIncorrectLetters, std::pair<int, int> aiCowsAndBulls, std::vector<std::string>& pastGuesses, std::string& blacklistedNumbers, std::vector<std::string>& all_answers, std::string& last_guess);
std::string get_random_password(std::pair<int, int> wordLength);
void cnb_vs_ai();
std::string cnb_vs_ai_guess(int guessCounter, const std::string& userTargetWord, Difficulty wordDifficulty, std::pair<int, int> wordLength, const std::string& aiGuess);
void cnb_vs_ai_round(Difficulty aiDifficulty, Difficulty wordDifficulty);
Difficulty get_cnb_ai_difficulty();
std::string cnb_easy_ai(int wordLength);
std::string cnb_medium_ai(int wordLength, std::vector<std::string>& pastGuesses);
std::string cnb_hard_ai(int wordLength, std::pair<int, int> cowsAndBulls, std::vector<std::string>& pastGuesses, std::string& blacklistedNumbers);
void generate_all_answers(std::vector<std::string>& all_answers, std::string answer, std::string digits);
std::pair<int, int> count_bulls_and_cows(const std::string& guess, const std::string& other_guess);
std::string cnb_insane_ai(int wordLength, std::pair<int, int> cowsAndBulls, std::vector<std::string>& all_answers, std::string& last_guess);
std::string cnb_soulslike_ai(std::string aiTargetWord);

// Wordle.cpp
void wordle_menu();
void wordle_instructions();
void wordle_mode_select();
std::string get_random_word(std::pair<int, int> wordLength);
void wordle_singleplayer();
void play_wordle_round(Difficulty difficulty);
std::string wordle_user_guess(int guessCounter, std::string& targetWord, Difficulty& difficulty, std::pair<int, int>& wordLength);
std::vector<std::pair<char, std::string>> check_wordle_guess(std::string userGuess, std::string targetWord);
void wordle_vs_ai();
std::string wordle_vs_ai_guess(int guessCounter, const std::string& userTargetWord, Difficulty wordDifficulty, std::pair<int, int> wordLength, const std::string& aiGuess);
void print_colored_text(std::vector<std::pair<char, std::string>> result);
Difficulty get_wordle_ai_difficulty();
std::string get_wordle_ai_guess(Difficulty aiDifficulty, std::string aiTargetWord, std::vector<std::pair<char, std::string>>& aiResult, std::string blacklistedLetter, std::vector<std::string> listWords);
void wordle_ai_check_guess(const std::string& userGuess, const std::string& aiGuess, const std::string& userTargetWord, const std::string& aiTargetWord);
std::string wordle_easy_ai(int length);
std::string wordle_medium_ai(int length, std::string blacklistedLetters);
std::string wordle_hard_ai(int length, std::string blacklistedLetters, std::vector<std::pair<char, std::string>>& aiResult);
std::string wordle_insane_ai(int length, std::vector<std::pair<char, std::string>>& aiResult, std::string blacklistedLetters);
std::string wordle_soulslike_ai(std::string aiTargetWord);
