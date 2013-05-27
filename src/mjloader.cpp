/*
    mjloader.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjloader.h"
#include "mjvector.h"
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
    float *normals = new float[vertexCount*3];
    float *interleaved = new float[vertexCount*3*2];
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
    delete normals;
    delete interleaved;
    return geo;
}

Geometry *loadCube(float width, float height, float depth) {
    float vertexData[] = {
        0.0, 0.0, 0.0,
        width, 0.0, 0.0,
        width, height, 0.0,
        0.0, height, 0.0,

        0.0, 0.0, depth,
        width, 0.0, depth,
        width, height, depth,
        0.0, height, depth,
    };
    GLuint indexData[] = {
        0, 3, 1,
        3, 2, 1,
        1, 2, 5,
        2, 6, 5,
        0, 7, 3,
        0, 4, 7,
        3, 7, 2,
        7, 6, 2,
        4, 5, 7,
        7, 5, 6,
        0, 1, 4,
        1, 5, 4,
    };
    return new Geometry(vertexData, indexData, 8, 36, 3);
}

Geometry *loadWireCube(float width, float height, float depth) {
    float vertexData[] = {
        0.0, 0.0, 0.0,
        width, 0.0, 0.0,
        width, height, 0.0,
        0.0, height, 0.0,

        0.0, 0.0, depth,
        width, 0.0, depth,
        width, height, depth,
        0.0, height, depth,
    };
    GLuint indexData[] = {
        0, 1, 1, 2, 2, 3, 3, 0,
        4, 5, 5, 6, 6, 7, 7, 4,
        1, 5, 2, 6, 3, 7, 0, 4,
    };
    return new Geometry(vertexData, indexData, 8, 24, 3);
}

Geometry *loadSquare(float width, float height) {
    float vertexData[] = {
        0.0, 0.0, 0.0,
        width, 0.0, 0.0,
        width, height, 0.0,
        0.0, height, 0.0,
    };
    GLuint indexData[] = {
        0, 1, 2, 0, 2, 3,
    };
    return new Geometry(vertexData, indexData, 4, 6, 3);
}

}
