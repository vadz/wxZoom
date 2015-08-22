/////////////////////////////////////////////////////////////////////////////
// Name:        wxZoom.cpp
// Purpose:     wxZoom
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.08.00
// Copyright:   (c) 2000 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "wxzoom.cpp"
    #pragma interface "wxzoom.cpp"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all standard wxWindows headers)
#ifndef WX_PRECOMP
    #include <wx/intl.h>
    #include <wx/dynarray.h>
    #include <wx/filename.h>
    #include <wx/timer.h>

    #include <wx/app.h>
    #include <wx/frame.h>

    #include <wx/menu.h>

    #include <wx/filedlg.h>
    #include <wx/msgdlg.h>

    #include <wx/dcscreen.h>
    #include <wx/dcclient.h>
    #include <wx/dcmemory.h>
#endif

#include <wx/image.h>

#include <wx/numdlg.h>

#ifdef __VISUALC__
    #pragma warning(disable: 4355) // this used in base member initializer list
#endif // VC++

#ifdef __WXMSW__
    #define VERSION "0.02"
#else // !MSW
    #include "../icons/wxZoom.xpm"
    #include "config.h"     // for VERSION
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// menu commands
enum
{
    Zoom_File_New,
    Zoom_File_Save_Zoomed,
    Zoom_File_Save_Unzoomed,
    Zoom_File_Quit,

    Zoom_Zoom_In,
    Zoom_Zoom_Out,
    Zoom_Toggle_Grid,
    Zoom_Toggle_AutoRefresh,
    Zoom_Set_AutoRefreshDelay,
    Zoom_Refresh,

    Zoom_Help_About,
    Zoom_Help_Help
};

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY_PTR(wxFrame *, ArrayFrames);

class ZoomApp : public wxApp
{
public:
    virtual bool OnInit();

    void CloseAll();

private:
    ArrayFrames m_frames;
};

// ----------------------------------------------------------------------------
// frame class
// ----------------------------------------------------------------------------

class ZoomFrame : public wxFrame
{
public:
    ZoomFrame(const wxSize& size = wxSize(300, 150),
              const wxPoint& ptInitial = wxDefaultPosition);

    // event handlers
    void OnCommand(wxCommandEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnSize(wxSizeEvent& event);
    void OnMouseDown(wxMouseEvent& event);
    void OnMouseMove(wxMouseEvent& event);
    void OnMouseUp(wxMouseEvent& event);
    void OnKeyDown(wxKeyEvent& event);
    void OnTimer(wxTimerEvent& event);
    void OnUpdateZoomOut(wxUpdateUIEvent& event);

protected:
    void RefreshBitmap();
    void RecreateBitmap();

    void RestartTimer();

    void DrawMarker();
    void DrawMarker(wxDC& dc, const wxPoint& pt);

    // zoom the image to have the size of this frame
    void ZoomImage(wxImage& image);

    // stop dragging the selection rect: erase the marker and reset the flag
    void EndDragging();

    // get the real mouse position on screen from coords provided by
    // wxMouseEvent
    wxPoint PtMouse2Screen(const wxMouseEvent& event);

    bool m_captured;
    wxPoint m_oldPt;
    wxColour m_oldCol;
    wxBitmap m_bmp;

    int m_zoomFactor;
    wxSize m_sizeBmp;

    bool m_showGrid;

    // current and saved refresh delays
    int m_refreshTime,
        m_refreshTimeOld;
    wxTimer m_timer;

    DECLARE_EVENT_TABLE()
};

// ============================================================================
// ZoomApp implementation
// ============================================================================

IMPLEMENT_APP(ZoomApp)

bool ZoomApp::OnInit()
{
    wxFrame *frame = new ZoomFrame;
    frame->Show();
    m_frames.Add(frame);

    return TRUE;
}

void ZoomApp::CloseAll()
{
    size_t count = m_frames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        m_frames[n]->Close();
    }
}

// ============================================================================
// ZoomFrame implementation
// ============================================================================

BEGIN_EVENT_TABLE(ZoomFrame, wxFrame)
    EVT_PAINT(ZoomFrame::OnPaint)

    EVT_SIZE(ZoomFrame::OnSize)

    EVT_LEFT_DOWN(ZoomFrame::OnMouseDown)
    EVT_MOTION(ZoomFrame::OnMouseMove)
    EVT_LEFT_UP(ZoomFrame::OnMouseUp)

    EVT_KEY_DOWN(ZoomFrame::OnKeyDown)

    EVT_MENU(-1, ZoomFrame::OnCommand)
    EVT_UPDATE_UI(Zoom_Zoom_Out, ZoomFrame::OnUpdateZoomOut)

    EVT_TIMER(-1, ZoomFrame::OnTimer)
