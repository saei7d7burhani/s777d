/**
 * =============================================================================
 * Animation.h - Animation System
 * =============================================================================
 * Provides a simple animation system for the car showroom. Handles:
 * - Property interpolation over time
 * - Easing functions for smooth motion
 * - Animation state management
 * 
 * Design Decision: Using a component-based animation approach where
 * animations operate on floating-point properties. This keeps the system
 * simple while being flexible enough for our needs (door opening, wheel
 * rotation, car movement).
 * =============================================================================
 */

#ifndef ANIMATION_H
#define ANIMATION_H

#include <functional>
#include <vector>
#include <glm/glm.hpp>

/**
 * Easing functions for smooth animation transitions.
 * 
 * Linear: Constant speed (no easing)
 * EaseIn: Starts slow, ends fast
 * EaseOut: Starts fast, ends slow
 * EaseInOut: Starts slow, speeds up, then slows down
 */
namespace Easing {
    float linear(float t);
    float easeInQuad(float t);
    float easeOutQuad(float t);
    float easeInOutQuad(float t);
    float easeInCubic(float t);
    float easeOutCubic(float t);
    float easeInOutCubic(float t);
    float easeInElastic(float t);
    float easeOutElastic(float t);
    float easeOutBounce(float t);
}

/**
 * Animation class - Animates a single float value over time.
 */
class Animation {
public:
    using EasingFunction = std::function<float(float)>;
    using CompletionCallback = std::function<void()>;
    
    /**
     * Create an animation.
     * 
     * @param startValue Initial value
     * @param endValue Target value
     * @param duration Animation duration in seconds
     * @param easing Easing function to use
     */
    Animation(float startValue, float endValue, float duration,
              EasingFunction easing = Easing::linear);
    
    /**
     * Update the animation.
     * @param deltaTime Time since last frame in seconds
     * @return Current animated value
     */
    float update(float deltaTime);
    
    /**
     * Get current value without updating.
     */
    float getValue() const { return m_currentValue; }
    
    /**
     * Check if animation has completed.
     */
    bool isComplete() const { return m_complete; }
    
    /**
     * Reset the animation to the beginning.
     */
    void reset();
    
    /**
     * Reverse the animation direction.
     */
    void reverse();
    
    /**
     * Set a callback to be called when animation completes.
     */
    void onComplete(CompletionCallback callback);
    
    /**
     * Pause/resume the animation.
     */
    void setPaused(bool paused) { m_paused = paused; }
    bool isPaused() const { return m_paused; }
    
private:
    float m_startValue;
    float m_endValue;
    float m_currentValue;
    float m_duration;
    float m_elapsed;
    bool m_complete;
    bool m_paused;
    EasingFunction m_easing;
    CompletionCallback m_completionCallback;
};

/**
 * AnimationController - Manages multiple animations.
 */
class AnimationController {
public:
    AnimationController() = default;
    
    /**
     * Add an animation and return its ID.
     */
    size_t addAnimation(Animation animation);
    
    /**
     * Update all animations.
     * @param deltaTime Time since last frame
     */
    void update(float deltaTime);
    
    /**
     * Get an animation by ID.
     */
    Animation* getAnimation(size_t id);
    
    /**
     * Remove completed animations.
     */
    void cleanupCompleted();
    
    /**
     * Remove all animations.
     */
    void clear();
    
private:
    std::vector<Animation> m_animations;
};

/**
 * PropertyAnimator - Animates object properties directly.
 * 
 * Usage:
 *   PropertyAnimator<float> animator(&object.rotation, 0.0f, 360.0f, 2.0f);
 *   animator.update(deltaTime);
 */
template<typename T>
class PropertyAnimator {
public:
    PropertyAnimator(T* property, T startValue, T endValue, float duration,
                     std::function<float(float)> easing = Easing::linear)
        : m_property(property)
        , m_startValue(startValue)
        , m_endValue(endValue)
        , m_duration(duration)
        , m_elapsed(0.0f)
        , m_easing(easing)
        , m_complete(false)
    {
        *m_property = m_startValue;
    }
    
    void update(float deltaTime) {
        if (m_complete) return;
        
        m_elapsed += deltaTime;
        float t = m_elapsed / m_duration;
        
        if (t >= 1.0f) {
            t = 1.0f;
            m_complete = true;
        }
        
        float easedT = m_easing(t);
        *m_property = m_startValue + (m_endValue - m_startValue) * easedT;
    }
    
    bool isComplete() const { return m_complete; }
    
    void reset() {
        m_elapsed = 0.0f;
        m_complete = false;
        *m_property = m_startValue;
    }
    
private:
    T* m_property;
    T m_startValue;
    T m_endValue;
    float m_duration;
    float m_elapsed;
    std::function<float(float)> m_easing;
    bool m_complete;
};

#endif // ANIMATION_H
