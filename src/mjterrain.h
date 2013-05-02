/*
    mjterrain.h
    Matthew Jee
    mcjee@ucsc.edu
*/

#include <vector>
#include <map>
#include "mjmath.h"

namespace mcjee {

    typedef std::pair<unsigned int, unsigned int> midkey;
    typedef std::map<midkey, unsigned int> midpointmap;
    typedef unsigned int index_t;

    class Terrain {
    private:
        // --- Internal Structures ---
        // Special triangle struct that references indices
        // but does not itself contain them.
        struct Triangle {
            index_t a;
            index_t b;
            index_t c;
        };

        // --- Class members ---
        std::vector<Vector3> vertices; // must be
        std::vector<Vector3> normals; // manually interleaved
        std::vector<Triangle> triangles; // 0 1 2 ...
        unsigned int curDepth;
        unsigned int targetDepth;
        float size;
        void subdivide(index_t a,
                       index_t b,
                       index_t c,
                       unsigned int depth,
                       midpointmap &midmap);
        index_t midpointIndex(index_t a,
                              index_t b,
                              midpointmap &midpoints,
                              unsigned int depth);
        Vector3 midpoint(Vector3 &a, Vector3 &b);
        float calculateOffset(unsigned int depth);
        void reset(void);
    public:
        explicit Terrain(float size);
        void generate(unsigned int depth, unsigned int seed);
        long vertexCount(void);
        long indexCount(void);
        float *vertexData(void);
        float *normalData(void);
        index_t *indexData(void);
    };
}
