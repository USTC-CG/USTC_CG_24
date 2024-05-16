# script.py


def declare_node():
    return [["a", "b"], ["c"]]

def wrap_exec(list):
    exec_node(*list)

import numpy as np
from pxr import Tf
from pxr import Usd, UsdGeom



def exec_node(a, b):
    return a + b
