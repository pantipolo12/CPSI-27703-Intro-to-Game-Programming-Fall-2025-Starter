#pragma once

/**
 * Simple timer class for tracking elapsed time
 * Useful for animations, delays, and other time-based logic
 */
class Timer {
public:
    Timer() : elapsed(0.0f), duration(0.0f), isRunning(false) {}
    
    /**
     * Create a timer with a specific duration
     * @param duration Time in seconds before timer expires
     * @param startRunning If true, timer starts running immediately
     */
    Timer(float duration, bool startRunning = false) 
        : elapsed(0.0f), duration(duration), isRunning(startRunning) {}
    
    /**
     * Update the timer with delta time
     * @param dt Delta time in seconds
     * @return true if timer has expired (elapsed >= duration)
     */
    bool update(float dt) {
        if (!isRunning) return false;
        
        elapsed += dt;
        return elapsed >= duration;
    }
    
    /**
     * Reset the timer to zero
     */
    void reset() {
        elapsed = 0.0f;
    }
    
    /**
     * Reset the timer and optionally change duration
     * @param newDuration New duration (if > 0, updates duration)
     */
    void reset(float newDuration) {
        elapsed = 0.0f;
        if (newDuration > 0.0f) {
            duration = newDuration;
        }
    }
    
    /**
     * Start the timer
     */
    void start() {
        isRunning = true;
    }
    
    /**
     * Stop the timer (doesn't reset elapsed time)
     */
    void stop() {
        isRunning = false;
    }
    
    /**
     * Check if timer has expired
     */
    bool isExpired() const {
        return elapsed >= duration;
    }
    
    /**
     * Check if timer is currently running
     */
    bool running() const {
        return isRunning;
    }
    
    /**
     * Get elapsed time
     */
    float getElapsed() const {
        return elapsed;
    }
    
    /**
     * Get remaining time
     */
    float getRemaining() const {
        return duration - elapsed;
    }
    
    /**
     * Get progress (0.0 to 1.0)
     */
    float getProgress() const {
        if (duration <= 0.0f) return 1.0f;
        return elapsed / duration;
    }
    
    /**
     * Set the duration
     */
    void setDuration(float newDuration) {
        duration = newDuration;
    }
    
    /**
     * Get the duration
     */
    float getDuration() const {
        return duration;
    }

private:
    float elapsed;      // Time elapsed since last reset
    float duration;    // Duration before timer expires
    bool isRunning;    // Whether timer is currently running
};

