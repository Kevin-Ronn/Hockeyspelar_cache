#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <list>
#include <vector>
#include <random>

class HockeyPlayer {
public:
    int id;
    std::string name;
    int jersey;
    std::string teamName;

    HockeyPlayer(int id, const std::string& name, int jersey, const std::string& teamName)
        : id(id), name(name), jersey(jersey), teamName(teamName) {}
};

class LRUCache {
private:
    int capacity;
    std::unordered_map<int, std::list<HockeyPlayer>::iterator> cacheMap;
    std::list<HockeyPlayer> cacheList;

public:
    LRUCache(int capacity) : capacity(capacity) {}

    HockeyPlayer* getPlayer(int id) {
        auto it = cacheMap.find(id);
        
        // Cache träff
        if (it != cacheMap.end()) {
            cacheList.splice(cacheList.begin(), cacheList, it->second); // Move to front
            return &(*(it->second));
        }

        // Cache miss
        HockeyPlayer player;
        if (!loadPlayerFromFile(id, player)) {
            return nullptr;  // Player not found in file
        }
        
        // Placera i  cache
        if (cacheList.size() == capacity) {
            cacheMap.erase(cacheList.back().id); // Remove from map
            cacheList.pop_back();               // Remove from list
        }
        cacheList.push_front(player);
        cacheMap[id] = cacheList.begin();
        return &cacheList.front();
    }

private:
    // Hämta spelare från fil via ID
    bool loadPlayerFromFile(int id, HockeyPlayer &player) {
        std::ifstream file("hockey_players.txt");
        if (!file.is_open()) {
            std::cerr << "Could not open file." << std::endl;
            return false;
        }

        std::string line;
        while (std::getline(file, line)) {
            int fileId, jersey;
            std::string name, teamName;
            std::istringstream iss(line);
            if (iss >> fileId >> name >> jersey >> teamName) {
                if (fileId == id) {
                    player = HockeyPlayer(fileId, name, jersey, teamName);
                    return true;
                }
            }
        }
        return false;
    }
};

// Generera random hockey spelare och spara till filen
void generateRandomHockeyPlayersFile(int numPlayers) {
    std::ofstream file("hockey_players.txt");
    if (!file.is_open()) {
        std::cerr << "Could not create file." << std::endl;
        return;
    }

    std::vector<std::string> names = {"Alex", "Ben", "Chris", "David", "Eli", "Finn", "Gabe", "Hank", "Ivan", "Jack"};
    std::vector<std::string> teams = {"Eagles", "Tigers", "Sharks", "Wolves", "Dragons", "Knights", "Falcons", "Pirates", "Bulls", "Lions"};
    
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> jerseyDist(1, 99);
    std::uniform_int_distribution<> nameDist(0, names.size() - 1);
    std::uniform_int_distribution<> teamDist(0, teams.size() - 1);

    for (int i = 1; i <= numPlayers; ++i) {
        int jersey = jerseyDist(gen);
        std::string name = names[nameDist(gen)];
        std::string team = teams[teamDist(gen)];
        file << i << " " << name << " " << jersey << " " << team << "\n";
    }

    file.close();
}

int main() {
    generateRandomHockeyPlayersFile(100000);

    LRUCache cache(10);
    
    while (true) {
        int id;
        std::cout << "Enter player ID to retrieve (0 to exit): ";
        std::cin >> id;
        
        if (id == 0) break;

        HockeyPlayer* player = cache.getPlayer(id);
        if (player) {
            std::cout << "Player found - ID: " << player->id 
                      << ", Name: " << player->name 
                      << ", Jersey: " << player->jersey 
                      << ", Team: " << player->teamName << std::endl;
        } else {
            std::cout << "Player not found." << std::endl;
        }
    }

    return 0;
}
