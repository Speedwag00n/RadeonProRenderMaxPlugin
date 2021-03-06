/**********************************************************************
Copyright 2020 Advanced Micro Devices, Inc
Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at
    http://www.apache.org/licenses/LICENSE-2.0
Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
********************************************************************/

#include "FireRenderMFRefractionMtl.h"
#include "FireRenderNormalMtl.h"
#include "Resource.h"
#include "parser\MaterialParser.h"
#include "maxscript\mxsplugin\mxsPlugin.h"

FIRERENDER_NAMESPACE_BEGIN;

IMPLEMENT_FRMTLCLASSDESC(MFRefractionMtl)

FRMTLCLASSDESCNAME(MFRefractionMtl) FRMTLCLASSNAME(MFRefractionMtl)::ClassDescInstance;

// All parameters of the material plugin. See FIRE_MAX_PBDESC definition for notes on backwards compatibility
static ParamBlockDesc2 pbDesc(
	0, _T("MFRefractionMtlPbdesc"), 0, &FRMTLCLASSNAME(MFRefractionMtl)::ClassDescInstance, P_AUTO_CONSTRUCT + P_AUTO_UI + P_VERSION, FIRERENDERMTLVER_LATEST, 0,
    //rollout
	IDD_FIRERENDER_MFREFRACTIONMTL, IDS_FR_MTL_MFREFRACTION, 0, 0, NULL,

	FRMFRefractionMtl_COLOR, _T("Color"), TYPE_RGBA, P_ANIMATABLE, 0,
    p_default, Color(0.5f, 0.5f, 0.5f), p_ui, TYPE_COLORSWATCH, IDC_REFR_COLOR, PB_END,

	FRMFRefractionMtl_IOR, _T("IOR"), TYPE_FLOAT, P_ANIMATABLE, 0,
	p_default, 1.52f, p_range, 1.f, 999.f, p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_REFR_IOR, IDC_REFR_IOR_S, SPIN_AUTOSCALE, PB_END,

	FRMFRefractionMtl_ROUGHNESS, _T("Roughness"), TYPE_FLOAT, P_ANIMATABLE, 0,
	p_default, 0.5f, p_range, 0.f, 1.f, p_ui, TYPE_SPINNER, EDITTYPE_FLOAT, IDC_REFR_ROUGHNESS, IDC_REFR_ROUGHNESS_S, SPIN_AUTOSCALE, PB_END,

	FRMFRefractionMtl_COLOR_TEXMAP, _T("colorTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRMFRefractionMtl_TEXMAP_COLOR, p_ui, TYPE_TEXMAPBUTTON, IDC_REFR_COLOR_TEXMAP, PB_END,

	FRMFRefractionMtl_ROUGHNESS_TEXMAP, _T("roughnessTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRMFRefractionMtl_TEXMAP_ROUGHNESS, p_ui, TYPE_TEXMAPBUTTON, IDC_REFR_ROUGHNESS_TEXMAP, PB_END,

	FRMFRefractionMtl_NORMALMAP, _T("normalTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRMFRefractionMtl_TEXMAP_NORMAL, p_ui, TYPE_TEXMAPBUTTON, IDC_NORMAL_TEXMAP, PB_END,

    PB_END
    );

std::map<int, std::pair<ParamID, MCHAR*>> FRMTLCLASSNAME(MFRefractionMtl)::TEXMAP_MAPPING = {
	{ FRMFRefractionMtl_TEXMAP_COLOR, { FRMFRefractionMtl_COLOR_TEXMAP, _T("Color Map") } },
	{ FRMFRefractionMtl_TEXMAP_ROUGHNESS, { FRMFRefractionMtl_ROUGHNESS_TEXMAP, _T("Roughness Map") } },
	{ FRMFRefractionMtl_TEXMAP_NORMAL, { FRMFRefractionMtl_NORMALMAP, _T("Normal Map") } }
};

FRMTLCLASSNAME(MFRefractionMtl)::~FRMTLCLASSNAME(MFRefractionMtl)()
{
}


frw::Shader FRMTLCLASSNAME(MFRefractionMtl)::getShader(const TimeValue t, MaterialParser& mtlParser, INode* node)
{
	auto ms = mtlParser.materialSystem;

	frw::Shader material(ms, frw::ShaderTypeMicrofacetRefraction);
		
	Texmap* normalTexmap = GetFromPb<Texmap*>(pblock, FRMFRefractionMtl_NORMALMAP);
	const Color diffuseColor = GetFromPb<Color>(pblock, FRMFRefractionMtl_COLOR);
	const float roughness = GetFromPb<float>(pblock, FRMFRefractionMtl_ROUGHNESS);
	Texmap* diffuseTexmap = GetFromPb<Texmap*>(pblock, FRMFRefractionMtl_COLOR_TEXMAP);
	Texmap* roughnessTexmap = GetFromPb<Texmap*>(pblock, FRMFRefractionMtl_ROUGHNESS_TEXMAP);
	const float ior = GetFromPb<float>(pblock, FRMFRefractionMtl_IOR);
		
	frw::Value color(diffuseColor.r, diffuseColor.g, diffuseColor.b);
	if (diffuseTexmap)
		color = mtlParser.createMap(diffuseTexmap, 0);
	material.SetValue(RPR_MATERIAL_INPUT_COLOR, color);

	frw::Value roughnessv(roughness, roughness, roughness);
	if (roughnessTexmap)
		roughnessv = mtlParser.createMap(roughnessTexmap, MAP_FLAG_NOGAMMA);
	material.SetValue(RPR_MATERIAL_INPUT_ROUGHNESS, roughnessv);

	frw::Value iorv(ior, ior, ior);
	material.SetValue(RPR_MATERIAL_INPUT_IOR, iorv);
	
	if (normalTexmap)
		material.SetValue(RPR_MATERIAL_INPUT_NORMAL, FRMTLCLASSNAME(NormalMtl)::translateGenericBump(t, normalTexmap, 1.f, mtlParser));
	
    return material;
}

FIRERENDER_NAMESPACE_END;
