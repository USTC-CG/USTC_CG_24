#include <pxr/base/vt/array.h>
#include <pxr/base/vt/arrayPyBuffer.h>

#include <boost/optional/optional_io.hpp>
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/python/numpy/ndarray.hpp>
#include <iostream>
#include <vector>

namespace bp = boost::python;
namespace bpn = boost::python::numpy;
// Function to call the Python script and pass the VtArray<float> to it
void call_python_script(const pxr::VtArray<float>& arr)
{
    Py_Initialize();
    bpn::initialize();

    // Import the Python module
    bp::object module = bp::import("script");

    // Call the 'process_array' function from the module
    bp::object result = (module.attr("process_array")(arr));
    bpn::ndarray np_arr = bpn::array(result);
    std::string err;
    boost::optional<pxr::VtArray<float>> vt_arr =
        pxr::VtArrayFromPyBuffer<float>(pxr::TfPyObjWrapper(result));

    std::cout << "C++ side:" << vt_arr << std::endl;

    std::cout << "C++ numpy side: ";
    for (int i = 0; i < 5; ++i) {
        auto val = ((float*)(np_arr.get_data()))[i];
        bp::list l;
        

        std::cout << val << ' ';
    }
    std::cout << std::endl;

    // Do something with the result vector...
}

int main()
{
    // Assuming 'arr' is your VtArray<float>
    pxr::VtArray<float> arr = { 1.1, 2.2, 3.3, 4.4, 5.5 };

    // Call the Python script with the VtArray<float>
    call_python_script(arr);

    Py_Finalize();

    return 0;
}
