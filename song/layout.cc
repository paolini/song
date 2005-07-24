#include <vector>
#include <cassert>
#include <climits>
#include <cstring>
#include "layout.hh"
#include "util.hh"

typedef unsigned int uint;

static int get_align(int total, int part,int align) {
    if (align==0) return (total-part)/2;
    if (align>0) return (total-part);
    else return 0;
  };

bool stanza_debug=false, verse_debug=false, song_debug=false, body_debug=false;

Media* Box::m=0;

ostream& operator<<(ostream& o, const Dim &dim) {
  return o<<dim.x<<","<<dim.y;
};

ostream& operator<<(ostream& o, const Badness &b) {
  return o<<b.bad;
}

DimNBad FixedBox::dim(Dim space) {
  if (space.x>=my_dim.x && space.y >= my_dim.y) 
    return DimNBad(my_dim); // ci sta
  else {
    DimNBad ret(my_dim);
    if (ret.x>space.x) {ret.x=space.x;ret.bad+=Badness::Spills();}
    if (ret.y>space.y) {ret.y=space.y;ret.bad+=Badness::Spills();}
    return ret;
  }
};

//Helper class: remembers last width computation:
void CacheBox::add_spilling() {
  if (my_dim.x>last_space.x || my_dim.y>last_space.y)
    my_dim.bad+=Badness::Spills();
}

DimNBad CacheBox::dim(Dim space) {
    if (!(
	   (space.x==last_space.x && space.y==last_space.y) || 
	   (space.x<=last_space.x && my_dim.x<=space.x &&
	    space.y<=last_space.y && my_dim.y<=space.y && 
	    my_dim.bad<=Badness(0))
	   )) {
      last_space=space;
      recalculate();
      add_spilling();
    }
    return my_dim;
  };
  
DimNBad CacheBox::write(Dim space) {
    dim(space);
    int x=m->get_x(),y=m->get_y();
    m->move_xy(get_align(space.x,my_dim.x,halign),
    	       get_align(space.y,my_dim.y,valign));
    current_write(); // le dimensioni sono in last_space;
    m->goto_xy(x+space.x,y);
    return my_dim;
  };

template<class T>
void put(vector<T> &v, unsigned int n, const T &elem) {
  if (n<v.size()) v[n]=elem;
  else {
    assert(v.size()==n);
    v.push_back(elem);
  }
}

//questa classe gestisce le sequenze... (ma va'!)
void SequenceBox::current_write() {
  int nline;
  
  if (test) {
    cerr<<"\nwrite "<<last_space<<"\n";
    m->frame(my_dim.x,my_dim.y);
  }
  
  int savex=m->get_x();
  int savey=m->get_y();
    

  // cerr<<"Writing hor="<<hor<<" sequence box at "<<savex<<", "<<savey
  //	<<" dim "<<d.x<<","<<d.y<<"\n";
    
  if (breakable) m->chordReset(); //inizio vers
    
  if (hor) {
    nline=-1;
    int y=savey+my_dim.y;
    m->goto_xy(savex,y);
    for (unsigned int i=0;i<list.size();++i) {
      if (nline<0 || end_item[nline]==i) { //prepara nuova riga
	nline++;
	y-=lines_height[nline]+(nline>0?sup_space:0);
	m->goto_xy(savex,y);
	if (nline)
	  m->chordReset();
      }
      list[i]->write(Dim(given_space[i].x,lines_height[nline]));
      m->move_xy(this->space,0);
    }
  } else {
    nline=0;
    int x=savex;
    int y=savey+my_dim.y;
    for (unsigned int i=0;i<list.size();++i) {
      if (end_item[nline]==i) { //prepara nuova colonna
	x+=lines_height[nline]+sup_space;
	y=savey+my_dim.y;
	nline++;
      }	
      DimNBad itdim=list[i]->dim(given_space[i]);
      y-=itdim.y;
      m->goto_xy(x,y);
      list[i]->write(Dim(lines_height[nline],given_space[i].y));
      //	list[i]->write(given_space[i]);
      y-=this->space;
    }
  }
    
  m->goto_xy(savex+my_dim.x,savey);
}

