// material_eval_sample_pdf.slang

#include "matrix_op.h"
#include "utils/HitObject.h"
#include "view_cb.h"
RaytracingAccelerationStructure SceneBVH : register(t0);

StructuredBuffer<HitObjectInfo> hitObjects : register(t1);
StructuredBuffer<uint2> in_PixelTarget : register(t2);

RWStructuredBuffer<uint2> PixelTarget : register(u0);
RWStructuredBuffer<float4> Eval : register(u1);
RWStructuredBuffer<RayDesc> Sample : register(u2);
RWStructuredBuffer<float> Weight : register(u3);
RWStructuredBuffer<float> Pdf : register(u4);

struct Payload {
    int foo;
};

struct Attributes {
    float2 uv;
};

// Ray generation shader
[shader("raygeneration")]
void RayGen()
{
    int id = DispatchRaysIndex().x;

    HitObjectInfo hitObject = hitObjects[id];

    HitObject object = HitObject.MakeHit<Attributes>(
        SceneBVH,
        hitObject.InstanceIndex,
        hitObject.GeometryIndex,
        hitObject.PrimitiveIndex,
        hitObject.HitKind,
        hitObject.RayContributionToHitGroupIndex,
        hitObject.MultiplierForGeometryContributionToHitGroupIndex,
        hitObject.Ray,
        { hitObject.attributes });

    ReorderThread(object);

    Payload payload;
    object.Invoke(SceneBVH, object, payload);
}

// Closest hit shader
[shader("closesthit")]
void ClosestHit(
    inout Payload payload: SV_RayPayload,
    Attributes attrib: SV_IntersectionAttributes)
{
    uint id = DispatchRaysIndex().x;
    PixelTarget[id] = in_PixelTarget[id];
    Eval[id] = float4(attrib.uv, 0, 1);

    // Acquire the material information here.
}

// Miss shader
[shader("miss")]
void Miss(inout Payload payload: SV_RayPayload)
{
    uint id = DispatchRaysIndex().x;
    PixelTarget[id] = in_PixelTarget[id];
    Eval[id] = float4(1, 0, 1, 1);
}
