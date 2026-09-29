/*****************************************************************************
 * doc.cpp: ++ Documentation handling.
 ****************************************************************************/
# include "axlobs.h"
struct Doc::Rep {
        bool hasCorpus;
        AInt hash;
        List<MutString> lines;
        MutString corpus;
        Length cc;
};
Doc Doc::makeNone() { static char empty[]=""; static Rep r={true,0,listNil<MutString>,empty,0}; return Doc(&r); }
const Doc Doc::none=Doc::makeNone();
Doc Doc::createEmpty(Length cc) {
 Rep *r=(Rep*)stoAlloc((unsigned)OB_Doc,sizeof(*r)+cc); r->hasCorpus=true; r->lines=listNil<MutString>; r->hash=0; r->corpus=((char*)r)+sizeof(*r); r->cc=cc; return Doc(r);
}
Doc Doc::fromString(String s){Doc d=createEmpty(strLength(s)+1); strcpy(d.rep_->corpus,s); d.rep_->hash=strHash(s); return d;}
Doc Doc::fromTokens(List<Token> tl){Length cc=1; for(auto l=tl;l;l=cdr(l))cc+=strLength(car(l).stringValue())+1; Doc d=createEmpty(cc); d.rep_->corpus[0]=0; for(auto l=tl;l;l=cdr(l)){strcat(d.rep_->corpus,car(l).stringValue());strcat(d.rep_->corpus,"\n");} d.rep_->hash=strHash(d.rep_->corpus); return d;}
bool Doc::done()const noexcept{return rep_->hasCorpus;} List<MutString> Doc::lines()const noexcept{return rep_->lines;} String Doc::string()const noexcept{return rep_->corpus;} Length Doc::length()const noexcept{return rep_->cc;} Hash Doc::hash()const noexcept{return rep_->hash;}
Doc Doc::copy()const{return fromString(string());} void Doc::free(){stoFree((void *)rep_);} Doc Doc::mergedWith(Doc d)const{MutString s=strConcat(string(),d.string()); Doc r=fromString(s); strFree(s); return r;} bool Doc::equals(Doc d)const{return *this==d||strEqual(string(),d.string());} int Doc::print(FILE*f)const{return *this?fprintf(f,"++%s",string()):0;}
Hash Doc::tableHash(Doc d){return d.hash();} bool Doc::tableEqual(Doc a,Doc b){return a.equals(b);}