END_EVENT_TABLE()

// ----------------------------------------------------------------------------
// ZoomFrame ctor
// ----------------------------------------------------------------------------

ZoomFrame::ZoomFrame(const wxSize& size, const wxPoint& ptOld)
         : wxFrame(NULL, -1, _("wxZoom"), wxDefaultPosition, size),
           m_timer(this)
{
    m_captured = FALSE;
    m_showGrid = FALSE;
    m_zoomFactor = 10;
    m_refreshTime = 0;
    m_refreshTimeOld = 3000;
    m_oldPt = ptOld;

    wxMenuBar *menuBar = new wxMenuBar;
    wxMenu *menuFile = new wxMenu("", wxMENU_TEAROFF);
    menuFile->Append(Zoom_File_New, _("&New window\tCtrl-N"));
    menuFile->AppendSeparator();
    menuFile->Append(Zoom_File_Save_Unzoomed, _("&Save image...\tCtrl-S"));
    menuFile->Append(Zoom_File_Save_Zoomed,
                     _("Save &zoomed image...\tAlt-Ctrl-S"));
    menuFile->AppendSeparator();
    menuFile->Append(Zoom_File_Quit, _("E&xit\tCtrl-Q"));
    menuBar->Append(menuFile, _("&File"));

    wxMenu *menuView = new wxMenu("", wxMENU_TEAROFF);
    menuView->Append(Zoom_Zoom_In, _("Zoom &in\tCtrl-I"));
    menuView->Append(Zoom_Zoom_Out, _("Zoom &out\tCtrl-O"));
    menuView->AppendSeparator();
    menuView->Append(Zoom_Toggle_AutoRefresh, _("Toggle &auto refresh\tCtrl-A"),
                    _T(""), TRUE);
    menuView->Append(Zoom_Set_AutoRefreshDelay, _("Set auto refresh &delay..."));
    menuView->Append(Zoom_Refresh, _("&Refresh\tF5"));
    menuView->AppendSeparator();
    menuView->Append(Zoom_Toggle_Grid, _("Toggle &grid\tCtrl-G"), _T(""), TRUE);
    menuBar->Append(menuView, _("&View"));

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(Zoom_Help_About, _T("&About..."));
    menuHelp->AppendSeparator();
    menuHelp->Append(Zoom_Help_Help, _T("&Help...\tF1"));
    menuBar->Append(menuHelp, _("&Help"));

    SetMenuBar(menuBar);

    CreateStatusBar(3);
    static const int widths[3] =
    {
        -1,
        100,
        100
    };
    SetStatusWidths(3, widths);

    SetIcon(wxICON(wxZoom));
}

// ----------------------------------------------------------------------------
// ZoomFrame helper methods
// ----------------------------------------------------------------------------

void ZoomFrame::DrawMarker(wxDC& dc, const wxPoint& pt)
{
    dc.SetLogicalFunction(wxINVERT);
    dc.SetPen(*wxBLACK_DASHED_PEN);
    dc.SetBrush(*wxTRANSPARENT_BRUSH);
    dc.DrawRectangle(pt, m_sizeBmp);
}

void ZoomFrame::DrawMarker()
{
    wxScreenDC dc;
    dc.StartDrawingOnTop();
    DrawMarker(dc, m_oldPt);
    dc.EndDrawingOnTop();
}

void ZoomFrame::RecreateBitmap()
{
    const wxSize& size = GetClientSize();
    m_sizeBmp.x = size.x / m_zoomFactor;
    m_sizeBmp.y = size.y / m_zoomFactor;
    m_bmp.Create(m_sizeBmp.x, m_sizeBmp.y);
    RefreshBitmap();
}

void ZoomFrame::ZoomImage(wxImage& image)
{
    image.Rescale(m_zoomFactor*m_sizeBmp.x, m_zoomFactor*m_sizeBmp.y);
}

void ZoomFrame::RefreshBitmap()
{
    wxScreenDC dcScreen;
    wxMemoryDC dcMem;
    dcMem.SelectObject(m_bmp);
    dcMem.Clear();
    dcMem.Blit(0, 0, m_bmp.GetWidth(), m_bmp.GetHeight(),
               &dcScreen, m_oldPt.x, m_oldPt.y);
    dcMem.SelectObject(wxNullBitmap);

    Refresh();
}

void ZoomFrame::RestartTimer()
{
    m_timer.Stop();

    if ( m_refreshTime )
        m_timer.Start(m_refreshTime);
}

void ZoomFrame::EndDragging()
{
    // erase it, in fact
    DrawMarker();

    ReleaseMouse();

    m_captured = FALSE;
}

