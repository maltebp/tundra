#pragma once

#include <tundra/engine/transform-matrix.hpp>
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/engine/transform.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>

namespace td {

    class StaticTransform : public Component<StaticTransform, Transform> {
    public:

        constexpr StaticTransform() = default;

        constexpr explicit StaticTransform(const TransformMatrix& world_matrix) : world_matrix(world_matrix) { }

        // Where are more "utility" constructors?
        // The matrix has to be computed using hardware acceleration, so 
        // constructing static transform from size, rotation and translation is
        // part of the platform-specific hardware acceleration 

        constexpr StaticTransform(const StaticTransform&) = default;

        [[nodiscard]] virtual TransformType get_type() const override {
           return TransformType::Static;
        }

        const TransformMatrix world_matrix{};

    };

}