#pragma once
#include <string>
#include <unordered_map>

// Forward declarations
class Engine;
class Object;

namespace tinyxml2 {
    class XMLElement;
}

/**
 * SaveGame - Handles saving and loading game state to/from XML files
 */
class SaveGame {
public:
    /**
     * Save the current game state to an XML file
     * @param filename Path to the save file (e.g., "saves/game_save.xml")
     * @param engine Reference to the game engine
     * @return true if save was successful, false otherwise
     */
    static bool save(const std::string& filename, Engine& engine);
    
    /**
     * Load game state from an XML file
     * @param filename Path to the save file
     * @param engine Reference to the game engine
     * @return true if load was successful, false otherwise
     */
    static bool load(const std::string& filename, Engine& engine);
    
    /**
     * Check if a save file exists
     * @param filename Path to the save file
     * @return true if file exists, false otherwise
     */
    static bool exists(const std::string& filename);
    
    /**
     * Get the default save file path
     * @return Default save file path
     */
    static std::string getDefaultSavePath() { return "saves/game_save.xml"; }

private:
    /**
     * Helper method to save a single GameObject to XML
     */
    static void saveGameObject(tinyxml2::XMLElement* parent, Object* obj, const std::string& id);
    
    /**
     * Helper method to load a single GameObject from XML
     */
    static void loadGameObject(tinyxml2::XMLElement* objElem, Engine& engine, std::unordered_map<std::string, Object*>& idMap);
};

