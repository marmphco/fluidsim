/*
    mjloader.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjloader.h"
#include "mjvector.h"
#include "mjprimitive.h"
#include <vector>
#include <stdio.h> //i like this better
#include <string.h>
#include <stdlib.h>

namespace mcjee {

int loadVertices(float **vertices, const char *path) {
    FILE *file = fopen(path, "r");
    int vertexCount;
    fscanf(file, "%d", &vertexCount);
    *vertices = new float[vertexCount*3];

    int dummy;
    for (int i = 0; i < vertexCount*3; i += 3) {
        fscanf(file, "%d,%f,%f,%f", &dummy, *vertices+i, *vertices+i+1, *vertices+i+2);
    }
    fclose(file);
    return vertexCount;
}

// needs error checking
//the indices are minus one because of zero-indexing
int loadTris(unsigned int **indices, const char *path) {
    FILE *file = fopen(path, "r");
    int polyCount;
    fscanf(file, "%d\n", &polyCount);
    
    std::vector<unsigned int> indexVector;
    for (int i = 0; i < polyCount; ++i) {
        char line[128];
        char *ptr = fgets(line, 128, file);
        if (ptr == NULL) {
            fprintf(stderr, "Invalid format.");
            fclose(file);
            return 0;
        }
        char *token = strtok(line, " ");
        //char *name = token; // first token is the mesh name
        int first = atoi(strtok(NULL, " "))-1; //second token is first vertex
        int prev = atoi(strtok(NULL, " "))-1; //third token is second vertex
        while ((token = strtok(NULL, " ")) != NULL) {
            if (*token == '\n') break;
            int cur = atoi(token)-1;
            indexVector.push_back(first);
            indexVector.push_back(prev);
            indexVector.push_back(cur);
            prev = cur;
        }
    }
    fclose(file);

    *indices = new unsigned int[indexVector.size()];
    memcpy(*indices, &(indexVector[0]), sizeof(unsigned int)*indexVector.size());
    return indexVector.size();
}

int loadTrisAndNormals(unsigned int **indices,
                       float *normals,
                       float *vertices,
                       long vertexCount,
                       const char *path) {
    Vector3 *vnormals = (Vector3 *)normals;
    Vector3 *vvertices = (Vector3 *)vertices;

    FILE *file = fopen(path, "r");
    int polyCount;
    fscanf(file, "%d\n", &polyCount);

    for (int i = 0; i < vertexCount; ++i) {
        vnormals[i] = Vector3(0, 0, 0);
    }

    std::vector<unsigned int> indexVector;
    for (int i = 0; i < polyCount; ++i) {
        char line[128];
        char *ptr = fgets(line, 128, file);
        if (ptr == NULL) {
            fprintf(stderr, "Invalid format.");
            fclose(file);
            return 0;
        }
        char *token = strtok(line, " ");
        //char *name = token; // first token is the mesh name
        int first = atoi(strtok(NULL, " "))-1; //second token is first vertex
        int prev = atoi(strtok(NULL, " "))-1; //third token is second vertex
        int cur = atoi(strtok(NULL, " "))-1;

        Vector3 a = vvertices[first];
        Vector3 b = vvertices[prev];
        Vector3 c = vvertices[cur];
        Vector3 cb = c-b;
        Vector3 ab = a-b;
        Vector3 surfaceNormal = (cb).cross(ab);
        vnormals[first] += surfaceNormal;
        vnormals[prev] += surfaceNormal;
        vnormals[cur] += surfaceNormal;

        indexVector.push_back(first);
        indexVector.push_back(prev);
        indexVector.push_back(cur);
        prev = cur;

        while ((token = strtok(NULL, " ")) != NULL) {
            if (*token == '\n') break;
            cur = atoi(token)-1;
            vnormals[cur] += surfaceNormal;
            indexVector.push_back(first);
            indexVector.push_back(prev);
            indexVector.push_back(cur);
            prev = cur;
        }
    }
    fclose(file);

    for (int i = 0; i < vertexCount; ++i) {
        vnormals[i].normalize();
    }

    *indices = new unsigned int[indexVector.size()];
    memcpy(*indices, &(indexVector[0]), sizeof(unsigned int)*indexVector.size());
    return indexVector.size();
}

Geometry *loadModel(const char *coorpath, const char *polypath) {
    float *vertices;
    unsigned int *indices;
    int vertexCount = loadVertices(&vertices, coorpath);
    float normals[vertexCount*3];
    GLfloat interleaved[vertexCount*3*2];
    int indexCount = loadTrisAndNormals(&indices,
                                        normals,
                                        vertices,
                                        vertexCount,
                                        polypath);
    interleaveVertexData(interleaved,
                         vertices,
                         normals,
                         vertexCount);
    Geometry *geo = new Geometry(interleaved,
                                 indices,
                                 vertexCount,
                                 indexCount,
                                 3*2);
    delete vertices;
    delete indices;
    return geo;
}

}

