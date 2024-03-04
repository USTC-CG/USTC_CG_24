#include "annoylib.h"
#include "kissrandom.h"

#include <vector>
#include <iostream>


using namespace Annoy;
int main() {
  int f = 40;
  AnnoyIndex<int, float, Euclidean, Kiss32Random, AnnoyIndexSingleThreadedBuildPolicy> index(f); //  π”√≈∑ œæ‡¿Î

  // Randomly sample 1000 points in dimension f
  for (int i = 0; i < 1000; ++i) {
    std::vector<float> vec(f);
    for (int z = 0; z < f; ++z) {
      vec[z] = (float)rand() / RAND_MAX;
    }
    index.add_item(i, vec.data());
  }

  index.build(10); // 10 trees

  int item_index = 43;
  std::vector<int> closest_items;
  std::vector<float> distances;
  // Find 10 nearest neighbors of the item at index 42
  index.get_nns_by_item(item_index, 10, -1, &closest_items, &distances); 

  for (int i = 0; i < closest_items.size(); ++i) {
    std::cout << "Found neighbor: " << closest_items[i] << " Distance: " << distances[i] << std::endl;
  }

  return 0;
}
