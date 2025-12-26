/**
 * =============================================================================
 * Collision.cpp - Basic Collision Detection Implementation
 * =============================================================================
 */

#include "Collision.h"
#include <algorithm>
#include <cmath>
#include <limits>

// =============================================================================
// AABB Methods
// =============================================================================

bool AABB::containsPoint(const glm::vec3& point) const {
    return point.x >= min.x && point.x <= max.x &&
           point.y >= min.y && point.y <= max.y &&
           point.z >= min.z && point.z <= max.z;
}

void AABB::expandToInclude(const glm::vec3& point) {
    min = glm::min(min, point);
    max = glm::max(max, point);
}

void AABB::expandToInclude(const AABB& other) {
    min = glm::min(min, other.min);
    max = glm::max(max, other.max);
}

AABB AABB::transformed(const glm::mat4& transform) const {
    // Transform all 8 corners and find new AABB
    glm::vec3 corners[8] = {
        {min.x, min.y, min.z},
        {max.x, min.y, min.z},
        {min.x, max.y, min.z},
        {max.x, max.y, min.z},
        {min.x, min.y, max.z},
        {max.x, min.y, max.z},
        {min.x, max.y, max.z},
        {max.x, max.y, max.z}
    };
    
    AABB result;
    result.min = glm::vec3(std::numeric_limits<float>::max());
    result.max = glm::vec3(std::numeric_limits<float>::lowest());
    
    for (int i = 0; i < 8; i++) {
        glm::vec4 transformed = transform * glm::vec4(corners[i], 1.0f);
        glm::vec3 pos = glm::vec3(transformed) / transformed.w;
        result.expandToInclude(pos);
    }
    
    return result;
}

// =============================================================================
// BoundingSphere Methods
// =============================================================================

bool BoundingSphere::containsPoint(const glm::vec3& point) const {
    return glm::length(point - center) <= radius;
}

// =============================================================================
// Collision Functions
// =============================================================================

namespace Collision {

bool testAABBvsAABB(const AABB& a, const AABB& b) {
    // Check for separation on each axis
    if (a.max.x < b.min.x || a.min.x > b.max.x) return false;
    if (a.max.y < b.min.y || a.min.y > b.max.y) return false;
    if (a.max.z < b.min.z || a.min.z > b.max.z) return false;
    
    return true;
}

CollisionResult testAABBvsAABBResponse(const AABB& a, const AABB& b) {
    CollisionResult result;
    
    if (!testAABBvsAABB(a, b)) {
        return result;
    }
    
    result.hit = true;
    
    // Find the axis with minimum penetration
    float overlapX = std::min(a.max.x - b.min.x, b.max.x - a.min.x);
    float overlapY = std::min(a.max.y - b.min.y, b.max.y - a.min.y);
    float overlapZ = std::min(a.max.z - b.min.z, b.max.z - a.min.z);
    
    // Determine collision normal and penetration
    if (overlapX <= overlapY && overlapX <= overlapZ) {
        result.penetration = overlapX;
        result.normal = (a.getCenter().x < b.getCenter().x) ? 
                        glm::vec3(-1, 0, 0) : glm::vec3(1, 0, 0);
    } else if (overlapY <= overlapX && overlapY <= overlapZ) {
        result.penetration = overlapY;
        result.normal = (a.getCenter().y < b.getCenter().y) ? 
                        glm::vec3(0, -1, 0) : glm::vec3(0, 1, 0);
    } else {
        result.penetration = overlapZ;
        result.normal = (a.getCenter().z < b.getCenter().z) ? 
                        glm::vec3(0, 0, -1) : glm::vec3(0, 0, 1);
    }
    
    // Calculate contact point (center of overlap region)
    result.point = (glm::max(a.min, b.min) + glm::min(a.max, b.max)) * 0.5f;
    
    return result;
}

bool testSphereVsSphere(const BoundingSphere& a, const BoundingSphere& b) {
    float distSq = glm::length2(b.center - a.center);
    float radiusSum = a.radius + b.radius;
    return distSq <= radiusSum * radiusSum;
}

bool testSphereVsAABB(const BoundingSphere& sphere, const AABB& box) {
    // Find the closest point on the AABB to the sphere center
    glm::vec3 closest = closestPointOnAABB(box, sphere.center);
    
    // Check if that point is within the sphere
    float distSq = glm::length2(closest - sphere.center);
    return distSq <= sphere.radius * sphere.radius;
}

bool testRayVsAABB(const Ray& ray, const AABB& box, float& t) {
    float tmin = 0.0f;
    float tmax = std::numeric_limits<float>::max();
    
    for (int i = 0; i < 3; i++) {
        if (std::abs(ray.direction[i]) < 0.0001f) {
            // Ray is parallel to slab
            if (ray.origin[i] < box.min[i] || ray.origin[i] > box.max[i]) {
                return false;
            }
        } else {
            float ood = 1.0f / ray.direction[i];
            float t1 = (box.min[i] - ray.origin[i]) * ood;
            float t2 = (box.max[i] - ray.origin[i]) * ood;
            
            if (t1 > t2) std::swap(t1, t2);
            
            tmin = std::max(tmin, t1);
            tmax = std::min(tmax, t2);
            
            if (tmin > tmax) return false;
        }
    }
    
    t = tmin;
    return true;
}

bool testRayVsSphere(const Ray& ray, const BoundingSphere& sphere, float& t) {
    glm::vec3 oc = ray.origin - sphere.center;
    float a = glm::dot(ray.direction, ray.direction);
    float b = 2.0f * glm::dot(oc, ray.direction);
    float c = glm::dot(oc, oc) - sphere.radius * sphere.radius;
    float discriminant = b * b - 4 * a * c;
    
    if (discriminant < 0) {
        return false;
    }
    
    t = (-b - std::sqrt(discriminant)) / (2.0f * a);
    return t >= 0;
}

bool testRayVsPlane(const Ray& ray, const glm::vec3& planeNormal, 
                    float planeD, float& t) {
    float denom = glm::dot(planeNormal, ray.direction);
    
    if (std::abs(denom) < 0.0001f) {
        return false;  // Ray parallel to plane
    }
    
    t = -(glm::dot(planeNormal, ray.origin) + planeD) / denom;
    return t >= 0;
}

glm::vec3 closestPointOnAABB(const AABB& box, const glm::vec3& point) {
    return glm::clamp(point, box.min, box.max);
}

glm::vec3 resolveAABBCollision(const AABB& a, const AABB& b) {
    CollisionResult result = testAABBvsAABBResponse(a, b);
    
    if (!result.hit) {
        return glm::vec3(0);
    }
    
    // Push 'a' out of 'b' along the collision normal
    return result.normal * result.penetration;
}

} // namespace Collision

