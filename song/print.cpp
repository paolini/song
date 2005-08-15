#include <vector>
#include <cassert>
#include <climits>
#include <cstring>
#include <libxml/tree.h>

#include "layout.h"
#include "util.h"
#include "print.h"

//#define SequenceBox SequenceBox2

string getTitle(xmlNodePtr p) {
  if (p) p=p->children;
  while (p && strcmp((char *)p->name,"title")) p=p->next;
  if (p) p=p->children;
  if (p) {
    xmlChar *s=xmlNodeListGetString(NULL,p,1);
    string r=utf8(s);
    xmlFree(s);
    return r;
  }
  else return string("");
}

string getAuthor(xmlNodePtr p, int n) {
  if (p) p=p->children;
  do {
    while (p && strcmp((char*)p->name,"author")) p=p->next;
    if (n==0 || p==0) break;
    p=p->next;
    --n;
  } while (1);
  if (p) p=p->children;
  if (p) {
    xmlChar *s=xmlNodeListGetString(NULL,p,1);
    string r=utf8(s);
    xmlFree(s);
    return r;
  }
  else return string();
}

void PutString(SequenceBox *verse, SequenceBox **word,xmlChar *s, Media::font f) {
  xmlChar *start;
  for (start=s;start && *start;) {
    if (isspace(*start)) {
      if ((*word)->size()!=0) {
	verse->push_back(*word);
	(*word)=new SequenceBox(true);
      }
      start++;
    } else {
      unsigned char *end;
      for (end=start;*end && !isspace(*end);++end);
      if (end>start) {
	(*word)->push_back(new StringBox(start,end,f));
      }
      start=end;
    }
  }
}

void ParseNodeList(SequenceBox *verse, SequenceBox **word, 
		   xmlNodePtr p,Media::font f) {
  for (;p;p=p->next) {
    if (p->type == XML_TEXT_NODE) {
      PutString(verse, word, p->content, f);
    } else if (!strcmp((char *)(p->name),"c")) {
      xmlChar *s=xmlNodeListGetString(NULL,p->children,1);
      if (p->parent && p->parent->name 
	  && !(strcmp((char*)p->parent->name,"v"))) {
	(*word)->push_back(new ChordBox(utf8(s)));
      } else PutString(verse, word, s, Media::CHORD);
      xmlFree(s);
    } else if (!strcmp((char*)(p->name),"note")) {
      int l=(*word)->size();
      Box* save=*word;
      
      PutString(verse, word, (xmlChar *) "[", f);
      ParseNodeList(verse,word,p->children, f);
      if (*word == save && (*word)->size()==l+1) {
	(*word)->free(l);
      } else
	PutString(verse, word, (xmlChar *) "]", f);
    } else if (!strcmp((char *)(p->name), "strum")) {
      ParseNodeList(verse,word,p->children, f);
    } else if (!strcmp((char *)(p->name), "tab")) {
      if ((*word)->size()) verse->push_back(*word);
      verse->push_back(new TabBox());
      (*word)=new SequenceBox(true);
    }
  }  
}

Box *VerseBox(Media &m, xmlNodePtr p, Media::font f) {
  assert(!strcmp((char *)(p->name),"v"));
  SequenceBox *verse=new SequenceBox(true, true);
  verse->space=m.spaceWidth(f);
  verse->test=verse_debug;
  verse->halign=-1;
  SequenceBox *word=new SequenceBox(true);

  ParseNodeList(verse,&word,p->children, f);

  if (word->size()) verse->push_back(word);
  else delete word;
  CompressBox* ret=new CompressBox(verse,true);
  ret->halign=-1;
  return ret;
};

