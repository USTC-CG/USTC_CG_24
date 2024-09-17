
#include "torch/torch.h"

#include <boost/python.hpp>
#include <iostream>

#include "boost/python/numpy.hpp"

namespace bp = boost::python;

namespace np = boost::python::numpy;

void print_array_info(const np::ndarray& arr, const std::string& name)
{
    std::cout << "Array: " << name << std::endl;
    std::cout << "Shape: (";
    for (int i = 0; i < arr.get_nd(); ++i) {
        std::cout << arr.shape(i);
        if (i < arr.get_nd() - 1) {
            std::cout << ", ";
        }
    }
    std::cout << ")" << std::endl;

    std::cout << "Data type: "
              << bp::extract<const char*>(bp::str(arr.get_dtype()))
              << std::endl;
    std::cout << "First element: " << ((float*)arr.get_data())[0]
              << std::endl;  // Assuming the array contains doubles
    std::cout << std::endl;
}

int main()
{
    Py_Initialize();
    boost::python::numpy::initialize();
    try {
        bp::object m = bp::import("read_model");
        bp::object read_results = m.attr("read_model")();
        auto list = bp::list(read_results);
        std::cout << len(list) << std::endl;

        auto xyz = np::array(list[0]);
        auto opacity = np::array(list[1]);
        auto trbf_center = np::array(list[2]);
        auto trbf_scale = np::array(list[3]);
        auto motion = np::array(list[4]);
        auto features_dc = np::array(list[5]);
        auto scales = np::array(list[6]);
        auto rots = np::array(list[7]);
        auto omegas = np::array(list[8]);
        auto fts = np::array(list[9]);

        print_array_info(xyz, "xyz");
        print_array_info(opacity, "opacity");
        print_array_info(trbf_center, "trbf_center");
        print_array_info(trbf_scale, "trbf_scale");
        print_array_info(motion, "motion");
        print_array_info(features_dc, "features_dc");
        print_array_info(scales, "scales");
        print_array_info(rots, "rots");
        print_array_info(omegas, "omegas");
        print_array_info(fts, "fts");
    }
    catch (const bp::error_already_set&) {
        PyErr_Print();
        throw std::runtime_error("Python error.");
    }
}