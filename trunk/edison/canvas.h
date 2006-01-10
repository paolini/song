#ifndef CANVAS_H
#define CANVAS_H

class MyFrame;

class MyCanvas: public wxScrolledWindow {
  MyFrame *frame;
public:
  PlugoutOptions opt;
  MyCanvas(wxWindow *parent, MyFrame *);
  
  //void OnPaint(wxPaintEvent &event);
  virtual void MyCanvas::OnDraw(wxDC &dc);
  void OnMouseMove(wxMouseEvent &event);
  
  //  void Show(ScreenToShow show) { m_show = show; Refresh(); }
};

#endif
