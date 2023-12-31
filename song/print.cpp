#include <vector>
#include <cassert>
#include <climits>
#include <cstring>

#include "song.h"
#include "iso.h"
#include "layout.h"
#include "print.h"
#include "debug.h"

void PutString(SequenceBox *verse, SequenceBox **word, const string &utf8, 
	       Media::font f) {
  const string &s=utf8;
  //xmlChar *start;
  size_t i;
  for (i=0;i<s.size();) {
    if (isspace(s[i])) {
      if ((*word)->size()!=0) {
	verse->push_back(*word);
	(*word)=new SequenceBox(true);
      }
      i++;
    } else {
      //unsigned char *end;
      size_t j;
      for (j=i;j<s.size() && !isspace(s[j]);++j);
      if (j>i) {
	(*word)->push_back(new StringBox(string(s,i,j-i),f));
      }
      i=j;
    }
  }
}

void ParseNodeList(SequenceBox *verse, SequenceBox **word, 
		   const PhraseList *p,Media::font f, int level);

void ParseNodeItem(SequenceBox *verse, SequenceBox **word, 
		   const PhraseItem *item,Media::font f, int level) {
  union {
    const Word *w;
    const Chord *c;
    const Modifier *m;
    const Tab *t;
  } p;

  p.w=dynamic_cast<const Word*>(item);
  if (p.w) {
    PutString(verse, word, p.w->word, f);
    return;
  } 
  
  p.c=dynamic_cast<const Chord *>(item);
  if (p.c) {
    if (level==0) {
      (*word)->push_back(new ChordBox(p.c->modifier));
    } else {
      PutString(verse, word, p.c->modifier, Media::CHORD);
    }
    return;
  }
  
  p.m=dynamic_cast<const Modifier *>(item);
  if (p.m) {
    if (p.m->attribute==Modifier::STRUM) {
      ParseNodeList(verse,word,p.m->child,f,level+1);
    }
    else if (p.m->attribute==Modifier::NOTES) {
      int l=(*word)->size();
      Box* save=*word;
      
      PutString(verse, word, "[", f);
      
      ParseNodeList(verse,word,p.m->child, f, level+1);
      if (*word == save && (*word)->size()==l+1) {
	(*word)->free(l);
      } else
	PutString(verse, word, "]", f);
    }
    else assert(false);
    return;
  }
  
  p.t=dynamic_cast<const Tab *>(item); 
  if (p.t) {
    if ((*word)->size()) verse->push_back(*word);
    verse->push_back(new TabBox());
    (*word)=new SequenceBox(true);
    return;
  }
  
  assert(false);
}

void ParseNodeList(SequenceBox *verse, SequenceBox **word, 
		   const PhraseList *p,Media::font f, int level) {
  size_t i;
  for (i=0;i<p->list.size();++i)
    ParseNodeItem(verse,word,p->list[i],f,level);
}
Box *VerseBox(Media &m, const PhraseList *p, Media::font f) {
  assert(p!=0);
  SequenceBox *verse=new SequenceBox(true, true);
  verse->space=m.spaceWidth(f);
  verse->test=verse_debug;
  verse->halign=-1;
  SequenceBox *word=new SequenceBox(true);

  ParseNodeList(verse,&word,p, f,0);

  if (word->size()) verse->push_back(word);
  else delete word;
  CompressBox* ret=new CompressBox(verse,true);
  ret->halign=-1;
  return ret;
};

Box* StanzaBox(Media &m, const Stanza* p) {
  Media::font f=Media::NORMAL;
  assert(p!=0);
  SequenceBox *stanza=new SequenceBox(false);
  stanza->test=stanza_debug;
  //  unsigned char *type=xmlGetProp(p,(xmlChar *)"type");
  if (p->type==Stanza::REFRAIN) 
    f=Media::REFRAIN;
  size_t i;
  for (i=0;i<p->verse.size();++i) {
    stanza->push_back(VerseBox(m,p->verse[i],f));
  }
  Box *r=stanza;
  if (p->type==Stanza::SPOKEN) { 
    r=new PleaseBox(stanza);
  }
  //  free(type);
  return /*new FrameBox*/ (r);
};

Box* BodyBox(Media &m, const Body* p) {
  assert(p!=0);
  SequenceBox *body=new SequenceBox(false,true,true);
  body->space=m.stanza_sep;
  body->sup_space=m.column_sep;
  body->test=body_debug;
  size_t i;
  for (i=0;i<p->stanza.size();++i) {
    body->push_back(StanzaBox(m, p->stanza[i]));
  }
  CompressBox *ret=new CompressBox(body,false);
  ret->halign=-1;
  return ret;
};

void addWordsToSequence(SequenceBox *seq, const string &s, Media::font f) {
  unsigned int start;
  for (start=0;start<s.size();) {
    while (start<s.size() && isspace(s[start])) start++;
    unsigned int end;
    for (end=start;end<s.size() && !isspace(s[end]);++end);
    if (end>start)
      seq->push_back(new StringBox(string(s,start,end-start),f));
    start=end;
  }
};

