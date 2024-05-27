    #pragma once

#include "tundra/engine/entity-system/internal/component-meta-data.hpp"
#include <tundra/core/vec/vec3.dec.hpp>
#include <tundra/engine/entity-system/component-ref.hpp>
#include <tundra/engine/transform-matrix.hpp>
#include <tundra/engine/transform.hpp>

namespace td {
    class DynamicTransform;
}

namespace td::gte {
    const TransformMatrix& compute_world_matrix(const DynamicTransform*);
}

namespace td {

    class DynamicTransform : public Component<DynamicTransform, Transform> {
    public:

        DynamicTransform() = default;

        // Avoiding copy constructor for now, as I am unsure how it should work

        enum class DirtyFlags : byte {
            None = 0,
            RotationAndScale = 1 << 0,
            Translation = 1 << 1,   
            All = 0xFF
        };

        virtual void on_destroy() override;

        void add_child(const ComponentRef<DynamicTransform>& new_child);
        void remove_child(DynamicTransform* child_to_remove);
        [[nodiscard]] uint32 get_num_children() const;

        [[nodiscard]] DynamicTransform* get_parent() const { return parent; }

        void set_scale(const Vec3<Fixed32<12>>& scale);
        void add_scale(const Vec3<Fixed32<12>>& scale);
        [[nodiscard]] const Vec3<Fixed32<12>>& get_scale() const;

        // Rotation where 1 is 360 degrees, and it rotates in order ZYX 
        void set_rotation(const Vec3<Fixed16<12>>& rotation);
        void add_rotation(const Vec3<Fixed16<12>>& rotation);
        [[nodiscard]] const Vec3<Fixed16<12>>& get_rotation() const;

        void set_translation(const Vec3<Fixed32<12>>& translation);
        void add_translation(const Vec3<Fixed32<12>>& translation);
        [[nodiscard]] const Vec3<Fixed32<12>>& get_translation() const;

        [[nodiscard]] virtual TransformType get_type() const override {
            return TransformType::Dynamic;
        }

    private:

        void mark_descendants_dirty(DirtyFlags flags);

        Vec3<Fixed32<12>> scale { 1 };
        Vec3<Fixed16<12>> rotation { 0 };
        Vec3<Fixed32<12>> translation { 0 };

        ComponentRef<DynamicTransform> parent { nullptr };
        ComponentRef<DynamicTransform> next_sibling { nullptr };
        ComponentRef<DynamicTransform> previous_sibling { nullptr };
        ComponentRef<DynamicTransform> first_child { nullptr };

        mutable DirtyFlags dirty_flags = DirtyFlags::All;
        mutable TransformMatrix cached_world_matrix;

        friend const TransformMatrix& ::td::gte::compute_world_matrix(const DynamicTransform*);
        
    };

    

    static inline DynamicTransform::DirtyFlags operator&(DynamicTransform::DirtyFlags a, DynamicTransform::DirtyFlags b) {
        return static_cast<DynamicTransform::DirtyFlags>(static_cast<byte>(a) & static_cast<byte>(b));
    }

    static inline DynamicTransform::DirtyFlags operator|(DynamicTransform::DirtyFlags a, DynamicTransform::DirtyFlags b) {
        return static_cast<DynamicTransform::DirtyFlags>(static_cast<byte>(a) | static_cast<byte>(b));
    }

    static inline DynamicTransform::DirtyFlags& operator|=(DynamicTransform::DirtyFlags& a, DynamicTransform::DirtyFlags b) {
        a = a | b;
        return a;
    }

}