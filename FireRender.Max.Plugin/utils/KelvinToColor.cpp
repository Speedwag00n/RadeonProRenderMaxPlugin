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


#include <algorithm>

#include "KelvinToColor.h"

FIRERENDER_NAMESPACE_BEGIN;

Color KelvinToColor(float kelvin)
{
	kelvin = std::max(MinKelvin, kelvin);
	kelvin = std::min(MaxKelvin, kelvin);
	kelvin = kelvin *= 0.01f;

	float r, g, b;
	// Red
	if (kelvin <= 66.f)
		r = 1.f;
	else
	{
		float tmp = kelvin - 60.f;
		tmp = 329.698727446f * (pow(tmp, -0.1332047592));
		r = tmp * 1.f / 255.f;
		if (r < 0.f) r = 0.f;
		if (r > 1.f) r = 1.f;
	}

	// Green
	if (kelvin <= 66.f)
	{
		float tmp = kelvin;
		tmp = 99.4708025861f * log(tmp) - 161.1195681661f;
		g = tmp * 1.f / 255.f;
		if (g < 0.f) g = 0.f;
		if (g > 1.f) g = 1.f;
	}
	else
	{
		float tmp = kelvin - 60.f;
		tmp = 288.1221695283f * (pow(tmp, -0.0755148492f));
		g = tmp * 1.f / 255.f;
		if (g < 0.f) g = 0.f;
		if (g > 1.f) g = 1.f;
	}

	// Blue
	if (kelvin >= 66.f)
		b = 1.f;
	else if (kelvin <= 19.f)
		b = 0.f;
	else
	{
		float tmp = kelvin - 10.f;
		tmp = 138.5177312231f * log(tmp) - 305.0447927307f;
		b = tmp * 1.f / 255.f;
		if (b < 0.f) b = 0.f;
		if (b > 1.f) b = 1.f;
	}

	return Color(r, g, b);
}

FIRERENDER_NAMESPACE_END;
