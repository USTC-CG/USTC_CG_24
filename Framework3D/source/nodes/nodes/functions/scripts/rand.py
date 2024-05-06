# script.py
import numpy as np
from pxr import Tf
from pxr import Usd, UsdGeom

def process_array(arr):
    # Assuming 'arr' is your VtArray<float>
    nparr = np.array(arr, dtype=np.float32)
    nparr = np.random.rand(5).astype(np.float32)
    print(nparr)
    return nparr
