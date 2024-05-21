# script.py


def declare_node():
    return [["a", "b"], ["c"]]


def wrap_exec(list):
    return exec_node(*list)


import numpy as np
import torch

import drjit as dr


def exec_node(a, b):
    print("a ", type(a))
    print("b ", type(b))
    return np.sin(a) + b
