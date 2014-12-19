#ifndef DistortionChroma_ps_refl

const OVR::CAPI::D3D_NS::ShaderBase::Uniform DistortionChroma_ps_refl[] =
{
	{ "OverdriveScales", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 0, 8 },
	{ "AaDerivativeMult", 	OVR::CAPI::D3D_NS::ShaderBase::VARTYPE_FLOAT, 8, 4 },
};

#endif
