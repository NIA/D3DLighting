#include "main.h"
#include "Application.h"
#include "cylinder.h"

INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, INT )
{
    srand( static_cast<unsigned>( time(NULL) ) );

    Vertex * cylinder_vertices = NULL;
    Index * cylinder_indices = NULL;
    try
    {
        Application app;
        
        cylinder_vertices = new Vertex[CYLINDER_VERTICES_COUNT];
        cylinder_indices = new Index[CYLINDER_INDICES_COUNT];
        
        const float height = 2.0f;
        D3DCOLOR colors[] =
        {
            D3DCOLOR_XRGB(250, 250, 250),
            D3DCOLOR_XRGB(250, 30, 10),
            D3DCOLOR_XRGB(250, 250, 0),
            D3DCOLOR_XRGB(30, 250, 0),
            D3DCOLOR_XRGB(0, 150, 250),
        };
        const unsigned colors_count = array_size(colors);

        cylinder( D3DXVECTOR3(0,0,-height/2), 0.7f, height,
                  colors, colors_count,
                  cylinder_vertices, cylinder_indices );

        Model cylinder( app.get_device(),
                        D3DPT_TRIANGLESTRIP,
                        cylinder_vertices,
                        CYLINDER_VERTICES_COUNT,
                        cylinder_indices,
                        CYLINDER_INDICES_COUNT,
                        CYLINDER_INDICES_COUNT - 2,
                        D3DXVECTOR3(0,0,-1));

        app.add_model(cylinder);
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
