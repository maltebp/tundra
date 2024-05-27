#include "tundra/core/list.dec.hpp"
#include <tundra/engine/dynamic-transform.hpp>

#include <tundra/core/assert.hpp>
#include <tundra/core/fixed.hpp>
#include <tundra/core/vec/vec3.hpp>


namespace td {
    
    void DynamicTransform::on_destroy() {

        if( first_child != nullptr ) {
            DynamicTransform* current_child = first_child;
            do {
                current_child->dirty_flags |= DirtyFlags::RotationAndScale | DirtyFlags::Translation;
                current_child->mark_descendants_dirty(DirtyFlags::RotationAndScale | DirtyFlags::Translation);

                DynamicTransform* next = current_child->next_sibling;

                current_child->parent = nullptr;
                current_child->next_sibling = nullptr;
                current_child->previous_sibling = nullptr;

                current_child = next;
            } while( current_child != first_child );

            first_child = nullptr;
        }

        if( parent != nullptr ) {
            parent->remove_child(this);
        }
    }

    void DynamicTransform::add_child(const ComponentRef<DynamicTransform>& new_child) {
        TD_ASSERT(new_child->parent == nullptr, "Child to add already has a parent");

        if( first_child == nullptr ) {
            first_child = new_child;
            new_child->next_sibling = new_child;
            new_child->previous_sibling = new_child;
        }
        else {
            new_child->next_sibling = first_child;
            new_child->previous_sibling = first_child->previous_sibling;
            
            first_child->previous_sibling->next_sibling = new_child;
            first_child->previous_sibling = new_child;
        }
        
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

        child_to_remove->previous_sibling->next_sibling = child_to_remove->next_sibling;
        child_to_remove->next_sibling->previous_sibling = child_to_remove->previous_sibling;

        if( child_to_remove == first_child ) {
            if( child_to_remove->next_sibling == child_to_remove ) {
                first_child = nullptr;
            } else {
                first_child = child_to_remove->next_sibling;
            }
        }

        child_to_remove->next_sibling = nullptr;
        child_to_remove->previous_sibling = nullptr;
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
            current = current->next_sibling;
        } while(current != first_child);

        return num_children;
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

        // OPTIMIZATION: (for all set/add): We can avoid marking descendants
        // if we already have marked them once (we should be able to determine this
        // based on whether we're marked outselves?)
        mark_descendants_dirty(DirtyFlags::Translation);
    }

    const Vec3<Fixed32<12>>& DynamicTransform::get_translation() const {
        return this->translation;
    }

    // We cannot have this as internal in the function, because it will call
    // std::atexit, which is it not defined
    static td::List<DynamicTransform*> mark_descendants_dirty_stack;

    void DynamicTransform::mark_descendants_dirty(DirtyFlags flags) {
        // Traversing using a non-recursive BFS, because 
        // recursion was 10x slower in some cases

        mark_descendants_dirty_stack.clear();

        auto add_children_to_stack = [](DynamicTransform* parent) {
            if( parent->first_child == nullptr ) return;
            DynamicTransform* current_child = parent->first_child;
            do {
                mark_descendants_dirty_stack.add(current_child);
            } while( current_child != parent->first_child );
        };

        add_children_to_stack(this);

        while( mark_descendants_dirty_stack.get_size() > 0 ) {
            DynamicTransform* current = mark_descendants_dirty_stack.get_last();
            mark_descendants_dirty_stack.remove_at(mark_descendants_dirty_stack.get_size() - 1);            

            current->dirty_flags = flags;

            add_children_to_stack(current);
        }
    }
}