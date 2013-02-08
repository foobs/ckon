#include "src/aux/myregex.h"

const char* re1 = // possibly leading whitespace:
    "^[[:space:]]*"
    // possible template declaration:
    "(template[[:space:]]*<[^;:{]+>[[:space:]]*)?"
    // class or namespace:
    "(REPLACE_W_OBJECT)[[:space:]]*"
    // leading declspec macros etc:
    "("
    "\\<\\w+\\>"
    "("
    "[[:blank:]]*\\([^)]*\\)"
    ")?"
    "[[:space:]]*"
    ")*"
    // the class name
    "(\\<\\w*\\>)[[:space:]]*"
    // template specialisation parameters
    "(<[^;:{]+>)?[[:space:]]*"
    // terminate in { or :
    "(\\{|:[^;\\{()]*\\{)";

const char* re2 = "^#include[[:space:]]*\"(.*?)\"[[:space:]]*$";

void myregex::IndexObjects(map_type& m, const string& file, const char* obj) {

  std::string::const_iterator start, end;
  start = file.begin();
  end = file.end();
  boost::match_results<std::string::const_iterator> what;
  boost::match_flag_type flags = boost::match_default;
  string restr = (obj) ? re1 : re2;
  if ( obj ) boost::replace_all(restr,"REPLACE_W_OBJECT",obj);
  boost::regex expression(restr.c_str());
  while(boost::regex_search(start, end, what, expression, flags))
  {
    if ( obj ) {
      // what[0] contains the whole string
      // what[5] contains the class name.
      // what[6] contains the template specialisation if any.
      // add class name and position to map:
      m[std::string(what[5].first, what[5].second) + std::string(what[6].first, what[6].second)] = 
	what[5].first - file.begin();
    }
    else {
      m[std::string(what[1].first, what[1].second)] = what[1].first - file.begin();
    }
    // update search position:
    start = what[0].second;
    // update flags:
    flags |= boost::match_prev_avail;
    flags |= boost::match_not_bob;
  }

}

void myregex::load_file(std::string& s, std::istream& is) {
  s.erase();
  if(is.bad()) return;
  s.reserve(static_cast<std::string::size_type>(is.rdbuf()->in_avail()));
  char c;
  while(is.get(c))
  {
    if(s.capacity() == s.size())
      s.reserve(s.capacity() * 3);
    s.append(1, c);
  }
}

map_type myregex::getIndexMap(const fs::path& p, const char* obj) {
  string text;
  fs::ifstream in(p);
  load_file(text,in);
  in.close();
  map_type m;
  IndexObjects(m,text,obj);
  return m;
}

void myregex::parseIncludes(const fs::path& p, string& cls, const fs::path& csd) {

  //if ( clopts.bVerbose ) cout << "Processing file " << p << endl;
  // find includes
  map_type mi = getIndexMap(p); // default searches for includes
  // compare to subdir, fill core_lib_string
  for ( map_type::iterator i = mi.begin(); i != mi.end(); ++i ) {
    fs::path fndIncl((*i).first);
    if ( fndIncl.string().find(csd.string()) != string::npos ) {
      parseIncludes(fndIncl,cls,csd);// check fndIncl for further includes recursively
    }
    else if ( fndIncl.parent_path().empty() ) {
      fs::path fndSameDirIncl(csd);
      fndSameDirIncl /= fndIncl;
      parseIncludes(fndSameDirIncl,cls,csd);
    }
    else if ( cls.find(getLibString(fndIncl)) == string::npos ) {
      //if ( clopts.bVerbose ) cout << "  link hdr: " << fndIncl << endl;
      cls += getLibString(fndIncl);
    }
  }

}