Box* HeadBox(Media &m, Head* p) {
  SequenceBox *title=0;
  string s=p->title;
  if (s.size()) {
    if (m.uppercase_title)
      for (unsigned int i=0;i<s.size();++i)
	s[i]=toupper(s[i]);
    title=new SequenceBox(true);
    title->space=m.spaceWidth(Media::TITLE);
    addWordsToSequence(title,s, Media::TITLE);
  }

  SequenceBox *author=0;
  for (int i=0;i<p->author.size();++i) {
    s=p->author[i]->firstName+" "+p->author[i]->Name;
    
    if (author==0) {
      author=new SequenceBox(true);
      author->space=m.spaceWidth(Media::AUTHOR);
    }

    if (i>0) addWordsToSequence(author,"-",Media::AUTHOR);
    addWordsToSequence(author,s,Media::AUTHOR);
  }

  SequenceBox *head=new SequenceBox(false);
  if (title) head->push_back(title);
  if (author) head->push_back(author);

  return head;
};

Box* SongBox(Media &m, Song* p) {
  //  assert(!strcmp((char *)(p->name), "song"));
  assert(p);
  Box::setMedia(m);
  SequenceBox *song=new SequenceBox(false);
  song->space=m.body_sep;
  song->halign=-1; song->valign=1;
  if (song_debug) song->test=true;
  
  Box *body=0;
  Box *head=0;

  if (p->head) head=HeadBox(m,p->head);
  if (p->body) body=BodyBox(m,p->body);

  if (head) song->push_back(head);
  if (body) song->push_back(body);
  // song->test=true;
  return song;
};

// helper class: contiene una box, ma quando viene distrutta non distrugge
// la scatola contenuta
class TryBox: public Box {
protected:
  Box *my_box;
public:
  TryBox(Box *box): my_box(box) {};  
  virtual ~TryBox() {};
  virtual DimNBad dim(Dim space) {return my_box->dim(space);};
  virtual DimNBad write(Dim space) {return my_box->write(space);};
};

// layout e' una stringa in notazione polacca
// operatori '|' e '-', operandi le lettere minuscole
Box* LayoutBoxProgressive(const char *&layout, Box **&list, 
			  int song_sep, bool temptative=true) {
  if (isalpha(layout[0])) { // single box
    layout++;
    if (temptative)
      return new TryBox((list++)[0]);
    else return (list++)[0];
  } else if (layout[0]=='|' || layout[0]=='-') {
    char c=layout[0];
    layout++;
    Box *left=LayoutBoxProgressive(layout,list,song_sep,temptative);
    Box *right=LayoutBoxProgressive(layout,list,song_sep,temptative);
    SequenceBox *ret;
    if (c=='|') {
      ret= new SequenceBox(true);
      ret->halign=-1;
    }
    else {
      ret= new SequenceBox(false);
    }
    ret->space=song_sep;
    //    assert(song_sep>0);
    ret->halign=-1;
    ret->valign=1;
    ret->push_back(left);
    ret->push_back(right);
    if (layout_debug) ret->test=true;
    return ret;
  } else {
    assert(false);
  }
}

Box* LayoutBox(const char *layout, Box **list, int song_sep, bool temptative=true) {
  return LayoutBoxProgressive(layout,list,song_sep,temptative);
}

static const char *trylayout[]={
  "a","|ab","-ab",
  "-a|bc","-|abc", "|a-bc", "|-abc",
  "-|ab|cd","|-ab-cd",
  0};

void PrintSongs(const vector<Song *> &songlist,Media &m) {
  vector<Box *> list;
  int count=0;
  for(unsigned int i=0;i<songlist.size();++i) {
    list.push_back(SongBox(m,songlist[i]));
  }
  int page=0;
  while(list.size()) {
    page++;
    int origsize=list.size();
    DimNBad best_dim;
    const char *bestlayout=0;
    for (int i=0;
	 trylayout[i] && (strlen(trylayout[i])+1)/2 <= list.size();i++) {
      Box *box=LayoutBox(trylayout[i],&list[0],m.song_sep,true); // try
      DimNBad dim=box->dim(Dim(m.page_width(),m.page_height()));
      //      cerr<<"trying layout "<<trylayout[i]<<": "<<dim<<"\n";
      delete box;
      
      if (bestlayout==0 || 
	  dim.bad<=best_dim.bad+Badness::BrokenLine()*2) { // e` il migliore, per ora
	bestlayout=trylayout[i];
	best_dim=dim;
      }
    }
    assert(bestlayout!=0);
    Box **bptr=&(list[0]);

    string layout=bestlayout;
    
    Box *box=LayoutBoxProgressive(bestlayout,bptr,m.song_sep,false);
    unsigned int n=bptr-&(list[0]); // numero di canzoni utilizzate
    list.erase(list.begin(),list.begin()+n);
    DimNBad dim=box->write(Dim(m.page_width(),m.page_height()));

    cerr<<n<<" songs in page "<<page<<
      ", dim[bad]: "<<dim<<" lay: "
	<<layout<<"\n  ";
    for (int i=0;i<n;++i) {
      if (i) cerr<<", ";
      assert(songlist[count]->head);
      cerr<<iso(songlist[count]->head->title);
      count++;
    }
    cerr<<"\n";

    if (list.size())
      m.newPage();
    delete box;
  }
}
