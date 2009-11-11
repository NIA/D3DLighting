#pragma once
#include "main.h"

typedef DWORD Index;
extern const D3DFORMAT INDEX_FORMAT;

// It must be a macro, not a constant, because it must be known at compile-time (it is used for array initialization)
#define VERTICES_PER_TRIANGLE 3

inline int rand_col_comp()
// Returns random color component: an integer between 0 and 255
{
    return rand()*255/RAND_MAX;
}

inline D3DCOLOR random_color()
{
    return D3DCOLOR_XRGB( rand_col_comp(), rand_col_comp(), rand_col_comp() );
}

class Vertex
{
public:
    D3DXVECTOR3 pos;            // The position for the vertex
    D3DXVECTOR4 normal;         // The outer normal of model
    D3DCOLOR color;             // The vertex color
    float weights[BONES_COUNT]; // Weights for skinning
    void set_weight(float weight)
    {
        weights[0] = weight;
        weights[1] = 1 - weight;
    }
    void set_normal(D3DXVECTOR3 tri_normal)
    {
        normal = D3DXVECTOR4(tri_normal, 0); // normal is a vector, not a point!
    }
    Vertex() : pos(), color(0)
    {
        set_weight(1.0f);
        set_normal(D3DXVECTOR3(1.0f, 0, 0));
    }
    Vertex(D3DXVECTOR3 pos, D3DCOLOR color, float weight, D3DXVECTOR3 normal) : pos(pos), color(color)
    {
        set_weight(weight);
        set_normal(normal);
    }
    Vertex(D3DXVECTOR3 pos, float weight, D3DXVECTOR3 normal) : pos(pos)
    {
        color = random_color();
        set_weight(weight);
        set_normal(normal);
    }
};

extern const D3DVERTEXELEMENT9 VERTEX_DECL_ARRAY[];
