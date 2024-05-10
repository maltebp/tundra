#include <tundra/engine/dynamic-transform.hpp>

#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.hpp>


namespace td {
    
    void DynamicTransform::on_destroy() {

        if( parent != nullptr ) {
            parent->remove_child(this);
        }

        if( first_child != nullptr ) {
            DynamicTransform* current_child = first_child;
            do {
                current_child->dirty_flags |= DirtyFlags::RotationAndScale | DirtyFlags::Translation;
                current_child->mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);
                current_child = current_child->parent_next_child;
            } while( current_child != first_child );

            first_child = nullptr;
        }
    }

    void DynamicTransform::add_child(const ComponentRef<DynamicTransform>& new_child) {
        TD_ASSERT(new_child->parent == nullptr, "Child to add already has a parent");
        
        if( first_child == nullptr ) {
            first_child = new_child;
        }
        else {
            DynamicTransform* last_child = first_child;
            while( last_child->parent_next_child != first_child ) {
                last_child = last_child->parent_next_child;
            }
            last_child->parent_next_child = new_child;
        }
        
        new_child->parent_next_child = first_child;
        new_child->parent = this;
        
        // OPTIMIZATION: We could change only the flags that are actually relevant
        new_child->dirty_flags = DirtyFlags::All;
        new_child->mark_descendants_dirty(DirtyFlags::All);
    }

    void DynamicTransform::remove_child(DynamicTransform* child_to_remove) {
        TD_ASSERT(child_to_remove != nullptr, "DynamicTransform child to remove is nullptr");
        TD_ASSERT(child_to_remove->parent != nullptr, "DynamicTransform child to remove has no parent");
        TD_ASSERT(child_to_remove->parent == this, "DynamicTransform child to remove has a different parent");
        TD_ASSERT(first_child != nullptr, "DynamicTransform has parent, but parent has no children");

        // TODO: Optimization: If the last child's sibling pointer is a nullptr, we
        // do not have to traverse the entire list of children when the child to
        // remove is the first child

        DynamicTransform* previous_sibling = this->first_child;
        DynamicTransform* sibling = previous_sibling->parent_next_child;
        while(sibling != child_to_remove && sibling != this->first_child) {
            previous_sibling = sibling;
            sibling = sibling->parent_next_child;
        }
        TD_ASSERT(sibling == child_to_remove, "Child to remove is not a child of this DynamicTransform");

        bool is_only_child = previous_sibling == child_to_remove;
        if( is_only_child ) {
            child_to_remove->parent_next_child = nullptr;
            this->first_child = nullptr;
        }
        else {
            previous_sibling->parent_next_child = child_to_remove->parent_next_child;
            if( child_to_remove == this->first_child ) {
                this->first_child = child_to_remove->parent_next_child;
            }
        }

        child_to_remove->parent = nullptr;
        child_to_remove->dirty_flags = DirtyFlags::All;
        child_to_remove->mark_descendants_dirty(DirtyFlags::All);
    }

    [[nodiscard]] uint32 DynamicTransform::get_num_children() const {
        if( first_child == nullptr ) return 0;
        
        uint32 num_children = 0;
        DynamicTransform* current = first_child;
        do {    
            num_children++;
            current = current->parent_next_child;
        } while(current != first_child);

        return num_children;
    }

    [[nodiscard]] DynamicTransform* DynamicTransform::get_parent() const {
        return parent;
    }

    void DynamicTransform::set_scale(const Vec3<Fixed32<12>>& scale) {
        if( scale == this->scale) return;
        this->scale = scale;
        dirty_flags |= DirtyFlags::RotationAndScale;
        mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);
    }

    void DynamicTransform::add_scale(const Vec3<Fixed32<12>>& scale) {
        if( scale == Vec3<Fixed32<12>>{0}) return;
        this->scale += scale;
        dirty_flags |= DirtyFlags::RotationAndScale;
        mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);
    }

    const Vec3<Fixed32<12>>& DynamicTransform::get_scale() const {
        return this->scale;
    }

    void DynamicTransform::set_rotation(const Vec3<Fixed16<12>>& rotation) {
        if( rotation == this->rotation) return;
        this->rotation = rotation;
        dirty_flags |= DirtyFlags::RotationAndScale;
        mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);
    }
 
    void DynamicTransform::add_rotation(const Vec3<Fixed16<12>>& rotation) {
        if( rotation == Vec3<Fixed16<12>>{0}) return;
        this->rotation += rotation;
        dirty_flags |= DirtyFlags::RotationAndScale;
        mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);
    } 

    const Vec3<Fixed16<12>>& DynamicTransform::get_rotation() const {
        return this->rotation;
    }

    void DynamicTransform::set_translation(const Vec3<Fixed32<12>>& translation) {
        if( translation == this->translation) return;
        this->translation = translation;
        dirty_flags |= DirtyFlags::Translation;
        mark_descendants_dirty(DirtyFlags::Translation);
    }

    void DynamicTransform::add_translation(const Vec3<Fixed32<12>>& translation) {
        if( translation == Vec3<Fixed32<12>>{0}) return;
        this->translation += translation;
        dirty_flags |= DirtyFlags::Translation;

        // TODO: Optimization (for all set/add): We can avoid marking descendants
        // if we already have marked them once (we should be able to determine this
        // based on whether we're marked outselves?)
        mark_descendants_dirty(DirtyFlags::Translation);
    }

    const Vec3<Fixed32<12>>& DynamicTransform::get_translation() const {
        return this->translation;
    }

    void DynamicTransform::mark_descendants_dirty(DirtyFlags flags) {
        if( first_child == nullptr ) return;

        DynamicTransform* current_child = first_child;
        do {
            current_child->dirty_flags |= flags;
            current_child->mark_descendants_dirty(flags);
            current_child = current_child->parent_next_child;
        } while( current_child != first_child );
    }

}