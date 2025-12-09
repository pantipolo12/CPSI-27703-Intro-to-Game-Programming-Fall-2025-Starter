#include "HealthComponent.h"
#include <algorithm>
#include <iostream>

HealthComponent::HealthComponent(int maxHealth) 
    : currentHealth(maxHealth), maxHealth(maxHealth) {
    std::cout << "[HEALTH] HealthComponent created with " << currentHealth << "/" << maxHealth << " health" << std::endl;
}

void HealthComponent::update(float dt) {
    // Reduce invulnerability time
    if (invulnerabilityTime > 0.0f) {
        invulnerabilityTime -= dt;
        if (invulnerabilityTime < 0.0f) {
            invulnerabilityTime = 0.0f;
        }
    }
}

void HealthComponent::takeDamage(int damage) {
    // Only take damage if not invulnerable
    if (invulnerabilityTime > 0.0f) {
        return; // Silently ignore if invulnerable
    }
    
    // Ensure we don't go below 0
    int oldHealth = currentHealth;
    currentHealth = std::max(0, currentHealth - damage);
    invulnerabilityTime = INVULNERABILITY_DURATION;
    
    std::cout << "[HEALTH] ========================================" << std::endl;
    std::cout << "[HEALTH] Player took " << damage << " damage!" << std::endl;
    std::cout << "[HEALTH] Health: " << oldHealth << " -> " << currentHealth << "/" << maxHealth << std::endl;
    
    if (currentHealth <= 0) {
        std::cout << "[HEALTH] *** PLAYER IS NOW DEAD! ***" << std::endl;
        std::cout << "[HEALTH] Player died after taking damage!" << std::endl;
    }
    std::cout << "[HEALTH] ========================================" << std::endl;
}