Box* StanzaBox(Media &m, xmlNodePtr p) {
  Media::font f=Media::NORMAL;
  assert(!strcmp((char *)(p->name),"stanza"));
  SequenceBox *stanza=new SequenceBox(false);
  stanza->test=stanza_debug;
  unsigned char *type=xmlGetProp(p,(xmlChar *)"type");
  if (type && !strcmp((char *)type,"refrain"))
    f=Media::REFRAIN;
  for (p=p->children;p;p=p->next) {
    if (!strcmp((char *)(p->name),"v"))
      stanza->push_back(VerseBox(m,p,f));
  }
  Box *r=stanza;
  if (type && !strcmp((char *)type,"talking")) { 
    r=new PleaseBox(stanza);
  }
  free(type);
  return /*new FrameBox*/ (r);
};

Box* BodyBox(Media &m, xmlNodePtr p) {
  assert(!strcmp((char *)(p->name),"body"));
  SequenceBox *body=new SequenceBox(false,true,true);
  body->space=m.stanza_sep;
  body->sup_space=m.column_sep;
  body->test=body_debug;
  for (p=p->children;p;p=p->next) {
    if (!strcmp((char *)(p->name),"stanza"))
      body->push_back(StanzaBox(m, p));
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

Box* HeadBox(Media &m, xmlNodePtr p) {
  SequenceBox *title=0;
  string s=getTitle(p);
  if (s.size()) {
    if (m.uppercase_title)
      for (unsigned int i=0;i<s.size();++i)
	s[i]=toupper(s[i]);
    title=new SequenceBox(true);
    title->space=m.spaceWidth(Media::TITLE);
    addWordsToSequence(title,s, Media::TITLE);
  }

  SequenceBox *author=0;
  for (int i=0;;++i) {
    s=getAuthor(p,i);
    if (s.size()==0) break;
    
    size_t comma=s.find(',');
    if (comma!=string::npos) {
      size_t end;
      for (end=comma+1;end<s.size() && isspace(s[end]);++end);
      s=string(s,end)+" "+string(s,0,comma);
    }
	
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

Box* SongBox(Media &m, xmlNodePtr p) {
  assert(!strcmp((char *)(p->name), "song"));
  Box::setMedia(m);
  SequenceBox *song=new SequenceBox(false);
  song->space=m.body_sep;
  song->halign=-1; song->valign=1;
  if (song_debug) song->test=true;
  
  Box *body=0;
  Box *head=0;
  for (p=p->children;p;p=p->next) {
    if (!strcmp((char *)(p->name), "body")) {
      body=BodyBox(m,p);
    } else if (!strcmp((char *)(p->name), "head")) {
      head=HeadBox(m,p);
    }
  }
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
    Box *left=LayoutBoxProgressive(layout,list,temptative);
    Box *right=LayoutBoxProgressive(layout,list,temptative);
    SequenceBox *ret;
    if (c=='|') {
      ret= new SequenceBox(true);
      ret->halign=-1;
    }
    else {
      ret= new SequenceBox(false);
    }
    ret->space=song_sep;
    ret->halign=-1;
    ret->valign=1;
    ret->push_back(left);
    ret->push_back(right);
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

void PrintSongs(vector<xmlNodePtr> &songlist,Media &m) {
  vector<Box *> list;
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
      delete box;
      
      if (bestlayout==0 || 
	  dim.bad<=best_dim.bad+Badness::BrokenLine()*2) { // e` il migliore, per ora
	bestlayout=trylayout[i];
	best_dim=dim;
      }
    }
    assert(bestlayout!=0);
    Box **bptr=&(list[0]);
    Box *box=LayoutBoxProgressive(bestlayout,bptr,m.song_sep,false);
    list.erase(list.begin(),list.begin()+(bptr-&(list[0])));
    DimNBad dim=box->write(Dim(m.page_width(),m.page_height()));
    if (list.size())
      m.newPage();
    cerr<<origsize-list.size()<<" songs in page "<<page<<
      ", dim: "<<dim.x<<"x"<<dim.y<<", bad: "<<dim.bad<<"\n";
    delete box;
  }
}
