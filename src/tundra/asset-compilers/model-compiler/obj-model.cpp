#include "tundra/asset-compilers/model-compiler/obj-model.hpp"

#include <unordered_map>

#include <tundra/assets/model/model-asset.hpp>
#include <tundra/assets/model/model-part.hpp>

namespace td::ac {

	ObjFace triangulate_face(const ObjFace& face) {

		ObjFace triangulated_face;

		const Int3 first_indices = face.indices[0];
		Int3 previous_indices = face.indices[1];
		for( int i = 2; i < face.indices.size(); i++ ) {
			triangulated_face.indices.push_back(first_indices);
			triangulated_face.indices.push_back(previous_indices);
			triangulated_face.indices.push_back(face.indices[i]);
			previous_indices = face.indices[i];
		}

		return triangulated_face;
	}

	ModelAsset* ObjModel::to_model_asset() const {
		
		const int16 FIXED_4_12_ONE = 1 << 12;

		ModelAsset* model_asset = new ModelAsset();

		model_asset->num_vertices = this->vertices.size();
		model_asset->vertices = new Vec3<int16>[model_asset->num_vertices];
		for( int i = 0; i < model_asset->num_vertices; i++ ) {
			model_asset->vertices[i] = {
				(int16)(FIXED_4_12_ONE * (this->vertices[i].x)),
				(int16)(FIXED_4_12_ONE * (this->vertices[i].y)),
				(int16)(FIXED_4_12_ONE * (this->vertices[i].z))
			};
		}

		model_asset->num_normals = this->normals.size();
		model_asset->normals = new Vec3<int16>[model_asset->num_normals];
		for( int i = 0; i < model_asset->num_normals; i++ ) {
			model_asset->normals[i] = {
				(int16)(FIXED_4_12_ONE * (this->normals[i].x)),
				(int16)(FIXED_4_12_ONE * (this->normals[i].y)),
				(int16)(FIXED_4_12_ONE * (this->normals[i].z))
			};
		}

		// Name, list of vertices
		std::unordered_map<std::string, std::vector<ObjFace>> triangles_grouped_by_material;

		for( ObjObject* obj_object : this->obj_objects ) {
			for( ObjObjectPart* obj_object_part : obj_object->parts ) {
				std::vector<ObjFace>& face_list = triangles_grouped_by_material[obj_object_part->material_name];
				for( ObjFace& face : obj_object_part->faces ) {
					face_list.push_back(triangulate_face(face));
				}
			}
		}

		model_asset->num_parts = triangles_grouped_by_material.size();
		model_asset->model_parts = new ModelPart*[model_asset->num_parts];

		int j = 0;
		for( auto& [material_name, triangles] : triangles_grouped_by_material ) {
			
			ModelPart* model_part = new ModelPart();
			model_part->num_triangles = triangles.size();
			model_part->vertex_indices = new Vec3<uint16>[model_part->num_triangles];
			model_part->normal_indices = new Vec3<uint16>[model_part->num_triangles];

			for( int i = 0; i < model_part->num_triangles; i++ ) {
				model_part->vertex_indices[i] = {
					(uint16)(triangles[i].indices[0].x),
					(uint16)(triangles[i].indices[1].x),
					(uint16)(triangles[i].indices[2].x),
				};

				model_part->normal_indices[i] = {
					(uint16)(triangles[i].indices[0].z),
					(uint16)(triangles[i].indices[1].z),
					(uint16)(triangles[i].indices[2].z)
				};
			}

			model_asset->model_parts[j] = model_part;
			j++;
		}

		return model_asset;
	}

}