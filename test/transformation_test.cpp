#include <transformation.h>
#include <vertex_array.h>
#include <test_utilities.h>
#include <string>
#include <iostream>
#include <memory>
#include <vector>


struct Configuration {
  int num_vertices;
  int num_iter;
};

struct TimingResults {
  double outofplace;
  double inplace;
};

Configuration parse_command_line(int argn, char **argv);
struct GeoVertexArray build_va(int n);


int main(int argn, char **argv) {
  Configuration conf = parse_command_line(argn, argv);

  TimingResults results = {0, 0};

  struct GeoVertexArray a = build_va(conf.num_vertices);
  struct GeoVertexArray b = build_va(conf.num_vertices);
  double T[3][4] = {
    {1, 2, 3, 4},
    {5, 6, 7, 8},
    {9, 0, 1, 2}};

  std::cout.precision(5);
  std::cout << std::scientific;

  std::cout << "{\n";
  std::cout << "  \"num_vertices\": " << conf.num_vertices << ",\n";
  std::cout << "  \"num_iter\": " << conf.num_iter << ",\n";
  for (int i = 0; i < conf.num_iter; ++i) {

    std::cout << "  \"iteration " << i << "\": {\n";

    std::cout << "    \"timings\": {\n";

    uint64_t start, end;
    start = rdtsc();
    GeoApplyTransform(T, &a, &b);
    end = rdtsc();
    std::cout << "      \"out-of-place\": " << (end - start) / 1.0e6 << ",\n";
    results.outofplace += (end - start) / 1.0e6;

    start = rdtsc();
    GeoApplyTransformInplace(T, &a);
    end = rdtsc();
    std::cout << "      \"in-place\":     " << (end - start) / 1.0e6 << "\n";
    results.inplace += (end - start) / 1.0e6;

    std::cout << "    }\n  }," << std::endl;
  }

  std::cout << "  \"totals\": {\n";
  std::cout << "    \"out-of-place\":   " << results.outofplace << ",\n";
  std::cout << "    \"in-place\":       " << results.inplace << "\n";
  std::cout << "  },\n";

  std::cout << "  \"averages\": {\n";
  std::cout << "    \"out-of-place\":   " << results.outofplace / conf.num_iter << ",\n";
  std::cout << "    \"in-place\":       " << results.inplace / conf.num_iter << ",\n";
  std::cout << "  }\n";
  std::cout << "}\n";

  GeoVADestroy(&a);
  GeoVADestroy(&b);
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
    );

Configuration parse_command_line(int argn, char **argv) {
  Configuration conf;
  conf.num_vertices = 100;
  conf.num_iter = 10;

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

  return conf;
}

struct GeoVertexArray build_va(int n) {
  struct GeoVertexArray va;
  GeoVAInitialize(&va);
  GeoVAResize(&va, n);
  for (int i = 0; i < n; ++i) {
    va.x[i] = 6;
    va.y[i] = 7;
    va.z[i] = 8;
  }
  return va;
}