wxPoint ZoomFrame::PtMouse2Screen(const wxMouseEvent& event)
{
    return event.GetPosition() + GetPosition();
}

// ----------------------------------------------------------------------------
// ZoomFrame event handlers
// ----------------------------------------------------------------------------

void ZoomFrame::OnMouseDown(wxMouseEvent& event)
{
    m_oldPt = PtMouse2Screen(event);

    CaptureMouse();
    DrawMarker();

    m_captured = TRUE;
}

void ZoomFrame::OnMouseMove(wxMouseEvent& event)
{
    if ( m_captured )
    {
        // continue dragging the rectangle on screen
        wxScreenDC dc;
        wxPoint pt = PtMouse2Screen(event);

        if ( pt != m_oldPt )
        {
            SetStatusText(wxString::Format(_("Pos: %dx%d"),
                                           pt.x, pt.y), 2);

            dc.StartDrawingOnTop();
            DrawMarker(dc, m_oldPt);

            wxColour col;
            dc.GetPixel(pt, &col);
            if ( col != m_oldCol )
            {
                SetStatusText(wxString::Format(_("Col: 0x%02x%02x%02x"),
                                               (int)col.Red(),
                                               (int)col.Green(),
                                               (int)col.Blue()), 1);

                m_oldCol = col;
            }

            m_oldPt = pt;
            DrawMarker(dc, m_oldPt);
            dc.EndDrawingOnTop();
        }
    }
    else // not captured: show the colour of the pixel under mouse
    {
        wxClientDC dc(this);
        wxColour col;
        dc.GetPixel(event.GetPosition(), &col);

        if ( col != m_oldCol )
        {
            SetStatusText(wxString::Format(_("Col: 0x%02x%02x%02x"),
                                           (int)col.Red(),
                                           (int)col.Green(),
                                           (int)col.Blue()), 1);

            m_oldCol = col;
        }
    }
}

void ZoomFrame::OnMouseUp(wxMouseEvent& WXUNUSED(event))
{
    EndDragging();

    // show the selected area
    RefreshBitmap();
}

void ZoomFrame::OnKeyDown(wxKeyEvent& event)
{
    // we only handle events while dragging the rect on screen
    if ( m_captured )
    {
        wxCoord dx = 0,
                dy = 0;

        switch ( event.GetKeyCode() )
        {
            case WXK_RIGHT:
                dx = 1;
                break;

            case WXK_LEFT:
                dx = -1;
                break;

            case WXK_UP:
                dy = -1;
                break;

            case WXK_DOWN:
                dy = 1;
                break;

            case WXK_ESCAPE:
                EndDragging();
                return;

            default:
                event.Skip();
                return;
        }

        if ( event.ControlDown() )
        {
            dx *= 10;
            dy *= 10;
        }

        if ( event.AltDown() || event.MetaDown() )
        {
            dx *= 50;
            dy *= 50;
        }

        wxPoint pt = event.GetPosition();
        pt.x += dx;
        pt.y += dy;
        WarpPointer(pt.x, pt.y);
    }
}

void ZoomFrame::OnSize(wxSizeEvent& WXUNUSED(event))
{
    RecreateBitmap();
}

void ZoomFrame::OnPaint(wxPaintEvent& WXUNUSED(event))
{
    const wxSize& size = GetClientSize();

    // first draw the bitmap
    wxPaintDC dc(this);
    wxBitmap bmp;
    if ( m_zoomFactor == 1 )
    {
        bmp = m_bmp;
    }
    else
    {
        wxImage img = m_bmp.ConvertToImage();
        ZoomImage(img);
        bmp = wxBitmap(img);
    }

    dc.DrawBitmap(bmp, 0, 0);

    // then the grid on top of it if configured
    if ( m_showGrid && m_zoomFactor > 1 )
    {
        dc.SetPen(*wxBLACK_PEN);
        for ( wxCoord x = m_zoomFactor; x < size.x; x += m_zoomFactor)
        {
            dc.DrawLine(x, 0, x, size.y);
        }

        for ( wxCoord y = m_zoomFactor; y < size.y; y += m_zoomFactor)
        {
            dc.DrawLine(0, y, size.x, y);
        }
    }
}

