/////////////////////////////////////////////////////////////////////////////
// Name:        wxZoom.cpp
// Purpose:     wxZoom
// Author:      Vadim Zeitlin
// Modified by:
// Created:     21.08.00
// RCS-ID:      $Id$
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
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/dynarray.h"
    #include "wx/timer.h"

    #include "wx/app.h"
    #include "wx/frame.h"

    #include "wx/menu.h"

    #include "wx/dcscreen.h"
    #include "wx/dcmemory.h"
#endif

#include "wx/image.h"

#include "wx/textdlg.h"

#ifdef __VISUALC__
#pragma warning(disable: 4355) // this used in base member initializer list
#endif // VC++

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// menu commands
enum
{
    Zoom_New_Window,
    Zoom_Quit,
    Zoom_Zoom_In,
    Zoom_Zoom_Out,
    Zoom_Toggle_Grid,
    Zoom_Toggle_AutoRefresh,
    Zoom_Set_AutoRefreshDelay,
    Zoom_Refresh
};

// ----------------------------------------------------------------------------
// app class
// ----------------------------------------------------------------------------

WX_DEFINE_ARRAY(wxFrame *, ArrayFrames);

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
    void OnTimer(wxTimerEvent& event);
    void OnUpdateZoomOut(wxUpdateUIEvent& event);

protected:
    void RefreshBitmap();
    void RecreateBitmap();

    void RestartTimer();

    void DrawMarker();
    void DrawMarker(wxDC& dc, const wxPoint& pt);

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
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// ZoomApp
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// ZoomFrame
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(ZoomFrame, wxFrame)
    EVT_PAINT(ZoomFrame::OnPaint)

    EVT_SIZE(ZoomFrame::OnSize)

    EVT_LEFT_DOWN(ZoomFrame::OnMouseDown)
    EVT_MOTION(ZoomFrame::OnMouseMove)
    EVT_LEFT_UP(ZoomFrame::OnMouseUp)

    EVT_MENU(-1, ZoomFrame::OnCommand)
    EVT_UPDATE_UI(Zoom_Zoom_Out, ZoomFrame::OnUpdateZoomOut)

    EVT_TIMER(-1, ZoomFrame::OnTimer)
END_EVENT_TABLE()

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
    menuFile->Append(Zoom_New_Window, _("&New window\tCtrl-N"));
    menuFile->AppendSeparator();
    menuFile->Append(Zoom_Quit, _("E&xit\tCtrl-Q"));
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
    SetMenuBar(menuBar);

    CreateStatusBar(3);

    SetIcon(wxICON(wxZoom));
}

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

void ZoomFrame::OnMouseDown(wxMouseEvent& event)
{
    m_oldPt = event.GetPosition() + GetPosition();

    CaptureMouse();
    DrawMarker();

    m_captured = TRUE;
}

void ZoomFrame::OnMouseMove(wxMouseEvent& event)
{
    wxPoint pt = event.GetPosition();

    if ( m_captured )
    {
        wxScreenDC dc;
        pt += GetPosition();

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
    else // not captured
    {
        wxClientDC dc(this);
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
    }
}

void ZoomFrame::OnMouseUp(wxMouseEvent& event)
{
    // erase it, in fact
    DrawMarker();

    ReleaseMouse();

    RefreshBitmap();

    m_captured = FALSE;
}

void ZoomFrame::OnSize(wxSizeEvent& event)
{
    RecreateBitmap();
}

void ZoomFrame::OnPaint(wxPaintEvent& event)
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
        bmp = wxImage(m_bmp).
                Rescale(m_zoomFactor*m_sizeBmp.x, m_zoomFactor*m_sizeBmp.y).
                    ConvertToBitmap();
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
        case Zoom_New_Window:
            {
                wxFrame *frame = new ZoomFrame(GetSize(), m_oldPt);
                frame->Show();
            }
            break;

        case Zoom_Quit:
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

        default:
            event.Skip();
    }
}

void ZoomFrame::OnUpdateZoomOut(wxUpdateUIEvent& event)
{
    // we can't zoom out when we're at zoom 1
    event.Enable( m_zoomFactor > 1 );
}

void ZoomFrame::OnTimer(wxTimerEvent& event)
{
    RefreshBitmap();
}