void SequenceBox::recalculate() {
  static int level=0;
  level++;
   
   
  lines_height.clear();
  vector<unsigned int> please_height;
 
  given_space.clear();
  unsigned int start_col=0; //numero di righe trovate al primo tentativo(?)

  if (test)
    cerr<<"\nrecalculate "<<last_space<<"\n";

  unsigned int area=UINT_MAX; // area totale utilizzata
  for (unsigned int retries=0;;++retries) {
    unsigned int sum=0; // larghezza raggiunta nella riga corrente
    unsigned int max=0; // altezza massima degli oggetti in questa riga
    unsigned int pleasemax=0;
    unsigned int summax=0; // altezza raggiunta nelle righe precedenti
    unsigned int maxsum=0; // larghezza massima raggiunta nelle righe
    // precedenti
      
    unsigned int nlines=0; // numero di righe completate
      
    end_item.clear();

    my_dim.bad=Badness();
      
    int worst_area=INT_MIN; // spreco maggiore di area
    unsigned int worst_col=0; // colonna con lo spreco maggiore
    int areasum=0; // somma delle aree degli oggetti
    unsigned int chordpos=0; // posizione degli accordi

    for (unsigned int i=0;i<list.size();++i) {
      put(given_space,i,last_space);

      if (retries>0) {
	assert(lines_height.size()>0);
	if (nlines < lines_height.size())
	  given_space[i].get(!hor)=please_height[nlines];
	else
	  given_space[i].get(!hor)=please_height.back();
      }

      if (!breakable && i>0 && last_space.get(hor)>sum+space) {
	given_space[i].get(hor)=last_space.get(hor)-sum-space;
	assert(given_space[i].get(!hor)==last_space.get(!hor));
      }

      DimNBad dim=list[i]->dim(given_space[i]);
      if (test)
	cerr<<"obj["<<i<<"]: "<<dim<<" (given: "<<given_space[i]<<") ";
      //	if (dim.x <= given_space[i].x && dim.y <= given_space[i].y)
      given_space[i]=dim;

      unsigned int actualsum=sum+space+dim.get(hor)+list[i]->chord_width();
      if (chordpos>actualsum) actualsum=chordpos;
	
      if (actualsum>last_space.get(hor)) { //sborda
	if (breakable && sum>0) { //break line
	  summax+=max;
	  assert(nlines<=lines_height.size());
	  assert(lines_height.size() == please_height.size());
	    
	  if (nlines<lines_height.size() && max>lines_height[nlines])
	    max=lines_height[nlines];
	  put(lines_height,nlines,max);
	  put(please_height,nlines,pleasemax);
	    
	  end_item.push_back(i);
	  summax+=sup_space;
	  if (chordpos>sum) sum=chordpos;
	  if (sum>maxsum) maxsum=sum;

	  { 
	    int dummy=int(last_space.get(hor)*max)-int(areasum);
	    if (test)
	      cerr<<"-- spreco: "<<dummy<<"\n";
	    if (dummy>worst_area) {
	      worst_area=dummy;
	      worst_col=nlines;
	    }
	    areasum=0;
	  }
	    
	  sum=0;
	  max=0;
	  chordpos=0;
	  pleasemax=0;

	  nlines++;
	  my_dim.bad+=Badness::BrokenLine();
	    
	  --i;
	  continue; // ricomincia dalla riga seguente.
	} else {
	  my_dim.bad+=Badness::Spills();
	  if (test)
	    cerr<<"[ spills !]\n";
	}
      }
      areasum+=int(dim.area());

      if (test)
	cerr<<" -- line "<<nlines<<" -- area "<<dim.area()<<"\n";


	
      if (sum>0) sum+=space;
      sum+=dim.get(hor);

      if (dim.get(!hor)>max) {
	max=dim.get(!hor);
      }

      if (list[i]->please() && dim.get(!hor) > pleasemax) {
	pleasemax=dim.get(!hor);
      }

      if (list[i]->chord_width()) {
	if (chordpos) chordpos+=space;
	if (chordpos<max) chordpos=max;
	chordpos+=list[i]->chord_width();
      }

      //if (test)
      //  cerr<<"max: "<<max<<" pleasemax: "<<pleasemax<<"\n";

      my_dim.bad+=dim.bad;
    }
      
    summax+=max;
    put(lines_height,nlines,max);
    put(please_height,nlines,pleasemax);
    end_item.push_back(list.size());
      
    if (chordpos>sum) sum=chordpos;
    if (sum>maxsum) maxsum=sum;
    if (hor) {
      my_dim.x=maxsum;
      my_dim.y=summax;
    } else {
      my_dim.x=summax;
      my_dim.y=maxsum;
    }

    { 
      int dummy=int(last_space.get(hor)*max)-int(areasum);
      if (test)
	cerr<<"-- spreco: "<<dummy<<"\n";
      if (dummy>worst_area) {
	worst_area=dummy;
	worst_col=nlines;
      }
      areasum=0;
    }
      
    nlines++;
    if (start_col==0) start_col=nlines;

    if (test) {
      cerr<<"SequenceBox["<<level<<"]: "
	  <<"given_space: "<<last_space<<"; "
	  <<"dim: "<<my_dim<<"; "
	  <<"badness: "<<my_dim.bad<<"; "
	  <<"\n";
      cerr<<"altezze/larghezze:";
      for (unsigned int ii=0; ii<lines_height.size(); ++ii)
	cerr<<" "<<lines_height[ii];
      cerr<<"\n";
    }

    if (!breakable || !adjustable 
	|| my_dim.get(!hor) <= last_space.get(!hor) 
	|| nlines>start_col*3) {
      // OK! ho finito!
      //	if (breakable)
      //	  assert(my_dim.get(hor)<=last_space.get(hor));
      break;
    }
      
    if (test) 
      cerr<<"temptative #"<<retries<<"; "
	  <<"hor: "<<hor<<"; "
	  <<"nlines/columns: "<<nlines<<"; "
	  <<"badness: "<<my_dim.bad<<"; "
	  <<"dim: "<<my_dim.x<<","<<my_dim.y<<";"
	  <<"space: "<<last_space.x<<","<<last_space.y<<";"
	  <<"\n";
      
    if (my_dim.x*my_dim.y>area) {
      if (test)
	cerr<<"peggiora...\n";
      break; // la cosa non migliora
    } else area=my_dim.x*my_dim.y;

    if (retries>100) {
      if (test)
	cerr<<"100 retries...\n";
      break;
    }

    //restringo la colonna peggiore
    assert(worst_col<nlines);
    if (please_height[worst_col]<lines_height[worst_col] && 
	please_height[worst_col]>0)
      lines_height[worst_col]=please_height[worst_col];
    else if (lines_height[worst_col]>1)
      lines_height[worst_col]--;
    else {
      if (test)
	cerr<<"let it be ...\n";
      break; //la colonna si e' azzerata
    }

  }
  level--;
};
    
