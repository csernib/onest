#include "git.h"
#include "../ext/git/git.h"


using namespace std;

string onest::git::getCommitInfo()
{
	if (!::git::IsPopulated())
		return "(unknown)";

	const string abbrevHash = ::git::CommitSHA1().substr(0, 8);
	const string dirty = ::git::AnyUncommittedChanges() ? " (MODIFIED)" : "";

	return abbrevHash + dirty;
}
