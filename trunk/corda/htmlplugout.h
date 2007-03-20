#include <map>
#include <iostream>

#include "plug.h"


class HtmlPlug: public Plugout {
private:
  static const bool dummy;
  static bool startup();
  static Plugout* Createout() {return new HtmlPlug();};

  std::ostream *op;

public:

  virtual void Write(std::ostream &out, const SongArray &list,
		     const PlugoutOptions &opt);
                     
  HtmlPlug(): Plugout("html","html","html format") {};

  void writeSong(std::ostream &out, const Song *song, const PlugoutOptions *options, int n);
  void writeHead(const Head *head, const PlugoutOptions *options, int n);
  void writeAuthor(const Author *author);
  std::map<const Stanza *,int> ids;
  void writeBody(const Body *body, const PlugoutOptions *options);
  void writeStanza(const Stanza *stanza, const PlugoutOptions *options);
  void writeVerse(const PhraseList *phrase, const PlugoutOptions *options);
  void writePhrase(const PhraseList *phrase, const PlugoutOptions *options,int chordmode);
  void writeItem(const PhraseItem *item, const PlugoutOptions *options,int chordmode);
};

