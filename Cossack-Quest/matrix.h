#ifndef UTIL_H
#define UTIL_H
#include "raylib.h"

struct TranslationMatrix {
    TranslationMatrix(float x, float y)
    {
        mat[0][0] = x;
        mat[0][1] = y;
        mat[1][0] = x;
        mat[1][1] = y;
        determinant = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    }

    float mat[2][2]{};
    float determinant;
};

struct mat2x2 : TranslationMatrix {
    mat2x2(Vector2 v1, Vector2 v2): TranslationMatrix(v1.x - v2.x, v1.y - v2.y)
    {
        mat[0][0] = v1.x;
        mat[0][1] = v1.y;
        mat[1][0] = v2.x;
        mat[1][1] = v2.y;
        determinant = mat[0][0] * mat[1][1] - mat[0][1] * mat[1][0];
    }

    Vector2 crossProduct(Vector2 v) const
    {
        float dx = mat[1][1] - mat[1][0];
        float dy = mat[0][1] - mat[0][0];
        return {v.x * dy, v.y * dx};
    }
};

struct mat3x2 : TranslationMatrix {
    mat3x2(Vector3 v1, Vector3 v2): TranslationMatrix(v1.x - v2.x, v1.y - v2.y)
    {
        mat[0][0] = v1.x;
        mat[0][1] = v1.y;
        mat[0][2] = v1.z;
        mat[1][0] = v2.x;
        mat[1][1] = v2.y;
        mat[1][2] = v2.z;
        determinant = mat[1][1] * mat[2][1] - mat[0][1] * mat[1][2] + mat[0][1] * mat[1][0] - mat[0][0] * mat[1][1];
    }

    Vector2 crossProduct(Vector2 v)
    {
        float dx = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
        float dy = mat[0][0] * mat[2][1] - mat[0][1] * mat[2][0];
        float dz = mat[1][0] * mat[0][1] - mat[1][1] * mat[0][0];

        crossProductVector = {v.x * dx, v.y * dy, 1.f * dz};
        return {v.x * dx, v.y * dy};
    };

    Vector3 crossProductVector;
    float mat[3][3]{};
};

struct mat3x3 : TranslationMatrix {
    mat3x3(Vector3 v1, Vector3 v2): TranslationMatrix(v1.x - v2.x, v1.y - v2.y)
    {
        mat[0][0] = v1.x;
        mat[0][1] = v1.y;
        mat[0][2] = v1.z;
        mat[1][0] = v2.x;
        mat[1][1] = v2.y;
        mat[1][2] = v2.z;
        mat[2][0] = v1.x + v2.x;
        mat[2][1] = v1.y + v2.y;
        mat[2][2] = v1.z + v2.z;
    }

    Vector3 crossProduct(Vector3 v)
    {
        float dx = mat[1][0] * mat[2][1] - mat[1][1] * mat[2][0];
        float dy = mat[0][0] * mat[2][1] - mat[0][1] * mat[2][0];
        float dz = mat[1][0] * mat[0][1] - mat[1][1] * mat[0][0];
        return {v.x * dx, v.y * dy, v.z * dz};
    }

    float mat[3][3]{};
};

#endif //UTIL_H
