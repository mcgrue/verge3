/// The VERGE 3 Project is originally by Ben Eirich and is made available via
///  the BSD License.
///
/// Please see LICENSE in the project's root directory for the text of the
/// licensing agreement.  The CREDITS file in the same directory enumerates the
/// folks involved in this public build.
///
/// If you have altered this source file, please log your name, date, and what
/// changes you made below this line.


#ifndef DICT_H
#define DICT_H

#include <map>

class dict {
	public:
		string GetString(string key);
		void SetString(string key, string val);
		int ContainsString(string key);
		void RemoveString(string key);
		int Size();
		string ListKeys(string separator);
	private:
		std::map<string,string> data;
};

#endif