SequenceBox::~SequenceBox() {
  for (unsigned int i=0;i<list.size();++i) delete list[i];
};

SequenceBox::SequenceBox(bool horizontal,bool Break,bool adjust): 
  test(false), 
  hor(horizontal), 
  breakable(Break),
  adjustable(adjust) 
{
  space=0;
  halign=-1;valign=-1;
  sup_space=0;
  word_align=-1;
};

void SequenceBox2::current_write() {
  int nline;
  
  //  cerr<<"\nwrite "<<last_space<<" at "<<m->get_x()<<","<<m->get_y()<<"\n";

  if (test) {
    cerr<<"\nwrite in "<<last_space<<" "<<my_dim<<"\n";
    m->frame(my_dim.x,my_dim.y);
  }
  
  int savex=m->get_x();
  int savey=m->get_y();


  // cerr<<"Writing hor="<<hor<<" sequence box at "<<savex<<", "<<savey
  //	<<" dim "<<d.x<<","<<d.y<<"\n";
    
  if (breakable) m->chordReset(); //inizio vers
    
  if (hor) {
    nline=-1;
    int y=savey+my_dim.y;
    m->goto_xy(savex,y);
    for (unsigned int i=0;i<list.size();++i) {
      if (nline<0 || next_item[nline]==i) { //prepara nuova riga
	nline++;
	y-=line_dim[nline].get(!hor)+(nline>0?sup_space:0);
	m->goto_xy(savex,y);
	if (nline)
	  m->chordReset();
      }
      list[i]->write(Dim(given_space[i].get(hor),line_dim[nline].get(!hor)));
      m->move_xy(this->space,0);
    }
  } else {
    nline=0;
    int x=savex;
    int y=savey+my_dim.y;
    for (unsigned int i=0;i<list.size();++i) {
      if (next_item[nline]==i) { //prepara nuova colonna
	x+=line_dim[nline].get(hor)+sup_space;
	y=savey+my_dim.y;
	nline++;
      }	
      DimNBad itdim=list[i]->dim(given_space[i]);
      y-=itdim.y;
      m->goto_xy(x,y);
      list[i]->write(Dim(line_dim[nline].get(hor),given_space[i].y));
      //	list[i]->write(given_space[i]);
      y-=this->space;
    }
  }
    
  m->goto_xy(savex+my_dim.x,savey);

  //  cerr<<"written "<<last_space<<"\n";

}

