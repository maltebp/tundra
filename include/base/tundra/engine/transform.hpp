#pragma once

#include <tundra/engine/entity-system/component.hpp>
#include <tundra/engine/transform-matrix.hpp>

namespace td {

    enum class TransformType {
        Static,
        Dynamic
    };

    class Transform : public internal::ComponentBase {
    public:

        virtual ~Transform() = default;

        [[nodiscard]] virtual TransformType get_type() const = 0;

        // Why there is no "get_world_matrix"-method:
        // We want to compute the matrix fast, so on PlayStation we use GTE. But
        // this will have to override the current matrix register values, which
        // I fear can be easy to forget, and thus leading to all kinds of weird
        // bugs. So instead, you compute it explicitly via a GTE call.

    };

}