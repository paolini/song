// questa classe astrae il formato di stampa
// dovrebbe adattarsi a txt a PS e altro...

class Media {
protected:
public:
  virtual void goto_xy(int x, int y)=0;
  virtual void move_xy(int x, int y) {goto_xy(get_x()+x,get_y()+y);};
  virtual int get_x() const =0;
  virtual int get_y() const =0;

  virtual int wordWidth(const string &s) const=0; 
  virtual void wordWrite(const string &s)=0; 
  virtual int chordWidth(const string &s) const {
    return wordWidth(s);};
  virtual void chordWrite(const string &s) {
    wordWrite(s);};
  virtual void chordReset() {}; //nuova riga di accordi
  virtual int spaceWidth() const {return wordWidth(" ");};
  virtual void spaceWrite() {wordWrite(" ");};

  virtual int lineSkip() const=0; 
  virtual int chordSkip() const {return 2*lineSkip();};
  
  virtual void out(ostream &o) const {o<<"Output Not implemented!\n";};
  virtual int page_width() const=0;
  virtual int page_height() const=0;
};

class TextMedia: public Media {
private:
  char *my_map;
  int x,y;
  int my_width,my_height;
  int chord_x;
protected:
  bool valid(int x, int y) const {
    return x>=0 && x<my_width && y>=0 && y<my_height;};

  char get(int x, int y) const {
    if (valid(x,y)) return my_map[y*my_width+x];
    else return 0;
  };

  void set(int x, int y, char c) {
    if (valid(x,y)) {
      if (get(x,y)!=' ') c='\\';
	my_map[y*my_width+x]=c;
    }
  };

  void clear() {
    memset(my_map,' ',my_width*my_height);
    chordReset();
  };

public:
  virtual int wordWidth(const string &s) const {
    return s.size();};
  virtual void goto_xy(int xx, int yy) {x=xx;y=yy;};
  virtual int get_x() const {return x;};
  virtual int get_y() const {return y;};

  TextMedia(int width, int height): my_width(width), my_height(height) {
    my_map=new char[my_width*my_height];
    clear();
  };

  virtual int page_width() const {return my_width;};
  virtual int page_height() const {return my_height;};

  virtual ~TextMedia() {delete[] my_map;};

  virtual void wordWrite(const string &s) {
    for (int i=0;i<s.size();++i)
      set(x++,y,s[i]);
  };

  virtual void chordWrite(const string &s) {
    int savex=x, savey=y;
    y++;
    //cout<<"writing chord "<<s<<" x,y="<<x<<","<<y<<" chord_x="<<chord_x<<"\n";
    if (x<chord_x) x=chord_x;
    wordWrite(s);
    chord_x=x+1;
    x=savex, y=savey;
  };

  virtual void chordReset() {
    chord_x=-1;
  };

  virtual int lineSkip() const {return 1;};

  virtual void out(ostream &o) const {
    int k;
    for (k=my_height-1;k>=0;--k) {
      int i;
      for (i=0;i<my_width && isspace(get(i,my_height-k-1));++i);
      if (i<my_width) break;
    }
    for (int i=0;i<=k;++i) {
      int j;
      for (j=my_width-1;j>=0 && isspace(get(j,my_height-i-1));--j); 
      o.write(my_map+(my_height-i-1)*my_width,j+1);
      o<<"\n";
    }
  }
};