static const char *lc(bool hor) {
  if (hor) return "line";
  else return "column";
};

void SequenceBox2::recalculate() {
  static int level=0;
  level++;
  
//   given_space.resize(list.size());
//   next_line.resize(1);
//   next_line[0]=1000;
//   for (uint i=0;i<given_space.size();++i) {
//     given_space[i].x=10000;
//     given_space[i].y=10000;
//   };
//   return;
  

  int savex=m->get_x();
  int savey=m->get_y();
  //  cerr<<"Pos: "<<savex<<" "<<savey<<"\n";
    

  if (test)
    cerr<<"\nrecalculate "<<last_space<<" (level "<<level<<") \n";

  // start new line
  int nlines=0;
  int current_line=0;
  
  given_space.resize(list.size());

  line_dim.resize(1);
  next_item.resize(1);
  line_dim[0]=DimNBad(); // tutto 0
  
  for (uint i=0;i<list.size();++i) { 
    // calcola la dimensione di tutti gli oggetti
    // con il massimo spazio a disposizione
    given_space[i]=list[i]->dim(last_space); //quanto spazio occupa?
    assert(given_space[i].x<= last_space.x 
	   && given_space[i].y<= last_space.y);
    int new_width=
      line_dim[current_line].get(hor)+space
      +given_space[i].get(hor)+list[i]->chord_width();
    if (new_width>last_space.get(hor)  // non sta
	&& breakable                   // posso cambiare riga
	&& line_dim[current_line].get(hor)>0 // ho usato la riga attuale
	) { 
      cerr<<"newline\n";
      // cambio riga
      assert(i>0);
      next_item[current_line]=i;
      current_line++;
      next_item.resize(current_line+1);
      line_dim.resize(current_line+1);
      line_dim[current_line]=DimNBad();
      new_width=given_space[i].get(hor);
    }
    line_dim[current_line].get(hor)=new_width;
    line_dim[current_line].bad+=given_space[i].bad;
    if (given_space[i].get(!hor)>line_dim[current_line].get(!hor)) 
      line_dim[current_line].get(!hor)=given_space[i].get(!hor);
    if (new_width<=last_space.get(hor)) {
      line_dim[current_line].bad+=Badness::Spills();
    }

    if (test) {
      cerr<<"item["<<i<<"]: given "<<last_space<<" uses "<<given_space[i]<<
	" line["<<current_line<<"]: "<<line_dim[current_line]<<"\n";
    }
  };
  next_item[current_line]=list.size();

  nlines=current_line+1;

  my_dim=DimNBad();

  assert(my_dim.x == 0 && my_dim.y==0);
  
  for (uint j=0;j<nlines;++j) {
    if (line_dim[j].get(hor)>my_dim.get(hor)) 
      my_dim.get(hor)=line_dim[j].get(hor);
    my_dim.get(!hor)+=line_dim[j].get(!hor);
  }
  
  if (my_dim.get(!hor)>last_space.get(!hor)) {
    my_dim.get(!hor)=last_space.get(!hor);
    my_dim.bad+=Badness::Spills();
  }
  
  if (my_dim.get(hor)>last_space.get(hor)) {
    my_dim.get(hor)=last_space.get(hor);
    my_dim.bad+=Badness::Spills();
  }

  m->goto_xy(savex,savey);
  level--;
  if (test)
    cerr<<"Result: "<<my_dim<<"\n";
};
    