void ZoomFrame::OnCommand(wxCommandEvent& event)
{
    switch ( event.GetId() )
    {
        case Zoom_File_New:
            {
                wxFrame *frame = new ZoomFrame(GetSize(), m_oldPt);
                frame->Show();
            }
            break;

        case Zoom_File_Save_Zoomed:
        case Zoom_File_Save_Unzoomed:
            {
                bool zoomed = event.GetId() == Zoom_File_Save_Zoomed;

                wxString what = zoomed ? _("zoomed image")
                                       : _("original image");
                wxString filename = wxSaveFileSelector
                                    (
                                      what,
                                      _("BMP files (*.bmp)|*.bmp|"
                                      "PNG files (*.png)|*.png|"
                                      "JPEG files (*.jpg)|*.jpg|"
                                      "GIF files (*.gif)|*.gif|"
                                      "TIFF files (*.tif)|*.tif|"
                                      "PCX files (*.pcx)|*.pcx"),
                                      _T(""),       // no default name
                                      this
                                    );
                if ( filename.empty() )
                {
                    // cancelled
                    break;
                }

                wxImage img = m_bmp.ConvertToImage();
                if ( zoomed )
                {
                    ZoomImage(img);
                }

                wxString ext;
                wxFileName::SplitPath(filename, NULL, NULL, &ext);
                ext.MakeLower();

                wxBitmapType type;
                if ( ext == _T("png") )
                    type = wxBITMAP_TYPE_PNG;
                else if ( ext == _T("jpg") || ext == _T("jpeg") )
                    type = wxBITMAP_TYPE_JPEG;
                else if ( ext == _T("pcx") )
                    type = wxBITMAP_TYPE_PCX;
                else if ( ext == _T("tif") || ext == _T("tiff") )
                    type = wxBITMAP_TYPE_TIF;
                else if ( ext == _T("pnm") )
                    type = wxBITMAP_TYPE_PNM;
                else // bmp by default
                    type = wxBITMAP_TYPE_BMP;

                if ( !img.SaveFile(filename, type) )
                {
                    wxLogError(_("Failed to save the image file."));
                }
                else
                {
                    wxLogStatus(this, _("Saved %s to '%s'."),
                                what.c_str(), filename.c_str());
                }
            }
            break;

        case Zoom_File_Quit:
            wxGetApp().CloseAll();
            break;

        case Zoom_Zoom_In:
        case Zoom_Zoom_Out:
            m_zoomFactor += event.GetId() == Zoom_Zoom_Out ? -1 : +1;
            if ( !m_zoomFactor )
            {
                m_zoomFactor = 1;
            }
            else
            {
                wxLogStatus(this, _("Zoom %d"), m_zoomFactor);
                RecreateBitmap();
            }
            break;

        case Zoom_Toggle_Grid:
            m_showGrid = !m_showGrid;
            Refresh();
            break;

        case Zoom_Toggle_AutoRefresh:
            if ( m_refreshTime )
            {
                // auto refreshing now, stop doing it
                m_refreshTimeOld = m_refreshTime;
                m_refreshTime = 0;
            }
            else
            {
                m_refreshTime = m_refreshTimeOld;
            }
            RestartTimer();
            break;

        case Zoom_Set_AutoRefreshDelay:
            {
                int refreshTime = wxGetNumberFromUser
                                  (
                                   _("Specify non zero refresh time to\n"
                                     "automatically refresh the picture"),
                                   _("Time in milliseconds:"),
                                   _("wxZoom: auto refresh"),
                                   m_refreshTimeOld, 0, 10000,
                                   this
                                  );
                if ( refreshTime != -1 && refreshTime != m_refreshTime )
                {
                    m_refreshTime = refreshTime;

                    RestartTimer();
                }
            }
            break;

        case Zoom_Refresh:
            RefreshBitmap();
            break;

        case Zoom_Help_About:
            wxMessageBox(_("wxZoom " VERSION
                                   ": cross-platform display magnifier.\n"
                           "\n"
                           "This is free software, enjoy using it!\n"
                           "\n"
                           "(c) 2000-2001 Vadim Zeitlin"),
                         _("About wxZoom"),
                         wxICON_INFORMATION | wxOK, this);
            break;

        case Zoom_Help_Help:
            wxMessageBox(_("Press the left mouse button in the window "
                           "and drag the selection rectangle to select "
                           "the screen area to zoom.\n"
                           "\n"
                           "You may change the zoom setting using the "
                           "commands from the \"View\" menu.\n"
                           "\n"
                           "Please send any questions/suggestions to "
                           "vadim@wxwindows.org"),
                         _("wxZoom Help"),
                         wxICON_INFORMATION | wxOK, this);
            break;

        default:
            event.Skip();
    }
}

void ZoomFrame::OnUpdateZoomOut(wxUpdateUIEvent& event)
{
    // we can't zoom out when we're at zoom 1
    event.Enable( m_zoomFactor > 1 );
}

void ZoomFrame::OnTimer(wxTimerEvent& WXUNUSED(event))
{
    RefreshBitmap();
}
