/**
 * =============================================================================
 * Animation.cpp - Animation System Implementation
 * =============================================================================
 */

#include "Animation.h"
#include <cmath>
#include <algorithm>

// =============================================================================
// Easing Functions
// =============================================================================

namespace Easing {

float linear(float t) {
    return t;
}

float easeInQuad(float t) {
    return t * t;
}

float easeOutQuad(float t) {
    return t * (2.0f - t);
}

float easeInOutQuad(float t) {
    if (t < 0.5f) {
        return 2.0f * t * t;
    }
    return -1.0f + (4.0f - 2.0f * t) * t;
}

float easeInCubic(float t) {
    return t * t * t;
}

float easeOutCubic(float t) {
    float f = t - 1.0f;
    return f * f * f + 1.0f;
}

float easeInOutCubic(float t) {
    if (t < 0.5f) {
        return 4.0f * t * t * t;
    }
    float f = 2.0f * t - 2.0f;
    return 0.5f * f * f * f + 1.0f;
}

float easeInElastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    
    float p = 0.3f;
    float s = p / 4.0f;
    float postFix = std::pow(2.0f, 10.0f * (t - 1.0f));
    return -(postFix * std::sin((t - 1.0f - s) * (2.0f * 3.14159265359f) / p));
}

float easeOutElastic(float t) {
    if (t == 0.0f || t == 1.0f) return t;
    
    float p = 0.3f;
    float s = p / 4.0f;
    return std::pow(2.0f, -10.0f * t) * std::sin((t - s) * (2.0f * 3.14159265359f) / p) + 1.0f;
}

float easeOutBounce(float t) {
    if (t < 1.0f / 2.75f) {
        return 7.5625f * t * t;
    } else if (t < 2.0f / 2.75f) {
        float postFix = t - 1.5f / 2.75f;
        return 7.5625f * postFix * postFix + 0.75f;
    } else if (t < 2.5f / 2.75f) {
        float postFix = t - 2.25f / 2.75f;
        return 7.5625f * postFix * postFix + 0.9375f;
    } else {
        float postFix = t - 2.625f / 2.75f;
        return 7.5625f * postFix * postFix + 0.984375f;
    }
}

} // namespace Easing

// =============================================================================
// Animation Class
// =============================================================================

Animation::Animation(float startValue, float endValue, float duration,
                     EasingFunction easing)
    : m_startValue(startValue)
    , m_endValue(endValue)
    , m_currentValue(startValue)
    , m_duration(duration)
    , m_elapsed(0.0f)
    , m_complete(false)
    , m_paused(false)
    , m_easing(easing)
    , m_completionCallback(nullptr)
{
}

float Animation::update(float deltaTime) {
    if (m_complete || m_paused) {
        return m_currentValue;
    }
    
    m_elapsed += deltaTime;
    
    // Calculate normalized time (0 to 1)
    float t = m_elapsed / m_duration;
    
    if (t >= 1.0f) {
        t = 1.0f;
        m_complete = true;
        
        // Call completion callback if set
        if (m_completionCallback) {
            m_completionCallback();
        }
    }
    
    // Apply easing function
    float easedT = m_easing(t);
    
    // Interpolate between start and end values
    m_currentValue = m_startValue + (m_endValue - m_startValue) * easedT;
    
    return m_currentValue;
}

void Animation::reset() {
    m_elapsed = 0.0f;
    m_complete = false;
    m_currentValue = m_startValue;
}

void Animation::reverse() {
    std::swap(m_startValue, m_endValue);
    m_elapsed = 0.0f;
    m_complete = false;
}

void Animation::onComplete(CompletionCallback callback) {
    m_completionCallback = std::move(callback);
}

// =============================================================================
// Animation Controller
// =============================================================================

size_t AnimationController::addAnimation(Animation animation) {
    m_animations.push_back(std::move(animation));
    return m_animations.size() - 1;
}

void AnimationController::update(float deltaTime) {
    for (auto& anim : m_animations) {
        anim.update(deltaTime);
    }
}

Animation* AnimationController::getAnimation(size_t id) {
    if (id < m_animations.size()) {
        return &m_animations[id];
    }
    return nullptr;
}

void AnimationController::cleanupCompleted() {
    m_animations.erase(
        std::remove_if(m_animations.begin(), m_animations.end(),
            [](const Animation& a) { return a.isComplete(); }),
        m_animations.end()
    );
}

void AnimationController::clear() {
    m_animations.clear();
}
