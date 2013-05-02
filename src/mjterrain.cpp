/*
    mjterrain.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjterrain.h"
#include <cstdlib>
#include <cmath>

namespace mcjee {

    Terrain::Terrain(float size) : curDepth(0), size(size) {
        reset();
    }
    
    void Terrain::generate(unsigned int depth, unsigned int seed) {
        targetDepth = depth;
        srand(seed);
        reset();
        triangles.pop_back();
        midpointmap midmap = midpointmap();
        subdivide(0, 1, 2, depth, midmap);
        //normalize normals
        std::vector<Vector3>::iterator i;
        for (i = normals.begin(); i != normals.end(); ++i) {
            i->normalize();
        }
        curDepth = depth;
    }

    // - new triangle is added to triangle array if bottom reached
    // - First finds midpoints
    // - if shared midpoints are found in the map those are used
    //   instead of generating new ones.
    // - generated midpoint indices are added to a map to share 
    //   with other tris.
    // - midpoint vertex data is added to vertices.
    void Terrain::subdivide(index_t a,
                            index_t b,
                            index_t c,
                            unsigned int depth,
                            midpointmap &midmap) {
        if (depth == 0) {
            Triangle tri = {a, b, c};
            triangles.push_back(tri);
            Vector3 &va = vertices[tri.a];
            Vector3 &vb = vertices[tri.b];
            Vector3 &vc = vertices[tri.c];
            Vector3 vx = va-vb;
            Vector3 vy = vc-vb;
            Vector3 surfaceNormal = vx.cross(vy);
            normals[a] += surfaceNormal;
            normals[b] += surfaceNormal;
            normals[c] += surfaceNormal;
        } else {
            index_t midab, midbc, midca;
            midab = midpointIndex(a, b, midmap, depth);
            midbc = midpointIndex(b, c, midmap, depth);
            midca = midpointIndex(c, a, midmap, depth);
            subdivide(a, midab, midca, depth-1, midmap);
            subdivide(b, midbc, midab, depth-1, midmap);
            subdivide(c, midca, midbc, depth-1, midmap);
            subdivide(midab, midbc, midca, depth-1, midmap);
        }
    }

    // returns index of midpoint between a and b
    // if found, the index is returned and delted from the map,
    // since each midpoint can be shared by at most one pair.
    // if not found, the midpoints is generated and added to the map.
    index_t Terrain::midpointIndex(index_t a,
                                   index_t b,
                                   midpointmap &midpoints,
                                   unsigned int depth) {
        index_t index;
        midkey abkey = midkey(a, b);
        midkey bakey = midkey(b, a);
        midpointmap::iterator it;
        if ((it = midpoints.find(abkey)) != midpoints.end()) {
            index = it->second;
            midpoints.erase(it);
        } else if ((it = midpoints.find(bakey)) != midpoints.end()) {
            index = it->second;
            midpoints.erase(it);
        } else {
            Vector3 newMidab = midpoint(vertices[a], vertices[b]);
            newMidab.y += calculateOffset(depth);
            index = vertices.size();
            vertices.push_back(newMidab);
            normals.push_back(Vector3(0, 0, 0));
            midpoints[abkey] = index;
        }
        return index;
    }

    // depth^2 falloff
    // during generation, depth goes from targetDepth -> 0
    float Terrain::calculateOffset(unsigned int depth) {
        float attenuate = 1.0/(targetDepth-depth+1);
        return (1.0*rand()/RAND_MAX-0.5)*pow(attenuate, 1.8);
    }

    Vector3 Terrain::midpoint(Vector3 &a, Vector3 &b) {
        return Vector3((a.x+b.x)/2, (a.y+b.y)/2, (a.z+b.z)/2);
    }

    void Terrain::reset(void) {
        vertices = std::vector<Vector3>();
        triangles = std::vector<Triangle>();
        normals = std::vector<Vector3>();

        Vector3 v1(0.0, 0.0, 0.0);
        Vector3 v2(size, 0.0, 0.0);
        Vector3 v3(size, 0.0, size);
        Triangle triangle = {0, 1, 2};

        vertices.push_back(v1);
        vertices.push_back(v2);
        vertices.push_back(v3);
        normals.push_back(Vector3(0, 0, 0));
        normals.push_back(Vector3(0, 0, 0));
        normals.push_back(Vector3(0, 0, 0));
        triangles.push_back(triangle);
    }

    // The formula for # of vertices in a subdivided triangle is:
    // c = (2^n + 1)(2^n + 2)/2
    // Derivation:
    // The # of vertices in the base of the triangle, b = 2^n + 1
    // So, the total number of vertices must be c = sum(i) from 1 to b
    // Fancified with bit shifting!
    long Terrain::vertexCount(void) {
        unsigned int n = curDepth;
        return ((1 << n+n) + (1 << n+1) + (1 << n) + 2) >> 1;
    }

    // number of triangles * 3 vertices
    long Terrain::indexCount(void) {
        return (long)pow(4, curDepth)*3;
    }

    float *Terrain::vertexData(void) {
        return (float *)(&vertices[0]);
    }

    float *Terrain::normalData(void) {
        return (float *)(&normals[0]);
    }

    index_t *Terrain::indexData(void) {
        return (index_t *)&triangles[0];
    }
}
