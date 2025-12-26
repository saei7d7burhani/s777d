/**
 * =============================================================================
 * Collision.h - Basic Collision Detection
 * =============================================================================
 * Implements simple collision detection for the car showroom:
 * - AABB (Axis-Aligned Bounding Box) collision
 * - Sphere collision
 * - Ray casting for picking
 * 
 * Design Decision: Using simple collision primitives rather than mesh-based
 * collision. This is sufficient for keeping the car within showroom walls
 * and provides good performance.
 * 
 * AABB vs OBB:
 * - AABB: Faster but less accurate for rotated objects
 * - OBB (Oriented Bounding Box): More accurate but slower
 * We use AABB for walls and simplified car bounds.
 * =============================================================================
 */

#ifndef COLLISION_H
#define COLLISION_H

#include <glm/glm.hpp>
#include <vector>

/**
 * AABB - Axis-Aligned Bounding Box
 * 
 * Defined by minimum and maximum corners. The box is always aligned
 * with the world axes (no rotation).
 */
struct AABB {
    glm::vec3 min;  // Minimum corner (smallest x, y, z)
    glm::vec3 max;  // Maximum corner (largest x, y, z)
    
    AABB() : min(0.0f), max(0.0f) {}
    AABB(const glm::vec3& min, const glm::vec3& max) : min(min), max(max) {}
    
    /**
     * Get the center of the box.
     */
    glm::vec3 getCenter() const { return (min + max) * 0.5f; }
    
    /**
     * Get the size (dimensions) of the box.
     */
    glm::vec3 getSize() const { return max - min; }
    
    /**
     * Get the half-extents (half-size) of the box.
     */
    glm::vec3 getHalfExtents() const { return getSize() * 0.5f; }
    
    /**
     * Check if a point is inside the box.
     */
    bool containsPoint(const glm::vec3& point) const;
    
    /**
     * Expand the box to include a point.
     */
    void expandToInclude(const glm::vec3& point);
    
    /**
     * Expand the box to include another AABB.
     */
    void expandToInclude(const AABB& other);
    
    /**
     * Transform the AABB by a matrix (results in a new AABB that may be larger).
     */
    AABB transformed(const glm::mat4& transform) const;
};

/**
 * BoundingSphere - Spherical bounding volume.
 */
struct BoundingSphere {
    glm::vec3 center;
    float radius;
    
    BoundingSphere() : center(0.0f), radius(0.0f) {}
    BoundingSphere(const glm::vec3& center, float radius) 
        : center(center), radius(radius) {}
    
    /**
     * Check if a point is inside the sphere.
     */
    bool containsPoint(const glm::vec3& point) const;
};

/**
 * Ray - For ray casting and picking.
 */
struct Ray {
    glm::vec3 origin;
    glm::vec3 direction;  // Should be normalized
    
    Ray() : origin(0.0f), direction(0.0f, 0.0f, -1.0f) {}
    Ray(const glm::vec3& origin, const glm::vec3& direction)
        : origin(origin), direction(glm::normalize(direction)) {}
    
    /**
     * Get a point along the ray at distance t from origin.
     */
    glm::vec3 getPoint(float t) const { return origin + direction * t; }
};

/**
 * CollisionResult - Result of a collision test.
 */
struct CollisionResult {
    bool hit;               // Whether collision occurred
    glm::vec3 point;        // Point of contact
    glm::vec3 normal;       // Surface normal at contact
    float penetration;      // How deep the collision is
    
    CollisionResult() : hit(false), point(0.0f), normal(0.0f), penetration(0.0f) {}
};

/**
 * Collision detection functions.
 */
namespace Collision {
    /**
     * Test AABB vs AABB intersection.
     */
    bool testAABBvsAABB(const AABB& a, const AABB& b);
    
    /**
     * Test AABB vs AABB with collision response info.
     */
    CollisionResult testAABBvsAABBResponse(const AABB& a, const AABB& b);
    
    /**
     * Test sphere vs sphere intersection.
     */
    bool testSphereVsSphere(const BoundingSphere& a, const BoundingSphere& b);
    
    /**
     * Test sphere vs AABB intersection.
     */
    bool testSphereVsAABB(const BoundingSphere& sphere, const AABB& box);
    
    /**
     * Test ray vs AABB intersection.
     * @param t Output: distance along ray to intersection point
     * @return true if ray intersects the box
     */
    bool testRayVsAABB(const Ray& ray, const AABB& box, float& t);
    
    /**
     * Test ray vs sphere intersection.
     */
    bool testRayVsSphere(const Ray& ray, const BoundingSphere& sphere, float& t);
    
    /**
     * Test ray vs plane intersection.
     * @param planeNormal Normal of the plane (should be normalized)
     * @param planeD Distance from origin to plane along normal
     * @param t Output: distance along ray to intersection point
     */
    bool testRayVsPlane(const Ray& ray, const glm::vec3& planeNormal, 
                        float planeD, float& t);
    
    /**
     * Find closest point on AABB to a given point.
     */
    glm::vec3 closestPointOnAABB(const AABB& box, const glm::vec3& point);
    
    /**
     * Resolve AABB collision by moving 'a' out of 'b'.
     * @return Displacement vector to apply to 'a'
     */
    glm::vec3 resolveAABBCollision(const AABB& a, const AABB& b);
}

/**
 * CollisionWorld - Manages all collision objects in the scene.
 */
class CollisionWorld {
public:
    CollisionWorld() = default;
    
    /**
     * Add a static AABB collider (e.g., walls).
     * @return Index of the added collider
     */
    size_t addStaticAABB(const AABB& box);
    
    /**
     * Test a moving AABB against all static colliders.
     * @return Collision result with deepest penetration
     */
    CollisionResult testAgainstStatic(const AABB& movingBox) const;
    
    /**
     * Resolve collisions and return corrected position.
     */
    glm::vec3 resolveCollisions(const AABB& movingBox, const glm::vec3& currentPos) const;
    
    /**
     * Cast a ray and find the first hit.
     * @param hitIndex Output: index of the hit collider
     */
    bool raycast(const Ray& ray, float maxDistance, float& hitT, size_t& hitIndex) const;
    
    /**
     * Clear all colliders.
     */
    void clear();
    
private:
    std::vector<AABB> m_staticBoxes;
};

#endif // COLLISION_H
