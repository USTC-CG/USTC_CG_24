// material_eval_sample_pdf.slang

#include "matrix_op.h"
#include "utils/HitObject.h"
#include "view_cb.h"
RaytracingAccelerationStructure SceneBVH : register(t0);

StructuredBuffer<HitObjectInfo> hitObjects : register(t1);

// Ray generation shader
[shader("raygeneration")]
void RayGen()
{
    int2 id = DispatchRaysIndex().xy;

    HitObjectInfo hitObject = hitObjects[id.x];

    // Ray generation logic here
    HitObject object = HitObject.MakeHit<float4>(
        SceneBVH,
        hitObject.InstanceIndex,
        hitObject.GeometryIndex,
        hitObject.PrimitiveIndex,
        hitObject.HitKind,
        hitObject.RayContributionToHitGroupIndex,
        hitObject.MultiplierForGeometryContributionToHitGroupIndex,
        hitObject.Ray,
        hitObject.attributes);

    ReorderThread(object);
}

// Closest hit shader
[shader("closesthit")]
void ClosestHit()
{
    // Closest hit logic here
}

// Miss shader
[shader("miss")]
void Miss()
{
    // Miss logic here
}
