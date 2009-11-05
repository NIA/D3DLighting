#include "cylinder.h"

const Index CYLINDER_EDGES_PER_BASE = 140;
const Index CYLINDER_EDGES_PER_HEIGHT = 14;

extern const Index CYLINDER_VERTICES_COUNT 
    = CYLINDER_EDGES_PER_BASE*(CYLINDER_EDGES_PER_HEIGHT + 2) // vertices per CYLINDER_EDGES_PER_HEIGHT+1 levels plus last level again
    + 1; // plus the center of the cap
extern const DWORD CYLINDER_INDICES_COUNT
    = 2*(CYLINDER_EDGES_PER_BASE + 1)*CYLINDER_EDGES_PER_HEIGHT // indices per CYLINDER_EDGES_PER_HEIGHT levels
    + (2*CYLINDER_EDGES_PER_BASE + 1); // plus cap

void cylinder( D3DXVECTOR3 base_center, float radius, float height,
                Vertex *res_vertices, Index *res_indices)
// Writes data into arrays given as `res_vertices' and `res_indices',
{
    Index vertex = 0; // current vertex
    DWORD index = 0; // current index

    _ASSERT(res_vertices != NULL);
    _ASSERT(res_indices != NULL);

    const float STEP_ANGLE = 2*D3DX_PI/CYLINDER_EDGES_PER_BASE;
    const float STEP_UP = height/CYLINDER_EDGES_PER_HEIGHT;
    
    for( Index level = 0; level <= CYLINDER_EDGES_PER_HEIGHT; ++level )
    {
        D3DCOLOR color = D3DCOLOR_XRGB(200,200,200);//= random_color();
        for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
        {
            if( level == CYLINDER_EDGES_PER_HEIGHT )
                color = random_color();
            
            res_vertices[vertex] = Vertex( base_center
                                           + D3DXVECTOR3( radius*cos(step*STEP_ANGLE),
                                                          radius*sin(step*STEP_ANGLE),
                                                          level*STEP_UP),
                                           color,
                                           static_cast<float>(level)/static_cast<float>(CYLINDER_EDGES_PER_HEIGHT),
                                           D3DXVECTOR3( cos(step*STEP_ANGLE),
                                                        sin(step*STEP_ANGLE),
                                                        0)
                                          );
            if( level != 0 )
            {
                res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE; // from previous level
                res_indices[index++] = vertex;                           // from current level
                if( step == CYLINDER_EDGES_PER_BASE - 1 ) // last step
                {
                    res_indices[index++] = vertex - 2*CYLINDER_EDGES_PER_BASE + 1; // first from previuos level
                    res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + 1; // first from current level
                }
            }
            ++vertex;
        }
    }
    // Cap
    D3DXVECTOR3 normal_up( 0, 0, 1.0f );
    for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
    {
        res_vertices[vertex] = res_vertices[vertex - CYLINDER_EDGES_PER_BASE];
        res_vertices[vertex].normal = normal_up;
        ++vertex;
    }
    res_vertices[vertex] = Vertex( base_center + D3DXVECTOR3( 0, 0, height), 1.0f, normal_up );
    for( Index step = 0; step < CYLINDER_EDGES_PER_BASE; ++step )
    {
        res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE + step;
        res_indices[index++] = vertex;
    }
    res_indices[index++] = vertex - CYLINDER_EDGES_PER_BASE;
}
