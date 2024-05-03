#include <pxr/base/vt/array.h>

#include <boost/python.hpp>
#include <vector>

using namespace pxr;

namespace bp = boost::python;

// Function to call the Python script and pass the VtArray<float> to it
void call_python_script(const VtArray<float>& arr)
{
    Py_Initialize();

    // Import the Python module
    bp::object module = bp::import("script");

    // Call the 'process_array' function from the module
    bp::object result = (module.attr("process_array")(arr));

    module.attr("a") = 0.1;

    module.attr("print_a")();

    // Do something with the result vector...
}

int main()
{
    // Assuming 'arr' is your VtArray<float>
    VtArray<float> arr = { 1.1, 2.2, 3.3, 4.4, 5.5 };

    // Call the Python script with the VtArray<float>
    call_python_script(arr);

    Py_Finalize();

    return 0;
}
