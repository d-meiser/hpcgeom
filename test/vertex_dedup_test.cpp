#include <hashed_octree.h>
#include <test_utilities.h>
#include <string>
#include <iostream>
#include <cassert>
#include <memory>
#include <vector>


struct Configuration {
  int num_vertices;
  int num_iter;
  double epsilon;
};

struct TimingResults {
  double ConstructTreeWithRandomItems;
  double VertexDedup1;
  double BuildTreeFromOrderedItems;
  double VertexDedup2;
};

Configuration parse_command_line(int argn, char **argv);
struct GeoHashedOctree BuildTreeWithRandomItems(struct GeoBoundingBox bbox, int n, int *indices);
struct GeoHashedOctree BuildTreeFromOrderedItems(
    struct GeoBoundingBox bbox, const struct GeoVertexArray *va);


int main(int argn, char **argv) {
  Configuration conf = parse_command_line(argn, argv);

  TimingResults results = {0, 0, 0, 0};

  std::cout.precision(5);
  std::cout << std::scientific;

  std::cout << "{\n";
  std::cout << "  \"num_vertices\": " << conf.num_vertices << ",\n";
  std::cout << "  \"num_iter\": " << conf.num_iter << ",\n";
  std::cout << "  \"epsilon\": " << conf.epsilon << ",\n";
  for (int i = 0; i < conf.num_iter; ++i) {

    std::cout << "  \"iteration " << i << "\": {\n";

    std::cout << "    \"timings\": {\n";

    uint64_t start, end;
    std::vector<int> indices(conf.num_vertices);
    start = rdtsc();
    struct GeoHashedOctree tree =
        BuildTreeWithRandomItems(UnitCube(), conf.num_vertices, &indices[0]);
    end = rdtsc();
    std::cout << "      \"ConstructTreeWithRandomItems\": " << (end - start) / 1.0e6 << ",\n";
    results.ConstructTreeWithRandomItems += (end - start) / 1.0e6;

    start = rdtsc();
    GeoHODeleteDuplicates(&tree, conf.epsilon, 0, 0);
    end = rdtsc();
    std::cout << "      \"VertexDedup1\":                 " << (end - start) / 1.0e6 << ",\n";
    results.VertexDedup1 += (end - start) / 1.0e6;

    start = rdtsc();
    struct GeoHashedOctree tree2 =
        BuildTreeFromOrderedItems(UnitCube(), &tree.vertices);
    end = rdtsc();
    std::cout << "      \"BuildTreeFromOrderedItems\":    " << (end - start) / 1.0e6 << ",\n";
    results.BuildTreeFromOrderedItems += (end - start) / 1.0e6;

    start = rdtsc();
    GeoHODeleteDuplicates(&tree2, conf.epsilon, 0, 0);
    end = rdtsc();
    std::cout << "      \"VertexDedup2\":                 " << (end - start) / 1.0e6 << "\n";
    results.VertexDedup2 += (end - start) / 1.0e6;

    std::cout << "    }\n  }," << std::endl;

    GeoHODestroy(&tree2);
    GeoHODestroy(&tree);
  }

  std::cout << "  \"totals\": {\n";
  std::cout << "    \"ConstructTreeWithRandomItems\":   " << results.ConstructTreeWithRandomItems << ",\n";
  std::cout << "    \"VertexDedup1\":                   " << results.VertexDedup1 << ",\n";
  std::cout << "    \"BuildTreeFromOrderedItems\":      " << results.BuildTreeFromOrderedItems << ",\n";
  std::cout << "    \"VertexDedup2\":                   " << results.VertexDedup2 << "\n";
  std::cout << "  },\n";

  std::cout << "  \"averages\": {\n";
  std::cout << "    \"ConstructTreeWithRandomItems\":   " << results.ConstructTreeWithRandomItems / conf.num_iter << ",\n";
  std::cout << "    \"VertexDedup1\":                   " << results.VertexDedup1 / conf.num_iter << ",\n";
  std::cout << "    \"BuildTreeFromOrderedItems\":      " << results.BuildTreeFromOrderedItems / conf.num_iter << ",\n";
  std::cout << "    \"VertexDedup2\":                   " << results.VertexDedup2 / conf.num_iter << "\n";
  std::cout << "  }\n";
  std::cout << "}\n";
}

struct GeoHashedOctree BuildTreeWithRandomItems(
    struct GeoBoundingBox bbox, int n, int *indices) {
  assert(n > 0);
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  GeoVAResize(&va, n);
  FillWithRandomItems(&va, &bbox, n, indices);

  struct GeoHashedOctree tree;
  GeoHOInitialize(&tree, bbox);
  GeoHOInsert(&tree, &va);

  GeoVADestroy(&va);

  return tree;
}

struct GeoHashedOctree BuildTreeFromOrderedItems(
    struct GeoBoundingBox bbox,
    const struct GeoVertexArray *va) {
  struct GeoHashedOctree tree;
  GeoHOInitialize(&tree, bbox);
  GeoHOInsert(&tree, va);
  return tree;
}


static int find_string(std::string s, int argn, char **argv) {
  int i = 1;
  for (; i != argn; ++i) {
    if (s == argv[i]) break;
  }
  return i;
}

static const std::string usage(
    "Usage: vertex_dedup_test "
    "[--num_vertices num_vertices] "
    "[--num_iter num_iter] "
    "[--epsilon epsilon] "
    );

Configuration parse_command_line(int argn, char **argv) {
  Configuration conf;
  conf.num_vertices = 100;
  conf.num_iter = 10;
  conf.epsilon = 1.0e-3;

  int i;
  i = find_string("--help", argn, argv);
  if (i != argn) {
    std::cout << usage << std::endl;
    exit(0);
  }

  i = find_string("--num_vertices", argn, argv);
  if (i != argn) {
    if (i == argn - 1) {
      std::cout << "Error: Number of vertices parameter missing." << std::endl;
      std::cout << usage << std::endl;
      exit(1);
    }
    conf.num_vertices = std::stoi(std::string(argv[i + 1]));
  }

  i = find_string("--num_iter", argn, argv);
  if (i != argn) {
    if (i == argn - 1) {
      std::cout << "Error: Number of iterations parameter missing." << std::endl;
      std::cout << usage << std::endl;
      exit(1);
    }
    conf.num_iter = std::stoi(std::string(argv[i + 1]));
  }

  i = find_string("--epsilon", argn, argv);
  if (i != argn) {
    if (i == argn - 1) {
      std::cout << "Error: epsilon missing." << std::endl;
      std::cout << usage << std::endl;
      exit(1);
    }
    conf.epsilon = std::stod(std::string(argv[i + 1]));
  }

  return conf;
}

