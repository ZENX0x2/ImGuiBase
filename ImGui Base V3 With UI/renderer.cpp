#include "includes.hpp"
#include "utils.hpp"

static ID3D11Device           * g_pd3dDevice           = nullptr;
static ID3D11DeviceContext    * g_pd3dDeviceContext    = nullptr;
static IDXGISwapChain         * g_pSwapChain           = nullptr;
static ID3D11RenderTargetView * g_mainRenderTargetView = nullptr;

bool CreateDeviceD3D( HWND );
void CleanupDeviceD3D( );
void CreateRenderTarget( );
void CleanupRenderTarget( );
LRESULT __stdcall WndProc( HWND, UINT , WPARAM, LPARAM );

/*
xo1337 ImGuiBase
- Version: 3.0.0

CProject website: https://cproject.xyz
CProject discord invite: https://discord.gg/dnkdDuUtQu

What is CProject? CProject is the best community to learn more
about programming in C++, C, C# and much more! Our main goal is to 
help and teach people about ImGui. If you're interested in learning more, 
come join us and visit our website!

- Note:
    We also sell ImGui menus! If you want a custom ImGui menu design or a remake of a ImGui menu, 
    we sell it as low and affordable as 14.99 euros! 
*/

void Colors( ) {
    ImGuiStyle& style = ImGui::GetStyle( );

    style.ScrollbarRounding = 0;
    style.WindowMinSize = ImVec2( 480 , 295 );

    style.Colors[ImGuiCol_WindowBg] = ImColor( 16 , 16 , 16 );
    style.Colors[ImGuiCol_ChildBg] = ImColor( 24 , 24 , 24 );
    style.Colors[ImGuiCol_Text] = ImColor( 255 , 255 , 255 );
    style.Colors[ImGuiCol_CheckMark] = ImColor( 255 , 255 , 255 );

    style.Colors[ImGuiCol_Header] = ImColor( 30 , 30 , 30 );
    style.Colors[ImGuiCol_HeaderActive] = ImColor( 28 , 28 , 28 );
    style.Colors[ImGuiCol_HeaderHovered] = ImColor( 28 , 28 , 28 );

    style.Colors[ImGuiCol_Button] = ImColor( 31 , 31 , 31 );
    style.Colors[ImGuiCol_ButtonActive] = ImColor( 34 , 34 , 34 );
    style.Colors[ImGuiCol_ButtonHovered] = ImColor( 34 , 34 , 34 );

    style.Colors[ImGuiCol_Border] = ImColor( 0 , 0 , 0 , 255 );
}

int StartRendering( ) 
{   
    const auto title = utils::random_string( );
    utils::spoof_exe( false );

    WNDCLASSEX wc = 
    { 
        sizeof( WNDCLASSEX ),
        CS_CLASSDC,
        WndProc, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        0, 
        title.c_str(),
        0
    };

    RegisterClassExA( &wc );
    HWND hwnd = CreateWindowExA(0, wc.lpszClassName , title.c_str() , WS_SYSMENU , 100 , 100 , 1280 , 800 , 0 , 0 , wc.hInstance , 0 );

    if ( !CreateDeviceD3D( hwnd ) )
    {
        CleanupDeviceD3D( );
        UnregisterClassA( wc.lpszClassName , wc.hInstance );

        return 1;
    }

    ShowWindow( GetConsoleWindow( ) , SW_HIDE );
    ShowWindow( hwnd , SW_SHOWDEFAULT );
    UpdateWindow( hwnd );

    ImGui::CreateContext( );

    ImGuiIO& io = ImGui::GetIO( ); 

    io.IniFilename = nullptr;
    io.LogFilename = nullptr;

    io.WantSaveIniSettings = false;
    io.Fonts->AddFontFromFileTTF( "C:\\Windows\\Fonts\\Tahoma.ttf" , 16.0f );

    Colors( );

    ImGui_ImplWin32_Init( hwnd );
    ImGui_ImplDX11_Init( g_pd3dDevice , g_pd3dDeviceContext );

    const auto clear_color = ImVec4( 0.45f , 0.55f , 0.60f , 1.00f );
    const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };

    while ( true )
    {
        MSG msg;

        while ( PeekMessageA( &msg , NULL , 0U , 0U , PM_REMOVE ) )
        {
            TranslateMessage( &msg );
            DispatchMessageA( &msg );

            if ( msg.message == WM_QUIT )
                break;
        }

        ImGui_ImplDX11_NewFrame( );
        ImGui_ImplWin32_NewFrame( );
        ImGui::NewFrame( );

        // 480 x 295
        {
            struct game_data {
                const char* name;
                std::string status; 
                std::string last_updated;
                bool undetected;
            };

            static std::vector<game_data> games = {
                { "FiveM", "Undetected", "Dec 23 2021", true },
                { "Apex", "Updating", "Jan 7 2021", false }
            };

            static int game = 0;

            ImGui::Begin( "MW" , 0 , ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize );

            ImGui::PushStyleColor( ImGuiCol_Border , ImColor( 0 , 0 , 0 , 255 ).Value );
            ImGui::BeginChild( "##LeftSide", ImVec2(120, ImGui::GetContentRegionAvail().y), true );
            {
                for ( unsigned int i = 0; i < games.size( ); i++ ) {
                    bool selected = ( game == i );

                    if ( ImGui::Selectable( games[i].name , &selected ) )
                        game = i;

                    if ( selected )
                        ImGui::SetItemDefaultFocus( );
                }
            }
            ImGui::EndChild( );

            {
                ImGui::SameLine( 0 );
                ImGui::SeparatorEx( ImGuiSeparatorFlags_Vertical );
                ImGui::SameLine( );
            }

            ImGui::BeginChild( "##RightSide" , ImVec2( ImGui::GetContentRegionAvail().x , ImGui::GetContentRegionAvail( ).y ) , true );
            {
                auto& data = games[game];
                auto center = [ ] ( float avail_width , float element_width , float padding = 0 ) {
                    ImGui::SameLine( ( avail_width / 2 ) - ( element_width / 2 ) + padding );
                };

                auto center_text = [ & ] ( const char* format, float spacing = 15, ImColor color = ImColor(255,255,255) ) {
                    center( ImGui::GetContentRegionAvail( ).x , ImGui::CalcTextSize( format ).x );
                    ImGui::SetCursorPosY( ImGui::GetCursorPosY( ) + spacing );
                    ImGui::TextColored( color.Value,  format );
                };

                center_text( data.name, 0 );              
                center_text( data.status.c_str( ) , 18, ( data.undetected ? ImColor( 0 , 255 , 0 ) : ImColor( 255 , 0 , 0 ) ) );
                center_text( data.last_updated.c_str( ) , 18 );

                char buf[64];
                sprintf_s( buf , "Load %s" , data.name );

                ImGui::PushStyleVar( ImGuiStyleVar_FrameBorderSize , 1 );
                ImGui::SetCursorPosY( ImGui::GetWindowHeight( ) - 37.5 );

                if ( ImGui::Button( buf , ImVec2( ImGui::GetContentRegionAvail( ).x , 29 ) ) ) {

                }
                ImGui::PopStyleVar( );
            }
            ImGui::EndChild( );

            ImGui::PopStyleColor( );
            
            ImGui::End( );      
        }

        ImGui::Render( );
       
        g_pd3dDeviceContext->OMSetRenderTargets( 1 , &g_mainRenderTargetView , NULL );
        g_pd3dDeviceContext->ClearRenderTargetView( g_mainRenderTargetView , clear_color_with_alpha );
       
        ImGui_ImplDX11_RenderDrawData( ImGui::GetDrawData( ) );

        g_pSwapChain->Present(0, 0); 
    }

    ImGui_ImplDX11_Shutdown( );
    ImGui_ImplWin32_Shutdown( );
    ImGui::DestroyContext( );

    CleanupDeviceD3D( );
    DestroyWindow( hwnd );
    UnregisterClassA( wc.lpszClassName , wc.hInstance );

    return 0;
}


