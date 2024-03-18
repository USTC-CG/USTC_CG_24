#include <openvdb/openvdb.h>
#include <openvdb/tools/LevelSetSphere.h>

#include <pxr/usd/usd/prim.h>
#include <pxr/usd/usd/stage.h>
#include <pxr/usd/usdVol/openVDBAsset.h>

int main()
{
    openvdb::initialize();
    openvdb::FloatGrid::Ptr grid =
        openvdb::tools::createLevelSetSphere<openvdb::FloatGrid>(
            50.0,
            openvdb::Vec3f(1.5, 2, 3),
            0.5,
            4.0);
    grid->insertMeta("radius", openvdb::FloatMetadata(50.0));
    grid->setName("LevelSetSphere");
    openvdb::io::File("sphere.vdb").write({ grid });

    pxr::UsdStageRefPtr stage = pxr::UsdStage::CreateNew("my_volume.usda");
    pxr::UsdVolOpenVDBAsset vdbVol =
        pxr::UsdVolOpenVDBAsset::Define(
            stage,
            pxr::SdfPath("/volume/vdbField"));

    vdbVol.GetFilePathAttr().Set(pxr::SdfAssetPath("sphere.vdb"));

    // Save the stage to disk
    stage->GetRootLayer()->Save();

    return 0;
}
