#pragma once

#include <vector>
#include <string>
#include <set>

#include "tundra/model/obj/obj-object-part.hpp"


namespace td::ac {
    
    class ObjObject {
    public:

        ~ObjObject() {
            for( ObjObjectPart* part : parts )
            {
                delete part;
            }
        }

        size_t num_total_faces() const {
            size_t num_faces = 0;
            for( ObjObjectPart* part : parts ) {
                num_faces += part->faces.size();
            }
            return num_faces;
        }

        bool has_any_faces() const {
            return num_total_faces() > 0;
        }
        
        std::string name;

        std::vector<ObjObjectPart*> parts;

    };

}