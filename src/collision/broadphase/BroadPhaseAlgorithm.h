/********************************************************************************
* ReactPhysics3D physics library, http://code.google.com/p/reactphysics3d/      *
* Copyright (c) 2010-2013 Daniel Chappuis                                       *
*********************************************************************************
*                                                                               *
* This software is provided 'as-is', without any express or implied warranty.   *
* In no event will the authors be held liable for any damages arising from the  *
* use of this software.                                                         *
*                                                                               *
* Permission is granted to anyone to use this software for any purpose,         *
* including commercial applications, and to alter it and redistribute it        *
* freely, subject to the following restrictions:                                *
*                                                                               *
* 1. The origin of this software must not be misrepresented; you must not claim *
*    that you wrote the original software. If you use this software in a        *
*    product, an acknowledgment in the product documentation would be           *
*    appreciated but is not required.                                           *
*                                                                               *
* 2. Altered source versions must be plainly marked as such, and must not be    *
*    misrepresented as being the original software.                             *
*                                                                               *
* 3. This notice may not be removed or altered from any source distribution.    *
*                                                                               *
********************************************************************************/

#ifndef REACTPHYSICS3D_BROAD_PHASE_ALGORITHM_H
#define REACTPHYSICS3D_BROAD_PHASE_ALGORITHM_H

// Libraries
#include <vector>
#include "../../body/CollisionBody.h"
#include "collision/ProxyShape.h"
#include "DynamicAABBTree.h"

/// Namespace ReactPhysics3D
namespace reactphysics3d {

// Declarations
class CollisionDetection;

// TODO : Check that when a kinematic or static body is manually moved, the dynamic aabb tree
//        is correctly updated

// TODO : Replace the names "body, bodies" by "collision shapes"

// TODO : Remove the pair manager

// TODO : RENAME THIS
// Structure BroadPair
/**
 * This structure represent a potential overlapping pair during the broad-phase collision
 * detection.
 */
struct BroadPair {

    // -------------------- Attributes -------------------- //

    /// Broad-phase ID of the first collision shape
    int collisionShape1ID;

    /// Broad-phase ID of the second collision shape
    int collisionShape2ID;

    // -------------------- Methods -------------------- //

    /// Method used to compare two pairs for sorting algorithm
    static bool smallerThan(const BroadPair& pair1, const BroadPair& pair2);
};

// Class BroadPhaseAlgorithm
/**
 * This class represents the broad-phase collision detection. The
 * goal of the broad-phase collision detection is to compute the pairs of bodies
 * that have their AABBs overlapping. Only those pairs of bodies will be tested
 * later for collision during the narrow-phase collision detection. A dynamic AABB
 * tree data structure is used for fast broad-phase collision detection.
 */
class BroadPhaseAlgorithm {

    protected :

        // -------------------- Attributes -------------------- //

        /// Dynamic AABB tree
        DynamicAABBTree mDynamicAABBTree;

        /// Array with the broad-phase IDs of all collision shapes that have moved (or have been
        /// created) during the last simulation step. Those are the shapes that need to be tested
        /// for overlapping in the next simulation step.
        int* mMovedShapes;

        /// Number of collision shapes in the array of shapes that have moved during the last
        /// simulation step.
        uint mNbMovedShapes;

        /// Number of allocated elements for the array of shapes that have moved during the last
        /// simulation step.
        uint mNbAllocatedMovedShapes;

        /// Number of non-used elements in the array of shapes that have moved during the last
        /// simulation step.
        uint mNbNonUsedMovedShapes;

        /// Temporary array of potential overlapping pairs (with potential duplicates)
        BroadPair* mPotentialPairs;

        /// Number of potential overlapping pairs
        uint mNbPotentialPairs;

        /// Number of allocated elements for the array of potential overlapping pairs
        uint mNbAllocatedPotentialPairs;

        /// Reference to the collision detection object
        CollisionDetection& mCollisionDetection;
        
        // -------------------- Methods -------------------- //

        /// Private copy-constructor
        BroadPhaseAlgorithm(const BroadPhaseAlgorithm& algorithm);

        /// Private assignment operator
        BroadPhaseAlgorithm& operator=(const BroadPhaseAlgorithm& algorithm);

    public :

        // -------------------- Methods -------------------- //

        /// Constructor
        BroadPhaseAlgorithm(CollisionDetection& collisionDetection);

        /// Destructor
        ~BroadPhaseAlgorithm();
        
        /// Add a proxy collision shape into the broad-phase collision detection
        void addProxyCollisionShape(ProxyShape* proxyShape, const AABB& aabb);

        /// Remove a proxy collision shape from the broad-phase collision detection
        void removeProxyCollisionShape(ProxyShape* proxyShape);

        /// Notify the broad-phase that a collision shape has moved and need to be updated
        void updateProxyCollisionShape(ProxyShape* proxyShape, const AABB& aabb,
                                       const Vector3& displacement);

        /// Add a collision shape in the array of shapes that have moved in the last simulation step
        /// and that need to be tested again for broad-phase overlapping.
        void addMovedCollisionShape(int broadPhaseID);

        /// Remove a collision shape from the array of shapes that have moved in the last simulation
        /// step and that need to be tested again for broad-phase overlapping.
        void removeMovedCollisionShape(int broadPhaseID);

        /// Notify the broad-phase about a potential overlapping pair in the dynamic AABB tree
        void notifyOverlappingPair(int node1ID, int node2ID);

        /// Compute all the overlapping pairs of collision shapes
        void computeOverlappingPairs();

        /// Return true if the two broad-phase collision shapes are overlapping
        bool testOverlappingShapes(ProxyShape* shape1, ProxyShape* shape2) const;
};

// Method used to compare two pairs for sorting algorithm
inline bool BroadPair::smallerThan(const BroadPair& pair1, const BroadPair& pair2) {

    if (pair1.collisionShape1ID < pair2.collisionShape1ID) return true;
    if (pair1.collisionShape1ID == pair2.collisionShape1ID) {
        return pair1.collisionShape2ID < pair2.collisionShape2ID;
    }
    return false;
}

// Return true if the two broad-phase collision shapes are overlapping
inline bool BroadPhaseAlgorithm::testOverlappingShapes(ProxyShape* shape1,
                                                       ProxyShape* shape2) const {
    // Get the two AABBs of the collision shapes
    const AABB& aabb1 = mDynamicAABBTree.getFatAABB(shape1->mBroadPhaseID);
    const AABB& aabb2 = mDynamicAABBTree.getFatAABB(shape2->mBroadPhaseID);

    // Check if the two AABBs are overlapping
    return aabb1.testCollision(aabb2);
}

}

#endif

