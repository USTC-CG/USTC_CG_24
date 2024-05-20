# script.py


def declare_node():
    return [["a", "b"], ["c"]]

def wrap_exec(list):
    return exec_node(*list)

import numpy as np

def exec_node(a, b):
    print(type(a))
    return a + b
