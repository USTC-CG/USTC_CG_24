from plyfile import PlyData, PlyElement
import numpy as np


def read_model():
    ply_path = "test.ply"
    plydata = PlyData.read(ply_path)
    xyz = np.stack(
        (
            np.asarray(plydata.elements[0]["x"]),
            np.asarray(plydata.elements[0]["y"]),
            np.asarray(plydata.elements[0]["z"]),
        ),
        axis=1,
    )
    opacity = np.asarray(plydata.elements[0]["opacity"])[..., np.newaxis]
    trbf_center = np.asarray(plydata.elements[0]["trbf_center"])[..., np.newaxis]
    trbf_scale = np.asarray(plydata.elements[0]["trbf_scale"])[..., np.newaxis]

    motion = np.zeros((xyz.shape[0], 9))
    for i in range(9):
        motion[:, i] = np.asarray(plydata.elements[0]["motion_{}".format(i)])

    features_dc = np.zeros((xyz.shape[0], 6))
    for i in range(6):
        features_dc[:, i] = np.asarray(plydata.elements[0]["f_dc_{}".format(i)])

    scale_names = [
        p.name for p in plydata.elements[0].properties if p.name.startswith("scale_")
    ]
    scale_names = sorted(scale_names, key=lambda x: int(x.split("_")[-1]))
    scales = np.zeros((xyz.shape[0], len(scale_names)))
    for idx, attr_name in enumerate(scale_names):
        scales[:, idx] = np.asarray(plydata.elements[0][attr_name])

    rot_names = [
        p.name for p in plydata.elements[0].properties if p.name.startswith("rot")
    ]
    rot_names = sorted(rot_names, key=lambda x: int(x.split("_")[-1]))
    rots = np.zeros((xyz.shape[0], len(rot_names)))

    for idx, attr_name in enumerate(rot_names):
        rots[:, idx] = np.asarray(plydata.elements[0][attr_name])

    omega_names = [
        p.name for p in plydata.elements[0].properties if p.name.startswith("omega")
    ]
    omegas = np.zeros((xyz.shape[0], len(omega_names)))
    for idx, attr_name in enumerate(omega_names):
        omegas[:, idx] = np.asarray(plydata.elements[0][attr_name])

    ft_names = [
        p.name for p in plydata.elements[0].properties if p.name.startswith("f_t")
    ]
    fts = np.zeros((xyz.shape[0], len(ft_names)))
    for idx, attr_name in enumerate(ft_names):
        fts[:, idx] = np.asarray(plydata.elements[0][attr_name])

    return (
        xyz,
        opacity,
        trbf_center,
        trbf_scale,
        motion,
        features_dc,
        scales,
        rots,
        omegas,
        fts,
    )

if __name__ == "__main__":
    d = read_model()
    print("Done")