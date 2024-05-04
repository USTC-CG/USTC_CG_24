# script.py
import numpy as np
from pxr import Tf
from pxr import Usd, UsdGeom
import torch

def process_array(arr):
    # Assuming 'arr' is your VtArray<float>
    nparr = np.array(arr, dtype=np.float32)
    tensor = torch.from_numpy(nparr)
    print(tensor)
    return tensor
