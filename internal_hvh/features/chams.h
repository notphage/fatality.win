#pragma once

class chams : public singleton<chams>
{
public:
	chams();
	~chams();

	static void extend_drawing();
	void draw_record( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* custom_bone_to_world );
	void add_material( material_index_t idx, IMaterial* mat );
	bool draw_hst( lag_record_t& record, matrix3x4_t* out );
	bool draw_ch( IMatRenderContext* context, const DrawModelState_t& state, const ModelRenderInfo_t& info, matrix3x4_t* pCustomBoneToWorld );
	void dark_mode() const;
private:
	IMaterial* materials[ material_index_t::NUM_MATERIALS ];
};
