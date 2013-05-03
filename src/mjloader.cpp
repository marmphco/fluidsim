/*
    mjloader.cpp
    Matthew Jee
    mcjee@ucsc.edu
*/

#include "mjloader.h"
#include <vector>
#include <stdio.h> //i like this better
#include <string.h> //i like this better
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
int loadPolygons(unsigned int **indices, const char *path) {
    FILE *file = fopen(path, "r");
    int polyCount;
    fscanf(file, "%d\n", &polyCount);
    //cheating
    std::vector<unsigned int> indexVector;
    for (int i = 0; i < polyCount; ++i) {
        char line[128];
        char *ptr = fgets(line, 128, file);
        char *token = strtok(line, " ");
        //char *name = token; // first token is the mesh name
        int first = atoi(strtok(NULL, " ")); //second token is first vertex
        int prev = atoi(strtok(NULL, " ")); //third token is second vertex
        while ((token = strtok(NULL, " ")) != NULL) {
            if (*token == '\n') break;
            int cur = atoi(token);
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

}

