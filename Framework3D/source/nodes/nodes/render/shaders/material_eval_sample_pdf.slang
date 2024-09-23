// material_eval_sample_pdf.slang

#include "matrix_op.h"
#include "utils/HitObject.h"
#include "view_cb.h"
RaytracingAccelerationStructure SceneBVH : register(t0);

StructuredBuffer<HitObjectInfo> hitObjects : register(t1);

RWStructuredBuffer<HitObjectInfo> PixelTarget : register(u0);
RWStructuredBuffer<HitObjectInfo> Eval : register(u1);
RWStructuredBuffer<HitObjectInfo> Sample : register(u2);
RWStructuredBuffer<HitObjectInfo> Weight : register(u3);
RWStructuredBuffer<HitObjectInfo> Pdf : register(u4);

struct HitInfo {
    int foo;
};

struct Attributes {
    float2 uv;
};

// Ray generation shader
[shader("raygeneration")]
void RayGen()
{
    int2 id = DispatchRaysIndex().xy;

    HitObjectInfo hitObject = hitObjects[id.x];

    // Ray generation logic here
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

    //ReorderThread(object);
}

// Closest hit shader
[shader("closesthit")]
void ClosestHit(
    inout HitInfo payload: SV_RayPayload,
    Attributes attrib: SV_IntersectionAttributes)
{
    // Closest hit logic here
}

// Miss shader
[shader("miss")]
void Miss(inout HitInfo payload: SV_RayPayload)
{
    // Miss logic here
}