bool CreateDeviceD3D( HWND hWnd )
{
    DXGI_SWAP_CHAIN_DESC sd;
    ZeroMemory( &sd , sizeof( sd ) );
    sd.BufferCount = 2;
    sd.BufferDesc.Width = 0;
    sd.BufferDesc.Height = 0;
    sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    sd.BufferDesc.RefreshRate.Numerator = 60;
    sd.BufferDesc.RefreshRate.Denominator = 1;
    sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
    sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
    sd.OutputWindow = hWnd;
    sd.SampleDesc.Count = 1;
    sd.SampleDesc.Quality = 0;
    sd.Windowed = TRUE;
    sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

    UINT createDeviceFlags = 0;

    D3D_FEATURE_LEVEL featureLevel;
    const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };

    if ( D3D11CreateDeviceAndSwapChain( NULL , D3D_DRIVER_TYPE_HARDWARE , NULL , createDeviceFlags , featureLevelArray , 2 , D3D11_SDK_VERSION , &sd , &g_pSwapChain , &g_pd3dDevice , &featureLevel , &g_pd3dDeviceContext ) != S_OK )
        return false;

    CreateRenderTarget( );
    return true;
}

void CleanupDeviceD3D( )
{
    CleanupRenderTarget( );
    if ( g_pSwapChain ) { g_pSwapChain->Release( ); g_pSwapChain = NULL; }
    if ( g_pd3dDeviceContext ) { g_pd3dDeviceContext->Release( ); g_pd3dDeviceContext = NULL; }
    if ( g_pd3dDevice ) { g_pd3dDevice->Release( ); g_pd3dDevice = NULL; }
}

void CreateRenderTarget( )
{
    ID3D11Texture2D* pBackBuffer;
    g_pSwapChain->GetBuffer( 0 , IID_PPV_ARGS( &pBackBuffer ) );
    g_pd3dDevice->CreateRenderTargetView( pBackBuffer , NULL , &g_mainRenderTargetView );
    pBackBuffer->Release( );
}

void CleanupRenderTarget( )
{
    if ( g_mainRenderTargetView ) { g_mainRenderTargetView->Release( ); g_mainRenderTargetView = NULL; }
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam );

LRESULT WINAPI WndProc( HWND hWnd , UINT msg , WPARAM wParam , LPARAM lParam )
{
    if ( ImGui_ImplWin32_WndProcHandler( hWnd , msg , wParam , lParam ) )
        return true;

    switch ( msg )
    {
    case WM_SIZE:
        if ( g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED )
        {
            CleanupRenderTarget( );
            g_pSwapChain->ResizeBuffers( 0 , (UINT)LOWORD( lParam ) , (UINT)HIWORD( lParam ) , DXGI_FORMAT_UNKNOWN , 0 );
            CreateRenderTarget( );
        }
        return 0;
    case WM_SYSCOMMAND:
        if ( ( wParam & 0xfff0 ) == SC_KEYMENU )
            return 0;
        break;
    case WM_DESTROY:
        ExitProcess( 0 );
        PostQuitMessage( 0 );
        return 0;
    }
    return ::DefWindowProc( hWnd , msg , wParam , lParam );
}