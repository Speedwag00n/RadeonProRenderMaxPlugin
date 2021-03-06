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

#include "FireRenderFresnelSchlickMtl.h"
#include "Resource.h"
#include "parser\MaterialParser.h"
#include "maxscript\mxsplugin\mxsPlugin.h"

FIRERENDER_NAMESPACE_BEGIN;

IMPLEMENT_FRMTLCLASSDESC(FresnelSchlickMtl)

FRMTLCLASSDESCNAME(FresnelSchlickMtl) FRMTLCLASSNAME(FresnelSchlickMtl)::ClassDescInstance;

// All parameters of the material plugin. See FIRE_MAX_PBDESC definition for notes on backwards compatibility
static ParamBlockDesc2 pbDesc(
	0, _T("FresnelSchlickMtlPbdesc"), 0, &FRMTLCLASSNAME(FresnelSchlickMtl)::ClassDescInstance, P_AUTO_CONSTRUCT + P_AUTO_UI + P_VERSION, FIRERENDERMTLVER_LATEST, 0,
    //rollout
	IDD_FIRERENDER_FRESNELSCHLICKMTL, IDS_FR_MTL_FRESNELSCHLICK, 0, 0, NULL,

	FRFresnelSchlickMtl_REFLECTANCE, _T("Reflectance"), TYPE_RGBA, P_ANIMATABLE, 0,
	p_default, Color(0.5f, 0.5f, 0.5f), p_ui, TYPE_COLORSWATCH, IDC_FRESNEL_REFLECTANCE, PB_END,

	FRFresnelSchlickMtl_REFLECTANCE_TEXMAP, _T("reflectanceTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRFresnelSchlickMtl_TEXMAP_REFLECTANCE, p_ui, TYPE_TEXMAPBUTTON, IDC_FRESNEL_REFLECTANCE_TEXMAP, PB_END,

	FRFresnelSchlickMtl_INVEC_TEXMAP, _T("invecTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRFresnelSchlickMtl_TEXMAP_INVEC, p_ui, TYPE_TEXMAPBUTTON, IDC_FRESNEL_INVEC_TEXMAP, PB_END,

	FRFresnelSchlickMtl_N_TEXMAP, _T("nTexmap"), TYPE_TEXMAP, 0, 0,
	p_subtexno, FRFresnelSchlickMtl_TEXMAP_N, p_ui, TYPE_TEXMAPBUTTON, IDC_FRESNEL_N_TEXMAP, PB_END,

    PB_END
    );

std::map<int, std::pair<ParamID, MCHAR*>> FRMTLCLASSNAME(FresnelSchlickMtl)::TEXMAP_MAPPING = {
	{ FRFresnelSchlickMtl_TEXMAP_INVEC,{ FRFresnelSchlickMtl_INVEC_TEXMAP, _T("Invec map") } },
	{ FRFresnelSchlickMtl_TEXMAP_N,{ FRFresnelSchlickMtl_N_TEXMAP, _T("N map") } },
	{ FRFresnelSchlickMtl_TEXMAP_REFLECTANCE,{ FRFresnelSchlickMtl_REFLECTANCE_TEXMAP, _T("reflectance map") } }
};

FRMTLCLASSNAME(FresnelSchlickMtl)::~FRMTLCLASSNAME(FresnelSchlickMtl)()
{
}


frw::Value FRMTLCLASSNAME(FresnelSchlickMtl)::getShader(const TimeValue t, MaterialParser& mtlParser)
{
	const Color reflectance = GetFromPb<Color>(pblock, FRFresnelSchlickMtl_REFLECTANCE);
	Texmap* invecMap = GetFromPb<Texmap*>(pblock, FRFresnelSchlickMtl_INVEC_TEXMAP);
	Texmap* reflectanceMap = GetFromPb<Texmap*>(pblock, FRFresnelSchlickMtl_REFLECTANCE_TEXMAP);
	Texmap* nMap = GetFromPb<Texmap*>(pblock, FRFresnelSchlickMtl_N_TEXMAP);

	frw::Value reflectanceValue(reflectance.r, reflectance.g, reflectance.b);
	if (reflectanceMap)
		reflectanceValue = mtlParser.createMap(reflectanceMap, MAP_FLAG_NOGAMMA);

	auto nValue = mtlParser.createMap(nMap, MAP_FLAG_NOGAMMA);
	auto invecValue = mtlParser.createMap(invecMap, MAP_FLAG_NOGAMMA);
	
	return mtlParser.materialSystem.ValueFresnelSchlick(reflectanceValue, nValue, invecValue);
}

void FRMTLCLASSNAME(FresnelSchlickMtl)::Update(TimeValue t, Interval& valid) {
    for (int i = 0; i < NumSubTexmaps(); ++i) {
        // we are required to recursively call Update on all our submaps
        Texmap* map = GetSubTexmap(i);
        if (map != NULL) {
            map->Update(t, valid);
        }
    }
    this->pblock->GetValidity(t, valid);
}

FIRERENDER_NAMESPACE_END;
