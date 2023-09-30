#include <iostream>
#include <fstream>
#include <functional>

class GameConfig {
public:
    GameConfig() = default;

    GameConfig(const std::string& _saveGamePath, const int _evaluationRoundIndex) {
        save_game_path = _saveGamePath;
        evaluation_round_index = _evaluationRoundIndex;
    }

    std::string save_game_path;
    int evaluation_round_index = 10;
};

class GameState {
public: 
    GameState() {
        round_index = 0;
        population = 100;
        land_amount = 1000;
        wheat_amount = 2800;
        people_died = 0;
        people_arrived = 0;
        land_price = 0;
        plague_multiplier = 0;
        wheat_per_acre = 0;
        wheat_lost = 0;

        people_died_totally = 0;

        land_bought = 0;
        land_sold = 0;
        wheat_consumed = 0;
        wheat_sown = 0;
    }

    int round_index;
    int population;
    int land_amount;
    int wheat_amount;
    int people_died;
    int people_arrived;
    int land_price;
    int plague_multiplier;
    int wheat_per_acre;
    int wheat_lost;

    int people_died_totally;

    int land_bought;
    int land_sold;
    int wheat_consumed;
    int wheat_sown;
};

class MathUtils {
public:
    static int rollRandomIntInRange(const int min_val, const int max_val) {
        return min_val + rand() % (max_val - min_val + 1);
    }

    static int clamp(const int n, const int min_val, const int max_val) {
        return std::max(min_val, std::min(n, max_val));
    }
};

class Game {
public:
    Game(const GameConfig& game_config) {
        game_config_ = game_config;
        loadGameState(game_config_.save_game_path);
    }

    void processRoundTick() {

        if (game_state_.round_index >= game_config_.evaluation_round_index) {
            evaluatePlayerPerformance();
            resetGameState();
        }

        pollQuitGameRequest();

        processPreUserInputRoundCalculations();
        echoGameState();
        pollUserInput();
        processPostUserInputRoundCalculations();
    }

private:
    GameState game_state_;
    GameConfig game_config_;

    std::string game_input_messages_[4] = { 
        "How much acres would you like to buy?", 
        "How much acres would you like to sell?", 
        "How much wheat would you like to consume?", 
        "How much wheat would you like to sow?"};

    void saveGameState(const std::string& save_path) const
    {
        std::ofstream save_file(save_path);
        if (!save_file) {
            std::cerr << "Could not save the game." << std::endl;
            return;
        }

        save_file 
            << game_state_.round_index << "\n"
            << game_state_.population << "\n"
            << game_state_.land_amount << "\n"
            << game_state_.wheat_amount << "\n"
            << game_state_.people_died << "\n"
            << game_state_.people_arrived << "\n"
            << game_state_.land_price << "\n"
            << game_state_.plague_multiplier << "\n"
            << game_state_.wheat_per_acre << "\n"
            << game_state_.wheat_lost << "\n"
            << game_state_.people_died_totally << "\n"
            << std::endl;

        save_file.close();
    }

    void loadGameState(const std::string& save_path)
    {
        std::ifstream save_file(save_path);
        
        if (!save_file) {
            std::cerr << "Save file not found. Starting new session..." << std::endl;
            return;
        }
        
        char response;

        std::cout << "Save file found. Type L to load the game. Type any other key to start new session." << std::endl;
        std::cin >> response;

        if (response != 'L' && response != 'l') {
            return;
        }

        save_file
            >> game_state_.round_index
            >> game_state_.population
            >> game_state_.land_amount
            >> game_state_.wheat_amount
            >> game_state_.people_died
            >> game_state_.people_arrived
            >> game_state_.land_price
            >> game_state_.plague_multiplier
            >> game_state_.wheat_per_acre
            >> game_state_.wheat_lost
            >> game_state_.people_died_totally
            ;

        save_file.close();
    }

    void echoGameState() const
    {
        std::cout 
            << "Current round: " << game_state_.round_index + 1<< "\n"
            << "People starved to death: " << game_state_.people_died << "\n"
            << "People Arrived: " << game_state_.people_arrived << "\n"
            << "Plague multiplier: " << game_state_.plague_multiplier << "\n"
            << "Population: " << game_state_.population << "\n"
            << "Wheat: " << game_state_.wheat_amount << "\n"
            << "Wheat per acre collected: " << game_state_.wheat_per_acre << "\n"
            << "Wheat lost to rats: " << game_state_.wheat_lost << "\n"
            << "Acres in use: " << game_state_.land_amount << "\n"
            << "Acre price: " << game_state_.land_price << "\n"
            << std::endl;
    }

    void resetGameState() {
        game_state_ = GameState();
    }

    void pollQuitGameRequest() const
    {
        char response;
        std::cout << "Type Q to quit the game. Type any other key to proceed." << std::endl;
        std::cin >> response;

        if (response == 'Q' || response == 'q') {
            saveGameState(game_config_.save_game_path);
            exit(0);
        }
    }

