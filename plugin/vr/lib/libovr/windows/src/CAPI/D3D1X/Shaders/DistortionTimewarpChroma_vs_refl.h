#ifndef DistortionTimewarpChroma_vs_refl

const OVR::CAPI::D3D_NS::ShaderBase::Uniform DistortionTimewarpChroma_vs_refl[] =
{
	{ "EyeToSourceUVScale", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 0, 8 },
	{ "EyeToSourceUVOffset", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 8, 8 },
	{ "EyeRotationStart", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 16, 64 },
	{ "EyeRotationEnd", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 80, 64 },
};

#endif
