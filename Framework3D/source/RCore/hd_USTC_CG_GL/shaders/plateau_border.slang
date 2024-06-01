struct RayPayload {
    float3 color;
};

struct Cylinder {
    float3 center;
    float radius;
    float height;
};

bool intersectCylinder(float3 rayOrigin, float3 rayDirection, Cylinder cylinder, out float t) {
    float3 oc = rayOrigin - cylinder.center;
    float3 q = rayDirection * rayDirection - dot(rayDirection, cylinder.center) * cylinder.center;
    float a = dot(q, q);
    float b = 2.0 * dot(q, oc);
    float c = dot(oc, oc) - cylinder.radius * cylinder.radius;

    float discriminant = b * b - 4.0 * a * c;

    if (discriminant < 0.0) {
        return false;
    }

    float sqrtDiscriminant = sqrt(discriminant);
    float t0 = (-b - sqrtDiscriminant) / (2.0 * a);
    float t1 = (-b + sqrtDiscriminant) / (2.0 * a);

    float tMin = min(t0, t1);
    float tMax = max(t0, t1);

    float tIntersect = -1.0;

    if (tMin >= 0.0 && tMax >= 0.0) {
        tIntersect = tMin;
    } else if (tMin < 0.0 && tMax >= 0.0) {
        tIntersect = tMax;
    }

    float3 intersectionPoint = rayOrigin + rayDirection * tIntersect;
    float heightCheck = dot(intersectionPoint - cylinder.center, cylinder.center + float3(0, cylinder.height, 0));
    
    if (heightCheck < 0 || heightCheck > cylinder.height * cylinder.height)
        return false;

    t = tIntersect;
    return true;
}

[shader("intersection")]
void IntersectCylinder(inout RayPayload payload, float t : SV_IntersectionDistance) {
    Cylinder cylinder;
    cylinder.center = float3(0, 0, 0); // Example center
    cylinder.radius = 1.0; // Example radius
    cylinder.height = 2.0; // Example height

    float3 rayOrigin = ray.origin;
    float3 rayDirection = ray.direction;

    float intersectionT;
    if (intersectCylinder(rayOrigin, rayDirection, cylinder, intersectionT)) {
        if (intersectionT < t) {
            t = intersectionT;
            payload.color = float3(1, 0, 0); // Example color
        }
    }
}
