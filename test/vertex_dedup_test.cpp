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
};

struct TimingResults {
  double ConstructTreeWithRandomItems;
  double VertexDedup1;
  double BuildTreeFromOrderedItems;
  double VertexDedup2;
  double ParallelVertexDedup;
};

Configuration parse_command_line(int argn, char **argv);
struct GeoHashedOctree BuildTreeWithRandomItems(struct GeoBoundingBox bbox, int n, int *indices);
struct GeoHashedOctree BuildTreeFromOrderedItems(
    struct GeoBoundingBox bbox, const struct GeoVertexArray *va);
void VertexDedup(struct GeoHashedOctree* tree);


int main(int argn, char **argv) {
  Configuration conf = parse_command_line(argn, argv);

  TimingResults results = {0, 0, 0, 0, 0};

  std::cout.precision(5);
  std::cout << std::scientific;

  std::cout << "{\n";
  std::cout << "  \"num_vertices\": " << conf.num_vertices << ",\n";
  std::cout << "  \"num_iter\": " << conf.num_iter << ",\n";
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
    VertexDedup(&tree);
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
    VertexDedup(&tree2);
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
  std::cout << "    \"ParallelVertexDedup\":            " << results.ParallelVertexDedup << "\n";
  std::cout << "  },\n";

  std::cout << "  \"averages\": {\n";
  std::cout << "    \"ConstructTreeWithRandomItems\":   " << results.ConstructTreeWithRandomItems / conf.num_iter << ",\n";
  std::cout << "    \"VertexDedup1\":                   " << results.VertexDedup1 / conf.num_iter << ",\n";
  std::cout << "    \"BuildTreeFromOrderedItems\":      " << results.BuildTreeFromOrderedItems / conf.num_iter << ",\n";
  std::cout << "    \"VertexDedup2\":                   " << results.VertexDedup2 / conf.num_iter << "\n";
  std::cout << "    \"ParallelVertexDedup\":            " << results.ParallelVertexDedup / conf.num_iter << "\n";
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
  GeoHOInsert(&tree, &va, 0, n);

  GeoVADestroy(&va);

  return tree;
}

struct GeoHashedOctree BuildTreeFromOrderedItems(
    struct GeoBoundingBox bbox,
    const struct GeoVertexArray *va) {
  struct GeoHashedOctree tree;
  GeoHOInitialize(&tree, bbox);
  GeoHOInsert(&tree, va, 0, va->size);
  return tree;
}

struct DedupCtx {
  int self;
  std::vector<int> duplicate_indices;
};

extern "C"
void DedupVisitor(struct GeoVertexArray* va, int i, void* ctx)
{
  DedupCtx *dedup_ctx = static_cast<DedupCtx*>(ctx);
  if (i < dedup_ctx->self) {
    dedup_ctx->duplicate_indices.push_back(dedup_ctx->self);
  }
}

void VertexDedup(struct GeoHashedOctree *tree) {
  double eps = 1.0e-3;
  DedupCtx ctx;
  int n = tree->vertices.size;
  for (int i = 0; i < n; ++i) {
    ctx.self = i;
    struct GeoPoint p =
        {tree->vertices.x[i], tree->vertices.y[i], tree->vertices.z[i]};
    GeoHOVisitNearVertices(tree, &p, eps, DedupVisitor, &ctx);
  }
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
    "\n\n"
    "Available tree_types:\n"
    "  HashedOctree\n"
    "  WideTree\n"
#ifdef HOT_HAVE_TBB
    "  HashedOctreeParallel\n"
    "  WideTreeParallel\n"
#endif
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

