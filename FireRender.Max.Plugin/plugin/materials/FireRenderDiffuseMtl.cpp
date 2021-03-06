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

#include "FireRenderDiffuseMtl.h"
#include "FireRenderNormalMtl.h"
#include "Resource.h"
#include "parser\MaterialParser.h"
#include "maxscript\mxsplugin\mxsPlugin.h"

FIRERENDER_NAMESPACE_BEGIN;

IMPLEMENT_FRMTLCLASSDESC(DiffuseMtl)

FRMTLCLASSDESCNAME(DiffuseMtl) FRMTLCLASSNAME(DiffuseMtl)::ClassDescInstance;


// All parameters of the material plugin. See FIRE_MAX_PBDESC definition for notes on backwards compatibility
static ParamBlockDesc2 pbDesc(
	0, _T("DiffuseMtlPbdesc"), 0, &FRMTLCLASSNAME(DiffuseMtl)::ClassDescInstance, P_AUTO_CONSTRUCT + P_AUTO_UI + P_VERSION, FIRERENDERMTLVER_LATEST, 0,
    //rollout
	IDD_FIRERENDER_DIFFUSEMTL, IDS_FR_MTL_DIFFUSE, 0, 0, NULL,

	FRDiffuseMtl_COLOR, _T("diffuseColor"), TYPE_RGBA, P_ANIMATABLE, 0,
    p_default, Color(0.5f, 0.5f, 0.5f), p_ui, TYPE_COLORSWATCH, IDC_DIFFUSE_COLOR, PB_END,

	FRDiffuseMtl_COLOR_TEXMAP, _T("diffuseTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRDiffuseMtl_TEXMAP_DIFFUSE, p_ui, TYPE_TEXMAPBUTTON, IDC_DIFFUSE_TEXMAP, PB_END,

	FRDiffuseMtl_NORMALMAP, _T("normalTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRDiffuseMtl_TEXMAP_NORMAL, p_ui, TYPE_TEXMAPBUTTON, IDC_NORMAL_TEXMAP, PB_END,

    PB_END
    );

std::map<int, std::pair<ParamID, MCHAR*>> FRMTLCLASSNAME(DiffuseMtl)::TEXMAP_MAPPING = {
	{ FRDiffuseMtl_TEXMAP_DIFFUSE, { FRDiffuseMtl_COLOR_TEXMAP, _T("Color Map") } },
	{ FRDiffuseMtl_TEXMAP_NORMAL, { FRDiffuseMtl_NORMALMAP, _T("Normal map") } }
};

FRMTLCLASSNAME(DiffuseMtl)::~FRMTLCLASSNAME(DiffuseMtl)()
{
}


frw::Shader FRMTLCLASSNAME(DiffuseMtl)::getShader(const TimeValue t, MaterialParser& mtlParser, INode* node)
{
	auto ms = mtlParser.materialSystem;

	frw::DiffuseShader material(ms);
		
	Texmap* normalTexmap = GetFromPb<Texmap*>(pblock, FRDiffuseMtl_NORMALMAP);
	const Color diffuseColor = GetFromPb<Color>(pblock, FRDiffuseMtl_COLOR);
	Texmap* diffuseTexmap = GetFromPb<Texmap*>(pblock, FRDiffuseMtl_COLOR_TEXMAP);
		
	frw::Value color(diffuseColor.r, diffuseColor.g, diffuseColor.b);
	if (diffuseTexmap)
		color = mtlParser.createMap(diffuseTexmap, 0);

	material.SetColor(color);
	
	if (normalTexmap)
		material.SetNormal(FRMTLCLASSNAME(NormalMtl)::translateGenericBump(t, normalTexmap, 1.f, mtlParser));
	
    return material;
}

FIRERENDER_NAMESPACE_END;