    void pollUserInput() {
        const int land_to_buy = getValidInput(game_input_messages_[0], 
                                            [](const int input, const GameState& state)
                                            { return input >= 0 && input * state.land_price <= state.wheat_amount; });
        game_state_.land_bought = land_to_buy;

        const int land_to_sell = getValidInput(game_input_messages_[1],
                                             [](const int input, const GameState& state)
                                             { return input >= 0 && input <= state.land_amount - state.land_bought; });
        game_state_.land_sold = land_to_sell;

        const int wheat_to_consume = getValidInput(game_input_messages_[2],
                                                 [](const int input, const GameState& state)
                                                 { return input >= 0 && input <= state.wheat_amount; });
        game_state_.wheat_consumed = wheat_to_consume;

        const int wheat_to_sow = getValidInput(game_input_messages_[3],
                                             [](const int input, const GameState& state)
                                             { return input >= 0 && input <= state.wheat_amount - state.wheat_consumed; });
        game_state_.wheat_sown = wheat_to_sow;
    }

    int getValidInput(const std::string& message, const std::function<bool(int, GameState)>& validity_predicate) const
    {        
        int value;
        bool is_valid;

        do {
            std::cout << message << std::endl;
            std::cin >> value;

            is_valid = validity_predicate(value, game_state_);

            if (std::cin.fail())
            {
                std::cin.clear(); 
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                is_valid = false;
            }
            
            if (!is_valid) {
                std::cout << "Invalid input" << std::endl;
            }
            
        } while (!is_valid);

        return value;
    }

    void evaluatePlayerPerformance() const
    {
        const int annual_death_rate = game_state_.people_died_totally / game_config_.evaluation_round_index;
        const int acres_per_person = game_state_.land_amount / game_state_.population;

        if (annual_death_rate > 33 && acres_per_person < 7) {
            std::cout << "Bad." << std::endl;
            return;
        } 

        if (annual_death_rate > 10 && acres_per_person < 9) {
            std::cout << "Satisfactory." << std::endl;
            return;
        }

        if (annual_death_rate > 3 && acres_per_person < 10) {
            std::cout << "Good." << std::endl;
            return;
        }

        std::cout << "Excellent." << std::endl;
    }

    void processPreUserInputRoundCalculations() {
        game_state_.land_price = MathUtils::rollRandomIntInRange(17, 26);
    }

    void processPostUserInputRoundCalculations() {
        game_state_.round_index++;

        // Process land purchase
        game_state_.land_amount += game_state_.land_bought;
        game_state_.wheat_amount -= game_state_.land_bought * game_state_.land_price;
        
        // Process land sale
        game_state_.land_amount -= game_state_.land_sold;
        game_state_.wheat_amount += game_state_.land_sold * game_state_.land_price;

        // Collect sown wheat
        game_state_.wheat_per_acre = MathUtils::rollRandomIntInRange(1, 6);
        const int available_sown_land_amount = MathUtils::clamp(game_state_.wheat_sown * 2, 0, game_state_.land_amount);
        const int processed_sown_land_amount = MathUtils::clamp(available_sown_land_amount, 0, game_state_.population * 10);
        game_state_.wheat_amount += processed_sown_land_amount * game_state_.wheat_per_acre;

        // Process wheat loss
        const int wheat_lost_percentage = MathUtils::rollRandomIntInRange(0, 7);
        game_state_.wheat_lost = game_state_.wheat_amount * wheat_lost_percentage / 100;
        game_state_.wheat_amount -= game_state_.wheat_lost;

        // Calculate survivors, casualties and arrivals
        const int people_survived_round = MathUtils::clamp(game_state_.wheat_consumed / 20, 0, game_state_.population);
        game_state_.people_died = game_state_.population - people_survived_round;
        
        // Force lose if death rate exceeded 45
        const int round_mortality_rate = game_state_.population == 0 ? 100 : (game_state_.people_died / game_state_.population) * 100;
        if (round_mortality_rate >= 45) {
            std::cout << "Your mortality rate, " << round_mortality_rate << ", was too high... Game Over." << std::endl;
            resetGameState();
            return;
        }

        game_state_.wheat_amount = MathUtils::clamp(game_state_.wheat_amount - people_survived_round * 20, 0, INT_MAX);        
        game_state_.people_died_totally += game_state_.people_died;
        game_state_.population = people_survived_round;
        
        game_state_.people_arrived = 
            MathUtils::clamp(game_state_.people_died / 2 * (5 - game_state_.wheat_per_acre) * game_state_.wheat_amount / 600 + 1,
                0, 50);
        game_state_.population += game_state_.people_arrived;

        // Process plague
        game_state_.plague_multiplier = MathUtils::rollRandomIntInRange(0, 100) <= 15 ? 1 : 0;
        game_state_.population /= (game_state_.plague_multiplier + 1);
    }
};

class GameBootstrapper {
public:
    Game InitializeGame() {
        srand(static_cast<unsigned>(time(0)));

        const GameConfig config = GameConfig("savegame.txt", 10);
        return Game(config);
    }
};

int main()
{
    GameBootstrapper boot = GameBootstrapper();
    Game game = boot.InitializeGame();

    while (true) {
        game.processRoundTick();
    }
}