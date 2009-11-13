#include "main.h"
#include "Application.h"
#include "Model.h"
#include "cylinder.h"

namespace
{
    const char *SKINNING_SHADER_FILENAME = "shader.vsh";
    const D3DCOLOR colors[] =
    {
        D3DCOLOR_XRGB(250, 250, 250),
        D3DCOLOR_XRGB(250, 30, 10),
        D3DCOLOR_XRGB(250, 250, 0),
        D3DCOLOR_XRGB(30, 250, 0),
        D3DCOLOR_XRGB(0, 150, 250),
    };
    const unsigned colors_count = array_size(colors);
}

INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
{
    srand( static_cast<unsigned>( time(NULL) ) );
    
    SkinningVertex * cylinder_vertices = NULL;
    Index * cylinder_indices = NULL;
    try
    {
        Application app;

        VertexShader skinning_shader(app.get_device(), SKINNING_VERTEX_DECL_ARRAY, SKINNING_SHADER_FILENAME);
        
        cylinder_vertices = new SkinningVertex[CYLINDER_VERTICES_COUNT];
        cylinder_indices = new Index[CYLINDER_INDICES_COUNT];

        float height = 2.0f;

        cylinder( 0.7f, height,
                  colors, colors_count,
                  cylinder_vertices, cylinder_indices );

        SkinningModel cylinder1(app.get_device(),
                                D3DPT_TRIANGLESTRIP,
                                skinning_shader,
                                cylinder_vertices,
                                CYLINDER_VERTICES_COUNT,
                                cylinder_indices,
                                CYLINDER_INDICES_COUNT,
                                CYLINDER_INDICES_COUNT - 2,
                                D3DXVECTOR3(0.5f, 0.5f, -height/2),
                                D3DXVECTOR3(0,0,0),
                                D3DXVECTOR3(0,0,-1));

        height = 2.3f;
        cylinder( 0.3f, height,
                  colors, colors_count,
                  cylinder_vertices, cylinder_indices );

        SkinningModel cylinder2(app.get_device(),
                                D3DPT_TRIANGLESTRIP,
                                skinning_shader,
                                cylinder_vertices,
                                CYLINDER_VERTICES_COUNT,
                                cylinder_indices,
                                CYLINDER_INDICES_COUNT,
                                CYLINDER_INDICES_COUNT - 2,
                                D3DXVECTOR3(-1.0f, 0.5f, height/2),
                                D3DXVECTOR3(D3DX_PI,0,-D3DX_PI/4),
                                D3DXVECTOR3(0,0,1));

        app.add_model(cylinder1);
        app.add_model(cylinder2);
        app.run();
        delete_array(&cylinder_indices);
        delete_array(&cylinder_vertices);
    }
    catch(RuntimeError &e)
    {
        delete_array(&cylinder_indices);
        delete_array(&cylinder_vertices);
        const TCHAR *MESSAGE_BOX_TITLE = _T("Lighting error!");
        MessageBox(NULL, e.message(), MESSAGE_BOX_TITLE, MB_OK | MB_ICONERROR);
        return -1;
    }
    return 0;
}