SequenceBox2::~SequenceBox2() {
  for (unsigned int i=0;i<list.size();++i) delete list[i];
};

SequenceBox2::SequenceBox2(bool horizontal,bool Break,bool adjust): 
  test(false), 
  hor(horizontal), 
  breakable(Break),
  adjustable(adjust) 
{
  space=0;
  halign=-1;valign=-1;
  sup_space=0;
  word_align=-1;
};

// cerca di ottimizzare la suddivisione della sequenza rimpicciolendo
// al massimo lo spazio a disposizione

void CompressBox::recalculate() {
//     narrow=last_space;
//     my_dim=my_box->dim(last_space);
//     narrow.get(my_hor)=my_dim.get(my_hor);
//     while (narrow.get(my_hor)>0) {
//       Dim narrower=narrow;
//       narrower.get(my_hor)--;
//       DimNBad test=my_box->dim(narrower);
//       if (test.bad <= my_dim.bad && test.get(my_hor)<=narrower.get(my_hor)) {
// 	my_dim=test;
// 	narrow.get(my_hor)=my_dim.get(my_hor);
//       }
//       else break;
//     }
    
    my_dim=my_box->dim(last_space);
    return;

    Dim narrower(my_dim);
    while (narrower.get(my_hor)>0) {
      narrower.get(my_hor)--;
      DimNBad test=my_box->dim(narrower);
      if (!(test.bad <= my_dim.bad) || test.get(my_hor)>narrower.get(my_hor)) 
	break;
      my_dim=test;
      narrower=test;
    }
    my_box->dim(my_dim);
};


void StringBox::calculate() {
  my_dim.x=m->wordWidth(my_s,f);
  my_dim.y=m->lineSkip(f);
}
StringBox::StringBox(unsigned char *utf8_string,unsigned char *end,
		     Media::font the_f): 
  my_s(utf8(utf8_string,end)), f(the_f) 
{
  calculate();
};

StringBox::StringBox(const string &s,Media::font the_f): 
  my_s(s), f(the_f) {
  calculate();};
  
DimNBad StringBox::write(Dim space) {
    //    cerr<<"Write: "<<my_s<<" ("<<m->get_x()<<","<<m->get_y()<<")\n";
    m->wordWrite(my_s,f);
    return dim(space);
  };

ChordBox::ChordBox(string s): StringBox(s,Media::CHORD) {
  my_dim.x=0;
  my_dim.y=m->chordSkip();
  my_chord_width=m->wordWidth(s,Media::CHORD);
};
DimNBad ChordBox::write(Dim space) {
  m->chordWrite(my_s);
  return dim(space);
};

// class FrameBox: public ModifierBox {
// protected:
//   char my_fill;
// public:
//   FrameBox(Box *box, char fill='.'): ModifierBox(box), my_fill(fill) {};
//   virtual void write(Dim space) {
//     int savex=m->get_x();
//     int savey=m->get_y();
//     DimNBad d=dim(space);
//     string wipe(d.x,my_fill);
//     for (unsigned int j=0;j<d.y;++j) {
//       m->goto_xy(savex,savey+j);
//       m->wordWrite(wipe);
//     }
//     m->goto_xy(savex,savey);
//     my_box->write(space);
//   };
// };