// =============================================================================
// CollisionWorld
// =============================================================================

size_t CollisionWorld::addStaticAABB(const AABB& box) {
    m_staticBoxes.push_back(box);
    return m_staticBoxes.size() - 1;
}

CollisionResult CollisionWorld::testAgainstStatic(const AABB& movingBox) const {
    CollisionResult deepest;
    deepest.penetration = 0;
    
    for (const auto& staticBox : m_staticBoxes) {
        CollisionResult result = Collision::testAABBvsAABBResponse(movingBox, staticBox);
        if (result.hit && result.penetration > deepest.penetration) {
            deepest = result;
        }
    }
    
    return deepest;
}

glm::vec3 CollisionWorld::resolveCollisions(const AABB& movingBox, 
                                            const glm::vec3& currentPos) const {
    glm::vec3 resolvedPos = currentPos;
    AABB testBox = movingBox;
    
    // Iterate a few times to resolve multiple collisions
    for (int iteration = 0; iteration < 4; iteration++) {
        CollisionResult result = testAgainstStatic(testBox);
        
        if (!result.hit) {
            break;
        }
        
        // Push the box out of collision
        glm::vec3 push = result.normal * (result.penetration + 0.001f);
        resolvedPos += push;
        
        // Update test box position
        testBox.min += push;
        testBox.max += push;
    }
    
    return resolvedPos;
}

bool CollisionWorld::raycast(const Ray& ray, float maxDistance, 
                             float& hitT, size_t& hitIndex) const {
    hitT = maxDistance;
    bool anyHit = false;
    
    for (size_t i = 0; i < m_staticBoxes.size(); i++) {
        float t;
        if (Collision::testRayVsAABB(ray, m_staticBoxes[i], t)) {
            if (t < hitT && t >= 0) {
                hitT = t;
                hitIndex = i;
                anyHit = true;
            }
        }
    }
    
    return anyHit;
}

void CollisionWorld::clear() {
    m_staticBoxes.clear();
}
